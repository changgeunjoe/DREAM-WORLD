#include "stdafx.h"
#include "IOCPNetwork.h"
#include "../../Logic/Logic.h"
#include "../../Session/SessionObject/PlayerSessionObject.h"
#include "../../Session/SessionObject/MonsterSessionObject.h"
#include "../../DB/DBObject.h"
#include "../../Room/RoomManager.h"
#include "../protocol/protocol.h"

extern  Logic		g_logic;
extern	DBObject	g_DBObj;
extern RoomManager	g_RoomManager;

IOCPNetwork::IOCPNetwork()
{
	b_isRunning = false;
	m_currentClientId = 0;
	Initialize();
}

IOCPNetwork::~IOCPNetwork()
{
	b_isRunning = false;
	IOCPNetwork::Destroy();
	delete m_acceptOver;
}

void IOCPNetwork::Initialize()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "wsaStartUp Error" << std::endl;
		//WSACleanup();
		//return -1;
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	bind(m_listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(m_listenSocket, SOMAXCONN);
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_hIocp, 9999, 0);
}

void IOCPNetwork::Start()
{
	int addr_size = sizeof(SOCKADDR_IN);
	m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	m_acceptOver = new ExpOver();
	ZeroMemory(&m_acceptOver->m_overlap, sizeof(m_acceptOver->m_overlap));
	m_acceptOver->m_opCode = OP_ACCEPT;
	AcceptEx(m_listenSocket, m_clientSocket, m_acceptOver->m_buffer, 0, addr_size + 16, addr_size + 16, 0, &m_acceptOver->m_overlap);

	int threadNum = std::thread::hardware_concurrency();
	b_isRunning = true;
	for (int i = 0; i < threadNum; ++i)
		m_workerThread.emplace_back([this]() {WorkerThread(); });
}

void IOCPNetwork::Destroy()
{
	for (auto& th : m_workerThread)
		if (th.joinable())
			th.join();
}

void IOCPNetwork::WorkerThread()
{
	while (b_isRunning)
	{
		DWORD ioByte;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(m_hIocp, &ioByte, &key, &over, INFINITE);
		ExpOver* ex_over = reinterpret_cast<ExpOver*>(over);
		if (FALSE == ret) {
			if (ex_over->m_opCode == OP_ACCEPT) {
				std::cout << "Accept Error" << std::endl;
			}
			else {
				std::cout << "GQCS Error on client[" << key << "]" << std::endl;
				//disconnect(static_cast<int>(key));
				if (ex_over->m_opCode == OP_SEND) delete ex_over;
				continue;
			}
		}
		switch (ex_over->m_opCode)
		{
		case OP_ACCEPT:
		{
			int addr_size = sizeof(SOCKADDR_IN);
			int userId = GetUserId();
			if (userId == -1) {
				std::cout << "Max user" << std::endl;
			}
			else {
				std::cout << "Accept User" << std::endl;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_clientSocket), m_hIocp, userId, 0);
				g_logic.AcceptPlayer(&m_session[userId], userId, m_clientSocket);
				m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				ZeroMemory(&m_acceptOver->m_overlap, sizeof(m_acceptOver->m_overlap));
				AcceptEx(m_listenSocket, m_clientSocket, m_acceptOver->m_buffer, 0, addr_size + 16, addr_size + 16, 0, &m_acceptOver->m_overlap);
			}
		}
		break;
		case OP_RECV:
		{
			PlayerSessionObject* pSession = dynamic_cast<PlayerSessionObject*>(m_session[key].m_sessionObject);
			pSession->ConstructPacket(ioByte);
		}
		break;
		case OP_SEND:
		{
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		//Timer Event Boss

		case OP_FIND_PLAYER:
		{
			std::string roomId{ ex_over->m_buffer };
			if (g_RoomManager.IsExistRunningRoom(roomId)) {
				Room& refRoom = g_RoomManager.GetRunningRoom(roomId);
				MonsterSessionObject* bossSession = dynamic_cast<MonsterSessionObject*>(refRoom.GetBoss().m_sessionObject);
				//임시
#define ALONE_TEST 1
#ifdef ALONE_TEST
				auto bossPos = bossSession->GetPosition();

				auto player1 = *refRoom.GetInGamePlayerMap().begin();
				auto palyerPos = m_session[player1.second].m_sessionObject->GetPosition();
				auto desLook = Vector3::Subtract(palyerPos, bossPos);
				//목적 룩 벡터 계산			
				Vector3::Normalize(desLook);
				//현재 보스가 바라보는 방향
				auto bossLook = bossSession->GetLook();
				Vector3::Normalize(bossLook);

				float desRotAngle = acosf(Vector3::DotProduct(desLook, bossLook)); //angle - radian
				desRotAngle = 180.0f * desRotAngle / 3.141592f; // angle - degree
				bossSession->SetRestRotateAngle(ROTATE_AXIS::Y, desRotAngle);
				SERVER_PACKET::BossChangeStateRotatePacket sendPacket;
				sendPacket.size = sizeof(SERVER_PACKET::BossChangeStateRotatePacket);
				sendPacket.type = SERVER_PACKET::BOSS_CHANGE_STATE_REST_ROTATE;
				sendPacket.angle = ROTATE_AXIS::Y;
				sendPacket.angle = desRotAngle;
				g_logic.BroadCastInRoom(roomId, &sendPacket);
#elif
				auto player1 = *refRoom.GetInGamePlayerMap().begin();
				auto desPos = m_session[player1.second].m_sessionObject->GetPosition();
#endif
			}
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_TIMER_RAND_MOVE:
		{
			std::string roomId{ ex_over->m_buffer };
			if (g_RoomManager.IsExistRunningRoom(roomId)) {
				Room& refRoom = g_RoomManager.GetRunningRoom(roomId);
				MonsterSessionObject* bossSession = dynamic_cast<MonsterSessionObject*>(refRoom.GetBoss().m_sessionObject);
				SERVER_PACKET::BossChangeStateMovePacket sendPacket;
				sendPacket.size = sizeof(SERVER_PACKET::BossChangeStateMovePacket);
				sendPacket.type = SERVER_PACKET::BOSS_CHANGE_STATE_MOVE_DES;
				sendPacket.desPos = bossSession->GetPosition();
				sendPacket.desPos.z -= 5.0f;
				g_logic.BroadCastInRoom(roomId, &sendPacket);
				bossSession->SetDestinationPos(sendPacket.desPos);
			}
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		default: break;
		}
	}
}

int IOCPNetwork::GetUserId()
{
	if (m_currentClientId < MAX_USER) return m_currentClientId++;
	return -1;
}

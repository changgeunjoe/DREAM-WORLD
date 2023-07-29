#include "stdafx.h"
#include "IOCPNetwork.h"
#include "../../Logic/Logic.h"
#include "../../Session/SessionObject/ChracterSessionObject.h"
#include "../../Session/SessionObject/MonsterSessionObject.h"
#include "../../DB/DBObject.h"
#include "../../Room/RoomManager.h"
#include "../../Timer/Timer.h"
#include "../protocol/protocol.h"

extern  Logic		g_logic;
extern	DBObject	g_DBObj;
extern RoomManager	g_RoomManager;
extern Timer		g_Timer;

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
				DisconnectClient(static_cast<int>(key));
				if (ex_over->m_opCode == OP_SEND) {
					delete ex_over;
					ex_over = nullptr;
				}
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
				std::cout << "Accept User[" << userId << "]" << std::endl;
				SOCKADDR_IN* localData = nullptr;
				int localDataSize = sizeof(SOCKADDR_IN);
				SOCKADDR_IN* remoteData = nullptr;
				int remoteDataSize = sizeof(SOCKADDR_IN);
				GetAcceptExSockaddrs(m_acceptOver->m_buffer, 0, addr_size + 16, addr_size + 16, (sockaddr**)&localData, &localDataSize, (sockaddr**)&remoteData, &remoteDataSize);

				char* clientIP = inet_ntoa(remoteData->sin_addr);
				short chlientPort = ntohs(remoteData->sin_port);

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_clientSocket), m_hIocp, userId, 0);
				g_logic.AcceptPlayer(&m_session[userId], userId, m_clientSocket);

				m_session[userId].SetInfoIpAndPort(clientIP, chlientPort);

				m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				//중복 로그인 확인 - logic::ProcessPacket::Login 부분에서 처리함
				ZeroMemory(&m_acceptOver->m_overlap, sizeof(m_acceptOver->m_overlap));
				AcceptEx(m_listenSocket, m_clientSocket, m_acceptOver->m_buffer, 0, addr_size + 16, addr_size + 16, 0, &m_acceptOver->m_overlap);
			}
		}
		break;
		case OP_RECV:
		{
			m_session[key].ConstructPacket(ioByte);
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
			g_RoomManager.BossFindPlayer(key);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_BOSS_STATE:
		{
			g_RoomManager.ChangeBossState(key);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_GAME_STATE_B_SEND:
		{
			g_RoomManager.UpdateGameStateForPlayer(key);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_GAME_STATE_S_SEND:
		{
			g_RoomManager.UpdateGameStateForPlayer(key);
			if (ex_over != nullptr)
				delete ex_over;
		}

		break;
		case OP_BOSS_ATTACK_EXECUTE:
		{
			g_RoomManager.BossAttackExecute(key);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_UPDATE_SMALL_MONSTER:
		{
			g_RoomManager.UpdateSmallMonster(key);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_PLAYER_HEAL:
		{
			g_RoomManager.HealPlayer(key);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_SET_BARRIER:
		{
			g_RoomManager.SetBarrier(key);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_PROJECTILE_ATTACK:
		{
			//std::cout << "projectile Attack Ptr: " << std::hex << ex_over << std::dec << std::endl;
			ex_over->m_opCode = OP_SEND;
			g_logic.BroadCastInRoom(key, ex_over->m_buffer);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_SKY_ARROW_ATTACK:
		{
			g_RoomManager.SkyArrowAttack(key);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_BOSS_CHANGE_DIRECTION:
		{
			g_logic.BroadCastInRoom(key, ex_over->m_buffer);
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		case OP_SYNC_TIME:
		{
			g_logic.BroadCastTimeSyncPacket();
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
	else {
		int restIdx = -1;
		if (m_restClientId.try_pop(restIdx))
			return restIdx;
	}
	return -1;
}

void IOCPNetwork::DisconnectClient(int id)
{
	//만약 인게임 중에 disconnect 된다면.
	if (id < 0)return;
	if (m_session[id].GetPlayerState() == PLAYER_STATE::IN_GAME_ROOM) {
		int roomId = m_session[id].GetRoomId();
		Room& room = g_RoomManager.GetRunningRoomRef(roomId);
		bool isRoomEnd = room.DeleteInGamePlayer(id);
		if (isRoomEnd) {
			g_RoomManager.RoomDestroy(roomId);
		}
	}
	g_logic.DeleteInGameUserSet(m_session[id].GetLoginId());//로직에 있는 인게임 유저 정보 삭제
	g_logic.DeleteInGameUserIdSet(id);
	m_session[id].ResetSession();
	m_restClientId.push(id);
}

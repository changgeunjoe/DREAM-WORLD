#include "stdafx.h"
#include <chrono>
#include "Logic.h"
#include "../Session/Session.h"
#include "../Session/SessionObject/PlayerSessionObject.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../DB/DBObject.h"
#include "../Room/RoomManager.h"
#include "../IOCPNetwork/protocol/protocol.h"


#pragma warning(disable : 4996)

extern IOCPNetwork	g_iocpNetwork;
extern DBObject		g_DBObj;
Logic::Logic()
{
	m_isRunningThread = true;
	m_PlayerMoveThread = std::thread{ [this]() {AutoMoveServer(); } };
	m_roomManager = new RoomManager();
}

Logic::~Logic()
{
	m_isRunningThread = false;
	if (m_PlayerMoveThread.joinable())
		m_PlayerMoveThread.join();
	delete m_roomManager;
}

void Logic::AcceptPlayer(Session* session, int userId, SOCKET& sock)
{
	session->RegistPlayer(userId, sock);
}

void Logic::ProcessPacket(int userId, char* p)
{
	switch (p[1])
	{
	case CLIENT_PACKET::MOVE_KEY_DOWN:
	{
		CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(p);

		SERVER_PACKET::MovePacket sendPacket;
		sendPacket.direction = recvPacket->direction;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::MOVE_KEY_DOWN;
		sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->StartMove(sendPacket.direction); // 움직임 start
#ifdef _DEBUG
		PrintCurrentTime();
		std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::MOVE_KEY_DOWN - MultiCastOtherPlayer" << std::endl;
#endif
		MultiCastOtherPlayer(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::ROTATE:
	{
		CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::RotatePacket*>(p);

		SERVER_PACKET::RotatePacket sendPacket;
		sendPacket.angle = recvPacket->angle;
		sendPacket.axis = recvPacket->axis;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::ROTATE;
		sendPacket.size = sizeof(SERVER_PACKET::RotatePacket);
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->Rotate(recvPacket->axis, recvPacket->angle);
#ifdef _DEBUG
		//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::ROTATE - MultiCastOtherPlayer" << std::endl;
#endif
		MultiCastOtherPlayer(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::MOVE_KEY_UP:
	{
		CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(p);

		SERVER_PACKET::MovePacket sendPacket;
		sendPacket.direction = recvPacket->direction;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::MOVE_KEY_UP;
		sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->ChangeDirection(sendPacket.direction); // 움직임 start
#ifdef _DEBUG
		PrintCurrentTime();
		std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::MOVE_KEY_UP - MultiCastOtherPlayer" << std::endl;
#endif
		MultiCastOtherPlayer(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::STOP:
	{
		CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::StopPacket*>(p);
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->StopMove();

		SERVER_PACKET::StopPacket sendPacket;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::STOP;
		sendPacket.size = sizeof(SERVER_PACKET::StopPacket);
		sendPacket.position = recvPacket->position;
		sendPacket.rotate = recvPacket->rotate;
#ifdef _DEBUG
		PrintCurrentTime();
		std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::STOP - " << std::endl;
		std::cout << "position: " << sendPacket.position.x << ", " << sendPacket.position.y << ", " << sendPacket.position.z << std::endl;
		std::cout << "rotation: " << sendPacket.rotate.x << ", " << sendPacket.rotate.y << ", " << sendPacket.rotate.z << std::endl;
#endif
		bool adjustRes = pSessionObj->AdjustPlayerInfo(recvPacket->position, recvPacket->rotate);
		if (!adjustRes) {
			sendPacket.position = pSessionObj->GetPosition();
			BroadCastPacket(&sendPacket);
#ifdef _DEBUG
			PrintCurrentTime();
			std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::STOP - BroadCastPacket" << std::endl;
#endif
		}
		else {
			MultiCastOtherPlayer(userId, &sendPacket);
#ifdef _DEBUG
			PrintCurrentTime();
			std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::STOP - MultiCastOtherPlayer" << std::endl;
#endif
		}
	}
	break;
	case CLIENT_PACKET::LOGIN:
	{
		CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::LoginPacket*>(p);
		DB_STRUCT::PlayerInfo* pInfo = new DB_STRUCT::PlayerInfo;
		std::string tempId = recvPacket->id;
		pInfo->PlayerLoginId.assign(tempId.begin(), tempId.end());
		std::string tempPw = recvPacket->pw;
		pInfo->pw.assign(tempPw.begin(), tempPw.end());
		DB_EVENT newEvent;
		newEvent.op = DB_OP_GET_PLAYER_INFO;
		newEvent.userId = userId;
		newEvent.Data = pInfo;
		g_DBObj.m_workQueue.push(newEvent);
	}
	break;
	case CLIENT_PACKET::MATCH:
	{
		CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MatchPacket*>(p);
		InsertMatchQueue((ROLE)recvPacket->Role, userId);
	}
	break;
	case CLIENT_PACKET::CREATE_ROOM:
	{
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		CLIENT_PACKET::CreateRoomPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::CreateRoomPacket*>(p);
		std::string roomId;
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_year % 100));
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mon));
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mday));
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_hour));
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_min));
		roomId.append(std::to_string(userId));
		std::string tempName{ recvPacket->roomName };
		std::wstring roomName;
		roomName.assign(tempName.begin(), tempName.end());

		SERVER_PACKET::CreateRoomResultPacket sendPacket;
		if (m_roomManager->InsertRecruitingRoom(roomId, roomName, userId)) {
			pSessionObj->SetRole((ROLE)recvPacket->Role);
			pSessionObj->SetRoomId(roomId);
			sendPacket.type = SERVER_PACKET::CREATE_ROOM_SUCCESS;
		}
		else sendPacket.type = SERVER_PACKET::CREATE_ROOM_FAILURE;
		//send Create Room Success Packet
		sendPacket.size = sizeof(SERVER_PACKET::CreateRoomResultPacket);
		memcpy(sendPacket.roomName, recvPacket->roomName, 30);
		pSessionObj->Send(&sendPacket);
	}
	break;
	case CLIENT_PACKET::REQUEST_ROOM_LIST:
	{
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		CLIENT_PACKET::RequestRoomListPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::RequestRoomListPacket*>(p);
		std::vector<Room> recruitRoom = m_roomManager->GetRecruitingRoomList();
		if (recruitRoom.size() == 0) {
			SERVER_PACKET::NoneRoomInfoPacket sendPacket;
			sendPacket.size = 2;
			sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST_NONE;
			pSessionObj->Send(p);
			return;
		}
		for (auto r = recruitRoom.begin(); r != recruitRoom.end(); r++) {
			SERVER_PACKET::RoomInfoPacket sendPacket;
			sendPacket.size = sizeof(SERVER_PACKET::RoomInfoPacket);
			ZeroMemory(sendPacket.playerName, 80);
			ZeroMemory(sendPacket.role, 4);
			std::set<int> pSet = r->GetPlayerSet();
			int i = 0;
			for (const auto& p : pSet) {
				PlayerSessionObject* partPSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[p].m_sessionObject);
				std::string nameStr;
				nameStr.assign(partPSessionObj->GetName().begin(), partPSessionObj->GetName().end());
				memcpy(sendPacket.playerName[i], nameStr.c_str(), nameStr.size());
				sendPacket.playerName[i][nameStr.size()] = 0;
				sendPacket.role[i] = partPSessionObj->GetRole();
			}
			//Name
			std::wstring tempWstring{ r->GetRoomName() };
			std::string roomNameStr;
			roomNameStr.assign(tempWstring.begin(), tempWstring.end());
			strcpy(sendPacket.roomName, roomNameStr.c_str());
			sendPacket.roomName[roomNameStr.size()] = 0;
			strcpy(sendPacket.roomId, r->GetRoomId().c_str());
			sendPacket.roomId[r->GetRoomId().size()] = 0;
			if (r == recruitRoom.end() - 1)
				sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST_END;
			else
				sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST;
			pSessionObj->Send(&sendPacket);
		}
	}
	break;
	case CLIENT_PACKET::PLAYER_APPLY_ROOM:
	{
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		CLIENT_PACKET::PlayerApplyRoomPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::PlayerApplyRoomPacket*>(p);
		recvPacket->roomId;
		recvPacket->role;
		m_roomManager->GetRecruitingRoom(recvPacket->roomId);
	}
	break;
	default:
		PrintCurrentTime();
		std::cout << "unknown Packet" << std::endl;
		break;
	}
}

void Logic::BroadCastPacket(void* p)
{
	for (auto& cli : g_iocpNetwork.m_session) {
		if (cli.m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::MultiCastOtherPlayer(int userId, void* p)
{
	for (auto& cli : g_iocpNetwork.m_session) {
		if (cli.GetId() == userId) continue;//자기 자신을 제외한 플레이어들에게 전송
		if (cli.GetId() > MAX_USER) break;
		if (cli.m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::MultiCastOtherPlayerInRoom(int userId, void* p)
{
	PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
	auto roomPlayerSet = m_roomManager->GetRecruitingRoom(pSessionObj->GetRoomId()).GetPlayerSet();
	for (auto& cli : roomPlayerSet) {
		if (cli == userId) continue;//자기 자신을 제외한 플레이어들에게 전송
		if (g_iocpNetwork.m_session[cli].m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[cli].m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::BroadCastOtherPlayerInRoom(int userId, void* p)
{
	PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
	auto roomPlayerSet = m_roomManager->GetRecruitingRoom(pSessionObj->GetRoomId()).GetPlayerSet();
	for (auto& cli : roomPlayerSet) {
		if (g_iocpNetwork.m_session[cli].m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[cli].m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::AutoMoveServer()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	while (m_isRunningThread)
	{
		if (g_iocpNetwork.GetCurrentId() == 0) continue;
		currentTime = std::chrono::high_resolution_clock::now();
		for (auto& cli : g_iocpNetwork.m_session) {
			if (cli.GetId() > MAX_USER) break;
			if (cli.GetPlayerState() == PLAYER_STATE::FREE) continue;
			if (cli.m_sessionObject->m_inputDirection != DIRECTION::IDLE) {
				PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
				pSessionObj->AutoMove();
			}
		}
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count() < 1000.0f / 60.0f) {
		}
	}
}

void Logic::MatchMaking()
{
	std::vector<ROLE> restRole;
	for (int i = 0; i < 4; i++) {
		if (!(m_MatchRole & (char)pow(2, i))) {
			restRole.push_back((ROLE)pow(2, i));
		}
	}
	if (restRole.size() == 0) {
		// All Role exist
		//Matching
	}
	if (restRole.size() < randPlayerIdQueue.unsafe_size()) {
		//matching
		//Rand
	}
}

void Logic::InsertMatchQueue(ROLE r, int userId)
{
	if (r != ROLE::RAND && (m_MatchRole.load() ^ r) && !(m_MatchRole.load() & r)) { // &연산시 0이 나와야 현재 비어있는 칸에 넣을 수 있고, ^때 1이 나오면 됨
		m_MatchRole |= r;
		m_matchPlayerSet.insert(userId);
		MatchMaking();
		return;
	}
	switch (r)
	{
	case NONE_SELECT:
		break;
	case WARRIOR:
		warriorPlayerIdQueue.push(userId);
		break;
	case PRIEST:
		priestPlayerIdQueue.push(userId);
		break;
	case TANKER:
		tankerPlayerIdQueue.push(userId);
		break;
	case ARCHER:
		archerPlayerIdQueue.push(userId);
		break;
	case RAND:
		randPlayerIdQueue.push(userId);
		MatchMaking();
		break;
	default:
		break;
	}
}

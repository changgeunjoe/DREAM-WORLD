#include "stdafx.h"
#include "Logic.h"
#include "../Session/Session.h"
#include "../Session/SessionObject/PlayerSessionObject.h"
#include "../Session/SessionObject/MonsterSessionObject.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../DB/DBObject.h"
#include "../IOCPNetwork/protocol/protocol.h"
#include "../Room/RoomManager.h"

#include <random>

#define ALONE_TEST 1

#pragma warning(disable : 4996)

extern IOCPNetwork	g_iocpNetwork;
extern DBObject		g_DBObj;
extern RoomManager	g_RoomManager;
Logic::Logic()
{
	m_isRunningThread = true;
	m_PlayerMoveThread = std::thread{ [this]() {AutoMoveServer(); } };
#ifndef ALONE_TEST
	m_MatchingThread = std::thread{ [this]() {MatchMaking(); } };
#endif

}

Logic::~Logic()
{
	m_isRunningThread = false;
	if (m_PlayerMoveThread.joinable())
		m_PlayerMoveThread.join();
	if (m_MatchingThread.joinable())
		m_MatchingThread.join();
}

void Logic::AcceptPlayer(Session* session, int userId, SOCKET& sock)
{
	session->RegistPlayer(userId, sock);
}

void Logic::ProcessPacket(int userId, char* p)
{
	switch (p[2])
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
		//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::MOVE_KEY_DOWN - MultiCastOtherPlayer" << std::endl;		
#endif
		MultiCastOtherPlayerInRoom(userId, &sendPacket);
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
		MultiCastOtherPlayerInRoom(userId, &sendPacket);
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
		MultiCastOtherPlayerInRoom(userId, &sendPacket);
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
		//sendPacket.rotate = recvPacket->rotate;
#ifdef _DEBUG
		PrintCurrentTime();
		//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::STOP - " << std::endl;
		//std::cout << "position: " << sendPacket.position.x << ", " << sendPacket.position.y << ", " << sendPacket.position.z << std::endl;
		//std::cout << "rotation: " << sendPacket.rotate.x << ", " << sendPacket.rotate.y << ", " << sendPacket.rotate.z << std::endl;
#endif
		bool adjustRes = pSessionObj->AdjustPlayerInfo(recvPacket->position); // , recvPacket->rotate
		if (!adjustRes) {
			sendPacket.position = pSessionObj->GetPosition();
			BroadCastOtherPlayerInRoom(userId, &sendPacket);
#ifdef _DEBUG
			PrintCurrentTime();
			std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::STOP - BroadCastPacket" << std::endl;
#endif
		}
		else {
			MultiCastOtherPlayerInRoom(userId, &sendPacket);
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

		/////////////////////////////////////////////////////////////////
		//여기다가 동접 테스트 하기위한 객체 생성하기 **
	}
	break;
	case CLIENT_PACKET::MATCH:
	{
#ifdef ALONE_TEST
		CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MatchPacket*>(p);
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->SetRole((ROLE)recvPacket->Role);
		char* sendAddPlayerPacket = pSessionObj->GetPlayerInfo();
		pSessionObj->Send(sendAddPlayerPacket);
		delete sendAddPlayerPacket;

		SERVER_PACKET::NotifyPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		sendPacket.type = SERVER_PACKET::INTO_GAME;
		pSessionObj->Send(&sendPacket);
#else
		CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MatchPacket*>(p);
		InsertMatchQueue((ROLE)recvPacket->Role, userId);
#endif // ALONE_TEST

		//매치 큐 걸어놓고 끝
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
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mon + 1));
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mday));
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_hour));
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_min));
		roomId.append(std::to_string(std::localtime(&in_time_t)->tm_sec));
		roomId.append(std::to_string(userId));

		std::wstring roomName{ recvPacket->roomName };

		SERVER_PACKET::CreateRoomResultPacket sendPacket;
		if (g_RoomManager.InsertRecruitingRoom(roomId, roomName, userId, (ROLE)recvPacket->Role)) {
			g_RoomManager.m_RecruitingRoomList[roomId].InsertInGamePlayer((ROLE)recvPacket->Role, userId);
			pSessionObj->SetRoomId(roomId);
			sendPacket.type = SERVER_PACKET::CREATE_ROOM_SUCCESS;
			//룸 생성 성공
		}
		else sendPacket.type = SERVER_PACKET::CREATE_ROOM_FAILURE;
		////////////////////////////////////////////////////////////////////////////
		sendPacket.size = sizeof(SERVER_PACKET::CreateRoomResultPacket);
		memcpy(sendPacket.roomName, recvPacket->roomName, 30);
		pSessionObj->Send(&sendPacket);
	}
	break;
	case CLIENT_PACKET::REQUEST_ROOM_LIST:
	{
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		CLIENT_PACKET::RequestRoomListPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::RequestRoomListPacket*>(p);
		std::vector<Room> recruitRoom = g_RoomManager.GetRecruitingRoomList();
		if (recruitRoom.size() == 0) {
			SERVER_PACKET::NotifyPacket sendPacket;
			sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
			sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST_NONE;
			pSessionObj->Send(&sendPacket);
			return;
		}
		for (auto r = recruitRoom.begin(); r != recruitRoom.end(); r++) {
			SERVER_PACKET::RoomInfoPacket sendPacket;
			sendPacket.size = sizeof(SERVER_PACKET::RoomInfoPacket);
			::ZeroMemory(sendPacket.playerName, sizeof(sendPacket.playerName));
			::ZeroMemory(sendPacket.role, 4);
			std::map<ROLE, int> pMap = r->GetInGamePlayerMap();
			int i = 0;
			for (const auto& p : pMap) {
				PlayerSessionObject* partPSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[p.second].m_sessionObject);
				memcpy(sendPacket.playerName[i], partPSessionObj->GetName().c_str(), partPSessionObj->GetName().size() * 2);
				sendPacket.playerName[i][partPSessionObj->GetName().size()] = 0;
				sendPacket.role[i] = partPSessionObj->GetRole();
			}
			//Name
			memcpy(sendPacket.roomName, r->GetRoomName().c_str(), r->GetRoomName().size() * 2);
			sendPacket.roomName[r->GetRoomName().size()] = 0;
			memcpy(sendPacket.roomId, r->GetRoomId().c_str(), r->GetRoomId().size());
			sendPacket.roomId[r->GetRoomId().size()] = 0;
			if (r == recruitRoom.end() - 1)
				sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST_END;
			else
				sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST;
			std::cout << "ProcessPacket() - CLIENT_PACKET::REQUEST_ROOM_LIST sendByte: " << sendPacket.size << std::endl;
			pSessionObj->Send(&sendPacket);
		}
	}
	break;
	case CLIENT_PACKET::PLAYER_APPLY_ROOM:
	{
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		CLIENT_PACKET::PlayerApplyRoomPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::PlayerApplyRoomPacket*>(p);
		g_RoomManager.m_RecruitRoomListLock.lock();
		if (g_RoomManager.m_RecruitingRoomList.count(recvPacket->roomId)) {
			g_RoomManager.m_RecruitingRoomList[recvPacket->roomId].InsertWaitPlayer((ROLE)recvPacket->role, userId);
			int roomOwner = g_RoomManager.m_RecruitingRoomList[recvPacket->roomId].roomOwner();
			g_RoomManager.m_RecruitRoomListLock.unlock();
			//PLAYER_APPLY_ROOM
			//방장한테 보내는 패킷 - 신청자 존재함을 알리는 패킷

			SERVER_PACKET::PlayerApplyRoomPacket sendPacket;
			memcpy(sendPacket.name, pSessionObj->GetName().c_str(), pSessionObj->GetName().size());
			sendPacket.name[pSessionObj->GetName().size()] = 0;
			sendPacket.role = recvPacket->role;
			sendPacket.size = sizeof(SERVER_PACKET::PlayerApplyRoomPacket);
			sendPacket.type = SERVER_PACKET::PLAYER_APPLY_ROOM;

			////////////////////////////////////////////////////////////////////////////////////////////
			PlayerSessionObject* sendPSession = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[roomOwner].m_sessionObject);
			sendPSession->Send(&sendPacket);
			return;
		}
		else {
			g_RoomManager.m_RecruitRoomListLock.unlock();
			//방이 사라짐(하필이면)
			//반송 패킷
			SERVER_PACKET::NotifyPacket sendPacket;
			sendPacket.type = SERVER_PACKET::NOT_FOUND_ROOM;
			sendPacket.size = 2;
			pSessionObj->Send(&sendPacket);
		}
	}
	break;
	case CLIENT_PACKET::CANCEL_APPLY_ROOM://클라이언트가 방 신청을 취소한 경우
	{
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		CLIENT_PACKET::PlayerCancelRoomPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::PlayerCancelRoomPacket*>(p);
		g_RoomManager.m_RecruitRoomListLock.lock();
		if (g_RoomManager.m_RecruitingRoomList.count(recvPacket->roomId)) {
			g_RoomManager.m_RecruitingRoomList[recvPacket->roomId].DeleteWaitPlayer(userId);
		}
		g_RoomManager.m_RecruitRoomListLock.unlock();
		//방에 있는 사람들에게 취소한 신청자 있음을 전송
		//multicast by people in room
	}
	break;
	case CLIENT_PACKET::MOUSE_INPUT:
	{
		CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MouseInputPacket*>(p);

		SERVER_PACKET::MouseInputPacket sendPacket;
		sendPacket.ClickedButton = recvPacket->ClickedButton;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::MOUSE_INPUT;
		sendPacket.size = sizeof(SERVER_PACKET::MouseInputPacket);

		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->SetMouseInput(sendPacket.ClickedButton);
#ifdef _DEBUG
		PrintCurrentTime();
		std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::MOUSE_INPUT - MultiCastOtherPlayer" << std::endl;
#endif
		MultiCastOtherPlayerInRoom(userId, &sendPacket);
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
	if (!g_RoomManager.IsExistRunningRoom(pSessionObj->GetRoomId())) return;
	auto roomPlayermap = g_RoomManager.GetRunningRoom(pSessionObj->GetRoomId()).GetInGamePlayerMap();
	for (auto& cli : roomPlayermap) {
		if (cli.second == userId) continue;//자기 자신을 제외한 플레이어들에게 전송
		if (g_iocpNetwork.m_session[cli.second].m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[cli.second].m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::BroadCastOtherPlayerInRoom(int userId, void* p)
{
	PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
	if (!g_RoomManager.IsExistRunningRoom(pSessionObj->GetRoomId())) return;
	auto roomPlayermap = g_RoomManager.GetRunningRoom(pSessionObj->GetRoomId()).GetInGamePlayerMap();
	for (auto& cli : roomPlayermap) {
		if (g_iocpNetwork.m_session[cli.second].m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[cli.second].m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::BroadCastInRoom(std::string& roomId, void* p)
{
	if (!g_RoomManager.IsExistRunningRoom(roomId)) return;
	auto roomPlayermap = g_RoomManager.GetRunningRoom(roomId).GetInGamePlayerMap();
	for (auto& cli : roomPlayermap) {
		if (g_iocpNetwork.m_session[cli.second].m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[cli.second].m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::AutoMoveServer()//2500명?
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	while (m_isRunningThread)
	{
		if (g_iocpNetwork.GetCurrentId() == 0) continue;
		currentTime = std::chrono::high_resolution_clock::now();
		for (auto& cli : g_iocpNetwork.m_session) {//플레이어 자동 움직임
			if (cli.GetId() > MAX_USER) break;
			if (cli.GetPlayerState() == PLAYER_STATE::FREE) continue;
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
			if (pSessionObj->m_inputDirection != DIRECTION::IDLE) {
				pSessionObj->AutoMove();
			}
		}
		auto RunningRooms = g_RoomManager.GetRunningRoomList();
		for (auto& room : RunningRooms) {
			MonsterSessionObject* mSessionObj = dynamic_cast<MonsterSessionObject*>(room.GetBoss().m_sessionObject);
			if (mSessionObj->GetRoomId() == room.GetRoomId() && mSessionObj->isMove) {
				mSessionObj->AutoMove();
			}
		}
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count() < 1000.0f / 60.0f) {
		}
	}
}

void Logic::MatchMaking()
{
	while (true) {
		std::map<ROLE, int> matchPlayer;
		std::vector<ROLE> restRole;
		char sumRole = 0;
		{
			//match Player Lock Guard
			std::lock_guard<std::mutex> lg{ m_matchPlayerLock };
			matchPlayer = m_matchPlayer;
		}
		for (const auto& a : matchPlayer) {
			sumRole |= a.first;
		}
		for (int i = 0; i < 4; i++) {
			if (!(sumRole & (char)pow(2, i))) {
				restRole.push_back((ROLE)pow(2, i));
			}
		}

		//아무도 없을 때, 
		if (restRole.size() == 4) {
			if (randPlayerIdQueue.unsafe_size() >= 4) {
				std::map<ROLE, int> randMatchPlayer;
				for (int i = 0; i < 4; i++) {
					int randUserId = -1;
					if (randPlayerIdQueue.try_pop(randUserId)) {
						ROLE r = (ROLE)pow(2, i);
						randMatchPlayer.emplace(r, randUserId); // 어쨌든 랜덤처럼 보이지 않을까 싶음
					}
				}
				SERVER_PACKET::NotifyPacket sendPacket;
				sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
				sendPacket.type = SERVER_PACKET::INTO_GAME;

				std::string roomId;
				auto now = std::chrono::system_clock::now();
				auto in_time_t = std::chrono::system_clock::to_time_t(now);
				roomId.append(std::to_string(std::localtime(&in_time_t)->tm_year % 100));
				roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mon + 1));
				roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mday));
				roomId.append(std::to_string(std::localtime(&in_time_t)->tm_hour));
				roomId.append(std::to_string(std::localtime(&in_time_t)->tm_min));
				roomId.append(std::to_string(std::localtime(&in_time_t)->tm_sec));
				roomId.append("Matching_");

				std::wstring roomName{ L"RandMatchingRoom" };
				roomName.append(std::to_wstring(matchPlayer.begin()->second));
				while (!g_RoomManager.InsertRunningRoom(roomId, roomName, randMatchPlayer));
				//룸 생성 성공
				for (const auto& p : matchPlayer) {
					PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[p.second].m_sessionObject);
					pSessionObj->SetRoomId(roomId);
				}

				for (const auto& p : randMatchPlayer) {
					//send match Success Packet
					PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[p.second].m_sessionObject);
					pSessionObj->SetRole(p.first);
					char* sendAddPlayerPacket = pSessionObj->GetPlayerInfo();
					pSessionObj->Send(sendAddPlayerPacket);
					BroadCastOtherPlayerInRoom(p.second, sendAddPlayerPacket);
					delete sendAddPlayerPacket;
				}
				BroadCastOtherPlayerInRoom(matchPlayer.begin()->second, &sendPacket);
			}
		}

		//모두가 Role을 가지고 돌렸을때
		else if (restRole.size() == 0) {
			SERVER_PACKET::NotifyPacket sendPacket;
			sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
			sendPacket.type = SERVER_PACKET::INTO_GAME;

			std::string roomId;
			auto now = std::chrono::system_clock::now();
			auto in_time_t = std::chrono::system_clock::to_time_t(now);
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_year % 100));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mon + 1));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mday));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_hour));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_min));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_sec));
			roomId.append("Matching_");
			roomId.append(std::to_string(matchPlayer.begin()->second));

			std::wstring roomName{ L"RandMatchingRoom" };
			roomName.append(std::to_wstring(matchPlayer.begin()->second)); //
			while (!g_RoomManager.InsertRunningRoom(roomId, roomName, matchPlayer));
			//룸 생성 성공
			for (const auto& p : matchPlayer) {
				PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[p.second].m_sessionObject);
				pSessionObj->SetRoomId(roomId);
			}
			for (const auto& p : matchPlayer) {
				//send match Success Packet
				PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[p.second].m_sessionObject);
				pSessionObj->SetRole(p.first);
				char* sendAddPlayerPacket = pSessionObj->GetPlayerInfo();
				BroadCastOtherPlayerInRoom(p.second, sendAddPlayerPacket);
				delete sendAddPlayerPacket;
			}
			BroadCastOtherPlayerInRoom(matchPlayer.begin()->second, &sendPacket);
			{
				std::lock_guard<std::mutex> lg{ m_matchPlayerLock };
				m_matchPlayer.clear();
			}
		}
		//일부만 Role가지고 나머지는 Rand가 존재할때
		else if (restRole.size() <= randPlayerIdQueue.unsafe_size()) {
			//없는 역할 넣어주기 추가하자
			for (int i = 0; i < restRole.size(); i++) {
				int randUserId = -1;
				if (randPlayerIdQueue.try_pop(randUserId)) {
					ROLE r = restRole[i];
					matchPlayer.emplace((ROLE)r, randUserId);
					PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[randUserId].m_sessionObject);
					pSessionObj->SetRole((ROLE)r);
					//
					//m_matchPlayer에 안넣었음
					//pop success
					matchPlayer.emplace(r, randUserId);
				}
				else {
					//error
				}
			}
			SERVER_PACKET::NotifyPacket sendPacket;
			sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
			sendPacket.type = SERVER_PACKET::INTO_GAME;

			std::string roomId;
			auto now = std::chrono::system_clock::now();
			auto in_time_t = std::chrono::system_clock::to_time_t(now);
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_year % 100));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mon + 1));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mday));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_hour));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_min));
			roomId.append(std::to_string(std::localtime(&in_time_t)->tm_sec));
			roomId.append("Matching_");

			std::wstring roomName{ L"RandMatchingRoom" };
			roomName.append(std::to_wstring(matchPlayer.begin()->second));
			while (!g_RoomManager.InsertRunningRoom(roomId, roomName, matchPlayer));
			//룸 생성 성공
			for (const auto& p : matchPlayer) {
				PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[p.second].m_sessionObject);
				pSessionObj->SetRoomId(roomId);
			}

			for (const auto& p : matchPlayer) {
				PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[p.second].m_sessionObject);
				pSessionObj->SetRole(p.first);
				char* sendAddPlayerPacket = pSessionObj->GetPlayerInfo();
				BroadCastOtherPlayerInRoom(p.second, sendAddPlayerPacket);
				delete sendAddPlayerPacket;
			}
			BroadCastOtherPlayerInRoom(matchPlayer.begin()->second, &sendPacket);
			{
				std::lock_guard<std::mutex> lg{ m_matchPlayerLock };
				m_matchPlayer.clear();
			}
		}
	}
}

void Logic::InsertMatchQueue(ROLE r, int userId)
{
	if (r != ROLE::RAND && (m_MatchRole.load() ^ r) && !(m_MatchRole.load() & r)) { // &연산시 0이 나와야 현재 비어있는 칸에 넣을 수 있고, ^때 1이 나오면 됨
		m_MatchRole |= r;
		{
			std::lock_guard<std::mutex> lg{ m_matchPlayerLock };
			m_matchPlayer.emplace(r, userId);
		}
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
		break;
	default:
		break;
	}
}

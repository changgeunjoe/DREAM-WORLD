#include "stdafx.h"
#include "Logic.h"
#include "../Session/UserSession.h"
#include "../Session/SessionObject/ChracterSessionObject.h"
#include "../Session/SessionObject/MonsterSessionObject.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../DB/DBObject.h"
#include "../IOCPNetwork/protocol/protocol.h"
#include "../Room/RoomManager.h"

#include <random>

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

void Logic::AcceptPlayer(UserSession* session, int userId, SOCKET& sock)
{
	session->RegistPlayer(sock, userId);
}

void Logic::ProcessPacket(int userId, char* p)
{
	switch (p[2])
	{
	case CLIENT_PACKET::MOVE_KEY_DOWN:
	{
		if (g_iocpNetwork.m_session[userId].GetPlayerState() == IN_GAME_ROOM) {
			CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(p);
			SERVER_PACKET::MovePacket sendPacket;
			sendPacket.direction = recvPacket->direction;
			sendPacket.userId = userId;
			sendPacket.type = SERVER_PACKET::MOVE_KEY_DOWN;
			sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
			g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).
				StartMovePlayCharacter(g_iocpNetwork.m_session[userId].GetRole(), sendPacket.direction); // ������ start;
#ifdef _DEBUG
			//PrintCurrentTime();
			//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::MOVE_KEY_DOWN - MultiCastOtherPlayer" << std::endl;
#endif
			MultiCastOtherPlayerInRoom(userId, &sendPacket);
		}
	}
	break;
	case CLIENT_PACKET::ROTATE:
	{
		if (g_iocpNetwork.m_session[userId].GetPlayerState() == IN_GAME_ROOM) {
			CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::RotatePacket*>(p);

			SERVER_PACKET::RotatePacket sendPacket;
			sendPacket.angle = recvPacket->angle;
			sendPacket.axis = recvPacket->axis;
			sendPacket.userId = userId;
			sendPacket.type = SERVER_PACKET::ROTATE;
			sendPacket.size = sizeof(SERVER_PACKET::RotatePacket);
			g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).
				RotatePlayCharacter(g_iocpNetwork.m_session[userId].GetRole(), recvPacket->axis, recvPacket->angle);
#ifdef _DEBUG
			//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::ROTATE - MultiCastOtherPlayer" << std::endl;
#endif
			MultiCastOtherPlayerInRoom(userId, &sendPacket);
		}
	}
	break;
	case CLIENT_PACKET::MOVE_KEY_UP:
	{
		if (g_iocpNetwork.m_session[userId].GetPlayerState() == IN_GAME_ROOM) {
			CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(p);

			SERVER_PACKET::MovePacket sendPacket;
			sendPacket.direction = recvPacket->direction;
			sendPacket.userId = userId;
			sendPacket.type = SERVER_PACKET::MOVE_KEY_UP;
			sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
			//room �Լ��� ����?
			g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).
				ChangeDirectionPlayCharacter(g_iocpNetwork.m_session[userId].GetRole(), sendPacket.direction);
#ifdef _DEBUG
			//PrintCurrentTime();
			//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::MOVE_KEY_UP - MultiCastOtherPlayer" << std::endl;
#endif
			MultiCastOtherPlayerInRoom(userId, &sendPacket);
		}
	}
	break;
	case CLIENT_PACKET::STOP:
	{
		if (g_iocpNetwork.m_session[userId].GetPlayerState() == IN_GAME_ROOM) {
			CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::StopPacket*>(p);
			g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).
				StopMovePlayCharacter(g_iocpNetwork.m_session[userId].GetRole());

			SERVER_PACKET::StopPacket sendPacket;
			sendPacket.userId = userId;
			sendPacket.type = SERVER_PACKET::STOP;
			sendPacket.size = sizeof(SERVER_PACKET::StopPacket);
			sendPacket.position = recvPacket->position;
			//sendPacket.rotate = recvPacket->rotate;
#ifdef _DEBUG
			//PrintCurrentTime();
			//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::STOP - " << std::endl;
			//std::cout << "position: " << sendPacket.position.x << ", " << sendPacket.position.y << ", " << sendPacket.position.z << std::endl;
			//std::cout << "rotation: " << sendPacket.rotate.x << ", " << sendPacket.rotate.y << ", " << sendPacket.rotate.z << std::endl;
#endif
			Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
			bool adjustRes = roomRef.AdjustPlayCharacterInfo(g_iocpNetwork.m_session[userId].GetRole(), recvPacket->position);
			if (!adjustRes) {
				sendPacket.position = roomRef.GetPositionPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
				BroadCastInRoomByPlayer(userId, &sendPacket);
#ifdef _DEBUG
				//	PrintCurrentTime();
				//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::STOP - BroadCastPacket" << std::endl;
#endif
			}
			else {
				MultiCastOtherPlayerInRoom(userId, &sendPacket);
#ifdef _DEBUG
				//PrintCurrentTime();
				//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::STOP - MultiCastOtherPlayer" << std::endl;
#endif
			}
		}
	}
	break;
	case CLIENT_PACKET::LOGIN:
	{
		CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::LoginPacket*>(p);
		DB_STRUCT::PlayerInfo* pInfo = new DB_STRUCT::PlayerInfo(recvPacket->id, recvPacket->pw);
		//std::string tempId = recvPacket->id;
		std::map<std::wstring, int>::iterator playerMapFindRes = m_inGameUser.end();
		{
			std::lock_guard<std::mutex>ll{ m_inGameUserLock };
			playerMapFindRes = m_inGameUser.find(pInfo->PlayerLoginId);
		}
		if (playerMapFindRes != m_inGameUser.end()) {//�̹� �÷��̾ ����
			//�� �� ���� �����ϰ� �����

			//�̹� �����ϱ⶧���� ���� ���� �÷��̾� ���� ���� ��Ŷ ����
			SERVER_PACKET::NotifyPacket preExistPacket;
			preExistPacket.type = SERVER_PACKET::PRE_EXIST_LOGIN;
			preExistPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
			g_iocpNetwork.m_session[userId].Send(&preExistPacket);//pre_exist notify


			//�̹� �����ϴ� �÷��̾�� �ߺ��α��� ��Ŷ ����
			SERVER_PACKET::NotifyPacket duplicateLoginPacket;
			duplicateLoginPacket.type = SERVER_PACKET::DUPLICATED_LOGIN;
			duplicateLoginPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
			g_iocpNetwork.m_session[playerMapFindRes->second].Send(&duplicateLoginPacket);//duplicate notify

			DeleteInGameUserMap(playerMapFindRes->first);
			g_iocpNetwork.DisconnectClient(playerMapFindRes->second);

			//{
			//	//���� ���� ��Ų ��, map���� ����
			//	std::lock_guard<std::mutex>ll{ m_inGameUserLock };
			//	m_inGameUser.erase(playerMapFindRes->first);
			//}
		}
		else {
			/*pInfo->PlayerLoginId.assign(tempId.begin(), tempId.end());
			std::string tempPw = recvPacket->pw;
			pInfo->pw.assign(tempPw.begin(), tempPw.end());*/
			DB_EVENT newEvent;
			newEvent.op = DB_OP_GET_PLAYER_INFO;
			newEvent.userId = userId;
			newEvent.Data = pInfo;
			g_DBObj.m_workQueue.push(newEvent);
		}
		/////////////////////////////////////////////////////////////////
		//����ٰ� ���� �׽�Ʈ �ϱ����� ��ü �����ϱ� **
	}
	break;
	case CLIENT_PACKET::MATCH:
	{
#ifdef ALONE_TEST
		CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MatchPacket*>(p);
		std::map<ROLE, int> alonePlayerMap;
		alonePlayerMap.insert(std::make_pair((ROLE)recvPacket->Role, userId));
		int newRoomId = g_RoomManager.GetRoomId();//���ο� �� ������Ʈ ��������
		Room& roomRef = g_RoomManager.GetRunningRoomRef(newRoomId);
		roomRef.InsertInGamePlayer(alonePlayerMap);
		roomRef.GameStart();
		g_iocpNetwork.m_session[userId].SetRole((ROLE)recvPacket->Role);
		g_iocpNetwork.m_session[userId].SetRoomId(newRoomId);
		roomRef.SendAllPlayerInfo();

		SERVER_PACKET::NotifyPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		sendPacket.type = SERVER_PACKET::INTO_GAME;
		g_iocpNetwork.m_session[userId].Send(&sendPacket);
#else
		std::cout << "match: " << userId << std::endl;
		CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MatchPacket*>(p);
		InsertMatchQueue((ROLE)recvPacket->Role, userId);
#endif // ALONE_TEST

		//��ġ ť �ɾ���� ��
	}
	break;
	//���� �游��� �� �ٸ� ��� ����
	//case CLIENT_PACKET::CREATE_ROOM:
	//{
	//	CLIENT_PACKET::CreateRoomPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::CreateRoomPacket*>(p);
	//	std::string roomId;
	//	auto now = std::chrono::system_clock::now();
	//	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_year % 100));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mon + 1));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mday));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_hour));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_min));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_sec));
	//	roomId.append(std::to_string(userId));
	//
	//	std::wstring roomName{ recvPacket->roomName };
	//
	//	SERVER_PACKET::CreateRoomResultPacket sendPacket;
	//	if (g_RoomManager.InsertRecruitingRoom(roomId, roomName, userId, (ROLE)recvPacket->Role)) {
	//		g_RoomManager.m_RecruitingRoomList[roomId].InsertInGamePlayer((ROLE)recvPacket->Role, userId);
	//		g_iocpNetwork.m_session[userId].SetRoomId(roomId);
	//		sendPacket.type = SERVER_PACKET::CREATE_ROOM_SUCCESS;
	//		//�� ���� ����
	//	}
	//	else sendPacket.type = SERVER_PACKET::CREATE_ROOM_FAILURE;
	//	////////////////////////////////////////////////////////////////////////////
	//	sendPacket.size = sizeof(SERVER_PACKET::CreateRoomResultPacket);
	//	memcpy(sendPacket.roomName, recvPacket->roomName, 30);
	//	g_iocpNetwork.m_session[userId].Send(&sendPacket);
	//}
	//break;
	//case CLIENT_PACKET::REQUEST_ROOM_LIST:
	//{
	//	ChracterSessionObject* pSessionObj = dynamic_cast<ChracterSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
	//	CLIENT_PACKET::RequestRoomListPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::RequestRoomListPacket*>(p);
	//	std::vector<Room> recruitRoom = g_RoomManager.GetRecruitingRoomList();
	//	if (recruitRoom.size() == 0) {
	//		SERVER_PACKET::NotifyPacket sendPacket;
	//		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	//		sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST_NONE;
	//		g_iocpNetwork.m_session[userId].Send(&sendPacket);
	//		return;
	//	}
	//	for (auto r = recruitRoom.begin(); r != recruitRoom.end(); r++) {
	//		SERVER_PACKET::RoomInfoPacket sendPacket;
	//		sendPacket.size = sizeof(SERVER_PACKET::RoomInfoPacket);
	//		::ZeroMemory(sendPacket.playerName, sizeof(sendPacket.playerName));
	//		::ZeroMemory(sendPacket.role, 4);
	//		std::map<ROLE, int> pMap = r->GetInGamePlayerMap();
	//		int i = 0;
	//		for (const auto& p : pMap) {
	//			memcpy(sendPacket.playerName[i], g_iocpNetwork.m_session[p.second].GetName().c_str(), g_iocpNetwork.m_session[p.second].GetName().size() * 2);
	//			sendPacket.playerName[i][g_iocpNetwork.m_session[p.second].GetName().size()] = 0;
	//			sendPacket.role[i] = g_iocpNetwork.m_session[p.second].m_sessionObject->GetRole();
	//		}
	//		//Name
	//		memcpy(sendPacket.roomName, r->GetRoomName().c_str(), r->GetRoomName().size() * 2);
	//		sendPacket.roomName[r->GetRoomName().size()] = 0;
	//		memcpy(sendPacket.roomId, r->GetRoomId().c_str(), r->GetRoomId().size());
	//		sendPacket.roomId[r->GetRoomId().size()] = 0;
	//		if (r == recruitRoom.end() - 1)
	//			sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST_END;
	//		else
	//			sendPacket.type = SERVER_PACKET::REQUEST_ROOM_LIST;
	//		std::cout << "ProcessPacket() - CLIENT_PACKET::REQUEST_ROOM_LIST sendByte: " << sendPacket.size << std::endl;
	//		g_iocpNetwork.m_session[userId].Send(&sendPacket);
	//	}
	//}
	//break;
	//case CLIENT_PACKET::PLAYER_APPLY_ROOM:
	//{
	//	CLIENT_PACKET::PlayerApplyRoomPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::PlayerApplyRoomPacket*>(p);
	//	g_RoomManager.m_RecruitRoomListLock.lock();
	//	if (g_RoomManager.m_RecruitingRoomList.count(recvPacket->roomId)) {
	//		g_RoomManager.m_RecruitingRoomList[recvPacket->roomId].InsertWaitPlayer((ROLE)recvPacket->role, userId);
	//		int roomOwner = g_RoomManager.m_RecruitingRoomList[recvPacket->roomId].roomOwner();
	//		g_RoomManager.m_RecruitRoomListLock.unlock();
	//		//PLAYER_APPLY_ROOM
	//		//�������� ������ ��Ŷ - ��û�� �������� �˸��� ��Ŷ
	//
	//		SERVER_PACKET::PlayerApplyRoomPacket sendPacket;
	//		memcpy(sendPacket.name, g_iocpNetwork.m_session[userId].GetName().c_str(), 2 * g_iocpNetwork.m_session[userId].GetName().size());
	//		sendPacket.name[g_iocpNetwork.m_session[userId].GetName().size()] = 0;
	//		sendPacket.role = recvPacket->role;
	//		sendPacket.size = sizeof(SERVER_PACKET::PlayerApplyRoomPacket);
	//		sendPacket.type = SERVER_PACKET::PLAYER_APPLY_ROOM;
	//
	//		////////////////////////////////////////////////////////////////////////////////////////////
	//		ChracterSessionObject* sendPSession = dynamic_cast<ChracterSessionObject*>(g_iocpNetwork.m_session[roomOwner].m_sessionObject);
	//		g_iocpNetwork.m_session[roomOwner].Send(&sendPacket);
	//		return;
	//	}
	//	else {
	//		g_RoomManager.m_RecruitRoomListLock.unlock();
	//		//���� �����(�����̸�)
	//		//�ݼ� ��Ŷ
	//		SERVER_PACKET::NotifyPacket sendPacket;
	//		sendPacket.type = SERVER_PACKET::NOT_FOUND_ROOM;
	//		sendPacket.size = 2;
	//		g_iocpNetwork.m_session[userId].Send(&sendPacket);
	//	}
	//}
	//break;
	//case CLIENT_PACKET::CANCEL_APPLY_ROOM://Ŭ���̾�Ʈ�� �� ��û�� ����� ���
	//{
	//	ChracterSessionObject* pSessionObj = dynamic_cast<ChracterSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
	//	CLIENT_PACKET::PlayerCancelRoomPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::PlayerCancelRoomPacket*>(p);
	//	g_RoomManager.m_RecruitRoomListLock.lock();
	//	if (g_RoomManager.m_RecruitingRoomList.count(recvPacket->roomId)) {
	//		g_RoomManager.m_RecruitingRoomList[recvPacket->roomId].DeleteWaitPlayer(userId);
	//	}
	//	g_RoomManager.m_RecruitRoomListLock.unlock();
	//	//�濡 �ִ� ����鿡�� ����� ��û�� ������ ����
	//	//multicast by people in room
	//}
	//break;
	case CLIENT_PACKET::MOUSE_INPUT:
	{
		CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MouseInputPacket*>(p);

		SERVER_PACKET::MouseInputPacket sendPacket;
		sendPacket.LClickedButton = recvPacket->LClickedButton;
		sendPacket.RClickedButton = recvPacket->RClickedButton;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::MOUSE_INPUT;
		sendPacket.size = sizeof(SERVER_PACKET::MouseInputPacket);
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		roomRef.SetMouseInputPlayCharacter(g_iocpNetwork.m_session[userId].GetRole(), sendPacket.LClickedButton, sendPacket.RClickedButton);

#ifdef _DEBUG
		//PrintCurrentTime();
		//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::MOUSE_INPUT - MultiCastOtherPlayer" << std::endl;
#endif
		MultiCastOtherPlayerInRoom(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::SHOOTING_ARROW:
	{
		CLIENT_PACKET::ShootingObject* recvPacket = reinterpret_cast<CLIENT_PACKET::ShootingObject*>(p);
		g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).ShootArrow(recvPacket->dir, recvPacket->pos, recvPacket->speed);
	}
	break;
	case CLIENT_PACKET::SHOOTING_BALL:
	{
		CLIENT_PACKET::ShootingObject* recvPacket = reinterpret_cast<CLIENT_PACKET::ShootingObject*>(p);
		g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).ShootBall(recvPacket->dir, recvPacket->pos, recvPacket->speed);
	}
	break;
	case CLIENT_PACKET::MELEE_ATTACK:
	{
		CLIENT_PACKET::MeleeAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MeleeAttackPacket*>(p);
		//���η� ���ߵ�
		//Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		//bool attacking = roomRef.GetLeftAttackPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
		//DirectX::XMFLOAT3 pos = roomRef.GetPositionPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
		//if (roomRef.MeleeAttack(recvPacket->dir, pos))//�̰Ŵ� 
		//	roomRef.m_bossDamagedQueue.push(g_iocpNetwork.m_session[userId].GetAttackDamagePlayCharacter());
	}
	break;
	case CLIENT_PACKET::TEST_GAME_END: // �ӽ÷�
	{
		Room& room = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		room.GetBoss().isBossDie = true;
		room.GetBoss().isMove = false;
		room.GetBoss().isAttack = false;
	}
	break;
	case CLIENT_PACKET::GAME_END_OK:
	{
		int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		roomRef.DeleteInGamePlayer(userId);
		g_iocpNetwork.m_session[userId].ResetPlayerToLobbyState();
		if (roomRef.GetPlayerNum() == 0) {
			g_RoomManager.RoomDestroy(roomId);
			std::cout << "Destroy Room: " << roomId << std::endl;
		}
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
		cli.Send(p);
	}
}

void Logic::MultiCastOtherPlayer(int userId, void* p)
{
	for (auto& cli : g_iocpNetwork.m_session) {
		if (cli.GetId() == userId) continue;//�ڱ� �ڽ��� ������ �÷��̾�鿡�� ����
		if (cli.GetId() > MAX_USER) break;
		cli.Send(p);
	}
}

void Logic::MultiCastOtherPlayerInRoom(int userId, void* p)
{
	auto roomPlayermap = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).GetPlayerMap();
	for (auto& cli : roomPlayermap) {
		if (cli.second == userId) continue;//�ڱ� �ڽ��� ������ �÷��̾�鿡�� ����
		g_iocpNetwork.m_session[cli.second].Send(p);
	}
}

void Logic::BroadCastInRoomByPlayer(int userId, void* p)
{
	auto roomPlayermap = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).GetPlayerMap();
	for (auto& cli : roomPlayermap) {
		g_iocpNetwork.m_session[cli.second].Send(p);
	}
}

void Logic::BroadCastInRoom(int roomId, void* p)
{
	auto roomPlayermap = g_RoomManager.GetRunningRoomRef(roomId).GetPlayerMap();
	for (auto& cli : roomPlayermap) {
		g_iocpNetwork.m_session[cli.second].Send(p);
	}
}

void Logic::AutoMoveServer()//2500��?
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	while (m_isRunningThread)
	{
		//if (g_iocpNetwork.GetCurrentId() == 0) continue;
		currentTime = std::chrono::high_resolution_clock::now();
		g_RoomManager.RunningRoomLogic();
		//Sleep(16.6667f - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count());
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count() < 1000.0f / 60.0f) {
		}
	}
}

void Logic::MatchMaking()
{
	while (true) {
		//if (r != ROLE::RAND && (m_MatchRole.load() ^ r) && !(m_MatchRole.load() & r)) { // &����� 0�� ���;� ���� ����ִ� ĭ�� ���� �� �ְ�, ^�� 1�� ������ ��
		//	m_MatchRole |= r;
		//	{
		//		std::lock_guard<std::mutex> lg{ m_matchPlayerLock };
		//		m_matchPlayer.emplace(r, userId);
		//	}
		//	return;
		//}

		char sumRole = 0;

		if (warriorPlayerIdQueue.unsafe_size() > 0) {
			/*	int playerId = -1;
				if (warriorPlayerIdQueue.try_pop(playerId)) {
					matchPlayer.emplace(std::make_pair(ROLE::WARRIOR, playerId));
				}*/
			sumRole |= (char)ROLE::WARRIOR;
		}
		if (priestPlayerIdQueue.unsafe_size() > 0) {
			/*int playerId = -1;
			if (priestPlayerIdQueue.try_pop(playerId)) {
				matchPlayer.emplace(std::make_pair(ROLE::PRIEST, playerId));
			}*/
			sumRole |= (char)ROLE::PRIEST;
		}
		if (tankerPlayerIdQueue.unsafe_size() > 0) {
			/*int playerId = -1;
			if (priestPlayerIdQueue.try_pop(playerId)) {
				matchPlayer.emplace(std::make_pair(ROLE::TANKER, playerId));
			}*/
			sumRole |= (char)ROLE::TANKER;
		}
		if (archerPlayerIdQueue.unsafe_size() > 0) {
			/*int playerId = -1;
			if (priestPlayerIdQueue.try_pop(playerId)) {
				matchPlayer.emplace(std::make_pair(ROLE::ARCHER, playerId));
			}*/
			sumRole |= (char)ROLE::ARCHER;
		}

		//{
		//	//match Player Lock Guard
		//	std::lock_guard<std::mutex> lg{ m_matchPlayerLock };
		//	matchPlayer = m_matchPlayer;
		//}
		/*for (const auto& a : matchPlayer) {
			sumRole |= a.first;
		}*/

		std::vector<ROLE> restRole;
		for (int i = 0; i < 4; i++) {
			if (!(sumRole & (char)pow(2, i))) {
				restRole.push_back((ROLE)pow(2, i));
			}
		}

		//�ƹ��� ���� ��, 
		if (restRole.size() == 4) {
			//if (randPlayerIdQueue.unsafe_size() >= 4) {
			//	std::map<ROLE, int> randMatchPlayer;
			//	for (int i = 0; i < 4; i++) {
			//		int randUserId = -1;
			//		if (randPlayerIdQueue.try_pop(randUserId)) {
			//			ROLE r = (ROLE)pow(2, i);
			//			randMatchPlayer.emplace(r, randUserId); // ��·�� ����ó�� ������ ������ ����
			//		}
			//	}
			//	std::string roomId = MakeRoomId();
			//	roomId.append(std::to_string(randMatchPlayer[ROLE::WARRIOR]));
			//	std::wstring roomName{ L"RandMatchingRoom" };
			//	roomName.append(std::to_wstring(randMatchPlayer.begin()->second));
			//	while (!g_RoomManager.InsertRunningRoom(roomId, roomName, randMatchPlayer));
			//	//�� ���� ����
			//
			//	for (const auto& p : randMatchPlayer) {//������ �÷��̾�鿡�� �ڱ� �ڽ� �����Ͽ� ������ ����
			//		//send match Success Packet
			//		g_iocpNetwork.m_session[p.second].SetRoomId(roomId);
			//		g_iocpNetwork.m_session[p.second].SetPlaySessionObject(p.first);
			//		char* sendAddPlayerPacket = g_iocpNetwork.m_session[p.second].m_sessionObject->GetPlayerInfo();
			//		g_iocpNetwork.m_session[p.second].Send(sendAddPlayerPacket);
			//		BroadCastInRoomByPlayer(p.second, sendAddPlayerPacket);
			//		delete sendAddPlayerPacket;
			//	}
			//	//UI -> InGame �Ѿ�� ��Ŷ
			//	SERVER_PACKET::NotifyPacket sendPacket;
			//	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
			//	sendPacket.type = SERVER_PACKET::INTO_GAME;
			//	BroadCastInRoom(roomId, &sendPacket);
			//}
		}

		//��ΰ� Role�� ������ ��������
		else if (restRole.size() == 0) {
			std::map<ROLE, int> matchPlayer;
			if (warriorPlayerIdQueue.unsafe_size() > 0) {
				int playerId = -1;
				if (warriorPlayerIdQueue.try_pop(playerId)) {
					matchPlayer.emplace(std::make_pair(ROLE::WARRIOR, playerId));
				}
			}
			if (priestPlayerIdQueue.unsafe_size() > 0) {
				int playerId = -1;
				if (priestPlayerIdQueue.try_pop(playerId)) {
					matchPlayer.emplace(std::make_pair(ROLE::PRIEST, playerId));
				}
			}
			if (tankerPlayerIdQueue.unsafe_size() > 0) {
				int playerId = -1;
				if (tankerPlayerIdQueue.try_pop(playerId)) {
					matchPlayer.emplace(std::make_pair(ROLE::TANKER, playerId));
				}
			}
			if (archerPlayerIdQueue.unsafe_size() > 0) {
				int playerId = -1;
				if (archerPlayerIdQueue.try_pop(playerId)) {
					matchPlayer.emplace(std::make_pair(ROLE::ARCHER, playerId));
				}
			}

			//std::string roomId = MakeRoomId();
			//roomId.append(std::to_string(matchPlayer.begin()->second));
			//std::wstring roomName{ L"RandMatchingRoom" };
			//roomName.append(std::to_wstring(matchPlayer.begin()->second)); //

			int newRoomId = g_RoomManager.GetRoomId();
			if (newRoomId != -1) {
				for (const auto& p : matchPlayer) {//�÷��̾� ���� �����ϰ� �ѷ��ֱ�
					//send match Success Packet

					//role Setting  ����
					g_iocpNetwork.m_session[p.second].SetRoomId(newRoomId);//roomId to Player					
					//�÷��̾� ���� �ؾߵ�
				}
				Room& roomRef = g_RoomManager.GetRunningRoomRef(newRoomId);
				roomRef.InsertInGamePlayer(matchPlayer);
				roomRef.SendAllPlayerInfo();

				SERVER_PACKET::NotifyPacket sendPacket;
				sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
				sendPacket.type = SERVER_PACKET::INTO_GAME;
				BroadCastInRoom(newRoomId, &sendPacket);
				roomRef.GameStart();
				//{//��Ī ������ ����� �ϴµ� ���� ����°� ������?
				//	std::lock_guard<std::mutex> lg{ m_matchPlayerLock };
				//	m_matchPlayer.clear();
				//}
			}
		}
		//while (!g_RoomManager.InsertRunningRoom(roomId, roomName, matchPlayer));
		//�� ���� ����



	//�Ϻθ� Role������ �������� Rand�� �����Ҷ�
	//else if (restRole.size() <= randPlayerIdQueue.unsafe_size()) {
	//	//���� ���� �־��ֱ� �߰�����
	//	for (int i = 0; i < restRole.size(); i++) {
	//		int randUserId = -1;
	//		if (randPlayerIdQueue.try_pop(randUserId)) {
	//			ROLE r = restRole[i];
	//			matchPlayer.emplace((ROLE)r, randUserId);
	//			g_iocpNetwork.m_session[randUserId].m_sessionObject->SetRole((ROLE)r);
	//			//
	//			//m_matchPlayer�� �ȳ־���
	//			//pop success
	//			matchPlayer.emplace(r, randUserId);
	//		}
	//		else {
	//			//error
	//		}
	//	}
	//	std::string roomId;
	//	auto now = std::chrono::system_clock::now();
	//	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_year % 100));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mon + 1));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_mday));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_hour));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_min));
	//	roomId.append(std::to_string(std::localtime(&in_time_t)->tm_sec));
	//	roomId.append("Matching_");
	//
	//	std::wstring roomName{ L"RandMatchingRoom" };
	//	roomName.append(std::to_wstring(matchPlayer.begin()->second));
	//	while (!g_RoomManager.InsertRunningRoom(roomId, roomName, matchPlayer));
	//	//�� ���� ����
	//	for (const auto& p : matchPlayer) {
	//		g_iocpNetwork.m_session[p.second].SetRoomId(roomId);
	//	}
	//
	//	for (const auto& p : matchPlayer) {
	//		g_iocpNetwork.m_session[p.second].m_sessionObject->SetRole(p.first);
	//		char* sendAddPlayerPacket = g_iocpNetwork.m_session[p.second].m_sessionObject->GetPlayerInfo();
	//		BroadCastInRoomByPlayer(p.second, sendAddPlayerPacket);
	//		delete sendAddPlayerPacket;
	//	}
	//	SERVER_PACKET::NotifyPacket sendPacket;
	//	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	//	sendPacket.type = SERVER_PACKET::INTO_GAME;
	//	BroadCastInRoom(roomId, &sendPacket);
	//	//{
	//	//	std::lock_guard<std::mutex> lg{ m_matchPlayerLock };
	//	//	m_matchPlayer.clear();
	//	//}
	//}
	}
}

void Logic::InsertMatchQueue(ROLE r, int userId)
{
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

std::string Logic::MakeRoomId()
{
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
	return roomId;
}

void Logic::InsertInGameUserMap(std::wstring& id, int userId)
{
	std::lock_guard<std::mutex>ll{ m_inGameUserLock };
	m_inGameUser.try_emplace(id, userId);
}

void Logic::DeleteInGameUserMap(std::wstring& id)
{
	std::lock_guard<std::mutex>ll{ m_inGameUserLock };
	m_inGameUser.erase(id);
}

void Logic::DeleteInGameUserMap(const std::wstring id)
{
	std::lock_guard<std::mutex>ll{ m_inGameUserLock };
	m_inGameUser.erase(id);
}

//std::chrono::utc_clock::time_point t = std::chrono::utc_clock::now();//latency üũ
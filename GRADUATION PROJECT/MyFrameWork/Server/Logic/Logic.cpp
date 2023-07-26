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
			Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
			//auto& playCharacters = roomRef.GetPlayCharacters();
			//auto currentTime = std::chrono::utc_clock::now();
			//double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - recvPacket->time).count();
			//std::cout << "Logic::ProcessPacket() - C2S latency" << durationTime / 1000'000.0f << "second" << std::endl;
			//XMFLOAT3 serverPosition = playCharacters[(ROLE)recvPacket->role]->GetPos();
			//serverPosition = Vector3::Add(serverPosition, playCharacters[g_iocpNetwork.m_session[userId].GetRole()]->GetDirectionVector(), 50.0f * durationTime);
			roomRef.StartMovePlayCharacter(g_iocpNetwork.m_session[userId].GetRole(), recvPacket->direction, recvPacket->time); // 움직임 start;			
#ifdef _DEBUG
			//PrintCurrentTime();
			//std::cout << "Logic::ProcessPacket() - CLIENT_PACKET::MOVE_KEY_DOWN - MultiCastOtherPlayer" << std::endl;
#endif
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
			sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
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
			sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
			sendPacket.type = SERVER_PACKET::MOVE_KEY_UP;
			sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
			//room 함수로 뺄까?
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
			sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
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
		recvPacket->id;
		DB_STRUCT::PlayerInfo* pInfo = new DB_STRUCT::PlayerInfo(recvPacket->id, recvPacket->pw);
		//std::string tempId = recvPacket->id;
		std::set<std::wstring>::iterator playerMapFindRes = m_inGameUser.end();
		{
			std::lock_guard<std::mutex>ll{ m_inGameUserLock };
			playerMapFindRes = m_inGameUser.find(pInfo->PlayerLoginId);
		}
		if (playerMapFindRes != m_inGameUser.end()) {//이미 플레이어가 존재
			//이미 존재하기때문에 지금 들어온 플레이어 접속 해제 패킷 전송
			SERVER_PACKET::NotifyPacket preExistPacket;
			preExistPacket.type = SERVER_PACKET::PRE_EXIST_LOGIN;
			preExistPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
			g_iocpNetwork.m_session[userId].Send(&preExistPacket);//pre_exist notify
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
	}
	break;
	case CLIENT_PACKET::MATCH:
	{
#ifdef ALONE_TEST
		CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MatchPacket*>(p);
		std::map<ROLE, int> alonePlayerMap;
		alonePlayerMap.insert(std::make_pair((ROLE)recvPacket->Role, userId));
		int newRoomId = g_RoomManager.GetRoomId();//새로운 룸 오브젝트 가져오기
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
		//매치 큐 걸어놓고 끝
#endif // ALONE_TEST

	}
	break;
	case CLIENT_PACKET::MOUSE_INPUT:
	{
		CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MouseInputPacket*>(p);

		SERVER_PACKET::MouseInputPacket sendPacket;
		sendPacket.LClickedButton = recvPacket->LClickedButton;
		sendPacket.RClickedButton = recvPacket->RClickedButton;
		sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
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
	/*case CLIENT_PACKET::SHOOTING_ARROW:
	{
		CLIENT_PACKET::ShootingObject* recvPacket = reinterpret_cast<CLIENT_PACKET::ShootingObject*>(p);
		g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).ShootArrow(recvPacket->dir, recvPacket->pos, recvPacket->speed, 100.0f);

		SERVER_PACKET::ShootingObject sendPacket;
		sendPacket.srcPos = recvPacket->pos;
		sendPacket.dir = recvPacket->dir;
		sendPacket.speed = recvPacket->speed;
		sendPacket.type = SERVER_PACKET::SHOOTING_ARROW;
		sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
		MultiCastOtherPlayerInRoom(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::SHOOTING_BALL:
	{
		CLIENT_PACKET::ShootingObject* recvPacket = reinterpret_cast<CLIENT_PACKET::ShootingObject*>(p);
		g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).ShootBall(recvPacket->dir, recvPacket->pos, recvPacket->speed, 100.0f);

		SERVER_PACKET::ShootingObject sendPacket;
		sendPacket.srcPos = recvPacket->pos;
		sendPacket.dir = recvPacket->dir;
		sendPacket.speed = recvPacket->speed;
		sendPacket.type = SERVER_PACKET::SHOOTING_BALL;
		sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
		MultiCastOtherPlayerInRoom(userId, &sendPacket);
	}
	break;*/
	case CLIENT_PACKET::MELEE_ATTACK:
	{
		//CLIENT_PACKET::MeleeAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MeleeAttackPacket*>(p);
		//내부로 빼야됨
		//Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		//bool attacking = roomRef.GetLeftAttackPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
		//DirectX::XMFLOAT3 pos = roomRef.GetPositionPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
		//if (roomRef.MeleeAttack(recvPacket->dir, pos))//이거는 
		//	roomRef.m_bossDamagedQueue.push(g_iocpNetwork.m_session[userId].GetAttackDamagePlayCharacter());
	}
	break;
	case CLIENT_PACKET::TEST_GAME_END: // 임시로
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
	case CLIENT_PACKET::SKILL_EXECUTE_Q:
	{
		CLIENT_PACKET::SkillAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::SkillAttackPacket*>(p);
		g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).
			StartFirstSkillPlayCharacter((ROLE)recvPacket->role, recvPacket->postionOrDirection);
	}
	break;
	case CLIENT_PACKET::SKILL_EXECUTE_E:
	{
		CLIENT_PACKET::SkillAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::SkillAttackPacket*>(p);
		g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).
			StartSecondSkillPlayCharacter((ROLE)recvPacket->role, recvPacket->postionOrDirection);
	}
	case CLIENT_PACKET::SKILL_INPUT_Q:
	{
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		SERVER_PACKET::CommonAttackPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::CommonAttackPacket);
		sendPacket.type = SERVER_PACKET::START_ANIMATION_Q;
		sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
		MultiCastOtherPlayerInRoom_R(roomRef.GetRoomId(), g_iocpNetwork.m_session[userId].GetRole(), &sendPacket);
	}
	break;
	case CLIENT_PACKET::SKILL_INPUT_E:
	{
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		SERVER_PACKET::CommonAttackPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::CommonAttackPacket);
		sendPacket.type = SERVER_PACKET::START_ANIMATION_E;
		sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
		MultiCastOtherPlayerInRoom_R(roomRef.GetRoomId(), g_iocpNetwork.m_session[userId].GetRole(), &sendPacket);
	}
	break;
	case CLIENT_PACKET::TRIGGER_BOX_ON:
	{
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		//roomRef.SetTriggerCntIncrease();
	}
	break;
	case CLIENT_PACKET::TRIGGER_BOX_OUT:
	{
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		//roomRef.SetTriggerCntDecrease();
	}
	break;
	case CLIENT_PACKET::SKIP_NPC_COMMUNICATION:
	{
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		roomRef.Recv_SkipNPC_Communication();
	}
	break;
	case CLIENT_PACKET::STAGE_CHANGE_BOSS:
	{
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		roomRef.ChangeStageBoss();
	}
	break;
	case CLIENT_PACKET::PLAYER_COMMON_ATTACK_EXECUTE:
	{
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		CLIENT_PACKET::PlayerCommonAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::PlayerCommonAttackPacket*>(p);
		roomRef.StartAttackPlayCharacter((ROLE)recvPacket->role, recvPacket->dir, recvPacket->power);
	}
	break;
	case CLIENT_PACKET::PLAYER_COMMON_ATTACK://애니메이션
	{
		Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
		SERVER_PACKET::CommonAttackPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::CommonAttackPacket);
		sendPacket.type = SERVER_PACKET::COMMON_ATTACK_START;
		sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
		MultiCastOtherPlayerInRoom_R(roomRef.GetRoomId(), g_iocpNetwork.m_session[userId].GetRole(), &sendPacket);
	}
	break;
	case  CLIENT_PACKET::CLIENT_FIRST_RECV:
	{
		auto currentServerTime = std::chrono::utc_clock::now();
		g_iocpNetwork.m_session[userId].firstRecvTime = currentServerTime;//e

		SERVER_PACKET::TimeSyncPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::TimeSyncPacket);
		sendPacket.type = SERVER_PACKET::TIME_SYNC;
		sendPacket.serverTime = std::chrono::utc_clock::now();
		g_iocpNetwork.m_session[userId].Send(&sendPacket);
	}
	break;
	case CLIENT_PACKET::CLIENT_SYNC_TIME:
	{
		CLIENT_PACKET::TimeSyncAdaptPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::TimeSyncAdaptPacket*>(p);
		auto latency = std::chrono::duration_cast<std::chrono::microseconds>(g_iocpNetwork.m_session[userId].firstRecvTime - g_iocpNetwork.m_session[userId].firstSendTime).count();
		latency /= 2;
		latency = std::abs(latency);
		auto diff = recvPacket->diff - std::chrono::microseconds(latency).count();
		SERVER_PACKET::TimeLatencyNotifyPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::TimeLatencyNotifyPacket);
		sendPacket.type = SERVER_PACKET::NOTIFY_LATENCY;
		sendPacket.diff = diff;
		g_iocpNetwork.m_session[userId].Send(&sendPacket);
	}
	break;
	default:
		PrintCurrentTime();
		std::cout << "unknown Packet" << std::endl; 
		std::cout << g_iocpNetwork.m_session[userId].GetUserAddrIn() << std::endl;
		g_iocpNetwork.DisconnectClient(userId);
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
		if (cli.GetId() == userId) continue;//자기 자신을 제외한 플레이어들에게 전송
		if (cli.GetId() > MAX_USER) break;
		cli.Send(p);
	}
}

void Logic::MultiCastOtherPlayerInRoom(int userId, void* p)
{
	auto roomPlayermap = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId()).GetPlayerMap();
	for (auto& cli : roomPlayermap) {
		if (cli.second == userId) continue;//자기 자신을 제외한 플레이어들에게 전송
		g_iocpNetwork.m_session[cli.second].Send(p);
	}
}

void Logic::MultiCastOtherPlayerInRoom_R(int roomId, ROLE role, void* p)
{
	auto roomPlayermap = g_RoomManager.GetRunningRoomRef(roomId).GetPlayerMap();
	for (auto& cli : roomPlayermap) {
		if (cli.first == role) continue;//자기 자신을 제외한 플레이어들에게 전송
		g_iocpNetwork.m_session[cli.second].Send(p);
	}
}

void Logic::OnlySendPlayerInRoom_R(int roomId, ROLE role, void* p)
{
	auto roomPlayermap = g_RoomManager.GetRunningRoomRef(roomId).GetPlayerMap();
	if (roomPlayermap.count(role)) {
		g_iocpNetwork.m_session[roomPlayermap[role]].Send(p);
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

void Logic::AutoMoveServer()//2500명?
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	while (m_isRunningThread)
	{
		//if (g_iocpNetwork.GetCurrentId() == 0) continue;
		currentTime = std::chrono::high_resolution_clock::now();
		g_RoomManager.RunningRoomLogic();
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count() < 1000.0f / 60.0f) {}
	}
}

void Logic::MatchMaking()
{
	while (true) {

		char sumRole = 0;

		if (warriorPlayerIdQueue.unsafe_size() > 0) {
			sumRole |= (char)ROLE::WARRIOR;
		}
		if (priestPlayerIdQueue.unsafe_size() > 0) {
			sumRole |= (char)ROLE::PRIEST;
		}
		if (tankerPlayerIdQueue.unsafe_size() > 0) {
			sumRole |= (char)ROLE::TANKER;
		}
		if (archerPlayerIdQueue.unsafe_size() > 0) {
			sumRole |= (char)ROLE::ARCHER;
		}
		std::vector<ROLE> restRole;
		for (int i = 0; i < 4; i++) {
			if (!(sumRole & (char)pow(2, i))) {
				restRole.push_back((ROLE)pow(2, i));
			}
		}
		//아무도 없을 때, 
		if (restRole.size() == 4) {}
		//모두가 Role을 가지고 돌렸을때
		else if (restRole.size() == 1) {
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
			int newRoomId = g_RoomManager.GetRoomId();
			if (newRoomId != -1) {
				Room& roomRef = g_RoomManager.GetRunningRoomRef(newRoomId);
				roomRef.InsertInGamePlayer(matchPlayer);
				for (const auto& p : matchPlayer) {//플레이어 정보 세팅하고 뿌려주기
					//send match Success Packet
					//플레이어 정보 해야됨
					g_iocpNetwork.m_session[p.second].SetRole(p.first);
					g_iocpNetwork.m_session[p.second].SetRoomId(newRoomId);
				}
				roomRef.SendAllPlayerInfo();
				SERVER_PACKET::NotifyPacket sendPacket;
				sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
				sendPacket.type = SERVER_PACKET::INTO_GAME;
				BroadCastInRoom(newRoomId, &sendPacket);
				roomRef.GameStart();
			}
		}
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

void Logic::InsertInGameUserSet(std::wstring& id)
{
	std::lock_guard<std::mutex>ll{ m_inGameUserLock };
	m_inGameUser.emplace(id);
}

void Logic::DeleteInGameUserSet(std::wstring& id)
{
	std::lock_guard<std::mutex>ll{ m_inGameUserLock };
	m_inGameUser.erase(id);
}

void Logic::SendTimeSyncPacket(int id)
{
	SERVER_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	sendPacket.type = SERVER_PACKET::FIRST_SEND;
	g_iocpNetwork.m_session[id].Send(&sendPacket);
	g_iocpNetwork.m_session[id].firstSendTime = std::chrono::utc_clock::now();
}


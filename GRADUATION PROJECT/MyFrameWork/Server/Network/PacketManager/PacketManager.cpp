#include "stdafx.h"
#include "PacketManager.h"
#include "../ExpOver/ExpOver.h"
#include "../../Logic/Logic.h"
#include "../protocol/protocol.h"
#include "../UserSession/UserSession.h"


extern std::array<UserSession, MAX_USER> g_userSession;

int PacketManager::ProccessPacket(const int& useId, const int& roomId, const unsigned long& ioByte, int remainSize, char* buffer)
{
	switch (buffer[2])
	{
	case CLIENT_PACKET::MOVE_KEY_DOWN:
	{
		CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(buffer);
		//Logic::CharacterAddDirection(userId, recvPacket->direction);
	}
	break;
	case CLIENT_PACKET::ROTATE:
	{
		CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::RotatePacket*>(buffer);
		//Logic::CharacterRotate(userId, recvPacket->axis, recvPacket->angle);
	}
	break;
	case CLIENT_PACKET::MOVE_KEY_UP:
	{
		CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(buffer);
		//Logic::CharacterRemoveDirection(userId, recvPacket->direction);
	}
	break;
	case CLIENT_PACKET::STOP:
	{
		CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::StopPacket*>(buffer);
		//Logic::CharacterStop(userId);
	}
	break;
	case CLIENT_PACKET::LOGIN:
	{
		//CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::LoginPacket*>(p);
		//recvPacket->id;
		//DB_STRUCT::PlayerInfo* pInfo = new DB_STRUCT::PlayerInfo(recvPacket->id, recvPacket->pw);
		////std::string tempId = recvPacket->id;
		//std::set<std::wstring>::iterator playerMapFindRes = m_inGameUser.end();
		//{
		//	std::lock_guard<std::mutex>ll{ m_inGameUserLock };
		//	playerMapFindRes = m_inGameUser.find(pInfo->PlayerLoginId);
		//}
		//if (playerMapFindRes != m_inGameUser.end()) {//이미 플레이어가 존재
		//	//이미 존재하기때문에 지금 들어온 플레이어 접속 해제 패킷 전송
		//	SERVER_PACKET::NotifyPacket preExistPacket;
		//	preExistPacket.type = SERVER_PACKET::PRE_EXIST_LOGIN;
		//	preExistPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		//	g_iocpNetwork.m_session[userId].Send(&preExistPacket);//pre_exist notify
		//}
		//else {
		//	/*pInfo->PlayerLoginId.assign(tempId.begin(), tempId.end());
		//	std::string tempPw = recvPacket->pw;
		//	pInfo->pw.assign(tempPw.begin(), tempPw.end());*/
		//	DB_EVENT newEvent;
		//	newEvent.op = DB_OP_GET_PLAYER_INFO;
		//	newEvent.userId = userId;
		//	newEvent.Data = pInfo;
		//	g_DBObj.m_workQueue.push(newEvent);
		//}
	}
	break;
	case CLIENT_PACKET::MATCH:
	{
#ifdef ALONE_TEST
		CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MatchPacket*>(p);
		if (recvPacket->Role != ROLE::NONE_SELECT && recvPacket->Role != ROLE::RAND) {
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
		}
#else
		//std::cout << "match: " << userId << std::endl;
		CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MatchPacket*>(buffer);
		//InsertMatchQueue((ROLE)recvPacket->Role, userId);
		//매치 큐 걸어놓고 끝
#endif // ALONE_TEST
	}
	break;
	case CLIENT_PACKET::MOUSE_INPUT:
	{
		CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MouseInputPacket*>(buffer);
		//Logic::CharacterInput(userId, recvPacket->LClickedButton, recvPacket->RClickedButton);
	}
	break;
	//case CLIENT_PACKET::MELEE_ATTACK:
	//{
	//	//CLIENT_PACKET::MeleeAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MeleeAttackPacket*>(p);
	//	//내부로 빼야됨
	//	//Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
	//	//bool attacking = roomRef.GetLeftAttackPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
	//	//DirectX::XMFLOAT3 pos = roomRef.GetPositionPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
	//	//if (roomRef.MeleeAttack(recvPacket->dir, pos))//이거는 
	//	//	roomRef.m_bossDamagedQueue.push(g_iocpNetwork.m_session[userId].GetAttackDamagePlayCharacter());
	//}
	//break;
	case CLIENT_PACKET::TEST_GAME_END: // 임시로
	{

		//if (g_iocpNetwork.m_session[userId].GetRoomId() != -1) {
		//	int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//	if (roomId != -1) {
		//		Room& room = g_RoomManager.GetRunningRoomRef(roomId);
		//		room.GetBoss().isBossDie = true;
		//		room.GetBoss().isMove = false;
		//		room.GetBoss().isAttack = false;
		//		SERVER_PACKET::NotifyPacket sendPacket;
		//		sendPacket.type = SERVER_PACKET::GAME_END;
		//		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		//
		//		MultiCastOtherPlayerInRoom(userId, &sendPacket);
		//	}
		//}
	}
	break;
	case CLIENT_PACKET::GAME_END_OK:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	roomRef.DeleteInGamePlayer(userId);
		//	g_iocpNetwork.m_session[userId].ResetPlayerToLobbyState();
		//	if (roomRef.GetPlayerNum() == 0) {
		//		g_RoomManager.RoomDestroy(roomId);
		//		std::cout << "Destroy Room: " << roomId << std::endl;
		//	}
		//}
	}
	break;
	case CLIENT_PACKET::SKILL_EXECUTE_Q:
	{
		//CLIENT_PACKET::SkillAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::SkillAttackPacket*>(p);
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	g_RoomManager.GetRunningRoomRef(roomId).
		//		StartFirstSkillPlayCharacter((ROLE)recvPacket->role, recvPacket->postionOrDirection);
		//}
	}
	break;
	case CLIENT_PACKET::SKILL_EXECUTE_E:
	{
		//CLIENT_PACKET::SkillAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::SkillAttackPacket*>(p);
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	g_RoomManager.GetRunningRoomRef(roomId).
		//		StartSecondSkillPlayCharacter((ROLE)recvPacket->role, recvPacket->postionOrDirection);
		//}
	}
	break;
	case CLIENT_PACKET::SKILL_INPUT_Q:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	SERVER_PACKET::CommonAttackPacket sendPacket;
		//	sendPacket.size = sizeof(SERVER_PACKET::CommonAttackPacket);
		//	sendPacket.type = SERVER_PACKET::START_ANIMATION_Q;
		//	sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
		//	MultiCastOtherPlayerInRoom_R(roomRef.GetRoomId(), g_iocpNetwork.m_session[userId].GetRole(), &sendPacket);
		//	if (g_iocpNetwork.m_session[userId].GetRole() == WARRIOR || g_iocpNetwork.m_session[userId].GetRole() == TANKER) {
		//		roomRef.StopMovePlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
		//	}
		//}
	}
	break;
	case CLIENT_PACKET::SKILL_INPUT_E:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	SERVER_PACKET::CommonAttackPacket sendPacket;
		//	sendPacket.size = sizeof(SERVER_PACKET::CommonAttackPacket);
		//	sendPacket.type = SERVER_PACKET::START_ANIMATION_E;
		//	sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
		//	MultiCastOtherPlayerInRoom_R(roomRef.GetRoomId(), g_iocpNetwork.m_session[userId].GetRole(), &sendPacket);
		//	if (g_iocpNetwork.m_session[userId].GetRole() == TANKER) {
		//		roomRef.StopMovePlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
		//	}
		//}
	}
	break;
	case CLIENT_PACKET::PLAYER_COMMON_ATTACK_EXECUTE:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	CLIENT_PACKET::PlayerCommonAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::PlayerCommonAttackPacket*>(p);
		//	roomRef.StartAttackPlayCharacter((ROLE)recvPacket->role, recvPacket->dir, recvPacket->power);
		//}
	}
	break;
	case CLIENT_PACKET::PLAYER_COMMON_ATTACK://애니메이션
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	SERVER_PACKET::CommonAttackPacket sendPacket;
		//	sendPacket.size = sizeof(SERVER_PACKET::CommonAttackPacket);
		//	sendPacket.type = SERVER_PACKET::COMMON_ATTACK_START;
		//	sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();
		//	MultiCastOtherPlayerInRoom_R(roomRef.GetRoomId(), g_iocpNetwork.m_session[userId].GetRole(), &sendPacket);
		//}
	}
	break;
	//case CLIENT_PACKET::TRIGGER_BOX_ON:
	//{
	//	int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
	//	if (roomId != -1) {
	//		Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
	//	}
	//	Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
	//	//roomRef.SetTriggerCntIncrease();
	//}
	//break;
	//case CLIENT_PACKET::TRIGGER_BOX_OUT:
	//{
	//	Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
	//	//roomRef.SetTriggerCntDecrease();
	//}
	//break;
	case CLIENT_PACKET::SKIP_NPC_COMMUNICATION:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	roomRef.Recv_SkipNPC_Communication();
		//}
	}
	break;
	case CLIENT_PACKET::STAGE_CHANGE_BOSS:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	roomRef.ChangeStageBoss();
		//}
	}
	break;
	case CLIENT_PACKET::TIME_SYNC_REQUEST:
	{
		//SERVER_PACKET::TimeSyncPacket sendPacket;
		//sendPacket.size = sizeof(SERVER_PACKET::TimeSyncPacket);
		//sendPacket.type = SERVER_PACKET::TIME_SYNC_RESPONSE;
		//sendPacket.t = std::chrono::utc_clock::now();
		//g_iocpNetwork.m_session[userId].Send(&sendPacket);
	}
	break;
	default:
		//PrintCurrentTime();
		//std::cout << "unknown Packet" << std::endl;
		//std::cout << g_iocpNetwork.m_session[userId].GetUserAddrIn() << std::endl;
		//std::cout << p << std::endl;
		//g_iocpNetwork.DisconnectClient(userId);
		//break;
		break;
	}
	return 0;
}


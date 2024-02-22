#include "stdafx.h"
#include "UserSession.h"
#include "../IocpEvent/IocpEventManager.h"
#include "../ExpOver/ExpOver.h"
#include "../protocol/protocol.h"
#include "../DB/DB.h"

UserSession::UserSession() : m_recvDataStorage(RecvDataStorage())
{
	m_id = -1;
	spdlog::debug("UserSession::UserSession() - 0x{0:0>16x}", long long(this));
}

UserSession::UserSession(int id) : m_id(id), m_recvDataStorage(RecvDataStorage())
{
	spdlog::debug("UserSession::UserSession({0:d}) - 0x{1:0>16x}", id, long long(this));
	m_playerName = L"m_playerName Test";
}

UserSession::UserSession(int id, SOCKET sock) : m_id(id), m_socket(sock), m_recvDataStorage(RecvDataStorage())
{
	spdlog::debug("UserSession::UserSession({0:d}) - 0x{1:0>16x}", id, long long(this));
}

UserSession::UserSession(const UserSession& other)
	: m_id(other.m_id), m_loginId(other.m_loginId), m_playerName(other.m_playerName), m_recvDataStorage(RecvDataStorage())//, m_roomId(other.m_roomId)
{
	spdlog::debug("UserSession::UserSession() - copyConstructor 0x{0:0>16x}", long long(this));
	m_socket = NULL;
}

UserSession::UserSession(UserSession&& other) noexcept
	: m_id(other.m_id), m_loginId(other.m_loginId), m_playerName(std::move(other.m_playerName)), m_recvDataStorage(RecvDataStorage())//, m_roomId(other.m_roomId)
{
	spdlog::debug("UserSession::UserSession() - moveConstructor 0x{0:0>16x}", long long(this));

	m_playerState.store(other.m_playerState);
	m_socket = other.m_socket;
	other.m_socket = NULL;
}

UserSession::~UserSession()
{
	long long ptr = long long(this);
	spdlog::debug("UserSession::~UserSession({0:d}) - 0x{1:0>16x}", m_id, ptr);
	m_id = -1;
	if (NULL != m_socket)
		closesocket(m_socket);
	m_recvDataStorage.Reset();
}

void UserSession::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	auto currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
	case OP_RECV:
		ContructPacket(ioByte);
		break;
	default:
		break;
	}
	DoRecv(over);
}

void UserSession::StartRecv()
{
	//expOVer ���� ��, ���� ��ü�� ����
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(OP_RECV, shared_from_this());
	DoRecv(expOver);
}

void UserSession::DoSend(const PacketHeader* packetHeader)
{
	IocpEventManager::GetInstance().Send(m_socket, packetHeader);
}

void UserSession::DoRecv(ExpOver* over)
{
	//wsaBuf length ���� �� ����
	m_recvDataStorage.m_wsabuf.len = MAX_RECV_BUF_SIZE - m_recvDataStorage.m_remainDataLength;
	int recvRes = WSARecv(m_socket, &m_recvDataStorage.m_wsabuf + m_recvDataStorage.m_remainDataLength, 1, nullptr, 0, over, nullptr);
	if (recvRes != 0) {
		int errCode = WSAGetLastError();
		if (WSA_IO_PENDING != errCode) {
			spdlog::warn("UserSession::DoRecv() - Error Code: {}", errCode);
		}
	}
}

void UserSession::ContructPacket(const DWORD& ioSize)
{
	int currentID = m_id;
	int remainSize = ioSize + m_recvDataStorage.m_remainDataLength;
	char* bufferPosition = m_recvDataStorage.m_buffer;
	while (remainSize > sizeof(PacketHeader::size)) {
		PacketHeader* currentPacket = reinterpret_cast<PacketHeader*>(bufferPosition);
		if (currentPacket->size > remainSize) {
			//�ϼ��� ��Ŷ�� ��������� ����.
			break;
		}
		//�ϼ��� ��Ŷ
		ExecutePacket(currentPacket);
		//���� �۹� ũ�� �ֽ�ȭ, ���� ���� ��ġ ���� ��Ŷ ���� ��ġ��
		remainSize -= currentPacket->size;
		bufferPosition = bufferPosition += currentPacket->size;
	}
	//���� ���� ������ ũ�� ����
	m_recvDataStorage.m_remainDataLength = remainSize;
	//���� ��Ŷ �����Ͱ� �ִٸ�, �� ������ ����
	if (remainSize > 0)
		std::memcpy(m_recvDataStorage.m_buffer, bufferPosition, remainSize);
}

void UserSession::ExecutePacket(const PacketHeader* packetHeader)
{
	switch (packetHeader->type)
	{
	case CLIENT_PACKET::LOGIN:
	{
		const CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::LoginPacket*>(packetHeader);
		std::shared_ptr<DB::EventBase> getPlayerInfoEvent = std::make_shared<DB::PlayerInfoEvent>(DB::DB_OP_CODE::DB_OP_GET_PLAYER_INFO, m_id, recvPacket->id, recvPacket->pw);
		DB::DBConnector::GetInstance().InsertDBEvent(getPlayerInfoEvent);
	}
	break;
	case CLIENT_PACKET::MATCH:
	{
		//1�� �׽�Ʈ
#ifdef ALONE_TEST
		const CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MatchPacket*>(packetHeader);
		if (recvPacket->Role != ROLE::NONE_SELECT && recvPacket->Role != ROLE::RAND) {
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
		}
#else
		//��Ī�� Ŭ���� �ϳ� ���� �ļ� �ϴ°� ������.
		//std::cout << "match: " << userId << std::endl;
		const CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MatchPacket*>(packetHeader);
		//InsertMatchQueue((ROLE)recvPacket->Role, userId);
		//��ġ ť �ɾ���� ��
#endif // ALONE_TEST
	}
	break;

#pragma region CHARACTER_MOVE
	case CLIENT_PACKET::MOVE_KEY_DOWN:
	{
		const CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MovePacket*>(packetHeader);
		//Logic::CharacterAddDirection(userId, recvPacket->direction);
	}
	break;
	case CLIENT_PACKET::MOVE_KEY_UP:
	{
		const CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MovePacket*>(packetHeader);
		//Logic::CharacterRemoveDirection(userId, recvPacket->direction);
	}
	break;
	case CLIENT_PACKET::STOP:
	{
		const CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::StopPacket*>(packetHeader);
		//Logic::CharacterStop(userId);
	}
	break;
	case CLIENT_PACKET::ROTATE:
	{
		const CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::RotatePacket*>(packetHeader);
		//Logic::CharacterRotate(userId, recvPacket->axis, recvPacket->angle);
	}
	break;
#pragma endregion

#pragma region CHARACTER_ATTACK
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
	case CLIENT_PACKET::PLAYER_COMMON_ATTACK://�ִϸ��̼�
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
	case CLIENT_PACKET::MOUSE_INPUT:
	{
		const CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MouseInputPacket*>(packetHeader);
		//Logic::CharacterInput(userId, recvPacket->LClickedButton, recvPacket->RClickedButton);
	}
	break;
#pragma endregion

#pragma region CHEAT_KEY
	case CLIENT_PACKET::STAGE_CHANGE_BOSS:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	roomRef.ChangeStageBoss();
		//}
	}
	break;

	case CLIENT_PACKET::TEST_GAME_END: // �ӽ÷�
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
#pragma endregion

	//�÷��̾� ���� ������ �� ������ OK -> �̶� �� ���� ���� Ȯ��
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
	//NPC��ȭ ����
	case CLIENT_PACKET::SKIP_NPC_COMMUNICATION:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	roomRef.Recv_SkipNPC_Communication();
		//}
	}
	break;
	//Ŭ���̾�Ʈ�� �ð� ����ȭ RTTüũ
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
		spdlog::critical("Recv Unknown Packet");
		break;
	}
}


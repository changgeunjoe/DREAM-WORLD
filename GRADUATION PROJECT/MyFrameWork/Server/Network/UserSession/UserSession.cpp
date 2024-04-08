#include "stdafx.h"
#include "UserSession.h"
#include "../IocpEvent/IocpEventManager.h"
#include "../ExpOver/ExpOver.h"
#include "../protocol/protocol.h"
#include "../DB/DB.h"
#include "../Match/Matching.h"
#include "../Room/Room.h"
#include "../GameObject/Character/ChracterObject.h"

UserSession::UserSession() : m_recvDataStorage(RecvDataStorage())
{
	spdlog::debug("UserSession::UserSession() - 0x{0:0>16x}", long long(this));
}

UserSession::UserSession(int id) : m_recvDataStorage(RecvDataStorage())
{
	spdlog::debug("UserSession::UserSession({0:d}) - 0x{1:0>16x}", id, long long(this));
	m_playerName = L"m_playerName Test";
}

UserSession::UserSession(int id, SOCKET sock) : m_socket(sock), m_recvDataStorage(RecvDataStorage())
{
	spdlog::debug("UserSession::UserSession({0:d}) - 0x{1:0>16x}", id, long long(this));
}

UserSession::UserSession(const UserSession& other)
	: m_playerName(other.m_playerName), m_recvDataStorage(RecvDataStorage())//, m_roomId(other.m_roomId)
{
	spdlog::debug("UserSession::UserSession() - copyConstructor 0x{0:0>16x}", long long(this));
	m_socket = NULL;
}

UserSession::UserSession(UserSession&& other) noexcept
	: m_playerName(std::move(other.m_playerName)), m_recvDataStorage(RecvDataStorage())//, m_roomId(other.m_roomId)
{
	spdlog::debug("UserSession::UserSession() - moveConstructor 0x{0:0>16x}", long long(this));

	m_playerState.store(other.m_playerState);
	m_socket = other.m_socket;
	other.m_socket = NULL;
}

UserSession::~UserSession()
{
	spdlog::debug("UserSession::~UserSession - Nick Name: {0}", ConvertWideStringToString(m_playerName.c_str()));
	//m_id = -1;
	if (NULL != m_socket)
		closesocket(m_socket);
	m_recvDataStorage.Reset();
}

void UserSession::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	auto currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
	case IOCP_OP_CODE::OP_RECV:
	{
		if (0 == ioByte) {
			int errCode = WSAGetLastError();
			if (errCode != WSA_IO_PENDING) {
				spdlog::critical("UserSession::Execute() - OP_RECV Error");
				DisplayWsaGetLastError(errCode);
				//User Disconnect
				return;
			}
		}
		else ContructPacket(ioByte);
		DoRecv(over);
	}
	break;
	default:
		spdlog::critical("UserSession::Execute() - UnDefined OP_CODE - {}", static_cast<int>(currentOpCode));
		return;
		break;
	}
}

void UserSession::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	//Iocp���� ret == false
	auto currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
	case IOCP_OP_CODE::OP_RECV:
	{
		if (0 == ioByte) {
			int errCode = WSAGetLastError();
			if (errCode != WSA_IO_PENDING) {
				spdlog::critical("UserSession::Execute() - OP_RECV Error");
				DisplayWsaGetLastError(errCode);
				//User Disconnect
				return;
			}
		}
		else ContructPacket(ioByte);
		DoRecv(over);
	}
	break;
	default:
		spdlog::critical("UserSession::Execute() - UnDefined OP_CODE - {}", static_cast<int>(currentOpCode));
		return;
		break;
	}
}

void UserSession::StartRecv()
{
	//expOVer ���� ��, ���� ��ü�� ����
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_RECV, shared_from_this());
	DoRecv(expOver);
}

void UserSession::DoSend(const PacketHeader* packetHeader) const
{
	IocpEventManager::GetInstance().Send(m_socket, packetHeader);
}

void UserSession::DoSend(const std::shared_ptr<PacketHeader> packetHeader) const
{
	IocpEventManager::GetInstance().Send(m_socket, packetHeader.get());
}

void UserSession::SetIngameRef(std::shared_ptr<Room>& roomRef, std::shared_ptr<CharacterObject>& characterRef)
{
	m_roomWeakRef = roomRef;
	m_possessCharacter = characterRef;
}

void UserSession::DoRecv(ExpOver*& over)
{
	//wsaBuf length ���� �� ����
	m_recvDataStorage.m_wsabuf.len = MAX_RECV_BUF_SIZE - m_recvDataStorage.m_remainDataLength;
	DWORD immediateRecvByte = 0;
	DWORD recvFlag = 0;
	int recvRes = WSARecv(m_socket, &m_recvDataStorage.m_wsabuf, 1, nullptr, &recvFlag, over, nullptr);
	if (recvRes != 0) {
		int errCode = WSAGetLastError();
		if (WSA_IO_PENDING != errCode) {
			spdlog::critical("UserSession::DoRecv() - Error");
			DisplayWsaGetLastError(errCode);
			//Disconnect??
		}
	}
}

void UserSession::ContructPacket(const DWORD& ioSize)
{
	//int currentID = m_id;
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
	//wsaBuf�� buf ��ġ�� �ٲ�
	m_recvDataStorage.m_wsabuf.buf = m_recvDataStorage.m_buffer + remainSize;
}

void UserSession::ExecutePacket(const PacketHeader* packetHeader)
{
	//m_playerState
	switch (static_cast<CLIENT_PACKET::TYPE>(packetHeader->type))
	{
	case CLIENT_PACKET::TYPE::LOGIN:
	{
		const CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::LoginPacket*>(packetHeader);
		std::shared_ptr<DB::EventBase> getPlayerInfoEvent = std::make_shared<DB::PlayerInfoEvent>(DB::DB_OP_CODE::DB_OP_GET_PLAYER_INFO, std::static_pointer_cast<UserSession, IOCP::EventBase>(shared_from_this()), recvPacket->id, recvPacket->pw);
		DB::DBConnector::GetInstance().InsertDBEvent(getPlayerInfoEvent);
	}
	break;
	case CLIENT_PACKET::TYPE::MATCH:
	{
		const CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MatchPacket*>(packetHeader);
		ROLE currentRole = static_cast<ROLE>(recvPacket->role);
		if (static_cast<ROLE>(recvPacket->role) == ROLE::NONE_SELECT) {
			//Send disable to User
		}
		else Matching::GetInstance().InserMatch(std::static_pointer_cast<UserSession, IOCP::EventBase>(shared_from_this()), currentRole);
	}
	break;

#pragma region CHARACTER_MOVE
	case CLIENT_PACKET::TYPE::MOVE_KEY_DOWN:
	{
		const CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MovePacket*>(packetHeader);
		//Logic::CharacterAddDirection(userId, recvPacket->direction);
		//
		//Effectively returns expired() ? shared_ptr<T>() : shared_ptr<T>(*this), executed atomically.
		//weak_ptr<>::lock() ����������, shared_ptr�� refCnt != 0 && Rep�� ��ȿ�� ��, ptr����, �ƴϸ� nullptr ��ȯ
		//RoomRef�� shared_ptr ������ _Rep�� �������� �ʾƼ� ���� ����.
		auto possessObject = m_possessCharacter.lock();
		if (nullptr != possessObject) {
			possessObject->RecvDirection(recvPacket->direction, true);

			auto roomRef = m_roomWeakRef.lock();
			if (nullptr == roomRef) {
				//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
			}
			PacketHeader* sendPacket = nullptr;
			sendPacket = &SERVER_PACKET::MovePacket(possessObject->GetRole(), recvPacket->direction, std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::MOVE_KEY_DOWN));
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());
		}
		else {
			//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
		}

	}
	break;
	case CLIENT_PACKET::TYPE::MOVE_KEY_UP:
	{
		const CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MovePacket*>(packetHeader);
		auto possessObject = m_possessCharacter.lock();
		if (nullptr != possessObject) {
			possessObject->RecvDirection(recvPacket->direction, false);

			auto roomRef = m_roomWeakRef.lock();
			if (nullptr == roomRef) {
				//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
			}
			PacketHeader* sendPacket = nullptr;
			sendPacket = &SERVER_PACKET::MovePacket(possessObject->GetRole(), recvPacket->direction, std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::MOVE_KEY_UP));
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());

		}
		else {
			//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
		}
		//Logic::CharacterRemoveDirection(userId, recvPacket->direction);
	}
	break;
	case CLIENT_PACKET::TYPE::STOP:
	{
		const CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::StopPacket*>(packetHeader);
		//Logic::CharacterStop(userId);
		auto possessObject = m_possessCharacter.lock();
		if (nullptr != possessObject) {
			possessObject->StopMove();

			auto roomRef = m_roomWeakRef.lock();
			if (nullptr == roomRef) {
				//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
			}
			PacketHeader* sendPacket = nullptr;
			sendPacket = &SERVER_PACKET::StopPacket(possessObject->GetRole(), std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::STOP));

			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());
		}
		else {
			//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
		}
	}
	break;
	case CLIENT_PACKET::TYPE::ROTATE:
	{
		const CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::RotatePacket*>(packetHeader);
		//Logic::CharacterRotate(userId, recvPacket->axis, recvPacket->angle);
		auto possessObject = m_possessCharacter.lock();
		if (nullptr != possessObject) {
			possessObject->RecvRotate(recvPacket->axis, recvPacket->angle);

			auto roomRef = m_roomWeakRef.lock();
			if (nullptr == roomRef) {
				//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
			}
			PacketHeader* sendPacket = nullptr;
			sendPacket = &SERVER_PACKET::RotatePacket(possessObject->GetRole(), recvPacket->axis, recvPacket->angle);
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());
		}
		else {
			//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
		}
	}
	break;
#pragma endregion

#pragma region CHARACTER_ATTACK
	case CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q://���� ������ ���� �� ���ƿ��� ��Ŷ
	{
		//CLIENT_PACKET::SkillAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::SkillAttackPacket*>(p);
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	g_RoomManager.GetRunningRoomRef(roomId).
		//		StartFirstSkillPlayCharacter((ROLE)recvPacket->role, recvPacket->postionOrDirection);
		//}
	}
	break;
	case CLIENT_PACKET::TYPE::SKILL_EXECUTE_E:
	{
		//CLIENT_PACKET::SkillAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::SkillAttackPacket*>(p);
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	g_RoomManager.GetRunningRoomRef(roomId).
		//		StartSecondSkillPlayCharacter((ROLE)recvPacket->role, recvPacket->postionOrDirection);
		//}
	}
	break;
	case CLIENT_PACKET::TYPE::SKILL_INPUT_Q://StartAnimation
	{
		auto possessCharacter = m_possessCharacter.lock();
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr == possessCharacter || nullptr == roomRef) {
			//Invalid InGameState
			return;
		}
		SERVER_PACKET::NotifyPlayerAnimationPacket sendPacket(static_cast<char>(SERVER_PACKET::TYPE::START_ANIMATION_Q), possessCharacter->GetRole());
		roomRef->MultiCastCastPacket(&sendPacket, possessCharacter->GetRole());
		//	if (g_iocpNetwork.m_session[userId].GetRole() == WARRIOR || g_iocpNetwork.m_session[userId].GetRole() == TANKER) {
		//		roomRef.StopMovePlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
		//	}
	}
	break;
	case CLIENT_PACKET::TYPE::SKILL_INPUT_E:
	{
		auto possessCharacter = m_possessCharacter.lock();
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr == possessCharacter || nullptr == roomRef) {
			//Invalid InGameState
			return;
		}
		SERVER_PACKET::NotifyPlayerAnimationPacket sendPacket(static_cast<char>(SERVER_PACKET::TYPE::START_ANIMATION_E), possessCharacter->GetRole());
		roomRef->MultiCastCastPacket(&sendPacket, possessCharacter->GetRole());
		//	if (g_iocpNetwork.m_session[userId].GetRole() == WARRIOR || g_iocpNetwork.m_session[userId].GetRole() == TANKER) {
		//		roomRef.StopMovePlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
		//	}
	}
	break;
	//Ŭ��� ���� ���� �ִϸ��̼� ��Ŷ�� mouseInput���� �ذ� �ϴ� ������ ����
	//PLAYER_ATTACK �� 2���� ���� �Ǵ� ��?
	case CLIENT_PACKET::TYPE::PLAYER_POWER_ATTACK_EXECUTE:
	{
		const CLIENT_PACKET::PlayerPowerAttackPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::PlayerPowerAttackPacket*>(packetHeader);
		auto possessObject = m_possessCharacter.lock();
		if (nullptr != possessObject) {
			possessObject->RecvAttackCommon(recvPacket->direction, recvPacket->power);
			//possessObject->RecvAttackCommon(recvPacket->direction, recvPacket->power);
		}
		else {
			//characterRef�� ��ȿ���� ���� -> roomRef�� ��ȿ���� ����.
			//PlayerState = LOBBY;
		}
	}
	break;
	case CLIENT_PACKET::TYPE::PLAYER_COMMON_ATTACK_EXECUTE:
	{
		const CLIENT_PACKET::PlayerCommonAttackPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::PlayerCommonAttackPacket*>(packetHeader);
		auto possessObject = m_possessCharacter.lock();
		if (nullptr != possessObject) {
			possessObject->RecvAttackCommon(recvPacket->direction);
			//possessObject->RecvAttackCommon(recvPacket->direction, recvPacket->power);
		}
		else {
			//characterRef�� ��ȿ���� ���� -> roomRef�� ��ȿ���� ����.
			//PlayerState = LOBBY;
		}
	}
	break;
	case CLIENT_PACKET::TYPE::MOUSE_INPUT:
	{
		const CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MouseInputPacket*>(packetHeader);
		auto possessObject = m_possessCharacter.lock();
		if (nullptr != possessObject) {
			//Room��ü�� ���� ��ȿ
			possessObject->RecvMouseInput(recvPacket->leftClickedButton, recvPacket->rightClickedButton);

			auto roomRef = m_roomWeakRef.lock();
			if (nullptr == roomRef) {
				//��ȿ���� ����.
				//m_playerState = PLAYER_STATE::LOBBY;
			}
			PacketHeader* sendPacket = nullptr;
			sendPacket = &SERVER_PACKET::MouseInputPacket(possessObject->GetRole(), recvPacket->leftClickedButton, recvPacket->rightClickedButton);
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());
		}
		else {
			//��ȿ���� ����.
			//m_playerState = PLAYER_STATE::LOBBY;
		}
		//Logic::CharacterInput(userId, recvPacket->LClickedButton, recvPacket->RClickedButton);
	}
	break;
#pragma endregion

#pragma region CHEAT_KEY
	case CLIENT_PACKET::TYPE::STAGE_CHANGE_BOSS:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	roomRef.ChangeStageBoss();
		//}
	}
	break;

	case CLIENT_PACKET::TYPE::TEST_GAME_END: // �ӽ÷�
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
	case CLIENT_PACKET::TYPE::GAME_END_OK:
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
	case CLIENT_PACKET::TYPE::SKIP_NPC_COMMUNICATION:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	roomRef.Recv_SkipNPC_Communication();
		//}
	}
	break;
	//Ŭ���̾�Ʈ�� �ð� ����ȭ RTTüũ
	case CLIENT_PACKET::TYPE::TIME_SYNC_REQUEST:
	{
		SERVER_PACKET::TimeSyncPacket sendPacket;
		DoSend(&sendPacket);
	}
	break;

	default:
		spdlog::critical("Recv Unknown Packet, Size: {}, Type: {}", packetHeader->size, packetHeader->type);
		//disconnect User
		break;
	}
}

void UserSession::Disconnect()
{
}


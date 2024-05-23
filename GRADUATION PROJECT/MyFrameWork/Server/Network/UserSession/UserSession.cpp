#include "stdafx.h"
#include "UserSession.h"
#include "../IocpEvent/IocpEventManager.h"
#include "../ExpOver/ExpOver.h"
#include "../protocol/protocol.h"
#include "../DB/DB.h"
#include "../Match/Matching.h"
#include "../Room/Room.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../Room/RoomEvent.h"
#include "../UserSession/UserManager.h"


UserSession::UserSession() : m_recvDataStorage(RecvDataStorage())
{
	spdlog::debug("UserSession::UserSession() - 0x{0:0>16x}", long long(this));
}

UserSession::UserSession(int id) : m_recvDataStorage(RecvDataStorage())
{
	spdlog::debug("UserSession::UserSession({0:d}) - 0x{1:0>16x}", id, long long(this));
	m_playerName = L"m_playerName Test";
}

UserSession::UserSession(int id, SOCKET sock)
	: m_socket(sock), m_recvDataStorage(RecvDataStorage()), m_connectState(CONNECT_STATE::CONNECTED),
	m_prevSendPacketBufferQueueSize(0), m_isAbleSend(true), m_sendOverlapped(nullptr), m_recvOverlapped(nullptr)
{
	spdlog::debug("UserSession::UserSession({0:d}) - 0x{1:0>16x}", id, long long(this));
	m_sendPacketBuffer.reserve(10);
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
	m_socket = NULL;
	IocpEventManager::GetInstance().DeleteExpOver(m_recvOverlapped);
	IocpEventManager::GetInstance().DeleteExpOver(m_sendOverlapped);
}

void UserSession::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	auto currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
	case IOCP_OP_CODE::OP_RECV:
	{
		RecvComplete(ioByte);
	}
	break;
	case IOCP_OP_CODE::OP_SEND:
	{
		SendComplete(ioByte);
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
	spdlog::critical("UserSession::Fail()");
	Disconnect();
}

void UserSession::StartRecv()
{
	//expOVer ���� ��, ���� ��ü�� ����
	m_recvOverlapped = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_RECV, shared_from_this());
	m_sendOverlapped = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_SEND, shared_from_this());
	DoRecv(m_recvOverlapped);
}

//void UserSession::DoSend(const PacketHeader* packetHeader) const
//{
//	if (CONNECT_STATE::DISCONNECTED == m_connectState) return;
//	IocpEventManager::GetInstance().Send(m_socket, packetHeader);
//}

void UserSession::DoSend(const std::shared_ptr<PacketHeader> packetHeader)
{
	if (CONNECT_STATE::DISCONNECTED == m_connectState) return;
	m_prevSendPacketBufferQueue.push(packetHeader);
	++m_prevSendPacketBufferQueueSize;
	bool sendAble = m_isAbleSend.exchange(false);//���� ���� ��ȯ ��.
	if (sendAble) {
		SendExecte();
	}
	//IocpEventManager::GetInstance().Send(m_socket, packetHeader.get());
}

void UserSession::Reconnect(std::shared_ptr<UserSession> prevDisconnectedUserSession)
{
	m_playerName = prevDisconnectedUserSession->m_playerName;
	m_playerState.store(prevDisconnectedUserSession->m_playerState);

	m_roomWeakRef = prevDisconnectedUserSession->m_roomWeakRef;
	m_possessCharacter = prevDisconnectedUserSession->m_possessCharacter;

	m_ingameRole = prevDisconnectedUserSession->m_ingameRole;
	m_connectState = CONNECT_STATE::CONNECTED;
	if (PLAYER_STATE::IN_GAME == m_playerState) {
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr == roomRef) {
			m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		auto characterRef = m_possessCharacter.lock();
		if (nullptr == characterRef) {
			m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		//���� ��Ÿ������ ������ ������.
		bool isValidRoom = roomRef->ReconnectUser(std::static_pointer_cast<UserSession>(shared_from_this()));
		if (!isValidRoom)m_playerState = PLAYER_STATE::LOBBY;
	}
}

void UserSession::SetIngameRef(std::shared_ptr<Room>& roomRef, std::shared_ptr<CharacterObject>& characterRef)
{
	m_playerState = PLAYER_STATE::IN_GAME;
	m_roomWeakRef = roomRef;
	m_possessCharacter = characterRef;
}

void UserSession::DoRecv(ExpOver*& over)
{
	m_recvOverlapped->SetIocpEvent(shared_from_this());//�ٽ� Set
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
			Disconnect();
			m_recvOverlapped->ResetEvent();
			return;
		}
	}
}

void UserSession::RecvComplete(const DWORD& ioByte)
{
	//recv �Ϸ� �� ���� overlapped�� iocpEvent ����
	if (CONNECT_STATE::DISCONNECTED == m_connectState) {
		m_recvOverlapped->ResetEvent();
		return;
	}
	if (0 == ioByte) {
		Disconnect();
		m_recvOverlapped->ResetEvent();
		return;
	}
	ContructPacket(ioByte);
	DoRecv(m_recvOverlapped);
}

void UserSession::ContructPacket(const DWORD& ioSize)
{
	//int currentID = m_id;
	int remainSize = ioSize + m_recvDataStorage.m_remainDataLength;
	char* bufferPosition = m_recvDataStorage.m_buffer;
	int i = 0;
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
		bufferPosition = bufferPosition + currentPacket->size;
		++i;
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
		std::string id = recvPacket->id;
		if (std::string::npos != id.find("module", 0)) {
			DoSend(std::make_shared<SERVER_PACKET::LoginPacket>());
			m_playerName = std::wstring(id.begin(), id.end());
			return;
		}
		std::shared_ptr<DB::EventBase> getPlayerInfoEvent = std::make_shared<DB::PlayerInfoEvent>(DB::DB_OP_CODE::DB_OP_GET_PLAYER_INFO, std::static_pointer_cast<UserSession>(shared_from_this()), recvPacket->id, recvPacket->pw);
		DB::DBConnector::GetInstance().InsertDBEvent(getPlayerInfoEvent);
	}
	break;

	case CLIENT_PACKET::TYPE::RECONN:
	{
		const CLIENT_PACKET::ReConnectPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::ReConnectPacket*>(packetHeader);
		auto prevUserSessionData = UserManager::GetInstance().FindDisconnectUser(recvPacket->nickName);
		//Reconnect ���� ���п� ���ؼ� Ŭ�� ���� �ؾ��ҵ�
		if (nullptr == prevUserSessionData) {
			ReconnectFail();
			//���� => Ŭ��� ���α׷� ����
			return;
		}
		Reconnect(prevUserSessionData);//���⼭ �ٽ� ����� �۾���.
	}
	break;

	case CLIENT_PACKET::TYPE::MATCH:
	{
		const CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MatchPacket*>(packetHeader);
		if (recvPacket->role != ROLE::NONE_SELECT) {
			Matching::GetInstance().InserMatch(std::static_pointer_cast<UserSession>(shared_from_this()), recvPacket->role);
			m_playerState = PLAYER_STATE::MATCH;
			m_matchedRole = recvPacket->role;
		}
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
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != possessObject && nullptr != roomRef) {
			possessObject->RecvDirection(recvPacket->direction, true);

			auto sendPacket = std::make_shared<SERVER_PACKET::MovePacket>(possessObject->GetRole(), recvPacket->direction, std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::MOVE_KEY_DOWN));
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());
		}
		else {
			//��ȿ���� ����.
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
		}

	}
	break;
	case CLIENT_PACKET::TYPE::MOVE_KEY_UP:
	{
		const CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MovePacket*>(packetHeader);
		auto possessObject = m_possessCharacter.lock();
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != possessObject || nullptr != roomRef) {
			possessObject->RecvDirection(recvPacket->direction, false);

			auto sendPacket = std::make_shared<SERVER_PACKET::MovePacket>(possessObject->GetRole(), recvPacket->direction, std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::MOVE_KEY_UP));
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());

		}
		else {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
		}
	}
	break;
	case CLIENT_PACKET::TYPE::STOP:
	{
		const CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::StopPacket*>(packetHeader);
		//Logic::CharacterStop(userId);
		auto possessObject = m_possessCharacter.lock();
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != possessObject && nullptr != roomRef) {
			possessObject->StopMove();
			auto sendPacket = std::make_shared<SERVER_PACKET::StopPacket>(possessObject->GetRole(), std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::STOP));
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());
		}
		else {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
		}
	}
	break;
	case CLIENT_PACKET::TYPE::ROTATE:
	{
		const CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::RotatePacket*>(packetHeader);
		//Logic::CharacterRotate(userId, recvPacket->axis, recvPacket->angle);
		auto possessObject = m_possessCharacter.lock();
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != possessObject && nullptr != roomRef) {
			possessObject->RecvRotate(recvPacket->axis, recvPacket->angle);
			auto sendPacket = std::make_shared<SERVER_PACKET::RotatePacket>(possessObject->GetRole(), recvPacket->axis, recvPacket->angle);
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());
		}
		else {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
		}
	}
	break;
#pragma endregion

#pragma region CHARACTER_ATTACK
	case CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q://���� ������ ���� �� ���ƿ��� ��Ŷ
	{
		auto possessCharacter = m_possessCharacter.lock();
		if (nullptr == possessCharacter) {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		possessCharacter->RecvSkill(CharacterObject::SKILL_TYPE::SKILL_TYPE_Q);
	}
	break;
	case CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_Q:
	{
		auto possessCharacter = m_possessCharacter.lock();
		if (nullptr == possessCharacter) {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		const CLIENT_PACKET::FloatDataSkillPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::FloatDataSkillPacket*>(packetHeader);
		possessCharacter->RecvSkill(CharacterObject::SKILL_TYPE::SKILL_TYPE_Q, recvPacket->floatData);
	}
	break;
	case CLIENT_PACKET::TYPE::SKILL_EXECUTE_E:
	{
		auto possessCharacter = m_possessCharacter.lock();
		if (nullptr == possessCharacter) {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		possessCharacter->RecvSkill(CharacterObject::SKILL_TYPE::SKILL_TYPE_E);
	}
	break;
	case CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_E:
	{
		auto possessCharacter = m_possessCharacter.lock();
		if (nullptr == possessCharacter) {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		const CLIENT_PACKET::FloatDataSkillPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::FloatDataSkillPacket*>(packetHeader);
		possessCharacter->RecvSkill(CharacterObject::SKILL_TYPE::SKILL_TYPE_E, recvPacket->floatData);
	}
	break;
	case CLIENT_PACKET::TYPE::SKILL_INPUT_Q://StartAnimation
	{
		auto possessCharacter = m_possessCharacter.lock();
		if (nullptr == possessCharacter) {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		possessCharacter->RecvSkillInput(CharacterObject::SKILL_TYPE::SKILL_TYPE_Q);
	}
	break;
	case CLIENT_PACKET::TYPE::SKILL_INPUT_E:
	{
		auto possessCharacter = m_possessCharacter.lock();
		if (nullptr == possessCharacter) {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		possessCharacter->RecvSkillInput(CharacterObject::SKILL_TYPE::SKILL_TYPE_E);
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
		}
		else {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
		}
	}
	break;
	case CLIENT_PACKET::TYPE::PLAYER_COMMON_ATTACK_EXECUTE:
	{
		const CLIENT_PACKET::PlayerCommonAttackPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::PlayerCommonAttackPacket*>(packetHeader);
		auto possessObject = m_possessCharacter.lock();
		if (nullptr != possessObject) {
			possessObject->RecvAttackCommon(recvPacket->direction);
		}
		else {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
		}
	}
	break;
	case CLIENT_PACKET::TYPE::MOUSE_INPUT:
	{
		const CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MouseInputPacket*>(packetHeader);
		auto possessObject = m_possessCharacter.lock();
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != possessObject && nullptr != roomRef) {
			//Room��ü�� ���� ��ȿ
			possessObject->RecvMouseInput(recvPacket->leftClickedButton, recvPacket->rightClickedButton);
			auto sendPacket = std::make_shared<SERVER_PACKET::MouseInputPacket>(possessObject->GetRole(), recvPacket->leftClickedButton, recvPacket->rightClickedButton);
			roomRef->MultiCastCastPacket(sendPacket, possessObject->GetRole());
		}
		else {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
		}
	}
	break;
#pragma endregion

#pragma region CHEAT_KEY
	case CLIENT_PACKET::TYPE::STAGE_CHANGE_BOSS:
	{
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr == roomRef) {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		auto changeBossStageEvent = std::make_shared<ChangeBossStageEvent>(roomRef);
		roomRef->InsertPrevUpdateEvent(std::static_pointer_cast<PrevUpdateEvent>(changeBossStageEvent));
	}
	break;

	case CLIENT_PACKET::TYPE::TEST_GAME_END: // �ӽ÷�
	{
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr == roomRef) {
			if (PLAYER_STATE::IN_GAME == m_playerState)
				m_playerState = PLAYER_STATE::LOBBY;
			return;
		}
		roomRef->ForceGameEnd();
	}
	break;
#pragma endregion

	//�÷��̾� ���� ������ �� ������ OK -> �̶� �� ���� ���� Ȯ��
	case CLIENT_PACKET::TYPE::GAME_END_OK:
	{
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef)
			roomRef->DisconnectUser(std::static_pointer_cast<UserSession>(shared_from_this()));
		m_playerState.store(PLAYER_STATE::LOBBY);
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
		DoSend(std::make_shared<SERVER_PACKET::TimeSyncPacket>());
	}
	break;

	case CLIENT_PACKET::TYPE::STRESS_TEST_DELAY:
	{
		DoSend(std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::STRESS_TEST_DELAY)));
	}
	break;
	default:
		spdlog::critical("Recv Unknown Packet, Size: {}, Type: {}", packetHeader->size, packetHeader->type);
		//disconnect User
		break;
	}
}

void UserSession::SendExecte()
{
	m_sendOverlapped->SetIocpEvent(shared_from_this());//�ٽ� Set
	//���´� ��Ŷ ������ ����
	m_sendPacketBuffer.clear();
	//queue�� ��� ��Ŷ�� ��Ƽ� ���� ��, send
	int currentProcessSendCnt = m_prevSendPacketBufferQueueSize;
	m_prevSendPacketBufferQueueSize -= currentProcessSendCnt;

	m_sendPacketBuffer.reserve(currentProcessSendCnt);
	//ť���� ���� ���ۿ� ����
	for (int i = 0; i < currentProcessSendCnt; ++i) {
		std::shared_ptr<PacketHeader> currentPacket = nullptr;
		bool isSuccess = m_prevSendPacketBufferQueue.try_pop(currentPacket);
		if (!isSuccess) {
			m_prevSendPacketBufferQueueSize += currentProcessSendCnt - i;
			currentProcessSendCnt = i;
			break;
		}
		m_sendPacketBuffer.push_back(currentPacket);
	}

	if (0 == currentProcessSendCnt) {
		m_isAbleSend = true;
		return;
	}
	//wsaBuf ���۵��� ����
	std::vector<WSABUF> sendBuffers;
	sendBuffers.reserve(currentProcessSendCnt);
	for (auto& packetHeader : m_sendPacketBuffer) {
		WSABUF buffer;
		buffer.buf = reinterpret_cast<char*>(packetHeader.get());
		buffer.len = packetHeader->size;
		sendBuffers.push_back(std::move(buffer));
	}
	DWORD ioByte = 0;
	int sendResult = WSASend(m_socket, sendBuffers.data(), sendBuffers.size(), &ioByte, 0, m_sendOverlapped, 0);
	if (sendResult != 0) {
		int errCode = WSAGetLastError();
		if (WSA_IO_PENDING != errCode) {
			spdlog::critical("UserSession::SendExecte() - Error");
			DisplayWsaGetLastError(errCode);
			Disconnect();
			m_sendOverlapped->ResetEvent();
			return;
		}
	}
}

void UserSession::SendComplete(const DWORD& ioByte)
{
	if (CONNECT_STATE::DISCONNECTED == m_connectState) {
		m_sendOverlapped->ResetEvent();
		return;
	}
	//���� ���⼭ refCnt = 0�� �ž ������ �߻�.
	if (0 == ioByte) {
		Disconnect();
		m_sendOverlapped->ResetEvent();
		return;
	}
	if (0 == m_prevSendPacketBufferQueueSize) {
		m_isAbleSend = true;
		return;
	}
	//ť�� ��Ŷ ������ �ִٸ� ���⼭ ����
	SendExecte();
}

void UserSession::Disconnect()
{
	if (PLAYER_STATE::RECONN_FAIL == m_playerState) return;
	if (CONNECT_STATE::DISCONNECTED == m_connectState) return;
	m_connectState = CONNECT_STATE::DISCONNECTED;
	//Reconnect�� �� ������ Accept���� ���ο� ���� �Ҵ��ϱ�빮�� close
	closesocket(m_socket);
	UserManager::GetInstance().LobbyUserToDisconnectUser(std::static_pointer_cast<UserSession>(shared_from_this()));
	switch (m_playerState)
	{
		//case PLAYER_STATE::LOBBY:
		//{
		//}
		//break;
	case PLAYER_STATE::MATCH:
	{
		Matching::GetInstance().CancelMatch(std::static_pointer_cast<UserSession>(shared_from_this()), m_matchedRole);
		m_playerState = PLAYER_STATE::LOBBY;
	}
	break;
	case PLAYER_STATE::IN_GAME:
	{
		auto characterRef = m_possessCharacter.lock();
		if (nullptr != characterRef)
			characterRef->StopMove();

		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef) {
			roomRef->DisconnectUser(std::static_pointer_cast<UserSession>(shared_from_this()));
		}
		m_playerState = PLAYER_STATE::LOBBY;
	}
	break;
	default:
		break;
	}
}

void UserSession::ReconnectFail()
{
	m_playerState = PLAYER_STATE::RECONN_FAIL;
	auto reconnectFailPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::RECONN_FAIL));
	DoSend(reconnectFailPacket);
}

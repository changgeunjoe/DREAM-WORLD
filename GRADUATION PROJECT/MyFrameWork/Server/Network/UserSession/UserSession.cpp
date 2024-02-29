#include "stdafx.h"
#include "UserSession.h"
#include "../IocpEvent/IocpEventManager.h"
#include "../ExpOver/ExpOver.h"
#include "../protocol/protocol.h"
#include "../DB/DB.h"
#include "../Match/Matching.h"
#include "../Room/Room.h"

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
	//Iocp에서 ret == false
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
	//expOVer 생성 후, 현재 객체를 연결
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_RECV, shared_from_this());
	DoRecv(expOver);
}

void UserSession::DoSend(const PacketHeader* packetHeader)
{
	IocpEventManager::GetInstance().Send(m_socket, packetHeader);
}

void UserSession::SetRoomRef(std::shared_ptr<Room>& roomRef)
{
	m_roomWeakRef = roomRef;
}

void UserSession::DoRecv(ExpOver*& over)
{
	//wsaBuf length 길이 재 설정
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
			//완성된 패킷이 만들어지지 않음.
			break;
		}
		//완성된 패킷
		ExecutePacket(currentPacket);
		//남은 퍼버 크기 최신화, 현재 버퍼 위치 다음 패킷 시작 위치로
		remainSize -= currentPacket->size;
		bufferPosition = bufferPosition += currentPacket->size;
	}
	//현재 남은 데이터 크기 저장
	m_recvDataStorage.m_remainDataLength = remainSize;
	//남은 패킷 데이터가 있다면, 맨 앞으로 당기기
	if (remainSize > 0)
		std::memcpy(m_recvDataStorage.m_buffer, bufferPosition, remainSize);
	//wsaBuf의 buf 위치를 바꿈
	m_recvDataStorage.m_wsabuf.buf = m_recvDataStorage.m_buffer + remainSize;
}

void UserSession::ExecutePacket(const PacketHeader* packetHeader)
{
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
		ROLE currentRole = static_cast<ROLE>(recvPacket->Role);
		if (static_cast<ROLE>(recvPacket->Role) == ROLE::NONE_SELECT) {
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
		//weak_ptr<>::lock() 내부적으로, shared_ptr의 refCnt != 0 && Rep가 유효할 때, ptr복사, 아니면 nullptr 반환
		//RoomRef는 shared_ptr 원본은 _Rep를 수정하진 않아서 문제 없음.
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef) {
			//Room객체가 아직 유효

		}
		else {
			//유효하지 않음.

		}

	}
	break;
	case CLIENT_PACKET::TYPE::MOVE_KEY_UP:
	{
		const CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MovePacket*>(packetHeader);
		//Logic::CharacterRemoveDirection(userId, recvPacket->direction);
	}
	break;
	case CLIENT_PACKET::TYPE::STOP:
	{
		const CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::StopPacket*>(packetHeader);
		//Logic::CharacterStop(userId);
	}
	break;
	case CLIENT_PACKET::TYPE::ROTATE:
	{
		const CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::RotatePacket*>(packetHeader);
		//Logic::CharacterRotate(userId, recvPacket->axis, recvPacket->angle);
	}
	break;
#pragma endregion

#pragma region CHARACTER_ATTACK
	case CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q:
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
	case CLIENT_PACKET::TYPE::SKILL_INPUT_Q:
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
	case CLIENT_PACKET::TYPE::SKILL_INPUT_E:
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
	case CLIENT_PACKET::TYPE::PLAYER_COMMON_ATTACK_EXECUTE:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	CLIENT_PACKET::PlayerCommonAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::PlayerCommonAttackPacket*>(p);
		//	roomRef.StartAttackPlayCharacter((ROLE)recvPacket->role, recvPacket->dir, recvPacket->power);
		//}
	}
	break;
	case CLIENT_PACKET::TYPE::PLAYER_COMMON_ATTACK://애니메이션
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
	case CLIENT_PACKET::TYPE::MOUSE_INPUT:
	{
		const CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<const CLIENT_PACKET::MouseInputPacket*>(packetHeader);
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

	case CLIENT_PACKET::TYPE::TEST_GAME_END: // 임시로
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

	//플레이어 게임 끝나고 룸 나갈때 OK -> 이때 룸 삭제 여부 확인
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
	//NPC대화 관련
	case CLIENT_PACKET::TYPE::SKIP_NPC_COMMUNICATION:
	{
		//int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
		//if (roomId != -1) {
		//	Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
		//	roomRef.Recv_SkipNPC_Communication();
		//}
	}
	break;
	//클라이언트와 시간 동기화 RTT체크
	case CLIENT_PACKET::TYPE::TIME_SYNC_REQUEST:
	{
		//SERVER_PACKET::TimeSyncPacket sendPacket;
		//sendPacket.size = sizeof(SERVER_PACKET::TimeSyncPacket);
		//sendPacket.type = SERVER_PACKET::TIME_SYNC_RESPONSE;
		//sendPacket.t = std::chrono::utc_clock::now();
		//g_iocpNetwork.m_session[userId].Send(&sendPacket);
	}
	break;

	default:
		spdlog::critical("Recv Unknown Packet, Size: {}, Type: {}", packetHeader->size, packetHeader->type);
		//disconnect User
		break;
}
}


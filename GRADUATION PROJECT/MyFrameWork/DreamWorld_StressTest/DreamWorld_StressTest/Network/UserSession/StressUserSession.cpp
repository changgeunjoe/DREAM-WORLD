#include "stdafx.h"
#include "StressUserSession.h"
#include "../../../../Server/Network/protocol/protocol.h"
#include "../../Ingame/UserState/UserState.h"
#include "../NetworkModule/NetworkModule.h"
#include "../../Ingame/CoolTimeEventCtrl/EventController.h"
#include "../../Ingame/CoolTimeEventCtrl/CoolDownEventBase.h"

DreamWorld::StressUserSession::StressUserSession() : UserSession(), m_currentUserState(nullptr), m_isActive(false), m_delayTime(0), m_isAbleCheckDelay(true)
, m_x(500000), m_z(50000)
, m_currentRole(ROLE::NONE_SELECT), m_dDelayTime(0)
{
	m_coolTimeCtrl = std::make_shared<EventController>();
	m_coolTimeCtrl->InsertCoolDownEventData(MOVE, MOVE_COOL_TIME);
	m_coolTimeCtrl->InsertCoolDownEventData(DELAY_CHECK, DELAY_CHECK_COOL_TIME);
}

void DreamWorld::StressUserSession::Initialize()
{
	m_userStates.try_emplace(USER_STATE::LOBBY, std::make_shared<LobbyState>(std::static_pointer_cast<DreamWorld::StressUserSession>(shared_from_this())));
	m_userStates.try_emplace(USER_STATE::MATCH, std::make_shared<MatchState>(std::static_pointer_cast<DreamWorld::StressUserSession>(shared_from_this())));
	m_userStates.try_emplace(USER_STATE::INGAME, std::make_shared<InGameState>(std::static_pointer_cast<DreamWorld::StressUserSession>(shared_from_this())));
}

void DreamWorld::StressUserSession::SendPacketByState()
{
	if (!m_isActive) return;
	auto delayCheckCoolTime = m_coolTimeCtrl->GetEventData(DELAY_CHECK);
	if (delayCheckCoolTime->IsAbleExecute()) {
		if (!m_isAbleCheckDelay) {
			delayCheckCoolTime->ResetCoolTime();
		}
		else {
			SendDelayCheckPacket();
			m_isAbleCheckDelay = false;
		}
	}
	if (nullptr == m_currentUserState) {
		ChangeUserState(USER_STATE::LOBBY);
	}
	m_currentUserState->UpdateState();
}

void DreamWorld::StressUserSession::ExecutePacket(const PacketHeader* executePacketHeader)
{
	switch (static_cast<SERVER_PACKET::TYPE>(executePacketHeader->type))
	{
	case SERVER_PACKET::TYPE::LOGIN_SUCCESS:
	{
		auto currentTIme = TIME::now();
		auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTIme - m_loginSendTime);
		long long currentDelayTime = durationTime.count();//currentDelay

		long long diffPrevDelay = currentDelayTime - m_delayTime;//이전 딜레이와 현재 딜레이 차이

		auto prevGlobalDelay = DreamWorld::StressTestNetwork::GetInstance().globalDelay.load();
		/*if (prevGlobalDelay > 2021090) {
			std::cout << "asf" << std::endl;
		}*/
		DreamWorld::StressTestNetwork::GetInstance().globalDelay += diffPrevDelay;
		auto debugGlobalDelay = DreamWorld::StressTestNetwork::GetInstance().globalDelay.load();
		/*if (debugGlobalDelay > 2021090) {
			std::cout << "asf" << std::endl;
		}*/
		m_delayTime = currentDelayTime;

		DreamWorld::StressTestNetwork::GetInstance().IncreaseActiveClient();
		int activeNum = DreamWorld::StressTestNetwork::GetInstance().GetActiveNum();
		double averValue = 0.0f;
		averValue = (double)currentDelayTime / (double)activeNum;
		DreamWorld::StressTestNetwork::GetInstance().dGlobalDelay += (averValue - m_dDelayTime);
		m_dDelayTime = averValue;
		m_isActive = true;
	}
	break;

	case SERVER_PACKET::TYPE::RECONN_FAIL:
	{
	}
	break;

	case SERVER_PACKET::TYPE::MOVE_KEY_DOWN:
	{

	}
	break;
	case SERVER_PACKET::TYPE::MOVE_KEY_UP:
	{

	}
	break;
	case SERVER_PACKET::TYPE::ROTATE:
	{

	}
	break;
	case SERVER_PACKET::TYPE::STOP:
	{

	}
	break;

	case SERVER_PACKET::TYPE::MOUSE_INPUT:
	{

	}
	break;
	case SERVER_PACKET::TYPE::INTO_GAME:
	{
		const SERVER_PACKET::IntoGamePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::IntoGamePacket*>(executePacketHeader);
		m_currentRole = recvPacket->role;
	}
	break;

	case SERVER_PACKET::TYPE::PLAYER_DIE:
	{

	}
	break;
	case SERVER_PACKET::TYPE::PLAYER_DAMAGED:
	{

	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_DIE:
	{

	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_DAMAGED:
	{

	}
	break;
	case SERVER_PACKET::TYPE::SHOOTING_ARROW://화살
	{

	}
	break;
	case SERVER_PACKET::TYPE::SHOOTING_ICE_LANCE://아이스 렌스
	{

	}
	break;
	case SERVER_PACKET::TYPE::EXECUTE_LIGHTNING:
	{

	}
	break;
	case SERVER_PACKET::TYPE::GAME_STATE_STAGE:
	{
		const SERVER_PACKET::GameState_STAGE* recvPacket = reinterpret_cast<const SERVER_PACKET::GameState_STAGE*>(executePacketHeader);
		for (int i = 0; i < 4; ++i) {
			if (recvPacket->userState[i].role == m_currentRole) {
				m_x = recvPacket->userState[i].position.x;
				m_z = recvPacket->userState[i].position.z;
				break;
			}
		}
	}
	break;

	case SERVER_PACKET::TYPE::GAME_STATE_BOSS:
	{
		const SERVER_PACKET::GameState_BOSS* recvPacket = reinterpret_cast<const SERVER_PACKET::GameState_BOSS*>(executePacketHeader);
		for (int i = 0; i < 4; ++i) {
			if (recvPacket->userState[i].role == m_currentRole) {
				m_x = recvPacket->userState[i].position.x;
				m_z = recvPacket->userState[i].position.z;
				break;
			}
		}
	}
	break;

	case SERVER_PACKET::TYPE::SMALL_MONSTER_ATTACK:
	{

	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_MOVE:
	{

	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_STOP:
	{

	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_SET_DESTINATION:
	{

	}
	break;

	case SERVER_PACKET::TYPE::GAME_END:
	{
		m_currentRole = ROLE::NONE_SELECT;
	}
	break;

	case SERVER_PACKET::TYPE::HEAL_START:
	{

	}
	break;
	case SERVER_PACKET::TYPE::HEAL_END:
	{

	}
	break;
	case SERVER_PACKET::TYPE::SHIELD_START:
	{

	}
	break;
	case SERVER_PACKET::TYPE::SHIELD_END:
	{

	}
	break;
	case SERVER_PACKET::TYPE::NOTIFY_HEAL_HP:
	{

	}
	break;

	case SERVER_PACKET::TYPE::START_ANIMATION_Q:
	{

	}
	break;
	case SERVER_PACKET::TYPE::START_ANIMATION_E:
	{

	}
	break;

	case SERVER_PACKET::TYPE::START_STAGE_BOSS:
	{

	}
	break;

	case SERVER_PACKET::TYPE::BOSS_MOVE_DESTINATION:
	{

	}
	break;

	case SERVER_PACKET::TYPE::BOSS_ON_SAME_NODE:
	{

	}
	break;

	case SERVER_PACKET::TYPE::BOSS_ATTACK:
	{

	}
	break;

	case SERVER_PACKET::TYPE::BOSS_DIRECTION_ATTACK:
	{

	}
	break;

	case SERVER_PACKET::TYPE::BOSS_ATTACK_METEOR:
	{

	}
	break;

	case SERVER_PACKET::TYPE::NAV_MESH_RENDER:
	{

	}
	break;

	case SERVER_PACKET::TYPE::STRESS_TEST_DELAY:
	{
		static std::atomic_llong MIN_DELAY = 100;
		//여기서 딜레이 파악
		auto nowTime = Time::now();
		//현재 시간과 보낸 시간의 차이를 계산
		auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - m_lastDelaySendTime);
		long long currentDelayTime = durationTime.count();//currentDelay

		if (StressTestNetwork::GetInstance().globalMaxDelay < currentDelayTime)
			StressTestNetwork::GetInstance().globalMaxDelay = currentDelayTime;

		if (MIN_DELAY < currentDelayTime) {
			MIN_DELAY = currentDelayTime;
			std::cout << currentDelayTime << std::endl;
		}

		long long diffPrevDelay = currentDelayTime - m_delayTime;//이전 딜레이와 현재 딜레이 차이
		//빨라졌다면 globalDelay가 줄어들 것.
		auto prevGlobalDelay = DreamWorld::StressTestNetwork::GetInstance().globalDelay.load();
		/*if (prevGlobalDelay > 2021090) {
			std::cout << "asf" << std::endl;
		}*/
		DreamWorld::StressTestNetwork::GetInstance().globalDelay += diffPrevDelay;
		auto debugGlobalDelay = DreamWorld::StressTestNetwork::GetInstance().globalDelay.load();
		/*if (debugGlobalDelay > 2021090) {
			std::cout << "asf" << std::endl;
		}*/
		m_delayTime = currentDelayTime;


		int activeNum = DreamWorld::StressTestNetwork::GetInstance().GetActiveNum();
		double averValue = (double)currentDelayTime / (double)activeNum;
		DreamWorld::StressTestNetwork::GetInstance().dGlobalDelay += (averValue - m_dDelayTime);
		m_dDelayTime = averValue;
		m_isAbleCheckDelay = true;
	}
	break;

	case SERVER_PACKET::TYPE::TIME_SYNC_RESPONSE:
	{

	}
	break;

	default:
	{
		std::cout << "Unknown Packet Recv: " << (int)executePacketHeader->type << std::endl;
	}
	break;
	}
}

void DreamWorld::StressUserSession::Connect(SOCKET connectSocket)
{
	UserSession::Connect(connectSocket);
	m_loginSendTime = TIME::now();//Login 패킷 이후에 했다가, 0으로 초기화된 값이어서 딜레이 엉망
	SendLoginPacket();
}

void DreamWorld::StressUserSession::Disconnect()
{
	UserSession::Disconnect();
	DreamWorld::StressTestNetwork::GetInstance().globalDelay -= m_delayTime;
	m_delayTime = 0;
}

DIRECTION DreamWorld::StressUserSession::GetRandomDirection()
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution directionRandom(-1, 3);

	int randomDirection = directionRandom(dre);
	if (-1 == randomDirection) return DIRECTION::IDLE;

	if (0 == randomDirection) return DIRECTION::RIGHT;
	if (1 == randomDirection) return DIRECTION::BACK;
	if (2 == randomDirection) return DIRECTION::FRONT;
	return DIRECTION::LEFT;
}

ROLE DreamWorld::StressUserSession::GetRandomRole()
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution roleRandom(0, 3);
	int radomRole = roleRandom(dre);
	if (0 == radomRole) return ROLE::ARCHER;
	if (1 == radomRole) return ROLE::PRIEST;
	if (2 == radomRole) return ROLE::TANKER;
	return ROLE::WARRIOR;
}

void DreamWorld::StressUserSession::ChangeUserState(const USER_STATE& state)
{
	if (nullptr != m_currentUserState)
		m_currentUserState->ExitState();
	m_currentUserState = m_userStates[state];
	m_currentUserState->EnterState();
}

void DreamWorld::StressUserSession::SendLoginPacket()
{
	CLIENT_PACKET::LoginPacket loginPacket;
	int id = GetId();
	sprintf_s(loginPacket.id, "module%d", id);
	Send(&loginPacket);
}

void DreamWorld::StressUserSession::SendMatchPacket()
{
	//ROLE randRole = GetRandomRole();
	int role = m_id % 4;
	ROLE randRole = ROLE::WARRIOR;
	switch (role)
	{
	case 0:
		randRole = ROLE::WARRIOR;
		break;
	case 1:
		randRole = ROLE::ARCHER;
		break;
	case 2:
		randRole = ROLE::PRIEST;
		break;
	case 3:
		randRole = ROLE::TANKER;
		break;
	default:
		break;
	}
	CLIENT_PACKET::MatchPacket sendPacket(randRole, static_cast<unsigned char>(CLIENT_PACKET::TYPE::MATCH));
	Send(&sendPacket);
}

void DreamWorld::StressUserSession::SendIngamePacket()
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution rotate(0, 1);
	int randRotate = rotate(dre);
	if (randRotate) {
		SendRotatePacket();
	}
	auto moveCoolTime = m_coolTimeCtrl->GetEventData(MOVE);
	if (!moveCoolTime->IsAbleExecute()) return;

	DIRECTION randDIr = GetRandomDirection();
	if (DIRECTION::IDLE == randDIr) {
		SendStopPacket();
		return;
	}
	std::uniform_int_distribution keyDownRandom(0, 1);
	int keyDown = keyDownRandom(dre);
	SendMovePacket(randDIr, keyDown);
}

void DreamWorld::StressUserSession::SendMovePacket(const DIRECTION& direction, const bool& isApply)
{
	if (isApply) {
		CLIENT_PACKET::MovePacket sendPacket(direction, static_cast<unsigned char>(CLIENT_PACKET::TYPE::MOVE_KEY_DOWN));
		Send(&sendPacket);
		return;
	}
	CLIENT_PACKET::MovePacket sendPacket(direction, static_cast<unsigned char>(CLIENT_PACKET::TYPE::MOVE_KEY_UP));
	Send(&sendPacket);
}

void DreamWorld::StressUserSession::SendStopPacket()
{
	CLIENT_PACKET::StopPacket sendPacket;
	Send(&sendPacket);
}

void DreamWorld::StressUserSession::SendRotatePacket()
{
	CLIENT_PACKET::RotatePacket sendPacket(ROTATE_AXIS::Y, 1.0f, static_cast<unsigned char>(CLIENT_PACKET::TYPE::ROTATE));
	Send(&sendPacket);
}

void DreamWorld::StressUserSession::SendStageChangeToBoss()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::STAGE_CHANGE_BOSS));
	Send(&sendPacket);
}

void DreamWorld::StressUserSession::SendForceGameEnd()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::TEST_GAME_END));
	Send(&sendPacket);
}

void DreamWorld::StressUserSession::SendGameEndOkayPacket()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::GAME_END_OK));
	Send(&sendPacket);
}

void DreamWorld::StressUserSession::SendDelayCheckPacket()
{
	m_lastDelaySendTime = Time::now();
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<char>(CLIENT_PACKET::TYPE::STRESS_TEST_DELAY));
	Send(&sendPacket);
}

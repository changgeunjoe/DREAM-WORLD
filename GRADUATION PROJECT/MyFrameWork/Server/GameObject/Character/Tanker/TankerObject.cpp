#include "stdafx.h"
#include "TankerObject.h"
#include "../Room/Room.h"
#include "../Room/RoomEvent.h"
#include "../../EventController/CoolDownEventBase.h"
#include "../../EventController/DurationEvent.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../Timer/Timer.h"
#include "../Room/TimerRoomEvent.h"

TankerObject::TankerObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:MeleeCharacterObject(maxHp, moveSpeed, boundingSize, 50.0f, roomRef, ROLE::TANKER)
{
	m_skillCtrl->InsertDurationEventData(SKILL_Q, EventController::MS(15'000), EventController::MS(7'000));
	m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(10'000));
	/*
	m_maxHp = m_hp = 780;
	m_attackDamage = 80;
	*/
}

void TankerObject::SetStagePosition(const ROOM_STATE& roomState)
{
	if (ROOM_STATE::ROOM_COMMON == roomState)
		SetPosition(XMFLOAT3(-1260.3f, 0, -1510.7f));
	else SetPosition(XMFLOAT3(82, 0, -223.0f));
	//	SetPosition(XMFLOAT3(106.8, 0, -157));
}

void TankerObject::RecvSkill(const SKILL_TYPE& type)
{
	if (SKILL_TYPE::SKILL_TYPE_Q == type) {
		auto durationEvent = std::static_pointer_cast<DurationEvent>(m_skillCtrl->GetEventData(SKILL_Q));
		auto shieldSkill = std::make_shared<TankerSkill::ShieldSkill>(std::static_pointer_cast<TankerObject>(shared_from_this()), durationEvent->GetDurationTIme());
		m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(shieldSkill)));
	}
	else spdlog::critical("TankerObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_E");
}

void TankerObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3)
{
	if (SKILL_TYPE::SKILL_TYPE_E == type) {
		auto hammerSkill = std::make_shared<TankerSkill::ThunderHammerSkill>(std::static_pointer_cast<TankerObject>(shared_from_this()), vector3);
		m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(hammerSkill)));
	}
	else spdlog::critical("TankerObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_Q");
}

void TankerObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto tankerCommonAttackSkill = std::make_shared<TankerSkill::CommonAttack>(std::static_pointer_cast<TankerObject>(shared_from_this()), attackDir);
	m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(tankerCommonAttackSkill)));
}

void TankerObject::ExecuteHammerSkill(const XMFLOAT3& direction)
{
	static constexpr float HAMMER_SKILL_LENGTH = 55.0f;
	static constexpr float HAMMER_SKILL_VALID_RADIAN = 17.0f * 3.14f / 180.0f;
	static constexpr float DAMAGE = 100.0f;
	auto enermyData = m_roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		auto length = GetDistance(monster->GetPosition());
		if (length > HAMMER_SKILL_LENGTH) continue;
		float betweenRadian = GetBetweenAngleRadian(monster->GetPosition());
		if (betweenRadian > HAMMER_SKILL_VALID_RADIAN) {
			monster->Attacked(DAMAGE);
		}
	}
}

void TankerObject::ExecuteShield(const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
{
	using namespace std::chrono;

	static constexpr milliseconds SHIELD_APPLY_TIME = milliseconds(2400);
	auto applyEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_APPLY_SHIELD, SHIELD_APPLY_TIME, m_roomRef);
	auto removeEventEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_APPLY_SHIELD, durationTime + SHIELD_APPLY_TIME, m_roomRef);

	TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(applyEvent));
	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(removeEventEvent));
}

void TankerObject::ExecuteCommonAttack(const XMFLOAT3& attackDir)
{
	static constexpr float DEFAULT_SKILL_LENGTH = 25.0f;
	static constexpr float DEFAULT_ATTAK_VALID_RADIAN = 15.0f * 3.14f / 180.0f;
	static constexpr float DAMAGE = 60.0f;
	auto enermyData = m_roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		auto length = GetDistance(monster->GetPosition());
		if (length > DEFAULT_SKILL_LENGTH) continue;
		float betweenRadian = GetBetweenAngleRadian(monster->GetPosition());
		if (betweenRadian > DEFAULT_ATTAK_VALID_RADIAN) {
			monster->Attacked(DAMAGE);
		}
	}
}

/*

void TankerObject::RecvSkill_1(const XMFLOAT3& vec3)
{
	//쉴드
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_Q);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//notiPacket보내서 탱커 쉴드 애니메이션 수행.
		//근데 지속 시간 어떻게 넘기지?
		//Timer에 2.4초 뒤에 쉴드 적용하고 브로드 캐스트해야 됨.
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(2400), m_roomId ,EV_TANKER_SHIELD_START };	//애니메이션 진행 시간 1.4초 + 공 날아가는 시간 1초
	g_Timer.InsertTimerQueue(gameStateEvent);
	SERVER_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	sendPacket.type = SERVER_PACKET::SHIELD_START;
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);
}

void TankerObject::RecvSkill_2(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_E);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//executeHammerAttack인데. 뭐 알아서 바꾸자.
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteHammerAttack(posOrDir, m_position);
}
*/

void TankerSkill::ThunderHammerSkill::Execute()
{
	tankerRef->ExecuteHammerSkill(direction);
}

void TankerSkill::ShieldSkill::Execute()
{
	tankerRef->ExecuteShield(durationTime);
}

void TankerSkill::CommonAttack::Execute()
{
	tankerRef->ExecuteCommonAttack(direction);
}

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
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef)
			roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(shieldSkill)));
	}
	else spdlog::critical("TankerObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_E");
}

void TankerObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3)
{
	if (SKILL_TYPE::SKILL_TYPE_E == type) {
		auto hammerSkill = std::make_shared<TankerSkill::ThunderHammerSkill>(std::static_pointer_cast<TankerObject>(shared_from_this()), vector3);
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef)
			roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(hammerSkill)));
	}
	else spdlog::critical("TankerObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_Q");
}

void TankerObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr != roomRef) {
		auto tankerCommonAttackSkill = std::make_shared<TankerSkill::CommonAttack>(std::static_pointer_cast<TankerObject>(shared_from_this()), attackDir);
		roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(tankerCommonAttackSkill)));
	}
}

void TankerObject::ExecuteHammerSkill(const XMFLOAT3& direction)
{
	static constexpr float FIRST_CHECK_DISTANCE = 70.0f;
	static constexpr float HAMMER_SKILL_LENGTH = 55.0f;
	static constexpr float HAMMER_SKILL_VALID_EULER = 17.0f;
	static const float HAMMER_VALID_COS_VALUE = cos(XMConvertToRadians(HAMMER_SKILL_VALID_EULER));
	static constexpr float DAMAGE = 100.0f;

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	auto attackBoudingBox = GetMeleeAttackJudgeBox(GetPosition(), m_moveVector, HAMMER_SKILL_LENGTH / 2.0f, 7.0f, HAMMER_SKILL_LENGTH, m_collisionSphere.Radius * 2.0f);

	auto enermyData = roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		auto length = GetDistance(monster->GetPosition());
		if (length > FIRST_CHECK_DISTANCE) continue;
		if (attackBoudingBox.Intersects(monster->GetCollision()))
			monster->Attacked(DAMAGE);
		//float betweenCosValue = GetBetweenAngleCosValue(monster->GetPosition());
		//if (betweenCosValue > HAMMER_VALID_COS_VALUE) {
		//}
	}
}

void TankerObject::ExecuteShield(const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
{
	using namespace std::chrono;

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	static constexpr milliseconds SHIELD_APPLY_TIME = milliseconds(2400);
	auto applyEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_APPLY_SHIELD, SHIELD_APPLY_TIME, roomRef);
	auto removeEventEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_APPLY_SHIELD, durationTime + SHIELD_APPLY_TIME, roomRef);

	TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(applyEvent));
	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(removeEventEvent));
}

void TankerObject::ExecuteCommonAttack(const XMFLOAT3& attackDir)
{
	static constexpr float FIRST_CHECK_DISTANCE = 55.0f;
	static constexpr float DEFAULT_SKILL_LENGTH = 35.0f;
	static constexpr float DAMAGE = 60.0f;
	static constexpr float DEFAULT_ATTAK_VALID_EULER = 15.0f;
	static const float DEFAULT_ATTAK_VALID_COS_VALUE = cos(XMConvertToRadians(DEFAULT_ATTAK_VALID_EULER));

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	auto attackBoudingBox = GetMeleeAttackJudgeBox(GetPosition(), m_moveVector, DEFAULT_SKILL_LENGTH / 2.0f, 7.0f, DEFAULT_SKILL_LENGTH, m_collisionSphere.Radius * 2.0f);

	auto enermyData = roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		auto length = GetDistance(monster->GetPosition());
		if (length > FIRST_CHECK_DISTANCE) continue;
		if (attackBoudingBox.Intersects(monster->GetCollision()))
			monster->Attacked(DAMAGE);

		//float betweenCosValue = GetBetweenAngleCosValue(monster->GetPosition());
		//if (betweenCosValue > DEFAULT_ATTAK_VALID_COS_VALUE) {
		//}
	}
}

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

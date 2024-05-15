#include "stdafx.h"
#include "ArcherObject.h"
#include "../Room/Room.h"
#include "../Room/RoomEvent.h"
#include "../../EventController/EventController.h"
#include "../../EventController/CoolDownEventBase.h"
#include "../../EventController/DurationEvent.h"
#include "../Timer/Timer.h"
#include "../Room/TimerRoomEvent.h"
#include "../Room/RoomEvent.h"
#include "../GameObject/Projectile/ProjectileObject.h"
#include "../../Monster/MonsterObject.h"

ArcherObject::ArcherObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:RangedCharacterObject(maxHp, moveSpeed, boundingSize, 80.0f, roomRef, ROLE::ARCHER)
{
	m_skillCtrl->InsertCoolDownEventData(SKILL_Q, EventController::MS(10'000));
	m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(15'000));
	/*
	m_maxHp = m_hp = 400;
	m_attackDamage = 120;
	*/
}

void ArcherObject::SetStagePosition(const ROOM_STATE& roomState)
{
	if (ROOM_STATE::ROOM_COMMON == roomState)
		SetPosition(XMFLOAT3(-1340.84f, 0, -1520.93f));
	else SetPosition(XMFLOAT3(123, 0, -293));
	//SetPosition(XMFLOAT3(-193, 0, -52));
}

void ArcherObject::RecvSkill(const SKILL_TYPE& type)
{
	spdlog::critical("ArcherObject::RecvSkill(const SKILL_TYPE&) - Non Use Func");
}

void ArcherObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3)
{
	if (SKILL_TYPE::SKILL_TYPE_Q == type) {
		auto tripleArrow = std::make_shared<ArcherSKill::TripleArrow>(std::static_pointer_cast<ArcherObject>(shared_from_this()), vector3);
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef)
			roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(tripleArrow)));
	}
	else if (SKILL_TYPE::SKILL_TYPE_E == type) {
		auto rainArrow = std::make_shared<ArcherSKill::RainArrow>(std::static_pointer_cast<ArcherObject>(shared_from_this()), vector3);
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef)
			roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(rainArrow)));
	}
}

void ArcherObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto commonAttack = std::make_shared<ArcherSKill::CommonAttack>(std::static_pointer_cast<ArcherObject>(shared_from_this()), attackDir, power);
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr != roomRef)
		roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(commonAttack)));
}

void ArcherObject::ExecuteTripleArrow(const XMFLOAT3& direction)
{
	//  .
	// . . <= 이런 형태
	auto currentPosition = GetPosition();
	auto rightVector = GetRightVector();

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	for (int i = 0; i < 3; ++i) {
		XMFLOAT3 startPosition = currentPosition;
		startPosition.y = 6.0f + float(i % 2) * 4.0f;
		startPosition = Vector3::Add(startPosition, rightVector, float(1 - i) * 4.0f);
		startPosition = Vector3::Add(startPosition, direction);
		auto tripleArrowObject = std::make_shared<TripleArrowObject>(roomRef, startPosition, direction);
		roomRef->InsertProjectileObject(tripleArrowObject);

	}
	auto shootingArrowEvent = std::make_shared<ShootingArrowEvent>(direction);
	roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(shootingArrowEvent));
}

void ArcherObject::ExecuteRainArrow(const XMFLOAT3& position)
{
	using namespace std::chrono;
	static constexpr seconds SKY_ARROW_ATTACK_TIME = seconds(1);

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	m_attackRainArrowPosition = position;
	TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
	auto skyArrowEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_RAIN_ARROW_ATTACK, SKY_ARROW_ATTACK_TIME, roomRef);
	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(skyArrowEvent));
}

void ArcherObject::ExecuteCommonAttack(const XMFLOAT3& direction, const int& power)
{
	auto currentPosition = GetPosition();
	currentPosition.y += 8.0f;
	float attackDamage = CommonArrowObject::POWER_ARROW_LEVEL1_SPEED;
	float Speed = CommonArrowObject::POWER_ARROW_LEVEL1_DAMAGE;
	switch (power)
	{
	case 0:
	{
		attackDamage = CommonArrowObject::POWER_ARROW_LEVEL1_SPEED;
		Speed = CommonArrowObject::POWER_ARROW_LEVEL1_DAMAGE;
	}
	break;
	case 1:
	{
		attackDamage = CommonArrowObject::POWER_ARROW_LEVEL2_SPEED;
		Speed = CommonArrowObject::POWER_ARROW_LEVEL2_DAMAGE;
	}
	break;
	case 2:
	{
		attackDamage = CommonArrowObject::POWER_ARROW_LEVEL3_SPEED;
		Speed = CommonArrowObject::POWER_ARROW_LEVEL3_DAMAGE;
	}
	break;
	default:
		break;
	}
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;
	auto arrowObject = std::make_shared<CommonArrowObject>(Speed, roomRef, currentPosition, direction, attackDamage);
	roomRef->InsertProjectileObject(arrowObject);
	auto shootingArrowEvent = std::make_shared<ShootingArrowEvent>(direction);
	roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(shootingArrowEvent));
}

void ArcherObject::AttackRainArrow()
{
	m_attackRainArrowPosition;
	static constexpr float RAIN_ARROW_RANGE = 40.0f;
	static constexpr float RAIN_ARROW_DAMAGE = 120.0f;

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	auto enermyData = roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		float monsterDistance = monster->GetDistance(m_attackRainArrowPosition);
		if (RAIN_ARROW_RANGE > monsterDistance) {
			monster->Attacked(RAIN_ARROW_DAMAGE);
		}
	}
}

void ArcherSKill::TripleArrow::Execute()
{
	archerRef->ExecuteTripleArrow(direction);
}

void ArcherSKill::RainArrow::Execute()
{
	archerRef->ExecuteRainArrow(position);
}

void ArcherSKill::CommonAttack::Execute()
{
	archerRef->ExecuteCommonAttack(direction, power);
}

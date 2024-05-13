#include "stdafx.h"
#include "MageObject.h"
#include "../Room/Room.h"
#include "../Room/RoomEvent.h"
#include "../../EventController/EventController.h"
#include "../../EventController/CoolDownEventBase.h"
#include "../../EventController/DurationEvent.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../Timer/Timer.h"
#include "../Room/TimerRoomEvent.h"
#include "../Room/RoomEvent.h"
#include "../GameObject/Projectile/ProjectileObject.h"


MageObject::MageObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:RangedCharacterObject(maxHp, moveSpeed, boundingSize, 80.0f, roomRef, ROLE::MAGE)
{
	m_skillCtrl->InsertDurationEventData(SKILL_Q, EventController::MS(15'000), EventController::MS(10'000));
	m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(10'000));
	/*
	m_maxHp = m_hp = 500;
	m_attackDamage = 50;
	*/
}

void MageObject::SetStagePosition(const ROOM_STATE& roomState)
{
	if (ROOM_STATE::ROOM_COMMON == roomState)
		SetPosition(XMFLOAT3(-1370.45f, 0, -1450.89f));
	else SetPosition(XMFLOAT3(20, 0, -285));
	//SetPosition(XMFLOAT3(-77.7, 0, -164.7));
}

void MageObject::RecvSkill(const SKILL_TYPE& type)
{
	if (SKILL_TYPE::SKILL_TYPE_Q == type) {
		auto durationEventData = std::static_pointer_cast<DurationEvent>(m_skillCtrl->GetEventData(SKILL_Q));
		auto headlEvent = std::make_shared<MageSkill::HealSkill>(std::static_pointer_cast<MageObject>(shared_from_this()), durationEventData->GetDurationTIme());
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef)
			roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(headlEvent)));
	}
	else spdlog::critical("MageObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_E");
}

void MageObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3)
{
	if (SKILL_TYPE::SKILL_TYPE_E == type) {
		auto thunderSkillEvent = std::make_shared<MageSkill::ThunderSkill>(std::static_pointer_cast<MageObject>(shared_from_this()), vector3);
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr != roomRef)
			roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(thunderSkillEvent)));
	}
	else spdlog::critical("MageObject::RecvSkill(const SKILL_TYPE&, const XMFLOAT3&) - Non Use SKILL_Q");
}

void MageObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr != roomRef) {
		auto commonAttackEvent = std::make_shared<MageSkill::CommonAttack>(std::static_pointer_cast<MageObject>(shared_from_this()), attackDir);
		roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(commonAttackEvent)));
	}
}

void MageObject::ExecuteThunderSkill(const XMFLOAT3& position)
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;
	auto thunderEvent = std::make_shared<ThunderEvent>(position);
	roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(thunderEvent));

	static constexpr float THUNDER_RANGE = 35.0f;
	static constexpr float THUNDER_DAMAGE = 150.0f;
	auto enermyData = roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		float monsterDistance = monster->GetDistance(position);
		if (THUNDER_RANGE > monsterDistance) {
			monster->Attacked(THUNDER_DAMAGE);
		}
	}
}

void MageObject::ExecuteHeal(const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
{
	using namespace std::chrono;

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
	auto startEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_HEAL_START, 1ms, roomRef);
	auto endEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_HEAL_END, durationTime, roomRef);

	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(startEvent));
	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(endEvent));
}

void MageObject::ExecuteCommonAttack(const XMFLOAT3& attackDir)
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;
	auto currentPosition = GetPosition();
	currentPosition.y += 8.0f;
	auto iceLance = std::make_shared<IceLanceBall>(roomRef, currentPosition, attackDir);
	roomRef->InsertProjectileObject(iceLance);

	auto shootingIceLanceEvent = std::make_shared<ShootingIceLaneEvent>(attackDir);
	roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(shootingIceLanceEvent));
}

void MageSkill::ThunderSkill::Execute()
{
	mageRef->ExecuteThunderSkill(position);
}

void MageSkill::HealSkill::Execute()
{
	mageRef->ExecuteHeal(durationTime);
}

void MageSkill::CommonAttack::Execute()
{
	mageRef->ExecuteCommonAttack(direction);
}

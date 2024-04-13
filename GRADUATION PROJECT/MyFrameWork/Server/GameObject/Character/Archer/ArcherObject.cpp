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
		m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(tripleArrow)));
	}
	else if (SKILL_TYPE::SKILL_TYPE_E == type) {
		auto rainArrow = std::make_shared<ArcherSKill::RainArrow>(std::static_pointer_cast<ArcherObject>(shared_from_this()), vector3);
		m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(rainArrow)));
	}
}

void ArcherObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto commonAttack = std::make_shared<ArcherSKill::CommonAttack>(std::static_pointer_cast<ArcherObject>(shared_from_this()), attackDir, power);
	m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(commonAttack)));
	/*
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	//offset ShootArrow 내부에서 적용함

	if (power == 0) {
		roomRef.ShootArrow(attackDir, m_position, 100.0f, 80.0f);
	}
	else if (power == 1) {
		roomRef.ShootArrow(attackDir, m_position, 140.0f, 115.0f);
	}
	else if (power == 2) {
		roomRef.ShootArrow(attackDir, m_position, 200.0f, 140.0f);
	}

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_ARROW;
	sendPacket.dir = attackDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
	*/
}

void ArcherObject::ExecuteTripleArrow(const XMFLOAT3& direction)
{
	//  .
	// . . <= 이런 형태
	auto currentPosition = GetPosition();
	auto rightVector = GetRightVector();
	for (int i = 0; i < 3; ++i) {
		XMFLOAT3 startPosition = currentPosition;
		startPosition.y = 6.0f + float(i % 2) * 4.0f;
		startPosition = Vector3::Add(startPosition, rightVector, float(1 - i) * 4.0f);
		startPosition = Vector3::Add(startPosition, direction);
		auto tripleArrowObject = std::make_shared<TripleArrowObject>(m_roomRef, startPosition, direction);
		m_roomRef->InsertProjectileObject(tripleArrowObject);
	}
	auto shootingArrowEvent = std::make_shared<ShootingArrowEvent>(direction);
	m_roomRef->InsertAftrerUpdateEvent(std::static_pointer_cast<RoomSendEvent>(shootingArrowEvent));
}

void ArcherObject::ExecuteRainArrow(const XMFLOAT3& position)
{
	using namespace std::chrono;
	static constexpr seconds SKY_ARROW_ATTACK_TIME = seconds(1);
	m_attackRainArrowPosition = position;
	TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
	auto skyArrowEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_RAIN_ARROW_ATTACK, SKY_ARROW_ATTACK_TIME, m_roomRef);
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
	auto arrowObject = std::make_shared<CommonArrowObject>(Speed, m_roomRef, currentPosition, direction, attackDamage);
	m_roomRef->InsertProjectileObject(arrowObject);
	auto shootingArrowEvent = std::make_shared<ShootingArrowEvent>(direction);
	m_roomRef->InsertAftrerUpdateEvent(std::static_pointer_cast<RoomSendEvent>(shootingArrowEvent));
}

void ArcherObject::AttackRainArrow()
{
	m_attackRainArrowPosition;
	static constexpr float RAIN_ARROW_RANGE = 40.0f;
	static constexpr float RAIN_ARROW_DAMAGE = 120.0f;
	auto enermyData = m_roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		float monsterDistance = monster->GetDistance(m_attackRainArrowPosition);
		if (RAIN_ARROW_RANGE > monsterDistance) {
			monster->Attacked(RAIN_ARROW_DAMAGE);
		}
	}
}

/*
void ArcherObject::RecvSkill_1(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_Q);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//탱커 쉴드랑 같은 방식으로 동작. 타이머에서 n초있다가 적용만 아님.
	}
	//3갈래 발사
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteThreeArrow(posOrDir, m_position);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_ARROW;
	sendPacket.dir = posOrDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
}

void ArcherObject::RecvSkill_2(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_E);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//탱커 쉴드랑 같은 방식으로 동작. 타이머에서 n초있다가 적용만 아님.
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.StartSkyArrow(posOrDir);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_ARROW;
	sendPacket.dir = posOrDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
}
*/

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

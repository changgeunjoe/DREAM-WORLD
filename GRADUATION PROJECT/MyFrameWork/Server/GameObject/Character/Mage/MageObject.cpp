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
		m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(headlEvent)));
	}
	else spdlog::critical("MageObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_E");
}

void MageObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3)
{
	if (SKILL_TYPE::SKILL_TYPE_E == type) {
		auto thunderSkillEvent = std::make_shared<MageSkill::ThunderSkill>(std::static_pointer_cast<MageObject>(shared_from_this()), vector3);
		m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(thunderSkillEvent)));
	}
	else spdlog::critical("MageObject::RecvSkill(const SKILL_TYPE&, const XMFLOAT3&) - Non Use SKILL_Q");
}

void MageObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto commonAttackEvent = std::make_shared<MageSkill::CommonAttack>(std::static_pointer_cast<MageObject>(shared_from_this()), attackDir);
	m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(commonAttackEvent)));
	//room에 있는 볼 날라기인데, 이거 바꿔야 할거같은데...
	/*
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	//offset 적용 안됨
	roomRef.ShootBall(attackDir, m_position);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_BALL;
	sendPacket.dir = attackDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::PRIEST, &sendPacket);
	*/
}

void MageObject::ExecuteThunderSkill(const XMFLOAT3& position)
{
	auto thunderEvent = std::make_shared<ThunderEvent>(position);
	m_roomRef->InsertAftrerUpdateEvent(std::static_pointer_cast<RoomSendEvent>(thunderEvent));

	static constexpr float THUNDER_RANGE = 35.0f;
	static constexpr float THUNDER_DAMAGE = 150.0f;
	auto enermyData = m_roomRef->GetEnermyData();
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
	TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
	auto startEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_HEAL_START, 1ms, m_roomRef);
	auto endEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_HEAL_END, durationTime, m_roomRef);

	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(startEvent));
	timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(endEvent));
}

void MageObject::ExecuteCommonAttack(const XMFLOAT3& attackDir)
{
	auto currentPosition = GetPosition();
	currentPosition.y += 8.0f;
	auto iceLance = std::make_shared<IceLanceBall>(m_roomRef, currentPosition, attackDir);
	m_roomRef->InsertProjectileObject(iceLance);

	auto shootingIceLanceEvent = std::make_shared<ShootingIceLaneEvent>(attackDir);
	m_roomRef->InsertAftrerUpdateEvent(std::static_pointer_cast<RoomSendEvent>(shootingIceLanceEvent));
}

/*
void MageObject::RecvSkill_1(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_Q);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//탱커 쉴드랑 같은 방식으로 동작. 타이머에서 n초있다가 적용만 아님.
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	g_RoomManager.GetRunningRoomRef(m_roomId).StartHealPlayerCharacter();
}

void MageObject::RecvSkill_2(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_E);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//탱커 쉴드랑 같은 방식으로 동작. 타이머에서 n초있다가 적용만 아님.
	}
	//번개 나오는거.
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteMageThunder(posOrDir);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::EXECUTE_LIGHTNING;
	sendPacket.dir = posOrDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::PRIEST, &sendPacket);
}
*/

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

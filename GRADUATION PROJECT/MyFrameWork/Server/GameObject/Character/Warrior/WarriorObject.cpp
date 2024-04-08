#include "stdafx.h"
#include "WarriorObject.h"
#include "../Room/Room.h"
#include "../Room/RoomEvent.h"

WarriorObject::WarriorObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:MeleeCharacterObject(maxHp, moveSpeed, boundingSize, 100.0f, roomRef, ROLE::WARRIOR)
{
	m_commonAttackDamage = 110.0f;
	m_skillCtrl->InsertCoolDownEventData(SKILL_Q, EventController::MS(7'000));
}

void WarriorObject::SetStagePosition(const ROOM_STATE& roomState)
{
	if (ROOM_STATE::ROOM_COMMON == roomState)
		SetPosition(XMFLOAT3(-1290.0f, 0, -1470.0f));
	else SetPosition(XMFLOAT3(0, 0, -211.0f));
	//SetPosition(XMFLOAT3(118, 0, 5));
}

void WarriorObject::RecvSkill(const SKILL_TYPE& skillType)
{
	spdlog::critical("WarriorObject::RecvSkill(const SKILL_TYPE &) - Non use Func");
}

void WarriorObject::RecvSkill(const SKILL_TYPE& skillType, const XMFLOAT3& vector3)
{
	if (SKILL_TYPE::SKILL_TYPE_Q == skillType) {
		auto longSwordSkill = std::make_shared<WarriorSkill::LongSwordSkill>(std::static_pointer_cast<WarriorObject>(shared_from_this()), vector3);
		//m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(longSwordSkill));
	}
}

void WarriorObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto warriorCommonAttackSkill = std::make_shared<WarriorSkill::CommonAttack>(std::static_pointer_cast<WarriorObject>(shared_from_this()), attackDir, power);
	//m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(warriorCommonAttackSkill));
}

void WarriorObject::ExecuteSwordSkill(const XMFLOAT3& direction)
{
	
}

void WarriorObject::ExecuteCommonAttack(const XMFLOAT3& direction, const int& power)
{
}

/*
void WarriorObject::RecvSkill_1(const XMFLOAT3& vec3)
{
	//auto skillCoolData = m_skillCtrl->GetEventData(SKILL_Q);
	//bool isAble = skillCoolData->IsAbleExecute();
	//long Sword
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	roomRef.ExecuteLongSwordAttack(posOrDir, m_position);
}

void WarriorObject::RecvSkill_2(const XMFLOAT3& vec3)
{
	//passive스킬인데, 이 함수는 쓸모 없는데, 스킬을 나중에 정해서 생긴 문제.
	//설계단계에서 생각하지 못했음.
}
*/

void WarriorSkill::LongSwordSkill::Execute()
{
}

void WarriorSkill::CommonAttack::Execute()
{
}

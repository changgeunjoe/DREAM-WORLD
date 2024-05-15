#include "stdafx.h"
#include "WarriorObject.h"
#include "../Room/Room.h"
#include "../Room/RoomEvent.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Monster/SmallMonsterObject.h"
//#include "../GameObject/Monster/BossMonsterObject.h"

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
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	if (SKILL_TYPE::SKILL_TYPE_Q == skillType) {
		auto longSwordSkill = std::make_shared<WarriorSkill::LongSwordSkill>(std::static_pointer_cast<WarriorObject>(shared_from_this()), vector3);
		roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(longSwordSkill)));
	}
}

void WarriorObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	auto warriorCommonAttackSkill = std::make_shared<WarriorSkill::CommonAttack>(std::static_pointer_cast<WarriorObject>(shared_from_this()), attackDir, power);
	roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(warriorCommonAttackSkill)));
}

void WarriorObject::ExecuteSwordSkill(const XMFLOAT3& direction)
{
	static constexpr float FIRST_CHECK_DISTANCE = 70.0;
	static constexpr float SWORD_SKILL_LENGTH = 45.0f;
	static constexpr float DAMAGE = 110.0f;

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	//static constexpr float SWORD_SKILL_VALID_EULER = 15.0f;
	//static const float SWORD_VALID_COS_VALUE = cos(XMConvertToRadians(SWORD_SKILL_VALID_EULER));
	auto attackBoudingBox = GetMeleeAttackJudgeBox(GetPosition(), m_moveVector, SWORD_SKILL_LENGTH / 2.0f, 6.0f, SWORD_SKILL_LENGTH, m_collisionSphere.Radius * 2.0f);


	auto enermyData = roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		auto length = GetDistance(monster->GetPosition());
		if (length > FIRST_CHECK_DISTANCE) continue;
		bool isAttacked = attackBoudingBox.Intersects(monster->GetCollision());
		if (isAttacked)
			monster->Attacked(DAMAGE);
		//float betweenCosValue = GetBetweenAngleCosValue(monster->GetPosition());
		//if (betweenCosValue > SWORD_VALID_COS_VALUE) {
		//}
	}
}

void WarriorObject::ExecuteCommonAttack(const XMFLOAT3& direction, const int& power)
{
	static constexpr float DEFAULT_DAMAGE = 50.0f;
	static constexpr float POWER_DAMAGE = 70.0f;
	static constexpr float LAST_POWER_DAMAGE = 110.0f;

	static constexpr float FIRST_CHECK_DISTANCE = 85.0;
	static constexpr float ATTACK_LENGTH = 60.0f;
	//static constexpr float ATTACK_VALID_EULER = 18.0f;
	//static const float ATTACK_VALID_COS_VALUE = cos(XMConvertToRadians(ATTACK_VALID_EULER));

	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;

	float applyDamage = DEFAULT_DAMAGE;
	switch (power)
	{
	case 0:
		applyDamage = DEFAULT_DAMAGE;
		break;
	case 1:
		applyDamage = POWER_DAMAGE;
		break;
	case 2:
		applyDamage = LAST_POWER_DAMAGE;
		break;
	default:
	{
		spdlog::warn("WarriorObject::ExecuteCommonAttack() - Unknown Power: {}", power);
	}
	break;
	}

	auto enermyData = roomRef->GetEnermyData();
	auto attackBoudingBox = GetMeleeAttackJudgeBox(GetPosition(), m_moveVector, ATTACK_LENGTH / 2.0f, 7.0f, ATTACK_LENGTH, m_collisionSphere.Radius * 2.0f);
	for (auto& monster : enermyData) {
		auto length = GetDistance(monster->GetPosition());
		if (length > FIRST_CHECK_DISTANCE) continue;
		bool isAttatked = attackBoudingBox.Intersects(monster->GetCollision());
		if (isAttatked)
			monster->Attacked(applyDamage);
		//float betweenCosValue = GetBetweenAngleCosValue(monster->GetPosition());
		//if (betweenCosValue > ATTACK_VALID_COS_VALUE) {
		//}
	}
}

void WarriorSkill::LongSwordSkill::Execute()
{
	warriorRef->ExecuteSwordSkill(direction);
}

void WarriorSkill::CommonAttack::Execute()
{
	warriorRef->ExecuteCommonAttack(direction, power);
}

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
	if (SKILL_TYPE::SKILL_TYPE_Q == skillType) {
		auto longSwordSkill = std::make_shared<WarriorSkill::LongSwordSkill>(std::static_pointer_cast<WarriorObject>(shared_from_this()), vector3);
		m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(longSwordSkill)));
	}
}

void WarriorObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto warriorCommonAttackSkill = std::make_shared<WarriorSkill::CommonAttack>(std::static_pointer_cast<WarriorObject>(shared_from_this()), attackDir, power);
	m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(std::static_pointer_cast<PlayerSkillBase>(warriorCommonAttackSkill)));
}

void WarriorObject::ExecuteSwordSkill(const XMFLOAT3& direction)
{
	static constexpr float SWORD_SKILL_LENGTH = 30.0f;
	static constexpr float SWORD_SKILL_VALID_RADIAN = 15.0f * 3.14f / 180.0f;
	static constexpr float DAMAGE = 110.0f;
	auto enermyData = m_roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		auto length = GetDistance(monster->GetPosition());
		if (length > SWORD_SKILL_LENGTH) continue;
		float betweenRadian = GetBetweenAngleRadian(monster->GetPosition());
		if (betweenRadian > SWORD_SKILL_VALID_RADIAN) {
			monster->Attacked(DAMAGE);
		}
	}
}

void WarriorObject::ExecuteCommonAttack(const XMFLOAT3& direction, const int& power)
{
	static constexpr float ATTACK_LENGTH = 30.0f;
	static constexpr float ATTACK_VALID_RADIAN = 18.0f * 3.14f / 180.0f;
	static constexpr float DEFAULT_DAMAGE = 50.0f;

	static constexpr float POWER_DAMAGE = 70.0f;
	static constexpr float LAST_POWER_DAMAGE = 110.0f;

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
	auto enermyData = m_roomRef->GetEnermyData();
	for (auto& monster : enermyData) {
		auto length = GetDistance(monster->GetPosition());
		if (length > ATTACK_LENGTH) continue;
		float betweenRadian = GetBetweenAngleRadian(monster->GetPosition());
		if (betweenRadian > ATTACK_VALID_RADIAN) {
			monster->Attacked(applyDamage);
		}
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

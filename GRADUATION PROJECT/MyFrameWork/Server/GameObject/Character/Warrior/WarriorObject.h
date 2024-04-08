#pragma once
#include "../PCH/stdafx.h"
#include "../ChracterObject.h"
#include "../PlayerSkill.h"

class WarriorObject;
namespace WarriorSkill
{
	//워리어 캐릭터 스킬
	//방향성 있는 스킬 하나 뿐이기 하나만 제작
	class DirectionSkill :public DirectionSkillBase
	{
	public:
		DirectionSkill(std::shared_ptr<WarriorObject> warriorRef, const XMFLOAT3& direction) : DirectionSkillBase(direction), warriorRef(warriorRef) {}
	protected:
		std::shared_ptr<WarriorObject> warriorRef;
	};

	//실제 구현될 공격 스킬
	class LongSwordSkill : public DirectionSkill
	{
	public:
		LongSwordSkill(std::shared_ptr<WarriorObject> warriorRef, const XMFLOAT3& direction) : DirectionSkill(warriorRef, direction) {}
		virtual void Execute() override;
	};

	class CommonAttack : public DirectionSkill//워리어는 연속형 공격
	{
	public:
		CommonAttack(std::shared_ptr<WarriorObject> warriorRef, const XMFLOAT3& direction, const int& power) : DirectionSkill(warriorRef, direction), power(power) {}
		virtual void Execute() override;
	protected:
		int power;
	};

}

class WarriorObject : public MeleeCharacterObject
{
public:
	WarriorObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~WarriorObject() = default;
public:
	virtual void SetStagePosition(const ROOM_STATE& roomState)override;

	virtual void RecvSkill(const SKILL_TYPE&) override;
	virtual void RecvSkill(const SKILL_TYPE&, const XMFLOAT3& vector3) override;
	virtual void RecvAttackCommon(const XMFLOAT3& attackDir, const int& power = 0) override;

	void ExecuteSwordSkill(const XMFLOAT3& direction);
	void ExecuteCommonAttack(const XMFLOAT3& direction, const int& power);
};


#pragma once
#include "../PCH/stdafx.h"
#include "../ChracterObject.h"
#include "../PlayerSkill.h"

class WarriorObject;
namespace WarriorSkill
{
	//������ ĳ���� ��ų
	//���⼺ �ִ� ��ų �ϳ� ���̱� �ϳ��� ����
	class DirectionSkill :public DirectionSkillBase
	{
	public:
		DirectionSkill(std::shared_ptr<WarriorObject> warriorRef, const XMFLOAT3& direction) : DirectionSkillBase(direction), warriorRef(warriorRef) {}
	protected:
		std::shared_ptr<WarriorObject> warriorRef;
	};

	//���� ������ ���� ��ų
	class LongSwordSkill : public DirectionSkill
	{
	public:
		LongSwordSkill(std::shared_ptr<WarriorObject> warriorRef, const XMFLOAT3& direction) : DirectionSkill(warriorRef, direction) {}
		virtual void Execute() override;
	};

	class CommonAttack : public DirectionSkill//������� ������ ����
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


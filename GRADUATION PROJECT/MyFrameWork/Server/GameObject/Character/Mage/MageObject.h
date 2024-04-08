#pragma once
#include "../PCH/stdafx.h"
#include "../ChracterObject.h"
#include "../PlayerSkill.h"

class MageObject;
namespace MageSkill
{
	//어떤 위치에 번개, 힐 스킬 => 2개
	class MageRef
	{
	public:
		MageRef(std::shared_ptr<MageObject> mageRef) : mageRef(mageRef) {}
	protected:
		std::shared_ptr<MageObject> mageRef;
	};

	class PositionSkill : public PositionSkillBase, public MageRef
	{
	public:
		PositionSkill(std::shared_ptr<MageObject> mageRef, const XMFLOAT3& position) : MageRef(mageRef), PositionSkillBase(position) {}
	};

	class DurationSKill : public CommonDurationSkill_MILSEC, public MageRef
	{
	public:
		DurationSKill(std::shared_ptr<MageObject> mageRef, const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
			:MageRef(mageRef), CommonDurationSkill_MILSEC(durationTime) {}
	};

	class DirectionSkill : public DirectionSkillBase, public MageRef
	{
	public:
		DirectionSkill(std::shared_ptr<MageObject> mageRef, const XMFLOAT3& direction) : MageRef(mageRef), DirectionSkillBase(direction) {}
	};

	class ThunderSkill :public PositionSkill
	{
	public:
		ThunderSkill(std::shared_ptr<MageObject> mageRef, const XMFLOAT3& position) : PositionSkill(mageRef, position) {}
		virtual void Execute() override;
	};

	class HealSkill : public DurationSKill
	{
	public:
		HealSkill(std::shared_ptr<MageObject> mageRef, const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
			:DurationSKill(mageRef, durationTime) {}
		virtual void Execute() override;
	};

	class CommonAttack : DirectionSkill//연속형 공격없이 그대로 공격하면 됨.
	{
		CommonAttack(std::shared_ptr<MageObject> mageRef, const XMFLOAT3& direction) : DirectionSkill(mageRef, direction) {}
		virtual void Execute() override;
	};
}

class MageObject : public RangedCharacterObject
{
public:
	MageObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~MageObject() = default;
public:
	virtual void SetStagePosition(const ROOM_STATE& roomState)override;

	virtual void RecvSkill(const SKILL_TYPE&) override;
	virtual void RecvSkill(const SKILL_TYPE&, const XMFLOAT3& vector3) override;
	virtual void RecvAttackCommon(const XMFLOAT3& attackDir, const int& power = 0) override;

	void ExecuteThunderSkill1(const XMFLOAT3& position);
	void ExecuteHeal();
	void ExecuteCommonAttack(const XMFLOAT3& attackDir);
};

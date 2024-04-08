#pragma once
#include "../PCH/stdafx.h"
#include "../ChracterObject.h"
#include "../PlayerSkill.h"

class TankerObject;
namespace TankerSkill
{
	//탱커 스킬은 쉴드 스킬, 햄머 앞으로 쾅 스킬 총 2개인데, 다른 결의 스킬
	class TankerRef
	{
	public:
		TankerRef(std::shared_ptr<TankerObject> tankerRef) : tankerRef(tankerRef) {}
	protected:
		std::shared_ptr<TankerObject> tankerRef;
	};

	class CommonDurationSkill :public CommonDurationSkill_MILSEC, public TankerRef
	{
	public:
		CommonDurationSkill(std::shared_ptr<TankerObject> tankerRef, const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
			: TankerRef(tankerRef), CommonDurationSkill_MILSEC(durationTime) {}
	};

	class DirectionSkill : public DirectionSkillBase, public TankerRef
	{
	public:
		DirectionSkill(std::shared_ptr<TankerObject> tankerRef, const XMFLOAT3& direction) : TankerRef(tankerRef), DirectionSkillBase(direction) {}
	};

	class ThunderHammerSkill : public DirectionSkill
	{
	public:
		ThunderHammerSkill(std::shared_ptr<TankerObject> tankerRef, const XMFLOAT3& direction) : DirectionSkill(tankerRef, direction) {}
		virtual void Execute() override;
	};

	class ShieldSkill : public CommonDurationSkill
	{
	public:
		ShieldSkill(std::shared_ptr<TankerObject> tankerRef, const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
			: CommonDurationSkill(tankerRef, durationTime) {}
		virtual void Execute() override;
	};

	class CommonAttack : DirectionSkill//연속형 공격없이 그대로 공격하면 됨.
	{
	public:
		CommonAttack(std::shared_ptr<TankerObject> tankerRef, const XMFLOAT3& direction) : DirectionSkill(tankerRef, direction) {}
		virtual void Execute() override;
	};
}

class TankerObject : public MeleeCharacterObject
{
public:
	TankerObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~TankerObject() = default;
public:
	virtual void SetStagePosition(const ROOM_STATE& roomState)override;

	virtual void RecvSkill(const SKILL_TYPE&) override;
	virtual void RecvSkill(const SKILL_TYPE&, const XMFLOAT3& vector3) override;
	virtual void RecvAttackCommon(const XMFLOAT3& attackDir, const int& power) override;

	void ExecuteHammerSkill(const XMFLOAT3& direction);
	void ExecuteShield();
	void ExecuteCommonAttack(const XMFLOAT3& attackDir);
};

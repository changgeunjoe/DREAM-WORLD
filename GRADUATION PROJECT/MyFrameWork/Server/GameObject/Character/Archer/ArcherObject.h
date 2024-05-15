#pragma once
#include "../PCH/stdafx.h"
#include "../ChracterObject.h"
#include "../PlayerSkill.h"

class ArcherObject;
namespace ArcherSKill
{
	class ArcherRef
	{
	public:
		ArcherRef(std::shared_ptr<ArcherObject> archerRef) : archerRef(archerRef) {}
	protected:
		std::shared_ptr<ArcherObject> archerRef;
	};

	class DirectionSkill : public DirectionSkillBase, public ArcherRef
	{
	public:
		DirectionSkill(std::shared_ptr<ArcherObject> archerRef, const XMFLOAT3& direction)
			:ArcherRef(archerRef), DirectionSkillBase(direction) {}
	};

	class PositionSkill : public PositionSkillBase, public ArcherRef
	{
	public:
		PositionSkill(std::shared_ptr<ArcherObject> archerRef, const XMFLOAT3& position)
			:ArcherRef(archerRef), PositionSkillBase(position) {}
	};

	class TripleArrow : public DirectionSkill
	{
	public:
		TripleArrow(std::shared_ptr<ArcherObject> archerRef, const XMFLOAT3& direction)
			:DirectionSkill(archerRef, direction) {}
		virtual void Execute() override;
	};

	class RainArrow : public PositionSkill
	{
	public:
		RainArrow(std::shared_ptr<ArcherObject> archerRef, const XMFLOAT3& position)
			:PositionSkill(archerRef, position) {}
		virtual void Execute() override;
	};

	class CommonAttack : public DirectionSkill//아쳐는 줌인으로 강화된 공격을 할 수 있음.
	{
	public:
		CommonAttack(std::shared_ptr<ArcherObject> archerRef, const XMFLOAT3& direction, const int& power) : DirectionSkill(archerRef, direction), power(power) {}
		virtual void Execute() override;
	protected:
		int power;
	};

}

class ArcherObject : public RangedCharacterObject
{
public:
	ArcherObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~ArcherObject() = default;
public:
	virtual void SetStagePosition(const ROOM_STATE& roomState)override;
	
	virtual void RecvSkill(const SKILL_TYPE&) override;
	virtual void RecvSkill(const SKILL_TYPE&, const XMFLOAT3& vector3) override;
	virtual void RecvAttackCommon(const XMFLOAT3& attackDir, const int& power) override;

	void ExecuteTripleArrow(const XMFLOAT3& direction);
	void ExecuteRainArrow(const XMFLOAT3& position);
	void ExecuteCommonAttack(const XMFLOAT3& direction, const int& power);

	void AttackRainArrow();
private:
	XMFLOAT3 m_attackRainArrowPosition;
};

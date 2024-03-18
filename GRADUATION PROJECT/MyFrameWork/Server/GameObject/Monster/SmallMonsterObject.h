#pragma once
#include "../../PCH/stdafx.h"
#include "MonsterObject.h"

class SmallMonsterObject : public MonsterObject
{
public:
	SmallMonsterObject() = delete;
	SmallMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const int& idx);
	~SmallMonsterObject() = default;

	virtual void Update() override;
	const int GetIdx() const
	{
		return m_idx;
	}
protected:
	virtual std::optional<std::shared_ptr<GameObject>> GetAggroCharacter() override;
	virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) override;
	//객체가 우측이라면 1 아니라면 -1, 사이 각도(라디안)
	std::optional<const std::pair<char, float>> GetAggroBetweenAngle();
	virtual const bool isAbleAttackRange() override;
	virtual void Attack(std::shared_ptr<LiveObject>& attackedCharacter) override;
protected:
	int m_idx;
	std::weak_ptr<GameObject> m_aggroCharacter;
};
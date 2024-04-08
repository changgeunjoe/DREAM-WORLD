#pragma once
#include "../../PCH/stdafx.h"
#include "MonsterObject.h"

class SmallMonsterObject : public MonsterObject
{
private:
	static constexpr float MONSTER_DAMAGE = 30.0f;
	static constexpr std::string_view UPDATE_DESTINATION = "UPDATE_DESTINATION";
public:
	SmallMonsterObject() = delete;
	SmallMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const int& idx);
	~SmallMonsterObject() = default;

	virtual void Update() override;
	virtual void Attacked(const float& damage) override;

	const int GetIdx() const
	{
		return m_idx;
	}
protected:
	std::shared_ptr<CharacterObject> GetAggroCharacter();
	virtual std::shared_ptr<CharacterObject> FindAggroCharacter() override;

	virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) override;
	//객체가 우측이라면 1 아니라면 -1, 사이 각도(라디안)

	const std::pair<float, float> GetAggroBetweenAngle();
	const float GetBetweenAngleRadian(const XMFLOAT3& position);

	void UpdateDestinationPosition();
	void SetMove();
	void SetStop();

	void Move();

	std::optional<const XMFLOAT3>  UpdateNextPosition(const float& elapsedTime);
	std::optional<std::pair<bool, XMFLOAT3>> CollideWall(const XMFLOAT3& nextPosition, const float& elapsedTime, const bool& isSlidingPosition);
	const bool IsAbleAttack();
	virtual void Attack() override;
protected:
	int m_idx;

	float m_attackDamage = 30.0f;

	//MILLISECS m_lastSendDestinationSendTime;
	XMFLOAT3 m_destinationPosition;
	std::weak_ptr<CharacterObject> m_aggroCharacter;
};

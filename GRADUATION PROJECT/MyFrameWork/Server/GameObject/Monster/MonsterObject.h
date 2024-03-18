#pragma once
#include "../../PCH/stdafx.h"
#include "../GameObject.h"

class MonsterObject : public LiveObject {
public:
	MonsterObject() = delete;
	MonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~MonsterObject() = default;

protected:
	//void StartMove();
	//void StopMove();

	void SetDestination(const XMFLOAT3& destinationPosition);
	const XMFLOAT3 GetDestinationPosition() const;
	const bool isReadyFindPlayer();
	virtual std::optional<std::shared_ptr<GameObject>> GetAggroCharacter() = 0;
	const bool isReadyAttack();
	virtual const bool isAbleAttackRange() = 0;
	virtual void Attack(std::shared_ptr<LiveObject>& attackedCharacter) = 0;
private:
	std::chrono::high_resolution_clock::time_point m_lastAttackTime;
	std::chrono::high_resolution_clock::time_point m_lastFindAggroTime;

	std::chrono::seconds m_coolTimeFindPlayer;
	std::chrono::seconds m_coolTimeAttack;

	std::atomic_bool m_isMove;
};

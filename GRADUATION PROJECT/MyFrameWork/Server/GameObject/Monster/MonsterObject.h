#pragma once
#include "../../PCH/stdafx.h"
#include "../GameObject.h"
#include "../EventController/EventController.h"

class CharacterObject;
class MonsterObject : public LiveObject
{
protected:
	static constexpr std::string_view FIND_PLAYER = "FIND_PLAYER";
	static constexpr std::string_view ATTACK_PLAYER = "ATTACK_PLAYER";

public:
	MonsterObject() = delete;
	MonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~MonsterObject() = default;

protected:
	//void StartMove();
	//void StopMove();
	const bool isReadyFindPlayer();

	virtual std::shared_ptr<CharacterObject> FindAggroCharacter() = 0;

	const bool isReadyAttack();
	//virtual const bool isAbleAttackRange() = 0;
	virtual void Attack() = 0;

protected:
	std::unique_ptr<EventController> m_behaviorTimeEventCtrl;
	std::atomic_bool m_isMove;
};

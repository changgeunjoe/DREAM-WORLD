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
	const bool IsReadyFindPlayer();

	virtual std::shared_ptr<CharacterObject> FindAggroCharacter() = 0;
	//방향, 각도
	const std::pair<float, float> GetBetweenAngleEuler(const XMFLOAT3& otherPosition) const;
	const bool IsReadyAttack();

protected:
	std::unique_ptr<EventController> m_behaviorTimeEventCtrl;
	std::atomic_bool m_isMove;
};

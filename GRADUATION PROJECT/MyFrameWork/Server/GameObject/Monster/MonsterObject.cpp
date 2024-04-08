#include "stdafx.h"
#include "MonsterObject.h"
#include "../EventController/CoolDownEventBase.h"
#include "../EventController/DurationEvent.h"

MonsterObject::MonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, const float& attackDamage, std::shared_ptr<Room>& roomRef)
	: LiveObject(maxHp, moveSpeed, boundingSize, roomRef)
{
	m_behaviorTimeEventCtrl = std::make_unique<EventController>();
}

const bool MonsterObject::isReadyFindPlayer()
{
	auto findPlayerEvent = m_behaviorTimeEventCtrl->GetEventData(FIND_PLAYER);
	const bool isReady = findPlayerEvent->IsAbleExecute();
	return isReady;
}

const bool MonsterObject::isReadyAttack()
{
	auto attackEvent = m_behaviorTimeEventCtrl->GetEventData(ATTACK_PLAYER);
	const bool isReady = attackEvent->IsAbleExecute();
	return isReady;
}

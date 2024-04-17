#include "stdafx.h"
#include "BossMonsterObject.h"
#include "../Room/Room.h"
#include "../Character/ChracterObject.h"
#include "../Room/RoomEvent.h"




BossMonsterObject::BossMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:MonsterObject(maxHp, moveSpeed, boundingSize, roomRef)
{
	//200ms마다 길 찾기 재수행
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(RESEARCH_ROAD, EventController::MS(200));
	//5초마다 어그로 대상 변경
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(FIND_PLAYER, EventController::MS(5000));

	m_behaviorTimeEventCtrl->InsertCoolDownEventData(ATTACK_PLAYER, EventController::MS(1100));
}

void BossMonsterObject::Update()
{
}

void BossMonsterObject::UpdateAggro()
{
	m_aggroCharacter = FindAggroCharacter();
	UpdateRoad();
}

void BossMonsterObject::UpdateRoad()
{
}

std::shared_ptr<CharacterObject> BossMonsterObject::FindAggroCharacter()
{
	auto characters = m_roomRef->GetCharacters();
	std::shared_ptr<CharacterObject> minDistanceObject = nullptr;
	float minDistance = 0.0f;
	for (auto& character : characters) {
		if (!character->IsAlive()) continue;
		//character->GetPosition();
		float distance = character->GetDistance(shared_from_this());
		if (nullptr == minDistanceObject || minDistance > distance) {//아직 정해진 최소 거리 객체가 없거나, 최소거리보다 더 짧은 거리일 때
			minDistanceObject = character;
			minDistance = distance;
		}
	}
	return minDistanceObject;
}

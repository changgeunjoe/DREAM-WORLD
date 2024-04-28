#include "stdafx.h"
#include "SmallMonsterObject.h"
#include "../Room/Room.h"
#include "../Character/ChracterObject.h"
#include "../EventController/CoolDownEventBase.h"
#include "../EventController/DurationEvent.h"
#include "../Room/RoomEvent.h"
#include "../MapData/MapData.h"
#include "../MapData/MapCollision/MapCollision.h"
//#include "SmallMonsterSessionObject.h"

SmallMonsterObject::SmallMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const int& idx)
	:MonsterObject(maxHp, moveSpeed, boundingSize, roomRef), m_idx(idx)
{
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(FIND_PLAYER, EventController::MS(600));
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(ATTACK_PLAYER, EventController::MS(2500));
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(UPDATE_DESTINATION, EventController::MS(200));

}

void SmallMonsterObject::Update()
{
	if (!m_isAlive) {
		UpdateLastUpdateTime();
		return;
	}

	bool ableAttack = IsAbleAttack();
	if (ableAttack) {
		Attack();
		UpdateLastUpdateTime();
		return;
	}
	auto currentAggroCharacter = GetAggroCharacter();
	if (nullptr == currentAggroCharacter) {
		UpdateLastUpdateTime();
		SetStop();
		return;
	}

	UpdateDestinationPosition();//일정 시간이 되면 알아서 목적지 바뀜
	SetMove();
	Move();
}

void SmallMonsterObject::Attacked(const float& damage)
{
	if (!m_isAlive) return;
	m_hp -= damage;
	auto damagedEvent = std::make_shared<SmallMonsterDamagedEvent>(m_idx, m_hp);
	m_roomRef->InsertAftrerUpdateSendEvent(damagedEvent);
	if (m_hp <= 0) {
		m_hp = 0;
		auto dieEvent = std::make_shared<SmallMonsterDieEvent>(m_idx);
		m_roomRef->InsertAftrerUpdateSendEvent(dieEvent);
		m_isAlive = false;
	}
}

std::shared_ptr<CharacterObject> SmallMonsterObject::GetAggroCharacter()
{
	if (IsReadyFindPlayer()) {
		auto aggroCharacter = FindAggroCharacter();
		m_aggroCharacter = aggroCharacter;
		if (nullptr != aggroCharacter)
			m_destinationPosition = aggroCharacter->GetPosition();
	}
	return m_aggroCharacter.lock();
}

std::shared_ptr<CharacterObject> SmallMonsterObject::FindAggroCharacter()
{
	static constexpr float AGGRO_RANGE = 170.0f;
	auto characters = m_roomRef->GetCharacters();
	std::shared_ptr<CharacterObject> minDisGameObject(nullptr);
	float minDistance = 0.0f;
	for (auto character : characters) {
		const float distance = character->GetDistance(shared_from_this());
		if (nullptr == minDisGameObject) {
			minDisGameObject = character;
			minDistance = distance;
		}
		else {
			if (distance < minDistance) {
				minDisGameObject = character;
				minDistance = distance;
			}
		}
	}
	if (AGGRO_RANGE > minDistance)
		return minDisGameObject;
	return nullptr;
}

void SmallMonsterObject::UpdateDestinationPosition()
{
	auto updateDestinationEvent = m_behaviorTimeEventCtrl->GetEventData(UPDATE_DESTINATION);
	const bool isUpdate = updateDestinationEvent->IsAbleExecute();
	if (isUpdate) {
		auto aggroCharacter = m_aggroCharacter.lock();
		if (nullptr == aggroCharacter) return;
		m_destinationPosition = aggroCharacter->GetPosition();
		auto eventData = std::make_shared<SmallMonsterDestinationEvent>(m_idx, m_destinationPosition);
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(eventData));
	}
}

void SmallMonsterObject::SetMove()
{
	if (m_isMove) return;
	m_isMove = true;
	auto eventData = std::make_shared<SmallMonsterMoveEvent>(SmallMonsterMoveEvent::EventType::SMALL_MONSTER_MOVE, m_idx);
	m_roomRef->InsertAftrerUpdateSendEvent(eventData);
}

void SmallMonsterObject::SetStop()
{
	if (!m_isMove) return;
	m_isMove = false;
	auto eventData = std::make_shared<SmallMonsterMoveEvent>(SmallMonsterMoveEvent::EventType::SMALL_MONSTER_STOP, m_idx);
	m_roomRef->InsertAftrerUpdateSendEvent(eventData);
}

void SmallMonsterObject::Move()
{
	static constexpr float ONLY_ROTATE_ANGLE = 20.0f;
	static constexpr float ONLY_MOVE_ANGLE = 3.0f;
	static constexpr float TICK_ANGLE = 45.0f;
	static constexpr float STOP_DISTANCE = 20.0f;

	float elapsedTime = GetElapsedLastUpdateTime();
	auto betweenAngle = GetAggroBetweenAngleEuler();
	if (betweenAngle.second > ONLY_ROTATE_ANGLE) {
		Rotate(ROTATE_AXIS::Y, betweenAngle.first * TICK_ANGLE * elapsedTime);
		return;
	}
	else if (betweenAngle.second > ONLY_MOVE_ANGLE) {
		float rotateAngle = betweenAngle.first * TICK_ANGLE * elapsedTime;
		if (betweenAngle.second - rotateAngle < FLT_EPSILON)
			rotateAngle = betweenAngle.second;
		Rotate(ROTATE_AXIS::Y, rotateAngle);
	}

	auto toDestinationVector = GetToVector(m_destinationPosition);
	if (Vector3::Length(toDestinationVector) - STOP_DISTANCE < FLT_EPSILON) {
		return;
	}

	auto nextPosition = UpdateNextPosition(elapsedTime);
	if (nextPosition.has_value()) {
		SetPosition(nextPosition.value());
	}
}

const XMFLOAT3 SmallMonsterObject::GetCommonNextPosition(const float& elapsedTime)
{
	XMFLOAT3 lookVector = GetLookVector();
	XMFLOAT3 position = GetPosition();
	return Vector3::Add(position, lookVector, elapsedTime * m_moveSpeed);
}

const std::pair<float, float> SmallMonsterObject::GetAggroBetweenAngleEuler() const
{
	return MonsterObject::GetBetweenAngleEuler(m_destinationPosition);
}

std::optional<const XMFLOAT3> SmallMonsterObject::UpdateNextPosition(const float& elapsedTime)
{
	XMFLOAT3 commonNextPosition = GetCommonNextPosition(elapsedTime);

	//collide함수는 slidingvector가 적용된 위치라면 true를 리턴
	auto collideWallResult = CollideWall(commonNextPosition, elapsedTime, false);
	if (collideWallResult.has_value()) {
		auto collideLiveObjectResult = CollideLiveObject(collideWallResult.value().second, elapsedTime, collideWallResult.value().first);
		if (collideLiveObjectResult.has_value()) {
			return collideLiveObjectResult.value().second;
		}
		else return std::nullopt;
	}
	return std::nullopt;
}

std::optional<std::pair<bool, XMFLOAT3>> SmallMonsterObject::CollideWall(const XMFLOAT3& nextPosition, const float& elapsedTime, const bool& isSlidingPosition)
{
	BoundingSphere boudingSphere{ nextPosition,  m_collisionSphere.Radius };
	boudingSphere.Center.y = 0;
	auto mapData = m_roomRef->GetMapData();
	auto& mapCollision = mapData->GetCollisionData();
	std::shared_ptr<MapCollision> collideMap{ nullptr };
	for (auto& collision : mapCollision) {
		if (collision->CollideMap(boudingSphere)) {
			if (nullptr != collideMap) return std::nullopt;
			collideMap = collision;
		}
	}
	if (nullptr == collideMap) return std::pair<bool, XMFLOAT3>(false, nextPosition);
	auto moveVector = GetLookVector();
	auto slidingVectorResult = collideMap->GetSlidingVector(shared_from_this(), moveVector);//power, vector
	XMFLOAT3 applySlidingPosition = GetPosition();
	applySlidingPosition = Vector3::Add(applySlidingPosition, slidingVectorResult.second, m_moveSpeed * elapsedTime * slidingVectorResult.first);
	return std::pair<bool, XMFLOAT3>(true, applySlidingPosition);
}

const bool SmallMonsterObject::IsAbleAttack()
{
	//공격 가능 거리, 공격 가능 범위(부채꼴)
	static constexpr float ABLE_ATTACK_RANGE = 20.0f;
	static const float ABLE_ATTACK_EULER = 13.0f;//cos(13degree)
	static const float ABLE_ATTACK_COS_VALUE = cos(XMConvertToRadians(ABLE_ATTACK_EULER));//cos(13degree)
	auto aggroCharacter = m_aggroCharacter.lock();
	if (nullptr == aggroCharacter)
		return false;
	float distance = aggroCharacter->GetDistance(shared_from_this());
	if (ABLE_ATTACK_RANGE > distance) {
		const float betweenCosValue = GetBetweenAngleCosValue(aggroCharacter->GetPosition());
		if (betweenCosValue > ABLE_ATTACK_COS_VALUE)
			return IsReadyAttack();
	}
	return false;
}

void SmallMonsterObject::Attack()
{
	auto aggroCharacter = m_aggroCharacter.lock();
	auto eventData = std::make_shared<SmallMonsterAttakEvent>(m_idx);
	m_roomRef->InsertAftrerUpdateSendEvent(eventData);
	aggroCharacter->Attacked(MONSTER_DAMAGE);
}

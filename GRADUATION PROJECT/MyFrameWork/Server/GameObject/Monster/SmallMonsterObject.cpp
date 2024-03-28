#include "stdafx.h"
#include "SmallMonsterObject.h"
#include "../Room/Room.h"
//#include "SmallMonsterSessionObject.h"

SmallMonsterObject::SmallMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const int& idx)
	:MonsterObject(maxHp, moveSpeed, boundingSize, 20.0f, roomRef), m_idx(idx)
{
}

void SmallMonsterObject::Update()
{
	static constexpr float ABLE_ATTACK_RANGE = 10.0f;

	static constexpr float ONLY_ROTATTE_ANGLE = 20.0f;
	static constexpr float ONLY_MOVE_ANGLE = 5.0f;
	if (!m_isAlive) return;
	if (isReadyFindPlayer()) {
		auto aggroCharacter = GetAggroCharacter();
		if (aggroCharacter.has_value()) {
			m_aggroCharacter = aggroCharacter.value();
		}
	}
	float elapsedTime = GetElapsedLastUpdateTime();
	if (isAbleAttackRange()) {
		//Attack Player
		return;
	}
	auto AggroBetweenData = GetAggroBetweenAngle();
	if (!AggroBetweenData.has_value()) {
		return;
	}
	float rotateAngleRadian = XMConvertToRadians(AggroBetweenData.value().second);
	if (rotateAngleRadian > ONLY_ROTATTE_ANGLE) {
		//Only Rotate GameObject
		return;
	}
	auto aggroCharacter = m_aggroCharacter.lock();
	if (nullptr == aggroCharacter) {
		return;
	}
	if (aggroCharacter->GetDistance(shared_from_this()) < ABLE_ATTACK_RANGE) {
		return;
	}
	if (rotateAngleRadian > ONLY_MOVE_ANGLE) {
		//Rotate
	}
	GetCommonNextPosition(elapsedTime);
}

std::optional<std::shared_ptr<GameObject>> SmallMonsterObject::GetAggroCharacter()
{
	static constexpr float AGGRO_RANGE = 50.0f;
	auto characters = m_roomRef->GetCharacters();
	std::shared_ptr<GameObject> minDisGameObject(nullptr);
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
	return std::nullopt;
}

const XMFLOAT3 SmallMonsterObject::GetCommonNextPosition(const float& elapsedTime)
{
	XMFLOAT3 lookVector = GetLookVector();
	XMFLOAT3 position = GetPosition();
	return Vector3::Add(position, lookVector, elapsedTime * m_moveSpeed);
}

std::optional<const std::pair<char, float>> SmallMonsterObject::GetAggroBetweenAngle()
{
	auto aggroCharacter = m_aggroCharacter.lock();
	if (nullptr == aggroCharacter) {
		return std::nullopt;
	}
	XMFLOAT3 aggroPosition = aggroCharacter->GetPosition();
	XMFLOAT3 toAggroVector = GetToVector(aggroPosition);
	toAggroVector = Vector3::Normalize(toAggroVector);
	XMFLOAT3 lookVector = GetLookVector();
	float lookDotResult = Vector3::DotProduct(lookVector, toAggroVector);//사이 각에 대한 cos()

	//좌측인지 우측인지 판단도 해야 됨.
	//객체 우측벡터와 객체까지의 벡터를 내적한 값은 cos()인데
	//cos()은 -90~90는 양수, 이외는 음수
	//내적 결과가 양수라면 객체 기준 우측에 상대 객체가 존재한다는 것을 판단
	XMFLOAT3 rightVector = GetRightVector();
	float rightDotResult = Vector3::DotProduct(lookVector, toAggroVector);//사이 각에 대한 cos()
	if (rightDotResult > 0)
		return std::make_pair<char, float>(1, std::acosf(lookDotResult));
	return std::make_pair<char, float>(-1, std::acosf(lookDotResult));

}

const bool SmallMonsterObject::isAbleAttackRange()
{
	static constexpr float ABLE_ATTACK_RANGE = 10.0f;
	if (!isReadyAttack()) return false;
	auto aggroCharacter = m_aggroCharacter.lock();
	if (nullptr == aggroCharacter)
		return false;
	float distance = aggroCharacter->GetDistance(shared_from_this());
	if (ABLE_ATTACK_RANGE > distance)
		return true;
	return false;
}

void SmallMonsterObject::Attack(std::shared_ptr<LiveObject>& attackedCharacter)
{
	attackedCharacter->Attacked(1);
}

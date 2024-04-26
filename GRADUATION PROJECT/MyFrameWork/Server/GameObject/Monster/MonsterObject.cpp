#include "stdafx.h"
#include "MonsterObject.h"
#include "../EventController/CoolDownEventBase.h"
#include "../EventController/DurationEvent.h"

MonsterObject::MonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	: LiveObject(maxHp, moveSpeed, boundingSize, roomRef)
{
	m_behaviorTimeEventCtrl = std::make_unique<EventController>();
}

const bool MonsterObject::IsReadyFindPlayer()
{
	auto findPlayerEvent = m_behaviorTimeEventCtrl->GetEventData(FIND_PLAYER);
	const bool isReady = findPlayerEvent->IsAbleExecute();
	return isReady;
}

const std::pair<float, float> MonsterObject::GetBetweenAngleEuler(const XMFLOAT3& otherPosition) const
{
	XMFLOAT3 aggroPosition = otherPosition;
	XMFLOAT3 toAggroVector = GetToVector(aggroPosition);
	toAggroVector = Vector3::Normalize(toAggroVector);
	XMFLOAT3 lookVector = GetLookVector();
	float lookDotResult = Vector3::DotProduct(lookVector, toAggroVector);//사이 각에 대한 cos()

	//좌측인지 우측인지 판단도 해야 됨.
	//객체 우측벡터와 객체까지의 벡터를 내적한 값은 cos()인데
	//cos()은 -90~90는 양수, 이외는 음수
	//내적 결과가 양수라면 객체 기준 우측에 상대 객체가 존재한다는 것을 판단
	XMFLOAT3 rightVector = GetRightVector();
	float rightDotResult = Vector3::DotProduct(rightVector, toAggroVector);//사이 각에 대한 cos()
	if (rightDotResult > 0)
		return std::make_pair<float, float>(1.0f, XMConvertToDegrees(std::acosf(lookDotResult)));
	return std::make_pair<float, float>(-1.0f, XMConvertToDegrees(std::acosf(lookDotResult)));
}

const bool MonsterObject::IsReadyAttack()
{
	auto attackEvent = m_behaviorTimeEventCtrl->GetEventData(ATTACK_PLAYER);
	const bool isReady = attackEvent->IsAbleExecute();
	return isReady;
}

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
	static XMFLOAT3 UP = XMFLOAT3(0, 1, 0);
	XMFLOAT3 aggroPosition = otherPosition;
	XMFLOAT3 toAggroVector = GetToVector(aggroPosition);
	toAggroVector = Vector3::Normalize(toAggroVector);
	XMFLOAT3 lookVector = GetLookVector();
	float lookDotResult = Vector3::DotProduct(lookVector, toAggroVector);//사이 각에 대한 cos()
	//좌측인지 우측인지 판단도 해야 됨.
	XMFLOAT3 crossProductResult = Vector3::Normalize(Vector3::CrossProduct(lookVector, toAggroVector));//어그로 객체가 우측에 있다면 UP 내적 했을 때 양수
	float dotProductFromUp = Vector3::DotProduct(UP, crossProductResult); // 양수라면 어그로는 우측에 존재 함.
	if (dotProductFromUp > 0)
		return std::make_pair<float, float>(1.0f, XMConvertToDegrees(std::acosf(lookDotResult)));
	return std::make_pair<float, float>(-1.0f, XMConvertToDegrees(std::acosf(lookDotResult)));
}

const bool MonsterObject::IsReadyAttack()
{
	auto attackEvent = m_behaviorTimeEventCtrl->GetEventData(ATTACK_PLAYER);
	const bool isReady = attackEvent->IsAbleExecute();
	return isReady;
}

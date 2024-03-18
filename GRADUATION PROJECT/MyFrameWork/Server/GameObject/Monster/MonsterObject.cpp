#include "stdafx.h"
#include "MonsterObject.h"

MonsterObject::MonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	: LiveObject(maxHp, moveSpeed, boundingSize, roomRef)
{
}

const XMFLOAT3 MonsterObject::GetDestinationPosition() const
{
	//return m_destinationPosition;
	return XMFLOAT3();
}

const bool MonsterObject::isReadyFindPlayer()
{
	using namespace std;
	using namespace chrono;
	auto nowTime = high_resolution_clock::now();
	auto durationTime = duration_cast<seconds>(nowTime - m_lastFindAggroTime - m_coolTimeFindPlayer).count();
	if (durationTime < 0) return false;
	m_lastFindAggroTime = nowTime;
	return true;
}

const bool MonsterObject::isReadyAttack()
{
	using namespace std;
	using namespace chrono;
	auto nowTime = high_resolution_clock::now();
	auto durationTime = duration_cast<seconds>(nowTime - m_lastAttackTime - m_coolTimeAttack).count();
	if (durationTime < 0) return false;
	m_lastAttackTime = nowTime;
	return true;
}

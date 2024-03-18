#include "stdafx.h"
#include "BossMonsterObject.h"

BossMonsterObject::BossMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:MonsterObject(maxHp, moveSpeed, boundingSize, roomRef)
{
}

void BossMonsterObject::Update()
{
}

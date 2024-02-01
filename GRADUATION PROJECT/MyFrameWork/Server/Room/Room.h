#pragma once
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Monster/SmallMonsterObject.h"
#include "../GameObject/Projectile/ShootingObject.h"
#include "../GameObject/Projectile/MeteoObject.h"

#ifdef _DEBUG
#include "../PCH/stdafx.h"
#endif


class ChracterObject;
class Room
{
public:
	Room();
	~Room();
private:
	ROOM_STATE m_roomState;
	//플레이어 id-캐릭터
	//tbb::concurrent_unordered_map은 erase연산이 thread-unsafe해서 안씀.
	tbb::concurrent_hash_map<unsigned int, ROLE> m_players;
	//캐릭터-캐릭터-객체
	std::unordered_map<ROLE, ChracterObject*> m_characters;

	//std::array<15, SmallMwnsterObject> m_smallMonsterArr;
	//MonsterObject m_boss;

	//std::array<10, ShootingObject> m_arrow;
	//std::array<10, ShootingObject> m_energyBall;



};


//
//void Room::ResetRoom()
//{
//	for (auto& character : m_characterMap) {
//		character.second->SetStage_1Position();
//	}
//	m_boss.SetBossStagePosition();
//	m_roomState = ROOM_STAGE1;
//	m_isAlive = false;
//	m_boss.isBossDie = false;
//}
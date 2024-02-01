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
	//�÷��̾� id-ĳ����
	//tbb::concurrent_unordered_map�� erase������ thread-unsafe�ؼ� �Ⱦ�.
	tbb::concurrent_hash_map<unsigned int, ROLE> m_players;
	//ĳ����-ĳ����-��ü
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
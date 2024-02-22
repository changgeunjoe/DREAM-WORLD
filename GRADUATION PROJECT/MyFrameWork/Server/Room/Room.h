#pragma once
#include "../PCH/stdafx.h"
#include "../Network/IocpEvent/IocpEventBase.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Monster/SmallMonsterObject.h"
#include "../GameObject/Projectile/ShootingObject.h"
#include "../GameObject/Projectile/MeteoObject.h"


class ChracterObject;
class Room : public IOCP::EventBase
{
public:
	Room();
	~Room();

	//IOCP���� PQGS�� ���� ��� �ذ� - update, gamestate...(�뿡 ���� �̺�Ʈ)
	virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

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
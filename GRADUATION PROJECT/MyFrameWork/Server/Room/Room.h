#pragma once
#include "../PCH/stdafx.h"
#include "../Network/IocpEvent/IocpEventBase.h"
//#include "../GameObject/Monster/MonsterObject.h"
//#include "../GameObject/Monster/SmallMonsterObject.h"
//#include "../GameObject/Projectile/ShootingObject.h"
//#include "../GameObject/Projectile/MeteoObject.h"


class ChracterObject;
class UserSession;
namespace IOCP {
	class Iocp;
}

class Room : public IOCP::EventBase
{
public:
	Room() = default;
	Room(std::vector<std::shared_ptr<UserSession>>& userRefVec);
	Room(std::shared_ptr<UserSession>& userRef);
	~Room();

	//IOCP에서 PQGS로 오는 경우 해결 - update, gamestate...(룸에 대한 이벤트)
	virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

private:
	void Update(const std::chrono::milliseconds& updateTick = std::chrono::milliseconds(17));
	void GameStateSend(const std::chrono::milliseconds& updateTick = std::chrono::milliseconds(50));

private:
	ROOM_STATE m_roomState = ROOM_STATE::ROOM_COMMON;
	//플레이어 id-캐릭터
	tbb::concurrent_hash_map<std::shared_ptr<UserSession>, ROLE> m_user;
	//역할-캐릭터-객체
	std::unordered_map<ROLE, std::shared_ptr<ChracterObject>> m_characters;

	//std::array<15, SmallMonsterObject> m_smallMonsterArr;
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
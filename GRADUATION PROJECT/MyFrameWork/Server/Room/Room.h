#pragma once
#pragma once
#include "../PCH/stdafx.h"
#include "../Network/IocpEvent/IocpEventBase.h"

struct PacketHeader;
class UserSession;
class GameObject;
class LiveObject;
class CharacterObject;
class MonsterObject;
class BossMonsterObject;
class SmallMonsterObject;
class RoomSendEvent;
class PrevUpdateEvent;
class ProjectileObject;
namespace IOCP {
	class Iocp;
}

namespace TIMER
{
	class EventBase;
}

class MapData;
class MonsterMapData;
class NavMapData;
class Room : public IOCP::EventBase
{
public:
	Room() = delete;
	Room(std::vector<std::shared_ptr<UserSession>>& userRefVec, std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef);
	Room(std::shared_ptr<UserSession>& userRef, std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef);
	~Room();

	//IOCP���� PQGS�� ���� ��� �ذ� - update, gamestate...(�뿡 ���� �̺�Ʈ)
	virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

	void Start();

	std::vector<std::shared_ptr<SmallMonsterObject>>& GetSmallMonsters();

	std::vector<std::shared_ptr<CharacterObject>> GetCharacters() const;
	std::vector<std::shared_ptr<LiveObject>> GetLiveObjects() const;

	std::vector<ROLE> GetConnectedUserRoles();
	std::vector<ROLE> GetLiveRoles();

	void InsertProjectileObject(std::shared_ptr<ProjectileObject> projectileObject);
	void UpdateProjectileObject();

	std::shared_ptr<MapData> GetMapData() const;
	std::shared_ptr<NavMapData> GetBossMapData() const;

	void InsertAftrerUpdateSendEvent(std::shared_ptr<RoomSendEvent> roomEvent);
	void InsertPrevUpdateEvent(std::shared_ptr<PrevUpdateEvent> prevUpdate);

	void BroadCastPacket(const PacketHeader* packetData);
	void BroadCastPacket(std::shared_ptr<PacketHeader> packetData);
	void MultiCastCastPacket(const PacketHeader* packetData, const ROLE& exclusiveRoles);
	void MultiCastCastPacket(const PacketHeader* packetData, const std::vector<ROLE>& exclusiveRoles);

	void MultiCastCastPacket(std::shared_ptr<PacketHeader> packetData, const ROLE& exclusiveRoles);
	void MultiCastCastPacket(std::shared_ptr<PacketHeader> packetData, const std::vector<ROLE>& exclusiveRoles);

	std::shared_ptr<CharacterObject> GetCharacterObject(const ROLE& role);
	std::vector <std::shared_ptr<MonsterObject>> GetEnermyData();

	void InitializeAllGameObject();

	void SetBossStage();
	void InserTimerEvent(std::shared_ptr<TIMER::EventBase> timerEvent);

	void SetBossRoad(std::shared_ptr<std::list<XMFLOAT3>> road);
	void SetBossAggro(std::shared_ptr<std::list<XMFLOAT3>> road, std::shared_ptr<CharacterObject> characteRef);

	void ForceGameEnd();//�� �� ���� ������ ����
private:

	void ProcessAfterUpdateSendEvent();
	void ProcessPrevUpdateEvent();
	void InsertTimerEvent(const TIMER_EVENT_TYPE& eventType, const std::chrono::milliseconds& updateTick = std::chrono::milliseconds(50));
	void Update();
	void UpdateGameState();
	void GameStateSend();

	std::vector<std::shared_ptr<UserSession>> GetAllUserSessions();


	void SetGameStatePlayer_Stage();
	void SetGameStatePlayer_Boss();
	void SetGameStateMonsters();
	void SetGameStateBoss();

	//PlayerSkill
	void PlayerHeal();
	void PlayerApplyShield();
	void PlayerRemoveShield();
	void RainArrowAttack();

	void SetRoomEndState();

private:
	ROOM_STATE m_roomState = ROOM_STATE::ROOM_COMMON;
	int m_updateCnt;

	//�÷��̾� id-ĳ����
	//concurrent_hash_map - insert/erase�� thread safe������
	//��ȸ�� �������� ����.
	//std::unordered_map���� ����
	std::unordered_set<std::shared_ptr<UserSession>> m_userSessions;
	//std::mutex m_userLock;
	//��Ŷ�� �����°� 2�� �̻��� �����忡�� �ϴ� ��찡 ������, read�� �ϴµ��� mutex�� ������ ����̶�� �Ǵ�-> read lock���� ����, ������ ���� ��, �������� write-lock�� �� �ǵ�
	std::shared_mutex m_userSessionsLock;
	//����-ĳ����-��ü
	std::unordered_map<ROLE, std::shared_ptr<CharacterObject>> m_characters;
	//Update�� ���� ��� ���� ������Ʈ�� ��� vector

	std::vector<std::shared_ptr<SmallMonsterObject>> m_smallMonsters;

	std::shared_ptr<BossMonsterObject> m_bossMonster;
	std::chrono::high_resolution_clock::time_point m_bossStartTime;

	//����ü�� ����� �� ������ list�� ó��
	std::list<std::shared_ptr<ProjectileObject>> m_projectileObjects;

	std::atomic_bool m_gameStateUpdateComplete;
	//0 stage // 1 boss
	ROOM_STATE m_applyRoomStateForGameState;
	std::vector<std::shared_ptr<PacketHeader>> m_gameStateData;

	//MonsterObject m_boss;

	//std::array<10, ShootingObject> m_arrow;
	//std::array<10, ShootingObject> m_energyBall;
	std::atomic_bool m_isContinueHeal;

	std::shared_ptr<MonsterMapData> m_stageMapData;
	std::shared_ptr<NavMapData> m_bossMapData;

	tbb::concurrent_queue<std::shared_ptr<RoomSendEvent>> m_afterUpdateSendEvent;
	std::atomic_int m_afterUpdateEventCnt = 0;
	tbb::concurrent_queue<std::shared_ptr<PrevUpdateEvent>> m_prevUpdateEvent;
	std::atomic_int m_prevUpdateEventCnt = 0;
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
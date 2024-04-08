#pragma once
#pragma once
#include "../PCH/stdafx.h"
#include "../Network/IocpEvent/IocpEventBase.h"

struct PacketHeader;
class UserSession;
class GameObject;
class LiveObject;
class CharacterObject;
class BossMonsterObject;
class SmallMonsterObject;
class RoomSendEvent;
class PrevUpdateEvent;
namespace IOCP {
	class Iocp;
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

	//IOCP에서 PQGS로 오는 경우 해결 - update, gamestate...(룸에 대한 이벤트)
	virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

	void Start();

	std::vector<std::shared_ptr<SmallMonsterObject>>& GetSmallMonsters();

	std::vector<std::shared_ptr<GameObject>> GetCharacters() const;
	std::vector<std::shared_ptr<LiveObject>> GetLiveObjects() const;

	std::shared_ptr<MapData> GetMapData() const;
	void RecvSkill_QInput(const ROLE& role, const XMFLOAT3& vector3);
	void RecvSkill_EInput(const ROLE& role, const XMFLOAT3& vector3);

	void RecvSkill_QExecute(const ROLE& role);
	void RecvSkill_EExecute(const ROLE& role);

	void InsertAftrerUpdateEvent(std::shared_ptr<RoomSendEvent> roomEvent);
	void InsertPrevUpdateEvent(std::shared_ptr<PrevUpdateEvent> prevUpdate);

	void BroadCastPacket(const PacketHeader* packetData);
	void BroadCastPacket(std::shared_ptr<PacketHeader>& packetData);
	void MultiCastCastPacket(const PacketHeader* packetData, const ROLE& exclusiveRoles);
	void MultiCastCastPacket(const PacketHeader* packetData, const std::vector<ROLE>& exclusiveRoles);

	void MultiCastCastPacket(std::shared_ptr<PacketHeader>& packetData, const ROLE& exclusiveRoles);
	void MultiCastCastPacket(std::shared_ptr<PacketHeader>& packetData, const std::vector<ROLE>& exclusiveRoles);

	std::shared_ptr<CharacterObject> GetCharacterObject(const ROLE& role);

	void InitializeAllGameObject();

private:

	void ProcessAfterUpdateEvent();
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

	void ProccessSmallMonsterEvent();
private:
	ROOM_STATE m_roomState = ROOM_STATE::ROOM_COMMON;
	int m_updateCnt;

	//플레이어 id-캐릭터
	//concurrent_hash_map - insert/erase는 thread safe하지만
	//순회는 안전하지 않음.
	//std::unordered_map으로 변경
	std::unordered_set<std::shared_ptr<UserSession>> m_userSessions;
	//std::mutex m_userLock;
	//패킷을 보내는건 2개 이상의 쓰레드에서 하는 경우가 많은데, read만 하는데도 mutex는 안좋은 방법이라고 판단-> read lock으로 변경, 유저가 들어올 때, 나갈때는 write-lock을 할 의도
	std::shared_mutex m_userSessionsLock;
	//역할-캐릭터-객체
	std::unordered_map<ROLE, std::shared_ptr<CharacterObject>> m_characters;
	//Update를 위한 모든 게임 오브젝트를 담는 vector

	std::vector<std::shared_ptr<SmallMonsterObject>> m_smallMonsters;

	//std::shared_ptr<BossMonsterObject> m_bossMonster;

	std::vector<std::shared_ptr<GameObject>> m_allGameObjects;

	std::atomic_bool m_gameStateUpdateComplete;
	//0 stage // 1 boss
	ROOM_STATE m_applyRoomStateForGameState;
	std::vector<std::shared_ptr<PacketHeader>> m_gameStateData;

	//MonsterObject m_boss;

	//std::array<10, ShootingObject> m_arrow;
	//std::array<10, ShootingObject> m_energyBall;
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
#include "stdafx.h"
#include "Room.h"
#include "../Timer/Timer.h"
#include "TimerRoomEvent.h"
#include "../Network/ExpOver/ExpOver.h"
#include "../Network/UserSession/UserSession.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Character/Warrior/WarriorObject.h"
#include "../GameObject/Character/Tanker/TankerObject.h"
#include "../GameObject/Character/Mage/MageObject.h"
#include "../GameObject/Character/Archer/ArcherObject.h"

#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Monster/SmallMonsterObject.h"
#include "../GameObject/Monster/BossMonsterObject.h"

#include "../GameObject/Projectile/ProjectileObject.h"

#include "../Network/protocol/protocol.h"
#include "../MapData/MapData.h"
#include "RoomEvent.h"

Room::Room(std::vector<std::shared_ptr<UserSession>>& userRefVec, std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef)
	: m_updateCnt(0), m_roomState(ROOM_STATE::ROOM_COMMON), m_gameStateUpdateComplete(false), m_isContinueHeal(false)
	, m_stageMapData(mapDataRef), m_bossMapData(navMapDataRef)
{
	m_gameStateData.reserve(2);
	std::lock_guard<std::shared_mutex> userLockGuard(m_userSessionsLock);
	for (auto& userRef : userRefVec)
		m_userSessions.insert(userRef);
}

Room::Room(std::shared_ptr<UserSession>& userRef, std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef)
	: m_updateCnt(0), m_roomState(ROOM_STATE::ROOM_COMMON), m_gameStateUpdateComplete(false)
	, m_stageMapData(mapDataRef), m_bossMapData(navMapDataRef)
{
	m_gameStateData.reserve(2);
	spdlog::warn("Room::Room() - make Room for Alone Test");

	std::lock_guard<std::shared_mutex> userLockGuard(m_userSessionsLock);
	m_userSessions.insert(userRef);
}

Room::~Room()
{
}

void ::Room::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	if (m_roomState == ROOM_STATE::ROOM_END) return;
	using namespace std::chrono;
	switch (over->GetOpCode())
	{
		//Room - update
	case IOCP_OP_CODE::OP_ROOM_UPDATE:
	{
		//spdlog::debug("Room::Execute() - OP_ROOM_UPDATE");
		ProcessPrevUpdateEvent();
		Update();
		//InsertTimerEvent(TIMER_EVENT_TYPE::EV_SEND_NPC_MOVE, 1ms);
		//Ÿ�̸ӿ� �켱 �ְ� ���� Update()���� ���� Send�̺�Ʈ ó��
		InsertTimerEvent(TIMER_EVENT_TYPE::EV_ROOM_UPDATE, 17ms);
		ProcessAfterUpdateEvent();
		//GameStateSend�� UPdate 3���� �� send�ϰ� �����ؾ���.
	}
	break;
	case IOCP_OP_CODE::OP_SEND_NPC_MOVE:
	{

	}
	break;
	case IOCP_OP_CODE::OP_GAME_STATE_SEND:
	{
		GameStateSend();
		//GameState������ �����ߴٸ�
		m_gameStateUpdateComplete = true;
	}
	break;

	//Room - player skill
	case IOCP_OP_CODE::OP_PLAYER_HEAL_START:
	{
		m_isContinueHeal = true;
		auto healStartPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::HEAL_START));
		BroadCastPacket(healStartPacket);
		PlayerHeal();
	}
	break;
	case IOCP_OP_CODE::OP_PLAYER_HEAL:
	{
		PlayerHeal();
	}
	break;
	case IOCP_OP_CODE::OP_PLAYER_HEAL_END:
	{
		auto healEndPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::HEAL_END));
		BroadCastPacket(healEndPacket);
		m_isContinueHeal = false;
	}
	break;
	case IOCP_OP_CODE::OP_PLAYER_APPLY_SIELD:
	{
		PlayerApplyShield();
	}
	break;
	case IOCP_OP_CODE::OP_PLAYER_REMOVE_SIELD:
	{
		PlayerRemoveShield();
	}
	break;
	case IOCP_OP_CODE::OP_RAIN_ARROW_ATTACK:
	{
		RainArrowAttack();
	}
	break;

	//Room - Boss
	case IOCP_OP_CODE::OP_FIND_PLAYER:
	{

	}
	break;
	case IOCP_OP_CODE::OP_BOSS_ATTACK_SELECT:
	{

	}
	break;
	case IOCP_OP_CODE::OP_BOSS_ATTACK_EXECUTE:
	{

	}
	break;
	default:
		break;
	}
}

void Room::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
}

void Room::Start()
{
	SERVER_PACKET::IntoGamePacket sendPacket;
	for (int i = 0; i < 15; ++i) {
		sendPacket.monsterData[i].id = i;
		sendPacket.monsterData[i].position = m_smallMonsters[i]->GetPosition();
		sendPacket.monsterData[i].lookVector = m_smallMonsters[i]->GetLookVector();
		sendPacket.monsterData[i].maxHp = m_smallMonsters[i]->GetMaxHp();
	}

	auto userSessions = GetAllUserSessions();
	for (auto& user : userSessions) {
		sendPacket.role = user->GetRole();
		user->DoSend(&sendPacket);
	}

	using namespace std::chrono;
	m_gameStateUpdateComplete = true;
	InsertTimerEvent(TIMER_EVENT_TYPE::EV_ROOM_UPDATE, 20ms);
	//InsertEvent(TIMER_EVENT_TYPE::EV_GAME_STATE_SEND, 50ms);
}

std::vector<std::shared_ptr<SmallMonsterObject>>& Room::GetSmallMonsters()
{
	return m_smallMonsters;
}

std::vector<std::shared_ptr<CharacterObject>> Room::GetCharacters() const
{
	std::vector<std::shared_ptr<CharacterObject>> characters;
	characters.reserve(4);
	for (auto& character : m_characters)
		characters.push_back(character.second);
	return characters;
}

std::vector<std::shared_ptr<LiveObject>> Room::GetLiveObjects() const
{
	std::vector<std::shared_ptr<LiveObject>> liveObjects;
	liveObjects.reserve(15);
	for (auto& character : m_characters)
		liveObjects.push_back(std::static_pointer_cast<LiveObject>(character.second->shared_from_this()));
	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
		for (auto& monster : m_smallMonsters)
			liveObjects.push_back(std::static_pointer_cast<LiveObject>(monster->shared_from_this()));
	}
	else {
		//liveObjects.push_back(std::static_pointer_cast<LiveObject>(m_bossMonster->shared_from_this()));
		//boss Insert
	}
	return liveObjects;
}

void Room::InsertProjectileObject(std::shared_ptr<ProjectileObject> projectileObject)
{
	//prevUpdateEvent�� Update()���ο��� �̱��ϰ� ���ư�.
	m_projectileObjects.push_back(projectileObject);
}

void Room::UpdateProjectileObject()
{
	for (auto projectileIter = m_projectileObjects.begin(); projectileIter != m_projectileObjects.end();) {
		(*projectileIter)->Update();
		const bool isDestroyTime = (*projectileIter)->IsDestroy();
		if (isDestroyTime) {
			projectileIter = m_projectileObjects.erase(projectileIter);
			continue;
		}
		++projectileIter;
	}
}

std::shared_ptr<MapData> Room::GetMapData() const
{
	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
		return m_stageMapData;
	}
	return m_bossMapData;
}

void Room::InsertAftrerUpdateEvent(std::shared_ptr<RoomSendEvent> roomEvent)
{
	m_afterUpdateSendEvent.push(roomEvent);
	++m_afterUpdateEventCnt;
}

void Room::InsertPrevUpdateEvent(std::shared_ptr<PrevUpdateEvent> prevUpdate)
{
	m_prevUpdateEvent.push(prevUpdate);
	++m_prevUpdateEventCnt;
}

void Room::ProcessAfterUpdateEvent()
{
	int prccessRoomEventCnt = m_afterUpdateEventCnt;
	m_afterUpdateEventCnt -= prccessRoomEventCnt;
	for (int i = 0; i < prccessRoomEventCnt; ++i) {
		std::shared_ptr<RoomSendEvent> currentEvent = nullptr;
		bool isSuccess = m_afterUpdateSendEvent.try_pop(currentEvent);
		if (isSuccess) {
			auto sendPacket = currentEvent->GetPacketHeader();
			BroadCastPacket(sendPacket);
		}
		else {
			m_afterUpdateEventCnt += prccessRoomEventCnt;
			return;
		}
	}
}

void Room::ProcessPrevUpdateEvent()
{
	int prccessPrevEventCnt = m_prevUpdateEventCnt;
	m_prevUpdateEventCnt -= prccessPrevEventCnt;
	for (int i = 0; i < prccessPrevEventCnt; ++i) {
		std::shared_ptr<PrevUpdateEvent> currentEvent = nullptr;
		bool isSuccess = m_prevUpdateEvent.try_pop(currentEvent);
		if (isSuccess) {
			currentEvent->ProcessEvent();
		}
		else {
			m_prevUpdateEventCnt += prccessPrevEventCnt;
			return;
		}
	}
}

void Room::InsertTimerEvent(const TIMER_EVENT_TYPE& eventType, const std::chrono::milliseconds& updateTick)
{
	auto timerEvent = std::make_shared<TIMER::RoomEvent>(eventType, updateTick, std::static_pointer_cast<Room>(shared_from_this()));
	TIMER::Timer::GetInstance().InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(timerEvent));
}

void Room::Update()
{
	static constexpr int GAME_STATE_SEND_CNT = 3;

	//Update GameObject
	for (auto& character : m_characters)
		character.second->Update();
	if (ROOM_STATE::ROOM_COMMON == m_roomState)
		for (auto& smallMonster : m_smallMonsters) {
			smallMonster->Update();
		}
	//else if(ROOM_STATE::ROOM_BOSS == m_roomState)
		
	UpdateProjectileObject();

	++m_updateCnt;
	if (!m_gameStateUpdateComplete) return;
	if (GAME_STATE_SEND_CNT <= m_updateCnt) {
		UpdateGameState();
		m_updateCnt = 0;
	}
}

void Room::UpdateGameState()
{
	m_gameStateUpdateComplete = false;
	if (m_roomState == ROOM_STATE::ROOM_COMMON) {
		SetGameStatePlayer_Stage();
		SetGameStateMonsters();
		m_applyRoomStateForGameState = ROOM_STATE::ROOM_COMMON;
	}
	else if (m_roomState == ROOM_STATE::ROOM_BOSS) {
		SetGameStatePlayer_Boss();
		SetGameStateBoss();
		m_applyRoomStateForGameState = ROOM_STATE::ROOM_BOSS;
	}
	//��Ŷ ������ ����
	//PQGS�� send()
	InsertTimerEvent(TIMER_EVENT_TYPE::EV_SEND_GAME_STATE, std::chrono::milliseconds(1));
}

void Room::GameStateSend()
{
	//���� ���� ���ӻ��¿� ����� �� ���¿� ���� �� ���°� �ٸ��ٸ� ������ ����.

	if (m_roomState != m_applyRoomStateForGameState) return;
	if (ROOM_STATE::ROOM_COMMON == m_applyRoomStateForGameState)
		BroadCastPacket(m_gameStateData[0]);
	else
		BroadCastPacket(m_gameStateData[1]);
}

std::vector<std::shared_ptr<UserSession>> Room::GetAllUserSessions()
{
	std::vector<std::shared_ptr<UserSession>> userSessions;
	userSessions.reserve(4);
	std::shared_lock<std::shared_mutex> userLockGuard(m_userSessionsLock);
	for (auto& userSession : m_userSessions) {
		userSessions.push_back(userSession);
	}
	return userSessions;
}

void Room::BroadCastPacket(const PacketHeader* packetData)
{
	auto userSessions = GetAllUserSessions();
	for (auto& userSession : userSessions) {
		userSession->DoSend(packetData);
	}
}

void Room::BroadCastPacket(std::shared_ptr<PacketHeader>& packetData)
{
	auto userSessions = GetAllUserSessions();
	for (auto& userSession : userSessions) {
		userSession->DoSend(packetData);
	}
}

void Room::MultiCastCastPacket(const PacketHeader* packetData, const ROLE& exclusiveRoles)
{
	auto userSessions = GetAllUserSessions();
	for (auto& userSession : userSessions) {
		if (userSession->GetRole() != exclusiveRoles)
		{
			userSession->DoSend(packetData);
		}
	}
}

void Room::MultiCastCastPacket(const PacketHeader* packetData, const std::vector<ROLE>& exclusiveRoles)
{
	auto userSessions = GetAllUserSessions();
	for (auto& userSession : userSessions) {
		if (exclusiveRoles.end() != std::find(exclusiveRoles.begin(), exclusiveRoles.end(), userSession->GetRole()))
		{
			userSession->DoSend(packetData);
		}
	}
}

void Room::MultiCastCastPacket(std::shared_ptr<PacketHeader>& packetData, const ROLE& exclusiveRoles)
{
	auto userSessions = GetAllUserSessions();
	for (auto& userSession : userSessions) {
		if (userSession->GetRole() != exclusiveRoles)
		{
			userSession->DoSend(packetData);
		}
	}
}

void Room::MultiCastCastPacket(std::shared_ptr<PacketHeader>& packetData, const std::vector<ROLE>& exclusiveRoles)
{
	auto userSessions = GetAllUserSessions();
	for (auto& userSession : userSessions) {
		if (exclusiveRoles.end() != std::find(exclusiveRoles.begin(), exclusiveRoles.end(), userSession->GetRole()))
		{
			userSession->DoSend(packetData);
		}
	}
}

std::shared_ptr<CharacterObject> Room::GetCharacterObject(const ROLE& role)
{
	if (!m_characters.count(role)) {
		spdlog::warn("std::shared_ptr<CharacterObject> Room::GetCharacterObject(const ROLE& role) - Invalid ROLE");
		return nullptr;
	}
	return m_characters[role];
}

std::vector<std::shared_ptr<MonsterObject>> Room::GetEnermyData()
{
	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
		std::vector<std::shared_ptr<MonsterObject>> monsterObjects;
		monsterObjects.reserve(15);
		for (auto& monster : m_smallMonsters)
			monsterObjects.push_back(monster);
		return monsterObjects;
	}
	else {
		std::vector<std::shared_ptr<MonsterObject>> bossObject;
		//...
	}
}

void Room::SetGameStatePlayer_Stage()
{
	auto gameStateData = std::static_pointer_cast<SERVER_PACKET::GameState_Base>(m_gameStateData[0]);
	int characterIdx = 0;
	for (auto& character : m_characters) {
		gameStateData->userState[characterIdx].role = character.first;
		gameStateData->userState[characterIdx].hp = character.second->GetHp();
		gameStateData->userState[characterIdx].position = character.second->GetPosition();
		gameStateData->userState[characterIdx].resetShield = character.second->GetShield();
		gameStateData->userState[characterIdx].time = character.second->GetLastUpdateTime();
		++characterIdx;
	}
}

void Room::SetGameStatePlayer_Boss()
{
	auto gameStateData = std::static_pointer_cast<SERVER_PACKET::GameState_Base>(m_gameStateData[1]);
	int characterIdx = 0;
	for (auto& character : m_characters) {
		gameStateData->userState[characterIdx].role = character.first;
		gameStateData->userState[characterIdx].hp = character.second->GetHp();
		gameStateData->userState[characterIdx].position = character.second->GetPosition();
		gameStateData->userState[characterIdx].resetShield = character.second->GetShield();
		gameStateData->userState[characterIdx].time = character.second->GetLastUpdateTime();
		++characterIdx;
	}
}

void Room::SetGameStateMonsters()
{
	auto gameStateData = std::static_pointer_cast<SERVER_PACKET::GameState_STAGE>(m_gameStateData[0]);
	for (auto& monster : m_smallMonsters) {
		gameStateData->smallMonster[monster->GetIdx()].isAlive = monster->IsAlive();
		gameStateData->smallMonster[monster->GetIdx()].idx = monster->GetIdx();
		gameStateData->smallMonster[monster->GetIdx()].hp = monster->GetHp();
		gameStateData->smallMonster[monster->GetIdx()].position = monster->GetPosition();
		gameStateData->smallMonster[monster->GetIdx()].time = monster->GetLastUpdateTime();
	}
}

void Room::SetGameStateBoss()
{
	auto gameStateData = std::static_pointer_cast<SERVER_PACKET::GameState_BOSS>(m_gameStateData[1]);
}

void Room::InitializeAllGameObject()
{
	static constexpr float SMALL_MONSTER_HP = 250.0f;
	static constexpr float BOSS_HP = 2500.0f;

	m_gameStateData.push_back(std::make_shared<SERVER_PACKET::GameState_STAGE>());
	m_gameStateData.push_back(std::make_shared<SERVER_PACKET::GameState_BOSS>());

	std::vector<std::chrono::seconds> duTime;
	std::vector<std::chrono::seconds> coolTime;
	//Character Initialize
	m_characters.emplace(ROLE::WARRIOR, std::make_shared<WarriorObject>(600.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this())));
	m_characters.emplace(ROLE::TANKER, std::make_shared<TankerObject>(780.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this())));
	m_characters.emplace(ROLE::ARCHER, std::make_shared<ArcherObject>(400.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this())));
	m_characters.emplace(ROLE::MAGE, std::make_shared<MageObject>(500.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this())));

	auto monsterInitData = m_stageMapData->GetMonsterInitData();
	for (int i = 0; i < monsterInitData.size(); ++i) {
		auto monster = std::make_shared<SmallMonsterObject>(SMALL_MONSTER_HP, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this()), i);
		monster->SetPosition(monsterInitData[i].position);
		monster->Rotate(ROTATE_AXIS::Y, monsterInitData[i].eulerRotate.y);
		monster->Rotate(ROTATE_AXIS::X, monsterInitData[i].eulerRotate.x);
		monster->Rotate(ROTATE_AXIS::Z, monsterInitData[i].eulerRotate.z);
		m_smallMonsters.push_back(monster);
	}

	//m_bossMonster = std::make_shared<BossMonsterObject>(1, 50.0f, 30.0f, std::static_pointer_cast<Room>(shared_from_this()));

	//ProjectileObject Initialize
	for (auto& character : m_characters) {
		character.second->SetStagePosition(m_roomState);
	}
}

void Room::SetBossStage()
{
	if (ROOM_STATE::ROOM_COMMON != m_roomState) return;
	for (auto& character : m_characters) {
		character.second->ForceStopMove();
		character.second->SetStagePosition(ROOM_STATE::ROOM_BOSS);
		character.second->ResetSkillCoolTime();
	}
}

void Room::PlayerHeal()
{
	static constexpr float HEAL_HP = 75.0f;
	if (!m_isContinueHeal)return;
	auto healPacket = std::make_shared<SERVER_PACKET::NotifyHealPacket>(static_cast<char>(SERVER_PACKET::TYPE::NOTIFY_HEAL_HP));
	int i = 0;
	for (auto& character : m_characters) {
		character.second->Heal(HEAL_HP);
		healPacket->applyHealPlayerInfo[i].role = character.first;
		healPacket->applyHealPlayerInfo[i].hp = character.second->GetHp();
		++i;
	}
	BroadCastPacket(std::static_pointer_cast<PacketHeader>(healPacket));
	InsertTimerEvent(TIMER_EVENT_TYPE::EV_HEAL, std::chrono::milliseconds(1000));
}

void Room::PlayerApplyShield()
{
	for (auto& character : m_characters)
		character.second->SetShield(true);
	auto shieldPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::SHIELD_START));
	BroadCastPacket(shieldPacket);

}

void Room::PlayerRemoveShield()
{
	for (auto& character : m_characters)
		character.second->SetShield(false);
	auto shieldPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::SHIELD_END));
	BroadCastPacket(shieldPacket);
}

void Room::RainArrowAttack()
{
	//UpdateAfterEvent�� ������ concurrent�� ���� ����
	std::static_pointer_cast<ArcherObject>(m_characters[ROLE::ARCHER])->AttackRainArrow();
}

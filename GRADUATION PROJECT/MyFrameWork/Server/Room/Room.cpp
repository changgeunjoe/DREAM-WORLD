#include "stdafx.h"
#include "Room.h"
#include "../Timer/Timer.h"
#include "RoomEvent.h"
#include "../Network/ExpOver/ExpOver.h"
#include "../Network/UserSession/UserSession.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Monster/SmallMonsterObject.h"
#include "../GameObject/Monster/BossMonsterObject.h"
#include "../Network/protocol/protocol.h"
#include "../MapData/MapData.h"


Room::Room(std::vector<std::shared_ptr<UserSession>>& userRefVec, std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef)
	: m_updateCnt(0), m_roomState(ROOM_STATE::ROOM_COMMON), m_gameStateUpdateComplete(false)
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
		Update();
		InsertEvent(TIMER_EVENT_TYPE::EV_ROOM_UPDATE, 17ms);
		//GameStateSend를 UPdate 3번할 때 send하게 수정해야함.
	}
	break;
	case IOCP_OP_CODE::OP_GAME_STATE_SEND:
	{
		GameStateSend();
		//GameState전송을 성공했다면
		m_gameStateUpdateComplete = true;
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

	//Room - player skill
	case IOCP_OP_CODE::OP_PLAYER_HEAL:
	{

	}
	break;
	case IOCP_OP_CODE::OP_SKY_ARROW_ATTACK:
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
	InitializeAllGameObject();
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
	InsertEvent(TIMER_EVENT_TYPE::EV_ROOM_UPDATE, 20ms);
	//InsertEvent(TIMER_EVENT_TYPE::EV_GAME_STATE_SEND, 50ms);
}

std::vector<std::shared_ptr<SmallMonsterObject>>& Room::GetSmallMonsters()
{
	return m_smallMonsters;
}

std::vector<std::shared_ptr<GameObject>> Room::GetCharacters() const
{
	std::vector<std::shared_ptr<GameObject>> characters;
	characters.reserve(4);
	for (auto& character : m_characters)
		characters.push_back(character.second->shared_from_this());
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

std::shared_ptr<MapData> Room::GetMapData() const
{
	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
		return m_stageMapData;
	}
	return m_bossMapData;
}

void Room::RecvCharacterMove(const ROLE& role, const DIRECTION& direction, const bool& apply)
{
	if (ROLE::NONE_SELECT == role)return;
	m_characters[role]->RecvDirection(direction, apply);

	PacketHeader* sendPacket = nullptr;
	if (apply) sendPacket = &SERVER_PACKET::MovePacket(role, direction, std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::MOVE_KEY_DOWN));
	else sendPacket = &SERVER_PACKET::MovePacket(role, direction, std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::MOVE_KEY_UP));

	MultiCastCastPacket(sendPacket, role);
}

void Room::RecvCharacterStop(const ROLE& role)
{
	if (ROLE::NONE_SELECT == role)return;
	m_characters[role]->StopMove();

	PacketHeader* sendPacket = nullptr;
	sendPacket = &SERVER_PACKET::StopPacket(role, std::chrono::high_resolution_clock::now(), static_cast<char>(SERVER_PACKET::TYPE::STOP));

	MultiCastCastPacket(sendPacket, role);
}

void Room::RecvCharacterRotate(const ROLE& role, const ROTATE_AXIS& axis, const float& angle)
{
	if (ROLE::NONE_SELECT == role)return;
	m_characters[role]->RecvRotate(axis, angle);

	PacketHeader* sendPacket = nullptr;
	sendPacket = &SERVER_PACKET::RotatePacket(role, axis, angle);
	MultiCastCastPacket(sendPacket, role);
}

void Room::RecvMouseInput(const ROLE& role, const bool& left, const bool& right)
{
	if (ROLE::NONE_SELECT == role)return;

}

void Room::InsertEvent(const TIMER_EVENT_TYPE& eventType, const std::chrono::milliseconds& updateTick)
{
	auto timerEvent = std::make_shared<TIMER::RoomEvent>(eventType, updateTick, std::static_pointer_cast<Room>(shared_from_this()));
	TIMER::Timer::GetInstance().InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(timerEvent));
}

void Room::Update()
{
	constexpr static int GAME_STATE_SEND_CNT = 3;

	//Update GameObject
	for (auto& gameObject : m_allGameObjects) {
		gameObject->Update();
	}
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
	//패킷 데이터 저장
	//PQGS로 send()
	InsertEvent(TIMER_EVENT_TYPE::EV_SEND_GAME_STATE, std::chrono::milliseconds(1));
}

void Room::GameStateSend()
{
	//현재 보낼 게임상태에 적용된 룸 상태와 현재 룸 상태가 다르다면 보내지 않음.

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
	int monsterIdx = 0;
	for (auto& monster : m_smallMonsters) {
		if (!gameStateData->smallMonster[monsterIdx].isAlive) continue;
		gameStateData->smallMonster[monsterIdx].idx = monster->GetIdx();
		gameStateData->smallMonster[monsterIdx].hp = monster->GetHp();
		gameStateData->smallMonster[monsterIdx].position = monster->GetPosition();
		gameStateData->smallMonster[monsterIdx].time = monster->GetLastUpdateTime();
		++monsterIdx;
	}
	gameStateData->aliveMonsterCnt = monsterIdx;
}

void Room::SetGameStateBoss()
{
	auto gameStateData = std::static_pointer_cast<SERVER_PACKET::GameState_BOSS>(m_gameStateData[1]);
}

void Room::InitializeAllGameObject()
{
	m_gameStateData.push_back(std::make_shared<SERVER_PACKET::GameState_STAGE>());
	m_gameStateData.push_back(std::make_shared<SERVER_PACKET::GameState_BOSS>());

	std::vector<std::chrono::seconds> duTime;
	std::vector<std::chrono::seconds> coolTime;
	//Character Initialize
	m_characters.emplace(ROLE::WARRIOR, std::make_shared<WarriorObject>(600.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this()), duTime, coolTime));
	m_characters.emplace(ROLE::TANKER, std::make_shared<TankerObject>(780.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this()), duTime, coolTime));
	m_characters.emplace(ROLE::ARCHER, std::make_shared<ArcherObject>(400.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this()), duTime, coolTime));
	m_characters.emplace(ROLE::MAGE, std::make_shared<MageObject>(500.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this()), duTime, coolTime));
	m_characters[ROLE::ARCHER]->debug = true;

	auto monsterInitData = m_stageMapData->GetMonsterInitData();
	for (int i = 0; i < monsterInitData.size(); ++i) {
		auto monster = std::make_shared<SmallMonsterObject>(150.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this()), i);
		monster->SetPosition(monsterInitData[i].position);
		monster->Rotate(ROTATE_AXIS::Y, monsterInitData[i].eulerRotate.y);
		monster->Rotate(ROTATE_AXIS::X, monsterInitData[i].eulerRotate.x);
		monster->Rotate(ROTATE_AXIS::Z, monsterInitData[i].eulerRotate.z);
		m_smallMonsters.push_back(monster);
	}

	//m_bossMonster = std::make_shared<BossMonsterObject>(1, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this()));

	//ProjectileObject Initialize
	for (auto& character : m_characters) {
		character.second->SetStagePosition(m_roomState);
		m_allGameObjects.push_back(character.second);
	}
	for (auto& monster : m_smallMonsters)
		m_allGameObjects.push_back(monster);
	//m_allGameObjects.push_back(m_bossMonster);
	//InsertProjectileObject
}

#include "stdafx.h"
//#include "Room.h"
//#include "../Timer/Timer.h"
//#include "../Logic/Logic.h"
//#include "../Network/IOCP/IOCP.h"
//#include "../SessionObject/Character/ChracterSessionObject.h"
//#include "../MapData/MapData.h"
//#include "../Network/UserSession/UserSession.h"
////#include "../Network/PacketManager/PacketManager.h"
//#include "../Network/protocol/protocol.h"
//
//extern MapData		g_stage1MapData;
//extern MapData		g_bossMapData;
//
//extern Timer g_Timer;
//extern IOCP g_IOCP;
//extern std::array<UserSession, MAX_USER> g_userSession;
//
//
//
//Room::Room() :m_boss(MonsterSessionObject(m_roomId)), m_isAlive(false)
//{
//	m_characterMap.try_emplace(ROLE::WARRIOR, new WarriorSessionObject(ROLE::WARRIOR));
//	m_characterMap.try_emplace(ROLE::TANKER, new TankerSessionObject(ROLE::TANKER));
//	m_characterMap.try_emplace(ROLE::ARCHER, new ArcherSessionObject(ROLE::ARCHER));
//	m_characterMap.try_emplace(ROLE::PRIEST, new MageSessionObject(ROLE::PRIEST));
//
//	while (!g_stage1MapData.GetCompleteState());
//	auto& monsterData = g_stage1MapData.GetMonsterData();
//	for (int i = 0; i < 15; i++) {
//		m_StageSmallMonster[i].SetInitPosition(monsterData[i].position);
//		m_StageSmallMonster[i].Rotate(ROTATE_AXIS::Y, monsterData[i].eulerRotate.y);
//		m_StageSmallMonster[i].SetId(i);
//	}
//
//	while (!g_bossMapData.GetCompleteState());
//	auto& bossMonsterData = g_bossMapData.GetMonsterData();
//	for (int i = 0; i < 10; i++) {
//		m_arrows[i].SetInfo(i);
//		m_arrows[i].SetRoomId(m_roomId);
//		m_arrows[i].SetOwnerRole(ROLE::ARCHER);
//
//		m_balls[i].SetInfo(i);
//		m_balls[i].SetRoomId(m_roomId);
//		m_balls[i].SetOwnerRole(ROLE::PRIEST);
//
//		m_restArrow.push(i);
//		m_restBall.push(i);
//
//		m_meteos[i].SetId(i);
//		m_meteos[i].SetRoomId(m_roomId);
//		m_meteos[i].InitSetDamage();
//	}
//	for (int i = 0; i < 3; i++) {
//		m_skillarrow[i];
//		m_skillarrow[i].SetInfo(i);
//		m_skillarrow[i].SetRoomId(m_roomId);
//		m_skillarrow[i].SetUseSkill(true);
//		m_skillarrow[i].SetDamage(200.0f);
//		m_skillarrow[i].SetOwnerRole(ROLE::ARCHER);
//	}
//}
//
//Room::Room(const Room& rhs)
//{
//	m_characterMap = rhs.m_characterMap;
//}
//
//Room::~Room()
//{
//	for (auto& character : m_characterMap) {
//		delete character.second;
//	}
//}
//
//Room& Room::operator=(Room& rhs)
//{
//	return *this;
//}
//
//std::unordered_map<int, ROLE> Room::GetPlayerInfo()
//{
//	std::lock_guard<mutex> lg{m_playerMapLock};
//	return m_playerMap;
//}
//
//std::map<ROLE, ChracterSessionObject*>& Room::GetCharactersInfo()
//{
//	return m_characterMap;
//	// TODO: 여기에 return 문을 삽입합니다.
//}
//
//int Room::GetPlayerIdFromRole(const ROLE& role)
//{
//	auto playerMap = GetPlayerInfo();
//	auto findPlayIter = find_if(playerMap.begin(), playerMap.end(), [&role](const auto& ele) {
//		return ele.second == role;
//		});
//	if (findPlayIter == playerMap.end())return -1;
//	return findPlayIter->first;
//}
//
//void Room::SetRoomId(int roomId)
//{
//	m_roomId = roomId;
//}
//
//ROOM_STATE Room::GetStageState()
//{
//	return m_roomState;
//}
//
//void Room::ChracterAddDirection(const int userId, const ROLE& role, const DIRECTION& direction)
//{
//	if (role == ROLE::NONE_SELECT) return;
//	m_characterMap[role]->StartMove(direction);
//	auto position = m_characterMap[role]->GetPos();
//	auto moveDirection = m_characterMap[role]->GetDirectionVector();
//	SERVER_PACKET::MovePacket sendPacket{SERVER_PACKET::MOVE_KEY_DOWN, role, direction, position, moveDirection};
//	MultiCastPacket(reinterpret_cast<char*>(&sendPacket), userId);
//}
//
//void Room::ChracterRemoveDirection(const int userId, const ROLE& role, const DIRECTION& direction)
//{
//	if (role == ROLE::NONE_SELECT) return;
//	m_characterMap[role]->ChangeDirection(direction);
//	auto position = m_characterMap[role]->GetPos();
//	auto moveDirection = m_characterMap[role]->GetDirectionVector();
//	SERVER_PACKET::MovePacket sendPacket{SERVER_PACKET::MOVE_KEY_DOWN, role, direction, position, moveDirection};
//	MultiCastPacket(reinterpret_cast<char*>(&sendPacket), userId);
//}
//
//void Room::ChracterStop(const ROLE& role)
//{
//	if (role == ROLE::NONE_SELECT) return;
//	m_characterMap[role]->StopMove();
//	auto position = m_characterMap[role]->GetPos();
//	SERVER_PACKET::StopPacket sendPacket{SERVER_PACKET::STOP, role, position};
//}
//
//void Room::CharacterRotate(const ROLE& role, const ROTATE_AXIS& axis, const float& angle)
//{
//	if (role == ROLE::NONE_SELECT) return;
//	m_characterMap[role]->Rotate(axis, angle);
//}
//
//void Room::CharacterMouseInput(const ROLE& role, const bool& left, const bool& right)
//{
//	m_characterMap[role]->SetMouseInput(left, right);
//}
//
//void Room::CharacterSkill_Execute_Q(const ROLE& role, const XMFLOAT3& float3)
//{
//	if (role == ROLE::NONE_SELECT) return;
//	m_characterMap[role]->Skill_1(float3);
//}
//
//void Room::CharacterSkill_Execute_E(const ROLE& role, const XMFLOAT3& float3)
//{
//	if (role == ROLE::NONE_SELECT) return;
//	m_characterMap[role]->Skill_2(float3);
//}
//
//void Room::CharacterSkill_Input_Q(const ROLE& role)
//{
//	//굳이 내용 없어도 될듯?
//}
//
//void Room::CharacterSkill_Input_E(const ROLE& role)
//{
//	//굳이 내용 없어도 될듯?
//}
//
//void Room::CharacterCommonAttackExecute(const ROLE& role, const XMFLOAT3& dir, const int& power)
//{
//}
//
//void Room::SkipNPC_Communication()
//{
//}
//
//void Room::GameEnd()
//{
//}
//
//void Room::ChangeToBossStage()
//{
//	//SERVER_PACKET::NotifyPacket sendPacket;
//	//sendPacket.type = SERVER_PACKET::STAGE_CHANGING_BOSS;
//	//sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
//	//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
//
//	//m_boss.SetInitPosition(XMFLOAT3(179.4, 0, -38.1));
//	//m_boss.SetOnidx(g_bossMapData.GetBossStartIdx());
//	//for (auto& character : m_characterMap) {
//	//	character.second->StopMove();
//	//	character.second->SetBossStagePosition();
//	//}
//
//	//SERVER_PACKET::GameState_BOSS_INIT sendInitPacket;
//	//sendInitPacket.type = SERVER_PACKET::STAGE_START_BOSS;
//	//sendInitPacket.size = sizeof(SERVER_PACKET::GameState_BOSS_INIT);
//	//int i = 0;
//	//std::map<ROLE, int> playerMap;
//	//{
//	//	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
//	//	playerMap = m_inGamePlayers;
//	//}
//	//sendInitPacket.userState[0].role = ROLE::NONE_SELECT;
//	//sendInitPacket.userState[1].role = ROLE::NONE_SELECT;
//	//sendInitPacket.userState[2].role = ROLE::NONE_SELECT;
//	//sendInitPacket.userState[3].role = ROLE::NONE_SELECT;
//	//sendInitPacket.bossPosition = m_boss.GetPos();
//	//for (auto& p : playerMap) {
//	//	sendInitPacket.userState[i].role = p.first;
//	//	sendInitPacket.userState[i].hp = m_characterMap[p.first]->GetHp();
//	//	sendInitPacket.userState[i].pos = m_characterMap[p.first]->GetPos();
//	//	sendInitPacket.userState[i].rot = m_characterMap[p.first]->GetRot();
//	//	++i;
//	//}
//	//g_logic.BroadCastInRoom(m_roomId, &sendInitPacket);
//
//	////boss Stage Start Packet send
//	////position 데이터 보내기
//
//	//TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), m_roomId ,EV_GAME_STATE_B_SEND };//GameState 30ms마다 전송하게 수정
//	//g_Timer.InsertTimerQueue(new_ev);
//	//TIMER_EVENT findEv{ std::chrono::system_clock::now() + std::chrono::seconds(3), m_roomId ,EV_FIND_PLAYER };
//	//g_Timer.InsertTimerQueue(findEv);
//	//TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(15), m_roomId, EV_BOSS_STATE };
//	////TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(11), m_roomId ,EV_BOSS_STATE };
//	//g_Timer.InsertTimerQueue(bossStateEvent);
//	//m_meteoTime = std::chrono::high_resolution_clock::now();
//
//}
//
//void Room::GameStart()
//{
//	auto playerMap = GetPlayerInfo();//first - id, second - role
//	for (auto player : playerMap) {
//		auto postion = m_characterMap[player.second]->GetPos();
//		auto rotation = m_characterMap[player.second]->GetRot();
//		SERVER_PACKET::AddPlayerPacket sendPacket{SERVER_PACKET::ADD_PLAYER, player.second, player.first, postion, rotation};
//		BroadCastPacket(reinterpret_cast<char*>(&sendPacket), playerMap);
//	}
//	//TIMER_EVENT smallMonsterEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1) + std::chrono::milliseconds(500), m_roomId ,EV_SM_UPDATE };
//	//g_Timer.InsertTimerQueue(smallMonsterEvent);
//	//TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(500), m_roomId ,EV_GAME_STATE_S_SEND };
//	//g_Timer.InsertTimerQueue(gameStateEvent);
//}
//
//void Room::Update()
//{
//	for (auto& playCharacter : m_characterMap) {//플레이어 무브
//		playCharacter.second->AutoMove();
//	}
//	if (m_roomState == ROOM_STATE::ROOM_STAGE1) {
//		for (int i = 0; i < 15; i++) {
//			m_StageSmallMonster[i].AutoMove();
//		}
//	}
//	else if (m_roomState == ROOM_STATE::ROOM_BOSS) {
//		if (m_boss.isMove)
//			m_boss.AutoMove();//보스 무브
//		for (auto& meteo : m_meteos)
//			if (meteo.GetActive()) {
//				meteo.AutoMove();
//			}
//	}
//	for (auto& arrow : m_arrows)
//		arrow.AutoMove();
//	for (auto& ball : m_balls)
//		ball.AutoMove();
//	for (auto& arrow : m_skillarrow)
//		arrow.AutoMove();
//}
//
//void Room::BroadCastPacket(const char* packet)
//{
//	auto playerMap = GetPlayerInfo();//first - id, second - role
//	for (auto player : playerMap)
//		g_userSession[player.first].SendPacket(packet);
//}
//
//void Room::BroadCastPacket(const char* packet, const std::unordered_map<int, ROLE>& playerMap)
//{
//	for (auto player : playerMap)
//		g_userSession[player.first].SendPacket(packet);
//}
//
//void Room::MultiCastPacket(const char* packet, int exclusionId)
//{
//	auto playerMap = GetPlayerInfo();//first - id, second - role
//	for (auto player : playerMap) {
//		if (player.first == exclusionId)continue;
//		g_userSession[player.first].SendPacket(packet);
//	}
//}
//
//void Room::ExecuteMageThunder(const XMFLOAT3& position)
//{
//	if (m_roomState == ROOM_STAGE1) {
//		std::vector<int> positions;
//		positions.reserve(6);
//		for (int i = 0; i < 15; i++) {
//			if (m_StageSmallMonster[i].IsAlive()) {
//				if (m_StageSmallMonster[i].GetDistance(position) < 24.0f) {
//					m_StageSmallMonster[i].AttackedHp(120.0f);
//					positions.emplace_back(i);
//				}
//			}
//		}
//		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
//		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
//		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
//		sendPacket.role = ROLE::PRIEST;
//		sendPacket.attackedMonsterCnt = positions.size();
//		sendPacket.damage = 120.0f;
//		for (int i = 0; i < positions.size(); i++) {
//			sendPacket.monsterIdx[i] = positions[i];
//		}
//
//		int sendId = GetPlayerIdFromRole(ROLE::PRIEST);
//		if (sendId != -1)
//			g_userSession[sendId].SendPacket(reinterpret_cast<char*> (&sendPacket));
//	}
//	else {
//		m_boss.AttackedHp(120.0f);
//	}
//	SERVER_PACKET::ShootingObject sendPacket{SERVER_PACKET::EXECUTE_LIGHTNING, ROLE::PRIEST, position};
//	int sendId = GetPlayerIdFromRole(ROLE::PRIEST);
//	MultiCastPacket(reinterpret_cast<char*>(&sendPacket), sendId);
//}
//
//void Room::ExecuteLongSwordAttack(const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& pos)
//{
//	DirectX::XMFLOAT3 attackDirection = dir;
//	DirectX::XMFLOAT3 position = pos;
//	if (m_roomState == ROOM_BOSS) {
//		DirectX::XMFLOAT3 bossPos = m_boss.GetPos();
//		DirectX::XMFLOAT3 toBoss = Vector3::Subtract(bossPos, attackDirection);
//		attackDirection = Vector3::Normalize(attackDirection);
//		if (Vector3::DotProduct(attackDirection, Vector3::Normalize(toBoss)) > MONSTER_ABLE_ATTACK_COS_VALUE) {
//			if (Vector3::Length(toBoss) < 80.0f) {
//				m_boss.AttackedHp(180.0f);
//				return;
//			}
//		}
//	}
//	else {
//		std::vector<int> attackedMonster;
//		attackedMonster.reserve(10);
//		for (int i = 0; i < 15; i++) {
//			DirectX::XMFLOAT3 monsterVector = Vector3::Subtract(m_StageSmallMonster[i].GetPos(), position);
//			attackDirection = Vector3::Normalize(attackDirection);
//			if (Vector3::DotProduct(attackDirection, Vector3::Normalize(monsterVector)) > MONSTER_ABLE_ATTACK_COS_VALUE) {
//				if (Vector3::Length(monsterVector) < 80.0f) {
//					m_StageSmallMonster[i].AttackedHp(180.0f);
//					attackedMonster.emplace_back(i);
//				}
//			}
//		}
//		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
//		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
//		sendPacket.role = ROLE::WARRIOR;
//		sendPacket.attackedMonsterCnt = attackedMonster.size();
//		for (int i = 0; i < sendPacket.attackedMonsterCnt; i++)
//			sendPacket.monsterIdx[i] = (char)attackedMonster[i];
//		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
//		sendPacket.damage = 180.0f;
//		int sendId = GetPlayerIdFromRole(ROLE::WARRIOR);
//		if (sendId == -1) return;
//		g_userSession[sendId].SendPacket(reinterpret_cast<char*> (&sendPacket));
//	}
//}
//
//void Room::ExecuteThreeArrow(const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& position)
//{
//	//offset 적용됨
//	XMFLOAT3 xmf3Position = position;
//	XMFLOAT3 attackDirection = dir;
//	XMFLOAT3 rightVector = Vector3::CrossProduct(DirectX::XMFLOAT3(0, 1, 0), attackDirection);
//	for (int i = 0; i < 3; i++) {
//		xmf3Position.y = 6.0f + (i % 2) * 4.0f;
//		xmf3Position = Vector3::Add(xmf3Position, rightVector, (1 - i) * 4.0f);
//		xmf3Position = Vector3::Add(xmf3Position, attackDirection, 1.0f);
//		m_skillarrow[i].SetStart(attackDirection, xmf3Position, 110.0f);
//	}
//	SERVER_PACKET::ShootingObject sendPacket{SERVER_PACKET::SHOOTING_ARROW, ROLE::ARCHER, position};
//	int sendId = GetPlayerIdFromRole(ROLE::WARRIOR);
//	MultiCastPacket(reinterpret_cast<char*>(&sendPacket), sendId);
//}
//
//void Room::ExecuteHammerAttack(const DirectX::XMFLOAT3& dir, const XMFLOAT3& pos)
//{
//	XMFLOAT3 position = pos;
//	XMFLOAT3 attackDirection = dir;
//	if (m_roomState == ROOM_BOSS) {
//		DirectX::XMFLOAT3 bossPos = m_boss.GetPos();
//		DirectX::XMFLOAT3 toBoss = Vector3::Subtract(bossPos, position);
//		attackDirection = Vector3::Normalize(attackDirection);
//		if (Vector3::DotProduct(attackDirection, Vector3::Normalize(toBoss)) > MONSTER_ABLE_ATTACK_COS_VALUE) {
//			if (Vector3::Length(toBoss) < 50.0f) {
//				m_boss.AttackedHp(140.0f);
//				SERVER_PACKET::PlayerAttackBossDamagePacket sendPacket;
//				sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT_BOSS;
//				sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackBossDamagePacket);
//				sendPacket.damage = 140.0f;
//				sendPacket.role = ROLE::TANKER;
//				BroadCastPacket(reinterpret_cast<char*>(&sendPacket));
//				return;
//			}
//		}
//	}
//	else {
//		std::vector<int> attackedMonster;
//		attackedMonster.reserve(10);
//		for (int i = 0; i < 15; i++) {
//			DirectX::XMFLOAT3 monsterVector = Vector3::Subtract(m_StageSmallMonster[i].GetPos(), position);
//			attackDirection = Vector3::Normalize(attackDirection);
//			if (Vector3::DotProduct(attackDirection, Vector3::Normalize(monsterVector)) > MONSTER_ABLE_ATTACK_COS_VALUE) {
//				if (Vector3::Length(monsterVector) < 50.0f) {
//					m_StageSmallMonster[i].AttackedHp(140.0f);
//					attackedMonster.emplace_back(i);
//				}
//			}
//		}
//		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
//		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
//		sendPacket.role = ROLE::TANKER;
//		sendPacket.attackedMonsterCnt = attackedMonster.size();
//		for (int i = 0; i < sendPacket.attackedMonsterCnt; i++)
//			sendPacket.monsterIdx[i] = (char)attackedMonster[i];
//		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
//		sendPacket.damage = 140.0f;
//		BroadCastPacket(reinterpret_cast<char*>(&sendPacket));
//	}
//}
//
//void Room::ExecuteSkyArrow()
//{
//	if (m_roomState == ROOM_BOSS) {
//		m_boss.AttackedHp(120.0f);
//	}
//	else {
//		std::vector<int> positions;
//		positions.reserve(6);
//		for (int i = 0; i < 15; i++) {
//			if (m_StageSmallMonster[i].IsAlive()) {
//				if (m_StageSmallMonster[i].GetDistance(m_skyArrowAttack) < 24.0f) {
//					m_StageSmallMonster[i].AttackedHp(120.0f);
//					positions.emplace_back(i);
//				}
//			}
//		}
//		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
//		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
//		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
//		sendPacket.role = ROLE::ARCHER;
//		sendPacket.attackedMonsterCnt = positions.size();
//		sendPacket.damage = 150.0f;
//		for (int i = 0; i < positions.size(); i++) {
//			sendPacket.monsterIdx[i] = positions[i];
//		}
//		int sendId = GetPlayerIdFromRole(ROLE::PRIEST);
//		if (sendId == -1) return;
//		g_userSession[sendId].SendPacket(reinterpret_cast<char*> (&sendPacket));
//	}
//}
//
//void Room::MeleeAttack(ROLE r, DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 pos, int power)
//{
//	float damage = 100.0f;
//	if (power == 0)damage = 100.0f;
//	if (power == 1)damage = 130.0f;
//	if (power == 2)damage = 160.0f;
//
//	if (m_roomState == ROOM_BOSS) {
//		DirectX::XMFLOAT3 bossPos = m_boss.GetPos();
//		DirectX::XMFLOAT3 toBoss = Vector3::Subtract(bossPos, pos);
//		dir = Vector3::Normalize(dir);
//		if (Vector3::DotProduct(dir, Vector3::Normalize(toBoss)) > PLAYER_ABLE_ATTACK_COS_VALUE) {
//			if (Vector3::Length(toBoss) < 45.0f) {
//				m_boss.AttackedHp(damage);
//				SERVER_PACKET::PlayerAttackBossDamagePacket sendPacket;
//				return;
//			}
//		}
//	}
//	else {
//		std::vector<int> attackedMonster;
//		attackedMonster.reserve(10);
//		for (int i = 0; i < 15; i++) {
//			DirectX::XMFLOAT3 monsterVector = Vector3::Subtract(m_StageSmallMonster[i].GetPos(), pos);
//			dir = Vector3::Normalize(dir);
//			if (Vector3::DotProduct(dir, Vector3::Normalize(monsterVector)) > PLAYER_ABLE_ATTACK_COS_VALUE) {
//				if (Vector3::Length(monsterVector) < 45.0f) {
//					m_StageSmallMonster[i].AttackedHp(damage);
//					attackedMonster.emplace_back(i);
//				}
//			}
//		}
//		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
//		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
//		sendPacket.role = r;
//		sendPacket.attackedMonsterCnt = attackedMonster.size();
//		for (int i = 0; i < sendPacket.attackedMonsterCnt; i++)
//			sendPacket.monsterIdx[i] = (char)attackedMonster[i];
//		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
//		sendPacket.damage = damage;
//		int userId = -1;
//		//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
//		int sendId = GetPlayerIdFromRole(r);
//		if (sendId == -1) return;
//		g_userSession[sendId].SendPacket(reinterpret_cast<char*> (&sendPacket));
//	}
//}
//
//void Room::StartHealPlayerCharacter()
//{
//	SERVER_PACKET::NotifyPacket sendPacket{SERVER_PACKET::HEAL_START};
//	BroadCastPacket(reinterpret_cast<char*>(&sendPacket));
//	HealPlayerCharacter();
//}
//
//void Room::UpdateShieldData()
//{
//	if (m_characterMap[ROLE::TANKER]->IsDurationEndTimeSkill_1())
//		RemoveBarrier();
//	else {
//		SERVER_PACKET::NotifyShieldPacket sendPacket{SERVER_PACKET::NOTIFY_SHIELD_APPLY};
//		int i = 0;
//		for (auto& playCharcter : m_characterMap) {
//			sendPacket.applyShieldPlayerInfo[i].shield = playCharcter.second->GetShield();
//			sendPacket.applyShieldPlayerInfo[i].role = playCharcter.first;
//			++i;
//		}
//		BroadCastPacket(reinterpret_cast<char*>(&sendPacket));
//	}
//}
//
//void Room::PutBarrierOnPlayer()
//{
//	for (auto& p : m_characterMap) {
//		p.second->SetShield(true);
//	}
//}
//
//void Room::RemoveBarrier()
//{
//	for (auto& p : m_characterMap) {
//		p.second->SetShield(false);
//	}
//	SERVER_PACKET::NotifyPacket sendPacket{SERVER_PACKET::SHIELD_END};
//	BroadCastPacket(reinterpret_cast<char*>(&sendPacket));
//}
//
//void Room::HealPlayerCharacter()
//{
//	if (!m_characterMap[ROLE::PRIEST]->IsDurationEndTimeSkill_1()) {
//		XMFLOAT3 xmf3MagePos = m_characterMap[ROLE::PRIEST]->GetPos();
//		for (auto& p : m_characterMap) {
//			XMFLOAT3 xmf3CharacterPos = p.second->GetPos();
//			if (Vector3::Length(Vector3::Subtract(xmf3CharacterPos, xmf3MagePos)) <= 75.0f) {	// HealRange Radius == 75.0f
//				p.second->HealHp(40.0f);
//			}
//		}
//		TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1), m_roomId ,EV_HEAL };
//		g_Timer.InsertTimerQueue(gameStateEvent);
//		SERVER_PACKET::NotifyHealPacket sendPacket{SERVER_PACKET::NOTIFY_HEAL_HP};
//		int i = 0;
//		for (auto& playCharcter : m_characterMap) {
//			sendPacket.applyHealPlayerInfo[i].hp = playCharcter.second->GetHp();
//			sendPacket.applyHealPlayerInfo[i].role = playCharcter.first;
//			++i;
//		}
//		BroadCastPacket(reinterpret_cast<char*>(&sendPacket));
//	}
//	else {
//		SERVER_PACKET::NotifyPacket sendPacket{SERVER_PACKET::HEAL_END};
//		BroadCastPacket(reinterpret_cast<char*>(&sendPacket));
//	}
//}
//
//void Room::StartSkyArrow(const XMFLOAT3& position)
//{
//	m_skyArrowAttack = position;
//	TIMER_EVENT skyArrowEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1), m_roomId,  EV_SKY_ARROW_ATTACK };
//	g_Timer.InsertTimerQueue(skyArrowEvent);
//	SERVER_PACKET::ShootingObject sendPacket{SERVER_PACKET::SHOOTING_ARROW, ROLE::ARCHER, position};
//	int sendId = GetPlayerIdFromRole(ROLE::WARRIOR);
//	MultiCastPacket(reinterpret_cast<char*>(&sendPacket), sendId);
//}
//
//void Room::PushRestArrow(int id)
//{
//	//std::cout << "push arrowIdx: " << id << std::endl;
//	m_restArrow.push(id);
//}
//
//void Room::PushRestBall(int id)
//{
//	m_restBall.push(id);
//}
//
//void Room::ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed, float damage)
//{
//	if (!m_isAlive)return;
//	int arrowIndex = -1;
//	if (m_restArrow.try_pop(arrowIndex)) {
//		//std::cout << "pop arrowIdx: " << arrowIndex << std::endl;
//		XMFLOAT3 xmf3Position = srcPos;
//		xmf3Position.y += 8.0f;
//		XMFLOAT3 rightVector = Vector3::CrossProduct(DirectX::XMFLOAT3(0, 1, 0), dir);
//		xmf3Position = Vector3::Add(xmf3Position, dir, 1.0f);
//		m_arrows[arrowIndex].SetStart(dir, xmf3Position, speed);
//		m_arrows[arrowIndex].SetDamage(damage);
//	}
//	SERVER_PACKET::ShootingObject sendPacket{SERVER_PACKET::SHOOTING_ARROW, ROLE::ARCHER, dir};
//	int sendId = GetPlayerIdFromRole(ROLE::WARRIOR);
//	MultiCastPacket(reinterpret_cast<char*>(&sendPacket), sendId);
//}
//
//void Room::ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos)
//{
//	if (!m_isAlive)return;
//	int ballIndex = -1;
//	if (m_restBall.try_pop(ballIndex)) {
//		XMFLOAT3 xmf3Position = srcPos;
//		xmf3Position.y += 8.0f;
//		m_balls[ballIndex].SetStart(dir, xmf3Position, 100.0f);
//		m_balls[ballIndex].SetDamage(m_characterMap[ROLE::PRIEST]->GetAttackDamage());
//		SERVER_PACKET::ShootingObject sendPacket{SERVER_PACKET::SHOOTING_BALL, ROLE::PRIEST, dir};
//		BroadCastPacket(reinterpret_cast<char*>(&sendPacket));
//	}
//}
//
//const MonsterSessionObject& Room::GetBoss()
//{
//	return m_boss;
//}
//
//SmallMonsterSessionObject* const Room::GetStageMonsterArr()
//{
//	return m_StageSmallMonster;
//}
//
//

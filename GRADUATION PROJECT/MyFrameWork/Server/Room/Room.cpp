#include "stdafx.h"
#include "Room.h"
#include "../Timer/Timer.h"
#include "../Logic/Logic.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../Session/SessionObject/ChracterSessionObject.h"
#include "../IOCPNetwork/protocol/protocol.h"
#include "../MapData/MapData.h"

extern MapData		g_stage1MapData;
extern MapData		g_bossMapData;

extern Timer g_Timer;
extern Logic g_logic;
extern IOCPNetwork g_iocpNetwork;

Room::Room() :m_boss(MonsterSessionObject(m_roomId))
{
	m_isAlive = false;
	m_characterMap.try_emplace(ROLE::WARRIOR, new WarriorSessionObject(ROLE::WARRIOR));
	m_characterMap.try_emplace(ROLE::TANKER, new TankerSessionObject(ROLE::TANKER));
	m_characterMap.try_emplace(ROLE::ARCHER, new ArcherSessionObject(ROLE::ARCHER));
	m_characterMap.try_emplace(ROLE::PRIEST, new MageSessionObject(ROLE::PRIEST));

	while (!g_stage1MapData.GetCompleteState());
	auto& monsterData = g_stage1MapData.GetMonsterData();
	for (int i = 0; i < 15; i++) {
		m_StageSmallMonster[i].SetInitPosition(monsterData[i].position);
		m_StageSmallMonster[i].Rotate(ROTATE_AXIS::Y, monsterData[i].eulerRotate.y);
		m_StageSmallMonster[i].SetId(i);
	}

	while (!g_bossMapData.GetCompleteState());
	auto& bossMonsterData = g_bossMapData.GetMonsterData();
	for (int i = 0; i < 15; i++) {
		m_BossSmallMonster[i].SetInitPosition(bossMonsterData[i].position);
		m_BossSmallMonster[i].Rotate(ROTATE_AXIS::Y, bossMonsterData[i].eulerRotate.y);
		m_BossSmallMonster[i].SetId(i);
	}
	for (int i = 0; i < 10; i++) {
		m_arrows[i].SetInfo(i);
		m_arrows[i].SetRoomId(m_roomId);
		m_arrows[i].SetOwnerRole(ROLE::ARCHER);
		m_balls[i].SetInfo(i);
		m_balls[i].SetRoomId(m_roomId);
		m_balls[i].SetOwnerRole(ROLE::PRIEST);
		m_restArrow.push(i);
		m_restBall.push(i);
	}
	for (int i = 0; i < 3; i++) {
		m_skillarrow[i];
		m_skillarrow[i].SetInfo(i);
		m_skillarrow[i].SetRoomId(m_roomId);
		m_skillarrow[i].SetUseSkill(true);
		m_skillarrow[i].SetDamage(200.0f);
		m_skillarrow[i].SetOwnerRole(ROLE::ARCHER);
	}
}

Room::Room(const Room& rhs)
{
	m_inGamePlayers = rhs.m_inGamePlayers;
	m_roomName = rhs.m_roomName;
	m_roomOwnerId = rhs.m_roomOwnerId;
	m_characterMap = rhs.m_characterMap;
}

Room::~Room()
{
	for (auto& character : m_characterMap) {
		delete character.second;
	}
}

Room& Room::operator=(Room& rhs)
{
	m_inGamePlayers = rhs.m_inGamePlayers;
	m_roomName = rhs.m_roomName;
	m_roomOwnerId = rhs.m_roomOwnerId;
	return *this;
}
void Room::SetRoomId(int roomId)
{
	m_roomId = roomId;
	m_boss.SetRoomId(m_roomId);
	for (auto& character : m_characterMap) {
		character.second->SetRoomId(m_roomId);
		character.second->SetRoomState(m_roomState);
	}
	for (int i = 0; i < 15; i++) {
		m_StageSmallMonster[i].SetRoomId(m_roomId);
	}
	for (int i = 0; i < 10; i++) {
		m_arrows[i].SetRoomId(m_roomId);
		m_balls[i].SetRoomId(m_roomId);
	}
	for (int i = 0; i < 3; i++)
		m_skillarrow[i].SetRoomId(m_roomId);
}

void Room::SendAllPlayerInfo()
{
	std::map<ROLE, int> players;
	{
		std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
		players = m_inGamePlayers;
	}
	for (auto& p : players) {
		SERVER_PACKET::AddPlayerPacket playerInfo;
		playerInfo.role = p.first;
		playerInfo.userId = p.second;
		//memcpy(playerInfo->name, m_playerName.c_str(), m_playerName.size() * 2);
		//playerInfo->name[m_playerName.size()] = 0;
		playerInfo.position = m_characterMap[p.first]->GetPos();
		playerInfo.rotate = m_characterMap[p.first]->GetRot();
		playerInfo.type = SERVER_PACKET::ADD_PLAYER;
		playerInfo.role = p.first;
		std::wstring& name = g_iocpNetwork.m_session[p.second].GetName();
		std::wmemcpy(playerInfo.name, name.c_str(), name.size());
		playerInfo.name[name.size()] = L'\0';
		playerInfo.size = sizeof(SERVER_PACKET::AddPlayerPacket);
		for (auto& sendPlayer : players)
			g_iocpNetwork.m_session[sendPlayer.second].Send(&playerInfo);
	}
}
void Room::InsertInGamePlayer(std::map<ROLE, int>& matchPlayer)
{
	m_roomOwnerId = matchPlayer.begin()->second;
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	m_inGamePlayers = matchPlayer;
	for (auto& i : matchPlayer)
		g_iocpNetwork.m_session[i.second].SetPlaySessionObject(m_characterMap[i.first]);
}

void Room::InsertInGamePlayer(ROLE r, int playerId)
{
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	m_inGamePlayers.insert(std::make_pair(r, playerId));
	//m_Players.insert(std::make_pair(r, playerId));
}

void Room::DeleteInGamePlayer(int playerId)
{
	m_lockInGamePlayers.lock();
	auto findPlayerIter = std::find_if(m_inGamePlayers.begin(), m_inGamePlayers.end(), [&playerId](std::pair<ROLE, int> p)
		{ // playerId가 같은 것을 찾아 제거
			return p.second == playerId;
		}
	);
	if (findPlayerIter == m_inGamePlayers.end()) {
		m_lockInGamePlayers.unlock();
		return;
	}
	m_lockInGamePlayers.unlock();
	{
		std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
		m_inGamePlayers.erase(findPlayerIter);//disconnected된 플레이어 처리
	}
	SkipNPC_Communication();
}

std::map<ROLE, int> Room::GetPlayerMap()
{
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	return m_inGamePlayers;
}

void Room::CreateBossMonster()
{
	//TIMER_EVENT firstEv{ std::chrono::system_clock::now() + std::chrono::milliseconds(200), m_roomId, -1,EV_FIND_PLAYER };
	//g_Timer.InsertTimerQueue(firstEv);
}

MonsterSessionObject& Room::GetBoss()
{
	return m_boss;
}

void Room::PushRestArrow(int id)
{
	std::cout << "push arrowIdx: " << id << std::endl;
	m_restArrow.push(id);
}

void Room::PushRestBall(int id)
{
	m_restBall.push(id);
}

void Room::ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed, float damage)
{
	if (!m_isAlive)return;
	int arrowIndex = -1;
	if (m_restArrow.try_pop(arrowIndex)) {
		std::cout << "pop arrowIdx: " << arrowIndex << std::endl;
		XMFLOAT3 xmf3Position = srcPos;
		xmf3Position.y += 8.0f;
		XMFLOAT3 rightVector = Vector3::CrossProduct(DirectX::XMFLOAT3(0, 1, 0), dir);
		xmf3Position = Vector3::Add(xmf3Position, dir, 1.0f);
		m_arrows[arrowIndex].SetStart(dir, xmf3Position, speed);
		m_arrows[arrowIndex].SetDamage(damage);
	}


}

void Room::ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos)
{
	if (!m_isAlive)return;
	int ballIndex = -1;
	if (m_restBall.try_pop(ballIndex)) {
		XMFLOAT3 xmf3Position = srcPos;
		xmf3Position.y += 8.0f;
		m_balls[ballIndex].SetStart(dir, xmf3Position, 100.0f);
		m_balls[ballIndex].SetDamage(100.0f);
	}
}

void Room::MeleeAttack(ROLE r, DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 pos, int power)
{
	float damage = 100.0f;
	if (power == 0)damage = 100.0f;
	if (power == 1)damage = 130.0f;
	if (power == 2)damage = 160.0f;

	if (m_roomState == ROOM_BOSS) {
		DirectX::XMFLOAT3 bossPos = GetBoss().GetPos();
		DirectX::XMFLOAT3 toBoss = Vector3::Subtract(bossPos, pos);
		dir = Vector3::Normalize(dir);
		if (Vector3::DotProduct(dir, Vector3::Normalize(toBoss)) > PLAYER_ABLE_ATTACK_COS_VALUE) {
			if (Vector3::Length(toBoss) < 45.0f) {
				m_boss.AttackedHp(250.0f);
				SERVER_PACKET::PlayerAttackBossDamagePacket sendPacket;
				sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT_BOSS;
				sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackBossDamagePacket);
				sendPacket.damage = damage;
				g_logic.OnlySendPlayerInRoom_R(m_roomId, r, &sendPacket);
				return;
			}
		}
	}
	else {
		std::vector<int> attackedMonster;
		attackedMonster.reserve(10);
		for (int i = 0; i < 15; i++) {
			DirectX::XMFLOAT3 monsterVector = Vector3::Subtract(m_StageSmallMonster[i].GetPos(), pos);
			dir = Vector3::Normalize(dir);
			if (Vector3::DotProduct(dir, Vector3::Normalize(monsterVector)) > PLAYER_ABLE_ATTACK_COS_VALUE) {
				if (Vector3::Length(monsterVector) < 45.0f) {
					m_StageSmallMonster[i].AttackedHp(damage);
					attackedMonster.emplace_back(i);
				}
			}
		}
		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
		sendPacket.role = r;
		sendPacket.attackedMonsterCnt = attackedMonster.size();
		for (int i = 0; i < sendPacket.attackedMonsterCnt; i++)
			sendPacket.monsterIdx[i] = (char)attackedMonster[i];
		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
		sendPacket.damage = damage;
		int userId = -1;
		//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		g_logic.OnlySendPlayerInRoom_R(m_roomId, r, &sendPacket);
	}
}

void Room::GameStart()
{
	m_isAlive = true;
	m_stageStart = false;
	//monster Update
	//TIMER_EVENT smallMonsterEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1) + std::chrono::milliseconds(500), m_roomId ,EV_SM_UPDATE };
	//g_Timer.InsertTimerQueue(smallMonsterEvent);
	TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(500), m_roomId ,EV_GAME_STATE_S_SEND };
	g_Timer.InsertTimerQueue(gameStateEvent);
}

void Room::BossStageStart()//클라에서 받아서 서버로 왔고 -> 클라에서 움직임 막아줘야할듯?
{
	SERVER_PACKET::NotifyPacket sendPacket;
	sendPacket.type = SERVER_PACKET::STAGE_CHANGING_BOSS;
	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);

	for (auto& character : m_characterMap) {
		character.second->StopMove();
		character.second->SetBossStagePosition();
	}

	SERVER_PACKET::GameState_BOSS_INIT sendInitPacket;
	sendInitPacket.type = SERVER_PACKET::STAGE_START_BOSS;
	sendInitPacket.size = sizeof(SERVER_PACKET::GameState_BOSS_INIT);
	int i = 0;
	std::map<ROLE, int> playerMap;
	{
		std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
		playerMap = m_inGamePlayers;
	}
	sendInitPacket.userState[0].role = ROLE::NONE_SELECT;
	sendInitPacket.userState[1].role = ROLE::NONE_SELECT;
	sendInitPacket.userState[2].role = ROLE::NONE_SELECT;
	sendInitPacket.userState[3].role = ROLE::NONE_SELECT;
	for (auto& p : playerMap) {
		sendInitPacket.userState[i].role = p.first;
		sendInitPacket.userState[i].hp = m_characterMap[p.first]->GetHp();
		sendInitPacket.userState[i].pos = m_characterMap[p.first]->GetPos();
		sendInitPacket.userState[i].rot = m_characterMap[p.first]->GetRot();
		++i;
	}
	g_logic.BroadCastInRoom(m_roomId, &sendInitPacket);

	//boss Stage Start Packet send
	//position 데이터 보내기

	TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), m_roomId ,EV_GAME_STATE_B_SEND };//GameState 30ms마다 전송하게 수정
	g_Timer.InsertTimerQueue(new_ev);
	TIMER_EVENT findEv{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), m_roomId ,EV_FIND_PLAYER };
	g_Timer.InsertTimerQueue(findEv);
	TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId, EV_BOSS_STATE };
	//TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(11), m_roomId ,EV_BOSS_STATE };
	g_Timer.InsertTimerQueue(bossStateEvent);
}

void Room::GameRunningLogic()
{
	for (auto& playCharacter : m_characterMap) {//플레이어 무브		
		playCharacter.second->AutoMove();
	}
	if (m_roomState == ROOM_STATE::ROOM_STAGE1) {
		for (int i = 0; i < 15; i++) {
			m_StageSmallMonster[i].AutoMove();
			//if (m_StageSmallMonster[i].GetHp() > 0.0f)
		}
	}
	else if (m_roomState == ROOM_STATE::ROOM_BOSS) {
		if (m_boss.isPhaseChange) {//페이즈 바뀔때 작은 몬스터 나오기
			for (int i = 0; i < 15; i++) {
				if (m_BossSmallMonster->GetHp() > 0.0f)
					m_BossSmallMonster->AutoMove();
			}
		}
		if (m_boss.isMove)
			m_boss.AutoMove();//보스 무브
	}

	//여기에 화살이나 ball 오브젝트 이동 구현
	for (auto& arrow : m_arrows)
	{
		arrow.AutoMove();
	}
	for (auto& ball : m_balls)
	{
		ball.AutoMove();
	}
	for (auto& arrow : m_skillarrow)
	{
		arrow.AutoMove();
	}
}

void Room::GameEnd()
{
	m_isAlive = false;
}

void Room::BossFindPlayer()
{
	if (!m_isAlive) return;
	if (m_boss.isBossDie) return;
	std::map<ROLE, int> playerMap;
	{
		std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
		playerMap = m_inGamePlayers;
	}
#ifdef ALONE_TEST
	m_boss.ReserveAggroPlayerRole(playerMap.begin()->first);
	m_boss.SetAggroPlayerRole();
#endif // ALONE_TEST
#ifndef ALONE_TEST
	ROLE randR = (ROLE)aggroRandomPlayer(dre);
	randR = (ROLE)std::pow(2, (int)randR);
	m_lockInGamePlayers.lock();
	if (m_inGamePlayers.count(randR)) {
		m_lockInGamePlayers.unlock();
		m_boss.ReserveAggroPlayerRole(randR);
	}
	else {
		//다 나갔을때 방 사망
		if (m_inGamePlayers.size() == 0) {
			m_lockInGamePlayers.unlock();
			return;
		}
		m_boss.ReserveAggroPlayerRole(m_inGamePlayers.begin()->first);
		m_lockInGamePlayers.unlock();
	}
	TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::seconds(5) + std::chrono::milliseconds(500), m_roomId ,EV_FIND_PLAYER };
	g_Timer.InsertTimerQueue(new_ev);
#endif // ALONE_TEST
}

void Room::ChangeBossState()
{
	if (!m_isAlive) return;
	else if (m_boss.isBossDie) {}
	else if (m_boss.GetAggroPlayerRole() == NONE_SELECT && m_boss.GetNewAggroPlayerRole() == NONE_SELECT) {
		TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(100), m_roomId ,EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(bossStateEvent);
		return;
	}
	//auto aggroCharacter = m_characterMap[m_boss.GetAggroPlayerRole()];			
	if (!m_boss.StartAttack()) {
		m_boss.isAttack = false;
		m_boss.SetAggroPlayerRole();
		m_boss.SetDestinationPos();
		if (!m_boss.isMove)
			m_boss.StartMove();
		TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(100), m_roomId ,EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(bossStateEvent);
	}
	else {
		SERVER_PACKET::BossAttackPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::BossAttackPacket);
		int randAttackNum = bossRandAttack(dre);
		sendPacket.type = SERVER_PACKET::BOSS_ATTACK;
		sendPacket.bossAttackType = (BOSS_ATTACK)randAttackNum;
		m_boss.currentAttack = (BOSS_ATTACK)randAttackNum;
		m_boss.AttackTimer();
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		m_boss.isMove = false;
		m_boss.isAttack = true;
	}
}

void Room::UpdateGameStateForPlayer_STAGE1()
{
	if (!m_isAlive) return;
	if (m_roomState == ROOM_BOSS) return;
	else {
		//std::cout << "bossHp : " << refRoom.GetBoss().GetHp() << std::endl;
		SERVER_PACKET::GameState_STAGE1 sendPacket;
		sendPacket.type = SERVER_PACKET::GAME_STATE_S;
		sendPacket.size = sizeof(SERVER_PACKET::GameState_STAGE1);
		int i = 0;
		std::map<ROLE, int> playerMap;
		{
			std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
			playerMap = m_inGamePlayers;
		}
		sendPacket.userState[0].role = ROLE::NONE_SELECT;
		sendPacket.userState[1].role = ROLE::NONE_SELECT;
		sendPacket.userState[2].role = ROLE::NONE_SELECT;
		sendPacket.userState[3].role = ROLE::NONE_SELECT;
		for (auto& p : playerMap) {
			sendPacket.userState[i].role = p.first;
			sendPacket.userState[i].hp = m_characterMap[p.first]->GetHp();
			sendPacket.userState[i].pos = m_characterMap[p.first]->GetPos();
			sendPacket.userState[i].rot = m_characterMap[p.first]->GetRot();
			sendPacket.userState[i].shield = m_characterMap[p.first]->GetShield();
			sendPacket.userState[i].moveVec = m_characterMap[p.first]->GetDirectionVector();
			++i;
		}
		sendPacket.userTime = std::chrono::utc_clock::now();

		//small monster state도 추가
		int aliveCnt = 0;
		for (int i = 0; i < 15; i++) {
			m_StageSmallMonster[i].UpdateMonsterState();
			sendPacket.smallMonster[i].isAlive = false;
			if (m_StageSmallMonster[i].IsAlive()) {
				sendPacket.smallMonster[i].isAlive = true;
				aliveCnt++;
			}
			sendPacket.smallMonster[i].hp = m_StageSmallMonster[i].GetHp();
			sendPacket.smallMonster[i].pos = m_StageSmallMonster[i].GetPos();
			sendPacket.smallMonster[i].rot = m_StageSmallMonster[i].GetRot();
			sendPacket.smallMonster[i].moveVec = m_StageSmallMonster[i].GetDirectionVector();
		}
		sendPacket.monsterTime = std::chrono::utc_clock::now();
		m_aliveSmallMonster = aliveCnt;
		sendPacket.aliveMonsterCnt = aliveCnt;
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId ,EV_GAME_STATE_S_SEND };//GameState 30ms마다 전송하게 수정
		g_Timer.InsertTimerQueue(new_ev);
	}
}

void Room::UpdateSmallMonster()
{
	if (m_roomState == ROOM_STATE::ROOM_BOSS) return;
	SERVER_PACKET::SmallMonsterMovePacket sendPacket;
	XMFLOAT3 pos[4];
	int chracterCnt = 0;
	for (auto& character : m_characterMap) {
		pos[chracterCnt] = character.second->GetPos();
		chracterCnt++;
	}
	for (int i = 0; i < 15; i++)
	{
		if (!m_StageSmallMonster[i].StartAttack()) {
			m_StageSmallMonster[i].SetDestinationPos(pos);
			sendPacket.desPositions[i] = m_StageSmallMonster[i].GetDesPos();
		}
	}
	sendPacket.size = sizeof(SERVER_PACKET::SmallMonsterMovePacket);
	sendPacket.type = SERVER_PACKET::SMALL_MONSTER_MOVE;
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);
	TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId ,EV_SM_UPDATE };//GameState 30ms마다 전송하게 수정
	g_Timer.InsertTimerQueue(new_ev);
}

void Room::UpdateGameStateForPlayer_BOSS()
{
	if (!m_isAlive) return;
	short damage = -1;
	if (m_boss.GetHp() <= 0)m_boss.isBossDie = true;
	if (m_boss.isBossDie) {
		m_boss.SetZeroHp();
		SERVER_PACKET::NotifyPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		sendPacket.type = SERVER_PACKET::GAME_END;
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		return;
	}
	else {
		//std::cout << "bossHp : " << refRoom.GetBoss().GetHp() << std::endl;
		SERVER_PACKET::GameState_BOSS sendPacket;
		sendPacket.type = SERVER_PACKET::GAME_STATE_B;
		sendPacket.size = sizeof(SERVER_PACKET::GameState_BOSS);
		sendPacket.bossState.hp = m_boss.GetHp();
		sendPacket.bossState.pos = m_boss.GetPos();
		sendPacket.bossState.rot = m_boss.GetRot();
		sendPacket.bossState.moveVec = m_boss.GetDirectionVector();
		int i = 0;
		std::map<ROLE, int> playerMap;
		{
			std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
			playerMap = m_inGamePlayers;
		}
		sendPacket.userState[0].role = ROLE::NONE_SELECT;
		sendPacket.userState[1].role = ROLE::NONE_SELECT;
		sendPacket.userState[2].role = ROLE::NONE_SELECT;
		sendPacket.userState[3].role = ROLE::NONE_SELECT;
		for (auto& p : playerMap) {
			sendPacket.userState[i].role = p.first;
			sendPacket.userState[i].hp = m_characterMap[p.first]->GetHp();
			sendPacket.userState[i].pos = m_characterMap[p.first]->GetPos();
			sendPacket.userState[i].rot = m_characterMap[p.first]->GetRot();
			sendPacket.userState[i].shield = m_characterMap[p.first]->GetShield();
			++i;
		}
		sendPacket.time = std::chrono::utc_clock::now();
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId ,EV_GAME_STATE_B_SEND };//GameState 30ms마다 전송하게 수정
		g_Timer.InsertTimerQueue(new_ev);
	}
}

void Room::BossAttackExecute()
{
	if (m_boss.GetHp() <= 0) return;
	SERVER_PACKET::BossAttackPlayerPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::BossAttackPlayerPacket);
	sendPacket.type = SERVER_PACKET::BOSS_ATTACK_PALYER;
	switch (m_boss.currentAttack)
	{
	case ATTACK_KICK:
	{
		for (auto& playCharater : m_characterMap) {
			auto bossToPlayerVector = Vector3::Subtract(playCharater.second->GetPos(), m_boss.GetPos());
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			bossToPlayerVector = Vector3::Normalize(bossToPlayerVector);
			float dotProductRes = Vector3::DotProduct(bossToPlayerVector, m_boss.GetDirectionVector());
			if (bossToPlayerDis < 60.0f && abs(dotProductRes) >= MONSTER_ABLE_ATTACK_COS_VALUE) {// 15 + 15 도 총 30도 내에 있다면
				playCharater.second->AttackedHp(40);
				sendPacket.currentHp = playCharater.second->GetHp();
				g_logic.OnlySendPlayerInRoom_R(m_roomId, playCharater.first, &sendPacket);
			}
		}
	}
	break;
	case ATTACK_PUNCH:
	{
		for (auto& playCharater : m_characterMap) {
			auto bossToPlayerVector = Vector3::Subtract(playCharater.second->GetPos(), m_boss.GetPos());
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			bossToPlayerVector = Vector3::Normalize(bossToPlayerVector);
			float dotProductRes = Vector3::DotProduct(bossToPlayerVector, m_boss.GetDirectionVector());
			if (bossToPlayerDis < 50.0f && abs(dotProductRes) >= MONSTER_ABLE_ATTACK_COS_VALUE) { // 15 + 15 도 총 30도 내에 있다면
				playCharater.second->AttackedHp(20);
				sendPacket.currentHp = playCharater.second->GetHp();
				g_logic.OnlySendPlayerInRoom_R(m_roomId, playCharater.first, &sendPacket);
			}
		}
	}
	break;
	case ATTACK_SPIN:
	{
		for (auto& playCharater : m_characterMap) {
			auto bossToPlayerVector = Vector3::Subtract(playCharater.second->GetPos(), m_boss.GetPos());
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 45.0f) {
				playCharater.second->AttackedHp(15);
				sendPacket.currentHp = playCharater.second->GetHp();
				g_logic.OnlySendPlayerInRoom_R(m_roomId, playCharater.first, &sendPacket);
			}
		}
	}
	break;
	default:
		break;
	}
	//m_boss.AttackPlayer();
}

void Room::HealPlayerCharacter()
{
	if (!m_characterMap[ROLE::PRIEST]->IsDurationEndTimeSkill_1()) {
		XMFLOAT3 xmf3MagePos = m_characterMap[ROLE::PRIEST]->GetPos();
		for (auto& p : m_characterMap) {
			XMFLOAT3 xmf3CharacterPos = p.second->GetPos();
			if (Vector3::Length(Vector3::Subtract(xmf3CharacterPos, xmf3MagePos)) <= 75.0f) {	// HealRange Radius == 75.0f
				p.second->HealHp(10.0f);
			}
		}
		TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1), m_roomId ,EV_HEAL };
		g_Timer.InsertTimerQueue(gameStateEvent);
		SERVER_PACKET::NotifyHealPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::NotifyHealPacket);
		sendPacket.type = SERVER_PACKET::NOTIFY_HEAL_HP;
		int i = 0;
		for (auto& playCharcter : m_characterMap) {
			sendPacket.applyHealPlayerInfo[i].hp = playCharcter.second->GetHp();
			sendPacket.applyHealPlayerInfo[i].role = playCharcter.first;
			++i;
		}
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
	}
	else {
		SERVER_PACKET::NotifyPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		sendPacket.type = SERVER_PACKET::HEAL_END;
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
	}
}

void Room::StartHealPlayerCharacter()
{
	SERVER_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	sendPacket.type = SERVER_PACKET::HEAL_START;
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);
	HealPlayerCharacter();
}

void Room::UpdateShieldData()
{
	if (m_characterMap[ROLE::TANKER]->IsDurationEndTimeSkill_1()) RemoveBarrier();
}

void Room::PutBarrierOnPlayer()
{
	for (auto& p : m_characterMap) {
		p.second->SetShield(true);
	}
	SERVER_PACKET::NotifyShieldPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::NotifyShieldPacket);
	sendPacket.type = SERVER_PACKET::NOTIFY_SHIELD_APPLY;
	int i = 0;
	for (auto& playCharcter : m_characterMap) {
		sendPacket.applyShieldPlayerInfo[i].shield = playCharcter.second->GetShield();
		sendPacket.applyShieldPlayerInfo[i].role = playCharcter.first;
		++i;
	}
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);
}

void Room::RemoveBarrier()
{
	for (auto& p : m_characterMap) {
		p.second->SetShield(false);
	}
	SERVER_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	sendPacket.type = SERVER_PACKET::SHIELD_END;
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);
}

void Room::Recv_SkipNPC_Communication()
{
	m_skipNPC_COMMUNICATION += 1;
	SkipNPC_Communication();
}

std::map<ROLE, ChracterSessionObject*>& Room::GetPlayCharacters()
{
	return m_characterMap;
}

void Room::ChangeDirectionPlayCharacter(ROLE r, DIRECTION d)
{
	m_characterMap[r]->ChangeDirection(d);
}

void Room::StopMovePlayCharacter(ROLE r)
{
	m_characterMap[r]->StopMove();
}

DirectX::XMFLOAT3 Room::GetPositionPlayCharacter(ROLE r)
{
	return m_characterMap[r]->GetPos();
}

bool Room::AdjustPlayCharacterInfo(ROLE r, DirectX::XMFLOAT3& postion)
{
	return m_characterMap[r]->AdjustPlayerInfo(postion);;
}

void Room::RotatePlayCharacter(ROLE r, ROTATE_AXIS axis, float& angle)
{
	m_characterMap[r]->Rotate(axis, angle);
}

void Room::StartMovePlayCharacter(ROLE r, DIRECTION d, std::chrono::utc_clock::time_point& recvTime)
{
	m_characterMap[r]->StartMove(d, recvTime);
}

void Room::SetMouseInputPlayCharacter(ROLE r, bool left, bool right)
{
	m_characterMap[r]->SetMouseInput(left, right);
}

bool Room::GetLeftAttackPlayCharacter(ROLE r)
{
	return m_characterMap[r]->GetLeftAttack();
}

short Room::GetAttackDamagePlayCharacter(ROLE r)
{
	return m_characterMap[r]->GetAttackDamage();
}

void Room::StartFirstSkillPlayCharacter(ROLE r, XMFLOAT3& dirOrPosition)
{
	m_characterMap[r]->Skill_1(dirOrPosition);
}

void Room::StartSecondSkillPlayCharacter(ROLE r, XMFLOAT3& dirOrPosition)
{
	m_characterMap[r]->Skill_2(dirOrPosition);
}

void Room::StartAttackPlayCharacter(ROLE r, XMFLOAT3& attackDir, int power)
{
	m_characterMap[r]->ExecuteCommonAttack(attackDir, power);
}

void Room::SkipNPC_Communication()
{
	if (m_stageStart) return;
	m_lockInGamePlayers.lock();
	if (m_skipNPC_COMMUNICATION >= m_inGamePlayers.size())
		m_stageStart = true;
	m_lockInGamePlayers.unlock();
	if (m_stageStart) {
		TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId ,EV_SM_UPDATE };//GameState 30ms마다 전송하게 수정
		g_Timer.InsertTimerQueue(new_ev);
		std::cout << "Room::SkipNPC_Communication() - RoomId: " << m_roomId << ", " << "gameStart" << std::endl;
	}
}

void Room::ChangeStageBoss()
{
	if (m_roomState == ROOM_STAGE1) {
		m_roomState = ROOM_BOSS;
		BossStageStart();
	}

}

void Room::StartSkyArrow(XMFLOAT3& position)
{
	m_skyArrowAttack = position;
	TIMER_EVENT skyArrowEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1), m_roomId,  EV_SKY_ARROW_ATTACK };
	g_Timer.InsertTimerQueue(skyArrowEvent);
}

void Room::ExecuteMageThunder(XMFLOAT3& position)
{
	if (m_roomState == ROOM_STAGE1) {
		std::vector<int> positions;
		positions.reserve(6);
		for (int i = 0; i < 15; i++) {
			if (m_StageSmallMonster[i].IsAlive()) {
				if (m_StageSmallMonster[i].GetDistance(position) < 24.0f) {
					m_StageSmallMonster[i].AttackedHp(120.0f);
					positions.emplace_back(i);
				}
			}
		}
		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
		sendPacket.role = ROLE::PRIEST;
		sendPacket.attackedMonsterCnt = positions.size();
		sendPacket.damage = 120.0f;
		for (int i = 0; i < positions.size(); i++) {
			sendPacket.monsterIdx[i] = positions[i];
		}
		g_logic.OnlySendPlayerInRoom_R(m_roomId, ROLE::PRIEST, &sendPacket);
	}
	else {
		m_boss.AttackedHp(120.0f);
		SERVER_PACKET::PlayerAttackBossDamagePacket sendPacket;
		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT_BOSS;
		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackBossDamagePacket);
		sendPacket.damage = 120.0f;
		g_logic.OnlySendPlayerInRoom_R(m_roomId, ROLE::PRIEST, &sendPacket);
	}
}

void Room::ExecuteLongSwordAttack(DirectX::XMFLOAT3& dir, DirectX::XMFLOAT3& pos)
{
	if (m_roomState == ROOM_BOSS) {
		DirectX::XMFLOAT3 bossPos = GetBoss().GetPos();
		DirectX::XMFLOAT3 toBoss = Vector3::Subtract(bossPos, pos);
		dir = Vector3::Normalize(dir);
		if (Vector3::DotProduct(dir, Vector3::Normalize(toBoss)) > MONSTER_ABLE_ATTACK_COS_VALUE) {
			if (Vector3::Length(toBoss) < 80.0f) {
				m_boss.AttackedHp(250.0f);
				SERVER_PACKET::PlayerAttackBossDamagePacket sendPacket;
				sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT_BOSS;
				sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackBossDamagePacket);
				sendPacket.damage = 120.0f;
				g_logic.OnlySendPlayerInRoom_R(m_roomId, ROLE::WARRIOR, &sendPacket);
				return;
			}
		}
	}
	else {
		std::vector<int> attackedMonster;
		attackedMonster.reserve(10);
		for (int i = 0; i < 15; i++) {
			DirectX::XMFLOAT3 monsterVector = Vector3::Subtract(m_StageSmallMonster[i].GetPos(), pos);
			dir = Vector3::Normalize(dir);
			if (Vector3::DotProduct(dir, Vector3::Normalize(monsterVector)) > MONSTER_ABLE_ATTACK_COS_VALUE) {
				if (Vector3::Length(monsterVector) < 80.0f) {
					m_StageSmallMonster[i].AttackedHp(250.0f);
					attackedMonster.emplace_back(i);
				}
			}
		}
		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
		sendPacket.role = ROLE::WARRIOR;
		sendPacket.attackedMonsterCnt = attackedMonster.size();
		for (int i = 0; i < sendPacket.attackedMonsterCnt; i++)
			sendPacket.monsterIdx[i] = (char)attackedMonster[i];
		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
		sendPacket.damage = 160.0f;
		g_logic.OnlySendPlayerInRoom_R(m_roomId, ROLE::WARRIOR, &sendPacket);
	}
}

void Room::ExecuteThreeArrow(DirectX::XMFLOAT3& dir, DirectX::XMFLOAT3& position)
{
	//offset 적용됨
	XMFLOAT3 xmf3Position = position;
	XMFLOAT3 rightVector = Vector3::CrossProduct(DirectX::XMFLOAT3(0, 1, 0), dir);
	for (int i = 0; i < 3; i++) {
		xmf3Position.y = 6.0f + (i % 2) * 4.0f;
		xmf3Position = Vector3::Add(xmf3Position, rightVector, (1 - i) * 4.0f);
		xmf3Position = Vector3::Add(xmf3Position, dir, 1.0f);
		m_skillarrow[i].SetStart(dir, xmf3Position, 250.0f);
	}
}

void Room::ExecuteHammerAttack(DirectX::XMFLOAT3& dir, XMFLOAT3& pos)
{
	if (m_roomState == ROOM_BOSS) {
		DirectX::XMFLOAT3 bossPos = GetBoss().GetPos();
		DirectX::XMFLOAT3 toBoss = Vector3::Subtract(bossPos, pos);
		dir = Vector3::Normalize(dir);
		if (Vector3::DotProduct(dir, Vector3::Normalize(toBoss)) > MONSTER_ABLE_ATTACK_COS_VALUE) {
			if (Vector3::Length(toBoss) < 50.0f) {
				m_boss.AttackedHp(250.0f);
				SERVER_PACKET::PlayerAttackBossDamagePacket sendPacket;
				sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT_BOSS;
				sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackBossDamagePacket);
				sendPacket.damage = 120.0f;
				g_logic.OnlySendPlayerInRoom_R(m_roomId, ROLE::TANKER, &sendPacket);
				return;
			}
		}
	}
	else {
		std::vector<int> attackedMonster;
		attackedMonster.reserve(10);
		for (int i = 0; i < 15; i++) {
			DirectX::XMFLOAT3 monsterVector = Vector3::Subtract(m_StageSmallMonster[i].GetPos(), pos);
			dir = Vector3::Normalize(dir);
			if (Vector3::DotProduct(dir, Vector3::Normalize(monsterVector)) > MONSTER_ABLE_ATTACK_COS_VALUE) {
				if (Vector3::Length(monsterVector) < 50.0f) {
					m_StageSmallMonster[i].AttackedHp(250.0f);
					attackedMonster.emplace_back(i);
				}
			}
		}
		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
		sendPacket.role = ROLE::TANKER;
		sendPacket.attackedMonsterCnt = attackedMonster.size();
		for (int i = 0; i < sendPacket.attackedMonsterCnt; i++)
			sendPacket.monsterIdx[i] = (char)attackedMonster[i];
		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
		sendPacket.damage = 160.0f;
		g_logic.OnlySendPlayerInRoom_R(m_roomId, ROLE::TANKER, &sendPacket);
	}
}

void Room::ExecuteSkyArrow()
{
	if (m_roomState == ROOM_BOSS) {
		m_boss.AttackedHp(120.0f);
		SERVER_PACKET::PlayerAttackBossDamagePacket sendPacket;
		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT_BOSS;
		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackBossDamagePacket);
		sendPacket.damage = 120.0f;
		g_logic.OnlySendPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
	}
	else {
		std::vector<int> positions;
		positions.reserve(6);
		for (int i = 0; i < 15; i++) {
			if (m_StageSmallMonster[i].IsAlive()) {
				if (m_StageSmallMonster[i].GetDistance(m_skyArrowAttack) < 24.0f) {
					m_StageSmallMonster[i].AttackedHp(120.0f);
					positions.emplace_back(i);
				}
			}
		}
		SERVER_PACKET::PlayerAttackMonsterDamagePacket sendPacket;
		sendPacket.type = SERVER_PACKET::PLAYER_ATTACK_RESULT;
		sendPacket.size = sizeof(SERVER_PACKET::PlayerAttackMonsterDamagePacket);
		sendPacket.role = ROLE::ARCHER;
		sendPacket.attackedMonsterCnt = positions.size();
		sendPacket.damage = 120.0f;
		for (int i = 0; i < positions.size(); i++) {
			sendPacket.monsterIdx[i] = positions[i];
		}
		g_logic.OnlySendPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
	}
}
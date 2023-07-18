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
	}

	while (!g_bossMapData.GetCompleteState());
	auto& bossMonsterData = g_bossMapData.GetMonsterData();
	for (int i = 0; i < 15; i++) {
		m_BossSmallMonster[i].SetInitPosition(bossMonsterData[i].position);
		m_BossSmallMonster[i].Rotate(ROTATE_AXIS::Y, bossMonsterData[i].eulerRotate.y);
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
	auto findPlayerIter = std::find_if(m_inGamePlayers.begin(), m_inGamePlayers.end(), [&playerId](std::pair<ROLE, int> p)
		{ // playerId가 같은 것을 찾아 제거
			return p.second == playerId;
		}
	);
	//{//diconnected Player 저장
	//	std::lock_guard<std::mutex> lg{ m_lockdisconnectedPlayer };
	//	m_disconnectedPlayers.insert(std::make_pair(g_iocpNetwork.m_session[playerId].GetName(), g_iocpNetwork.m_session[playerId].GetRole() ));
	//}
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	m_inGamePlayers.erase(findPlayerIter);//disconnected된 플레이어 처리
}

std::map<ROLE, int> Room::GetPlayerMap()
{
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	return m_inGamePlayers;
}

void Room::InsertDisconnectedPlayer(int id)
{
	auto findPlayer = std::find_if(m_inGamePlayers.begin(), m_inGamePlayers.end(), [&id](std::pair<ROLE, int> p)
		{
			return p.second == id;
		}
	);
	{// playerId find
		std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
		if (findPlayer == m_inGamePlayers.end()) return;
	}
	{//desconneced Player insert
		std::lock_guard<std::mutex> lg{ m_lockdisconnectedPlayer };
		m_disconnectedPlayers.try_emplace(g_iocpNetwork.m_session[id].GetName(), findPlayer->first);
	}
	//ingame Player erase
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	m_inGamePlayers.erase(findPlayer);
}

bool Room::CheckDisconnectedPlayer(std::wstring& name)
{
	{	//desconneced Player find
		std::lock_guard<std::mutex> lg{ m_lockdisconnectedPlayer };
		if (m_disconnectedPlayers.count(name) == 1) {
			return true;
		}
	}
	return false;
}

void Room::DeleteDisconnectedPlayer(int playerId, std::wstring& name)
{
	if (CheckDisconnectedPlayer(name)) {//disconnected Player find

	}
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

void Room::ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed)
{
	if (!m_isAlive)return;
	int arrowIndex = -1;
	if (m_restArrow.try_pop(arrowIndex)) {
		std::cout << arrowIndex << std::endl;
		//발사체 발사했다는 패킷보내기
		//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		m_arrows[arrowIndex].SetStart(dir, srcPos, speed);
	}
	if (m_restArrow.empty())	// 추후 삭제
	{
		m_arrows[m_arrowCount++ % 10].SetStart(dir, srcPos, speed);
	}
}

void Room::ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed)
{
	if (!m_isAlive)return;
	int ballIndex = -1;
	if (m_restBall.try_pop(ballIndex)) {
		//발사체 발사했다는 패킷보내기
		//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		m_balls[ballIndex].SetStart(dir, srcPos, speed);
	}
}

bool Room::MeleeAttack(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 pos)
{
	DirectX::XMFLOAT3 bossPos = GetBoss().GetPos();
	DirectX::XMFLOAT3 toBoss = Vector3::Subtract(bossPos, pos);
	dir = Vector3::Normalize(dir);
	if (Vector3::DotProduct(dir, Vector3::Normalize(toBoss)) > cosf(3.141592f / 12.0f)) {
		if (Vector3::Length(toBoss) < 45.0f) {
			std::cout << "데미지 입히기" << std::endl;
			return true;
		}
	}
	return false;
}

void Room::GameStart()
{
	//m_isAlive = true;
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
	if (m_roomState == ROOM_STATE::ROOM_STAGE1) {
		for (int i = 0; i < 15; i++) {
			if (m_StageSmallMonster->GetHp() > 0.0f)
				m_StageSmallMonster->AutoMove();
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
	for (auto& playCharacter : m_characterMap) {//플레이어 무브		
		playCharacter.second->AutoMove();
	}
	//여기에 화살이나 ball 오브젝트 이동 구현
	for (auto& arrow : m_arrows)
	{
		if (arrow.m_active)
		{
			arrow.AutoMove();
			if (arrow.DetectCollision(&m_boss) != -1) {
				m_restArrow.push(arrow.GetId());
				m_bossDamagedQueue.push(200);
			}

		}
	}
	for (auto& ball : m_balls)
	{
		if (ball.m_active)
		{
			ball.AutoMove();
			if (ball.DetectCollision(&m_boss) != -1) {
				m_restBall.push(ball.GetId());
				m_bossDamagedQueue.push(30);
				SERVER_PACKET::BossHitObject sendPacket;
				sendPacket.size = sizeof(SERVER_PACKET::BossHitObject);
				sendPacket.type = SERVER_PACKET::HIT_BOSS_MAGE;
				sendPacket.pos = ball.GetPos();
				g_logic.BroadCastInRoom(m_roomId, &sendPacket);
			}
		}
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
		if (m_checkNpc) {
			bool npcComStart = false;//처음 트리거 박스
			if (m_stage1TrigerCnt != 0) {
				m_lockInGamePlayers.lock();
				if (m_stage1TrigerCnt >= m_inGamePlayers.size()) {
					npcComStart = true;
				}
				m_lockInGamePlayers.unlock();
				if (npcComStart) {
					SERVER_PACKET::NotifyPacket sendPacket;
					sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
					sendPacket.type = SERVER_PACKET::START_NPC_COMMUNICATE;
					g_logic.BroadCastInRoom(m_roomId, &sendPacket);
					m_checkNpc = false;
				}
			}
		}

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
			++i;
		}
		//small monster state도 추가
		sendPacket.time = std::chrono::utc_clock::now();
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId ,EV_GAME_STATE_S_SEND };//GameState 30ms마다 전송하게 수정
		g_Timer.InsertTimerQueue(new_ev);
	}
}

void Room::UpdateSmallMonster()
{
	if (m_roomState == ROOM_STATE::ROOM_BOSS) return;
	XMFLOAT3 pos[4];
	int i = 0;
	for (auto& character : m_characterMap) {
		pos[i] = character.second->GetPos();
	}
	for (int i = 0; i < 15; i++)
	{
		m_StageSmallMonster[i].SetDestinationPos(pos);
	}

	TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId ,EV_SM_UPDATE };//GameState 30ms마다 전송하게 수정
	g_Timer.InsertTimerQueue(new_ev);
}

void Room::UpdateGameStateForPlayer_BOSS()
{
	if (!m_isAlive) return;
	short damage = -1;
	while (m_bossDamagedQueue.try_pop(damage)) {
		m_boss.AttackedHp(damage);
	}
	if (m_boss.GetHp() <= 0)m_boss.isBossDie = true;
	if (m_boss.isBossDie) {
		//disconnect Player Set Clear
		{
			std::lock_guard<std::mutex>disconnectedPlayerLock{ m_lockdisconnectedPlayer };
			m_disconnectedPlayers.clear();
		}
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
		sendPacket.bossState.directionVector = m_boss.GetDirectionVector();
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
	switch (m_boss.currentAttack)
	{
	case ATTACK_KICK:
	{
		for (auto& playCharater : m_characterMap) {
			auto bossToPlayerVector = Vector3::Subtract(playCharater.second->GetPos(), m_boss.GetPos());
			float dotProductRes = Vector3::DotProduct(bossToPlayerVector, m_boss.GetDirectionVector());
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 50.0f && abs(dotProductRes) < cosf(3.141592f / 12.0f)) {// 15 + 15 도 총 30도 내에 있다면
				playCharater.second->AttackedHp(40);
			}
		}
	}
	break;
	case ATTACK_PUNCH:
	{
		for (auto& playCharater : m_characterMap) {
			auto bossToPlayerVector = Vector3::Subtract(playCharater.second->GetPos(), m_boss.GetPos());
			float dotProductRes = Vector3::DotProduct(bossToPlayerVector, m_boss.GetDirectionVector());
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 45.0f && abs(dotProductRes) < cosf(3.141592f / 12.0f)) { // 15 + 15 도 총 30도 내에 있다면
				playCharater.second->AttackedHp(20);
				//player Hit Foward
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
				//player Hit spin
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
	auto roomPlayermap = GetPlayerMap();
	XMFLOAT3 xmf3MagePos = m_characterMap[ROLE::PRIEST]->GetPos();
	for (auto& p : roomPlayermap)
	{
		XMFLOAT3 xmf3CharacterPos = m_characterMap[p.first]->GetPos();
		if (Vector3::Length(Vector3::Subtract(xmf3CharacterPos, xmf3MagePos)) < 150.0f) {	// HealRange == 150.0f
			m_characterMap[p.first]->HealHp(100.0f);
		}
	}
	
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_characterMap[ROLE::PRIEST]->GetSkillInputTime(0));
	if (m_characterMap[ROLE::PRIEST]->GetSkillDuration(0) > durationTime)
	{
		TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1), m_roomId ,EV_MAGE_HEAL };
		g_Timer.InsertTimerQueue(gameStateEvent);
	}
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

void Room::StartMovePlayCharacter(ROLE r, DIRECTION d)
{
	m_characterMap[r]->StartMove(d);
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

void Room::StartFirstSkillPlayCharacter(ROLE r)
{
	m_characterMap[r]->Skill_1();
}

void Room::StartSecondSkillPlayCharacter(ROLE r)
{
	m_characterMap[r]->Skill_2();
}

void  Room::SetTriggerCntIncrease()
{
	m_stage1TrigerCnt += 1;
}
void  Room::SetTriggerCntDecrease()
{
	m_stage1TrigerCnt -= 1;
}

void Room::SkipNPC_Communication()
{
	m_skipNPC_COMMUNICATION += 1;
	bool start = false;
	m_lockInGamePlayers.lock();
	if (m_skipNPC_COMMUNICATION == m_inGamePlayers.size())
		start = true;
	m_lockInGamePlayers.unlock();
	if (start) {
		TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId ,EV_SM_UPDATE };//GameState 30ms마다 전송하게 수정
		g_Timer.InsertTimerQueue(new_ev);
	}
}

void Room::ChangeStageBoss()
{
	if (m_roomState == ROOM_STAGE1) {
		m_roomState = ROOM_BOSS;
		BossStageStart();
	}

}
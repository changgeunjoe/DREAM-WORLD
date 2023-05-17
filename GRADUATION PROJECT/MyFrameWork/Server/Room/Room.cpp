#include "stdafx.h"
#include "Room.h"
#include "../Timer/Timer.h"
#include "../Logic/Logic.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../Session/SessionObject/PlayerSessionObject.h"
#include "../IOCPNetwork/protocol/protocol.h"

extern Timer g_Timer;
extern Logic g_logic;
extern IOCPNetwork g_iocpNetwork;

Room::Room()
{
	m_isAlive = false;
}

Room::Room(const Room& rhs)
{
	m_inGamePlayers = rhs.m_inGamePlayers;
	m_roomName = rhs.m_roomName;
	m_roomOwnerId = rhs.m_roomOwnerId;
}

Room::~Room()
{

}

Room& Room::operator=(Room& rhs)
{
	m_inGamePlayers = rhs.m_inGamePlayers;
	m_roomName = rhs.m_roomName;
	m_roomOwnerId = rhs.m_roomOwnerId;
	return *this;
}

void Room::InsertInGamePlayer(std::map<ROLE, int>& matchPlayer)
{
	m_roomOwnerId = matchPlayer.begin()->second;
	m_inGamePlayers = matchPlayer;
}

void Room::InsertInGamePlayer(ROLE r, int playerId)
{
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	m_inGamePlayers.insert(std::make_pair(r, playerId));
	//m_Players.insert(std::make_pair(r, playerId));
}

void Room::DeleteInGamePlayer(int playerId)
{
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	m_inGamePlayers.erase(
		std::find_if(m_inGamePlayers.begin(), m_inGamePlayers.end(), [&playerId](std::pair<ROLE, int> p)
			{ // playerId가 같은 것을 찾아 제거
				return p.second == playerId;
			}
		)
	);
}

std::map<ROLE, int> Room::GetPlayerMap()
{
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	return m_inGamePlayers;
}

void Room::CreateBossMonster()
{
	m_boss.SetRoomId(m_roomId);
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
	m_isAlive = true;
	PrintCurrentTime();
	
	//std::cout << "PlayerNum: " << m_inGamePlayers.size() << std::endl;
	//for (auto& playerInfo : m_inGamePlayers) {
	//	std::cout << "PlayerId: " << playerInfo.second << std::endl;
	//}

	CreateBossMonster(); //임시 입니다.
	TIMER_EVENT findEv{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), m_roomId ,EV_FIND_PLAYER };
	g_Timer.InsertTimerQueue(findEv);
	//TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(3), m_roomId, -1,EV_BOSS_STATE };
	TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(11), m_roomId ,EV_BOSS_STATE };
	g_Timer.InsertTimerQueue(bossStateEvent);
	TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1) + std::chrono::milliseconds(500), m_roomId ,EV_GAME_STATE_SEND };
	g_Timer.InsertTimerQueue(gameStateEvent);
}

void Room::GameRunningLogic()
{
	if (m_boss.isMove)
		m_boss.AutoMove();//보스 무브
	for (auto& p : m_inGamePlayers) {//플레이어 무브
		if (g_iocpNetwork.m_session[p.second].m_sessionObject != nullptr) {
			if (g_iocpNetwork.m_session[p.second].m_sessionObject->m_inputDirection != DIRECTION::IDLE) {
				g_iocpNetwork.m_session[p.second].m_sessionObject->AutoMove();
			}
		}
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
	std::map<ROLE, int> playerMap;
	{
		std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
		playerMap = m_inGamePlayers;
	}
#ifdef ALONE_TEST	
	m_boss.ReserveAggroPlayerId(playerMap.begin()->second);
	m_boss.SetAggroPlayerId();
#endif // ALONE_TEST
#ifndef ALONE_TEST
	if (m_boss.isBossDie) {
		ROLE randR = (ROLE)aggroRandomPlayer(dre);
		m_boss.ReserveAggroPlayerId(playerMap[randR]);
	}
	else {
		TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::seconds(5) + std::chrono::milliseconds(500), m_roomId ,EV_FIND_PLAYER };
		g_Timer.InsertTimerQueue(new_ev);
	}
#endif // ALONE_TEST
}

void Room::ChangeBossState()
{
	if (!m_isAlive) return;
	if (m_boss.isBossDie) {}
	else if (!m_boss.StartAttack()) {
		m_boss.isAttack = false;
		m_boss.SetAggroPlayerId();
		if (m_boss.GetAggroPlayerId() != -1) {
			XMFLOAT3 playerPos = g_iocpNetwork.m_session[m_boss.GetAggroPlayerId()].m_sessionObject->GetPos();
			m_boss.SetDestinationPos(playerPos);
			SERVER_PACKET::BossChangeStateMovePacket sendPacket;
			sendPacket.type = SERVER_PACKET::BOSS_CHANGE_STATE_MOVE_DES;
			sendPacket.size = sizeof(SERVER_PACKET::BossChangeStateMovePacket);
			sendPacket.desPos = playerPos;
			sendPacket.bossPos = m_boss.GetPos();
			g_logic.BroadCastInRoom(m_roomId, &sendPacket);
			if (!m_boss.isMove)
				m_boss.StartMove(DIRECTION::FRONT);
		}
		TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(700), m_roomId ,EV_BOSS_STATE };
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

void Room::UpdateGameStateForPlayer()
{
	if (!m_isAlive) return;
	std::map<ROLE, int> playerMap;
	{
		std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
		playerMap = m_inGamePlayers;
	}
	short damage = -1;
	while (m_bossDamagedQueue.try_pop(damage)) {
		m_boss.AttackedHp(damage);
	}
	if (m_boss.GetHp() <= 0)m_boss.isBossDie = true;
	if (m_boss.isBossDie) {
		m_boss.SetZeroHp();
		SERVER_PACKET::NotifyPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		sendPacket.type = SERVER_PACKET::GAME_END;
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
	}
	else {
		//std::cout << "bossHp : " << refRoom.GetBoss().GetHp() << std::endl;
		SERVER_PACKET::GameState sendPacket;
		sendPacket.type = SERVER_PACKET::GAME_STATE;
		sendPacket.size = sizeof(SERVER_PACKET::GameState);
		sendPacket.bossState.hp = m_boss.GetHp();
		sendPacket.bossState.pos = m_boss.GetPos();
		sendPacket.bossState.rot = m_boss.GetRot();
		sendPacket.bossState.directionVector = m_boss.GetDirectionVector();
		int i = 0;
		for (auto& p : playerMap) {
			sendPacket.userState[i].userId = p.second;
			sendPacket.userState[i].hp = g_iocpNetwork.m_session[p.second].m_sessionObject->GetHp();
			sendPacket.userState[i].pos = g_iocpNetwork.m_session[p.second].m_sessionObject->GetPos();
			sendPacket.userState[i].rot = g_iocpNetwork.m_session[p.second].m_sessionObject->GetRot();
			++i;
		}
		sendPacket.time = std::chrono::utc_clock::now();
		g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(30), m_roomId ,EV_GAME_STATE_SEND };//GameState 30ms마다 전송하게 수정
		g_Timer.InsertTimerQueue(new_ev);
	}
}

void Room::BossAttackExecute()
{
	m_boss.AttackPlayer();
}

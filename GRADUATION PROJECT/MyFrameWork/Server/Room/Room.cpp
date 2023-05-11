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
			{ // playerId�� ���� ���� ã�� ����
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
	//m_boss.SetRoomId(m_roomId);
	//TIMER_EVENT firstEv{ std::chrono::system_clock::now() + std::chrono::milliseconds(200), m_roomId, -1,EV_FIND_PLAYER };
	//g_Timer.InsertTimerQueue(firstEv);
}

MonsterSessionObject& Room::GetBoss()
{
	return m_boss;
}

void Room::ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed)
{
	int arrowIndex = -1;
	if (m_restArrow.try_pop(arrowIndex)) {
		std::cout << arrowIndex << std::endl;
		//�߻�ü �߻��ߴٴ� ��Ŷ������
		//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		m_arrows[arrowIndex].SetStart(dir, srcPos, speed);
	}
	if (m_restArrow.empty())	// ���� ����
	{
		m_arrows[m_arrowCount++ % 10].SetStart(dir, srcPos, speed);
	}
}

void Room::ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed)
{
	int ballIndex = -1;
	if (m_restBall.try_pop(ballIndex)) {
		//�߻�ü �߻��ߴٴ� ��Ŷ������
		//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		m_balls[ballIndex].SetStart(dir, srcPos, speed);
	}
}

bool Room::MeleeAttack(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 pos)
{
	DirectX::XMFLOAT3 bossPos = GetBoss().GetPos();
	DirectX::XMFLOAT3 toBoss = Vector3::Subtract(bossPos, pos);
	toBoss = Vector3::Normalize(toBoss);
	dir = Vector3::Normalize(dir);
	if (Vector3::DotProduct(dir, toBoss) > cosf(3.141592f / 12.0f)) {
		if (Vector3::Length(toBoss) < 45.0f) {
			std::cout << "������ ������" << std::endl;
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

	CreateBossMonster(); //�ӽ� �Դϴ�.
	//TIMER_EVENT findEv{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), m_roomId, -1,EV_FIND_PLAYER };
	//g_Timer.InsertTimerQueue(findEv);
	////TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(3), m_roomId, -1,EV_BOSS_STATE };
	//TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(11), m_roomId, -1,EV_BOSS_STATE };
	//g_Timer.InsertTimerQueue(bossStateEvent);
	//TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1) + std::chrono::milliseconds(500), m_roomId, -1,EV_GAME_STATE_SEND };
	//g_Timer.InsertTimerQueue(gameStateEvent);
}

void Room::GameRunningLogic()
{
	if (m_boss.isMove)
		m_boss.AutoMove();//���� ����
	for (auto& p : m_inGamePlayers) {//�÷��̾� ����
		if (g_iocpNetwork.m_session[p.second].m_sessionObject != nullptr) {
			if (g_iocpNetwork.m_session[p.second].m_sessionObject->m_inputDirection != DIRECTION::IDLE) {
				g_iocpNetwork.m_session[p.second].m_sessionObject->AutoMove();
			}
		}
	}
	//���⿡ ȭ���̳� ball ������Ʈ �̵� ����
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
				//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
			}
		}
	}
}

void Room::GameEnd()
{
	m_isAlive = false;
}

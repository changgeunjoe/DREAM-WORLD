#include "stdafx.h"
#include "Room.h"
#include "../Timer/Timer.h"
#include "../Logic/Logic.h"
#include "../Session/SessionObject/ShootingSessionObject.h"

extern Timer g_Timer;
extern Logic g_logic;
Room::Room()
{

}

Room::Room(std::string& roomId, std::wstring& roomName, int onwerId, ROLE r) : m_roomId(roomId), m_roomName(roomName), m_roomOwnerId(onwerId)
{
	//m_inGamePlayers.insert(r, m_roomOwnerId);//ROLE이 없음
	//m_inGamePlayers.insert(std::make_pair(r, m_roomOwnerId));//ROLE이 없음
	m_inGamePlayers.insert(std::make_pair(r, m_roomOwnerId));
	//m_inGamePlayers.try_emplace(r, m_roomOwnerId);//ROLE이 없음
		//test용으로 존재함
	CreateBossMonster(); //임시 입니다.
}

Room::Room(std::string& roomId, int player1, int player2, int player3, int player4)
{

}
Room::Room(std::string roomId) : m_roomId(roomId)
{
}

Room::Room(std::string roomId, std::wstring roomName) : m_roomId(roomId), m_roomName(roomName)
{
	CreateBossMonster(); //임시 입니다.	
	int i = 0;
	for (auto& arrow : m_arrows) {
		m_restArrow.push(i);
		arrow.RegistArrow(roomId, i++);
	}
	i = 0;
	for (auto& arrow : m_balls) {
		m_restBall.push(i);
		arrow.RegistEnergtBall(roomId, i++);
	}
}

Room::Room(const Room& rhs)
{
	//memcpy(this, &rhs, sizeof(Room));
	m_inGamePlayers = rhs.m_inGamePlayers;
	m_roomId = rhs.m_roomId;
	m_roomName = rhs.m_roomName;
	m_roomOwnerId = rhs.m_roomOwnerId;
}

Room::~Room()
{

}

Room& Room::operator=(Room& rhs)
{
	m_inGamePlayers = rhs.m_inGamePlayers;
	m_roomId = rhs.m_roomId;
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

void Room::InsertWaitPlayer(ROLE r, int playerId)
{
	std::lock_guard<std::mutex> lg{ m_lockWaitPlayers };
	m_waitPlayers.insert(std::make_pair(r, playerId));
	//m_WaitPlayers.insert(std::make_pair(r, playerId));
}

void Room::DeleteInGamePlayer(int playerId)
{
	std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
	m_inGamePlayers.erase(
		std::find_if(m_inGamePlayers.begin(), m_inGamePlayers.end(), [&playerId](std::pair<ROLE, int> p) { // playerId가 같은 것을 찾아 제거
			return p.second == playerId;
			})
	);
}

void Room::DeleteWaitPlayer(int playerId)
{
	std::lock_guard<std::mutex> lg{ m_lockWaitPlayers };
	m_waitPlayers.erase(
		std::find_if(m_waitPlayers.begin(), m_waitPlayers.end(), [&playerId](std::pair<ROLE, int> p) { // playerId가 같은 것을 찾아 제거
			return p.second == playerId;
			})
	);
}

void Room::CreateBossMonster()
{
	m_boss.RegistMonster(MAX_USER + m_roomOwnerId, m_roomId);
	TIMER_EVENT firstEv{ std::chrono::system_clock::now() + std::chrono::seconds(3), m_roomId, -1,EV_FIND_PLAYER };
	g_Timer.m_TimerQueue.push(firstEv);
}

void Room::ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed)
{
	int arrowIndex = -1;
	if (m_restArrow.try_pop(arrowIndex)) {
		{
			std::lock_guard<std::mutex> lg(m_arrowLock);
			m_shootingArrow.push_back(arrowIndex);
		}
		dynamic_cast<ShootingSessionObject*>(m_arrows[arrowIndex].m_sessionObject)->SetStart(dir, srcPos, speed);
		//발사체 발사했다는 패킷보내기
		//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
	}
}

void Room::ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed)
{
	int ballIndex = -1;
	if (m_restBall.try_pop(ballIndex)) {
		{
			std::lock_guard<std::mutex> lg(m_ballLock);
			m_shootingBall.push_back(ballIndex);
		}
		//발사체 발사했다는 패킷보내기
		//g_logic.BroadCastInRoom(m_roomId, &sendPacket);
		dynamic_cast<ShootingSessionObject*>(m_balls[ballIndex].m_sessionObject)->SetStart(dir, srcPos, speed);
	}
}

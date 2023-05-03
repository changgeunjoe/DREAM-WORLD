#include "stdafx.h"
#include "Room.h"
#include "../Timer/Timer.h"
#include "../Logic/Logic.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"

extern Timer g_Timer;
extern Logic g_logic;
extern IOCPNetwork g_iocpNetwork;

Room::Room()
{

}

Room::Room(std::string& roomId, std::wstring& roomName, int onwerId, ROLE r) : m_roomId(roomId), m_roomName(roomName), m_roomOwnerId(onwerId)
{
	//m_inGamePlayers.insert(r, m_roomOwnerId);//ROLE이 없음
	//m_inGamePlayers.insert(std::make_pair(r, m_roomOwnerId));//ROLE이 없음
	m_inGamePlayers.insert(std::make_pair(r, m_roomOwnerId));
	//m_inGamePlayers.try_emplace(r, m_roomOwnerId);//ROLE이 없음
}

Room::Room(std::string& roomId, int player1, int player2, int player3, int player4)
{

}
Room::Room(std::string roomId) : m_roomId(roomId)
{
}

Room::Room(std::string roomId, std::wstring roomName) : m_roomId(roomId), m_roomName(roomName)
{
	m_roomId = roomId;
	int i = 0;
	for (auto& arrow : m_arrows) {
		m_restArrow.push(i);
		arrow.SetInfo(roomId, i++);
	}
	i = 0;
	for (auto& arrow : m_balls) {
		m_restBall.push(i);
		arrow.SetInfo(roomId, i++);
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
	m_boss.SetRoomId(m_roomId);
	TIMER_EVENT firstEv{ std::chrono::system_clock::now() + std::chrono::milliseconds(200), m_roomId, -1,EV_FIND_PLAYER };
	g_Timer.InsertTimerQueue(firstEv);
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
	if (abs(Vector3::DotProduct(dir, toBoss)) < 10.0f) {
		if (Vector3::Length(toBoss) < 5.0f) {
			std::cout << "데미지 입히기" << std::endl;
			return true;
		}
	}
	return false;
}

void Room::GameStart()
{
	CreateBossMonster(); //임시 입니다.
	TIMER_EVENT findEv{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), m_roomId, -1,EV_FIND_PLAYER };
	g_Timer.InsertTimerQueue(findEv);
	TIMER_EVENT bossStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(3), m_roomId, -1,EV_BOSS_STATE };
	g_Timer.InsertTimerQueue(bossStateEvent);
	TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1) + std::chrono::milliseconds(500), m_roomId, -1,EV_GAME_STATE_SEND };
	g_Timer.InsertTimerQueue(gameStateEvent);
	//ExpOver* ov = new ExpOver();
	//ov->m_opCode = OP_BOSS_STATE;
	//memcpy(ov->m_buffer, m_roomId.c_str(), m_roomId.size());//exOver의 cchar*버퍼에 roomId를 담는다면?
	//ov->m_buffer[m_roomId.size()] = 0;
	//PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, -1, &ov->m_overlap);
	m_boss.StartMove(DIRECTION::FRONT);
}

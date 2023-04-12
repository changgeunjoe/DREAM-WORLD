#include "stdafx.h"
#include "Room.h"

Room::Room()
{

}

Room::Room(std::string& roomId, std::wstring& roomName, int onwerId, ROLE r) : m_roomId(roomId), m_roomName(roomName), m_roomOwnerId(onwerId)
{
	//m_inGamePlayers.insert(r, m_roomOwnerId);//ROLE�� ����
	//m_inGamePlayers.insert(std::make_pair(r, m_roomOwnerId));//ROLE�� ����
	m_inGamePlayers.insert(std::make_pair(r, m_roomOwnerId));
	//m_inGamePlayers.try_emplace(r, m_roomOwnerId);//ROLE�� ����
		//test������ ������
	m_boss.RegistMonster(MAX_USER + m_roomOwnerId);

}

Room::Room(std::string& roomId, int player1, int player2, int player3, int player4)
{

}
Room::Room(std::string roomId) : m_roomId(roomId)
{
}

Room::Room(std::string roomId, std::wstring roomName) : m_roomId(roomId), m_roomName(roomName)
{

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
		std::find_if(m_inGamePlayers.begin(), m_inGamePlayers.end(), [&playerId](std::pair<ROLE, int> p) { // playerId�� ���� ���� ã�� ����
			return p.second == playerId;
			})
	);
}

void Room::DeleteWaitPlayer(int playerId)
{
	std::lock_guard<std::mutex> lg{ m_lockWaitPlayers };
	m_waitPlayers.erase(
		std::find_if(m_waitPlayers.begin(), m_waitPlayers.end(), [&playerId](std::pair<ROLE, int> p) { // playerId�� ���� ���� ã�� ����
			return p.second == playerId;
			})
	);
}

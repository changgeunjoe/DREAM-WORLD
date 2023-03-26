#include "stdafx.h"
#include "Room.h"

Room::Room()
{
}

Room::Room(std::string& roomId, std::wstring& roomName, int onwerId) : m_roomId(roomId), m_roomName(roomName), m_roomOwnerId(onwerId)
{
	//m_Players.insert(m_roomOwnerId);
}

Room::Room(std::string roomId) : m_roomId(roomId)
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

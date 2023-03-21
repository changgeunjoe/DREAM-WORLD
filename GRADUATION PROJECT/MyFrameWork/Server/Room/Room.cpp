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
	m_Players = rhs.m_Players;
	m_roomId = rhs.m_roomId;
	m_roomName = rhs.m_roomName;
	m_roomOwnerId = rhs.m_roomOwnerId;

}

Room::~Room()
{
}

Room& Room::operator=(Room& rhs)
{
	m_Players = rhs.m_Players;
	m_roomId = rhs.m_roomId;
	m_roomName = rhs.m_roomName;
	m_roomOwnerId = rhs.m_roomOwnerId;
	return *this;
}

void Room::InsertPlayer(ROLE r, int playerId)
{
	std::lock_guard<std::mutex> lg{ m_LockPlayerSet };
	m_Players.insert(std::make_pair(r, playerId));
}

void Room::InsertWaitPlayer(ROLE r, int playerId)
{
	std::lock_guard<std::mutex> lg{ m_LockWaitPlayerSet };
	m_WaitPlayers.insert(std::make_pair(r, playerId));
}

void Room::DeletePlayer(int playerId)
{
	std::lock_guard<std::mutex> lg{ m_LockWaitPlayerSet };
	m_WaitPlayers.erase(
		std::find_if(m_WaitPlayers.begin(), m_WaitPlayers.end(), [&playerId](std::pair<ROLE, int> p) { // playerId가 같은 것을 찾아 제거
			return p.second == playerId;
			})
	);
}

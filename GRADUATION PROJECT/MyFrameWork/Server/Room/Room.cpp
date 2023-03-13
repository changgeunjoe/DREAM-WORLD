#include "stdafx.h"
#include "Room.h"

Room::Room()
{
}

Room::Room(std::string& roomId, std::wstring& roomName, int onwerId) : m_roomId(roomId), m_roomName(roomName), m_roomOwnerId(onwerId)
{
	m_Players.insert(m_roomOwnerId);
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
	// TODO: 여기에 return 문을 삽입합니다.
}

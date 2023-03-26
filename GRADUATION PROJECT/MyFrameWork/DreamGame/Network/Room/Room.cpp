#include "stdafx.h"
#include "Room.h"

const wstring Room::GetRoomName()
{
	return m_roomName;
}

const string Room::GetRoomId()
{
	return m_roomId;
}

Room::Room(wstring roomName, string roomId, char* role, string* playerName)
{
	m_roomName = roomName;
	m_roomId = roomId;
	for (int i = 0; i < 4; i++) {
		//m_inGamePlayers.insert( (ROLE)role[i], (std::string)playerName[i]    );
		m_inGamePlayers.emplace( (ROLE)role[i], (std::string)playerName[i]    );
	}
}

const map<ROLE, string> Room::GetIngamePlayersInfo()
{
	//lock?
	map<ROLE, string> returnValue = m_inGamePlayers;
	return returnValue;
}

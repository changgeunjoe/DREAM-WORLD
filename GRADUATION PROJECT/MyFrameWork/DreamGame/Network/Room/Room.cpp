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

Room::Room(wstring roomName, string roomId, char* role, wstring* playerName)
{
	m_roomName = roomName;
	m_roomId = roomId;
	for (int i = 0; i < 4; i++) {
		m_inGamePlayers.emplace((ROLE)role[i], (std::wstring)playerName[i]);
	}
}

const map<ROLE, wstring> Room::GetIngamePlayersInfo()
{
	//lock?
	map<ROLE, wstring> returnValue = m_inGamePlayers;
	return returnValue;
}

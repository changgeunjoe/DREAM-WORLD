#include "stdafx.h"
#include "RoomManger.h"

void RoomManger::InsertRoom(string& roomId, wstring& roomName, string* playerName, char* roleArr)
{
	m_roomList.try_emplace(roomId, roomName, roomId, roleArr, playerName);
}

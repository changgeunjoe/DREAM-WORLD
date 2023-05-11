#include "stdafx.h"
#include "RoomManager.h"

RoomManager::RoomManager()
{
}

RoomManager::~RoomManager()
{
}

int RoomManager::GetRoomId()
{
	{
		std::lock_guard<std::mutex> lg{ m_currentLastRoomIdLock };
		if (m_currentLastRoomId < MAX_USER / 4)
			return m_currentLastRoomId++;
	}
	int restRoomId = -1;
	m_restRoomId.try_pop(restRoomId);
	return restRoomId;
}

Room& RoomManager::GetRunningRoomRef(int id)
{
	return m_roomArr[id];
};

void RoomManager::RoomDestroy(int roomId)
{
	m_roomArr[roomId].GameEnd();
}
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
		if (m_currentLastRoomId < MAX_USER / 4) {
			std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
			m_runningRoomIdSet.insert(m_currentLastRoomId);
			return m_currentLastRoomId++;
		}
	}
	int restRoomId = -1;
	m_restRoomId.try_pop(restRoomId);
	std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
	m_runningRoomIdSet.insert(restRoomId);
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

void RoomManager::RunningRoomLogic()
{
	std::set<int> runningRoomSet;
	{
		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
		runningRoomSet = m_runningRoomIdSet;
	}
	for (auto a : runningRoomSet) {
		m_roomArr[a].GameRunningLogic();
	}
}

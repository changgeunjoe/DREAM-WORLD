#include "stdafx.h"
#include "RoomManager.h"

RoomManager::RoomManager()
{
	for (int i = 0; i < m_roomArr.size(); i++)
		m_roomArr[i].SetRoomId(i);
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
	std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
	m_runningRoomIdSet.erase(roomId);
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

void RoomManager::BossFindPlayer(int roomId)
{
	{
		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
		if (!m_runningRoomIdSet.count(roomId)) return;
	}
	m_roomArr[roomId].BossFindPlayer();
}

void RoomManager::ChangeBossState(int roomId)
{
	{
		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
		if (!m_runningRoomIdSet.count(roomId)) return;
	}
	m_roomArr[roomId].ChangeBossState();
}

void RoomManager::UpdateGameStateForPlayer(int roomId)
{
	{
		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
		if (!m_runningRoomIdSet.count(roomId)) return;
	}
	;
	if (m_roomArr[roomId].GetRoomState() == ROOM_STAGE1) {
		m_roomArr[roomId].UpdateGameStateForPlayer_STAGE1();
		return;
	}
	m_roomArr[roomId].UpdateGameStateForPlayer_BOSS();
}

void RoomManager::UpdateSmallMonster(int roomId)
{
	{
		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
		if (!m_runningRoomIdSet.count(roomId)) return;
	}
	//m_roomArr[roomId].GetRoomState();
	m_roomArr[roomId].UpdateSmallMonster();
}

void RoomManager::BossAttackExecute(int roomId)
{
	{
		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
		if (!m_runningRoomIdSet.count(roomId)) return;
	}
	m_roomArr[roomId].BossAttackExecute();
}

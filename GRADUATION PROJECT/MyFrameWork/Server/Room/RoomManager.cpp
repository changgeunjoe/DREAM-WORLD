#include "stdafx.h"
#include "RoomManager.h"

RoomManager::RoomManager()
{	
}

RoomManager::~RoomManager()
{
}

bool RoomManager::InsertRecruitingRoom(std::string& roomId, std::wstring& roomName, int createPlayerId, ROLE r)
{
	std::lock_guard<std::mutex> lg{ m_RecruitRoomListLock };
	if (m_RecruitingRoomList.count(roomId) > 0)return false;
	m_RecruitingRoomList.try_emplace(roomId, roomId, roomName, createPlayerId, r);
	return true;
}

void RoomManager::ChangeRecruitToRunning(std::string& roomId)
{
	Room changeRoom;
	{
		std::lock_guard<std::mutex> lg{ m_RecruitRoomListLock };
		changeRoom = m_RecruitingRoomList[roomId];
		m_RecruitingRoomList.erase(roomId);
	}
	InsertRunningRoom(changeRoom);
}

void RoomManager::InsertRunningRoom(Room& recruitRoom)
{
	std::lock_guard<std::mutex> lg{ m_runningRoomListLock };
	m_RunningRoomList.emplace(recruitRoom.GetRoomId(), recruitRoom);
}

bool RoomManager::InsertRunningRoom(std::string& roomId, int player1, int player2, int player3, int player4)
{
	std::lock_guard<std::mutex> lg{ m_runningRoomListLock };
	if (m_RunningRoomList.count(roomId) > 0)return false;
	m_RunningRoomList.try_emplace(roomId, roomId, player1, player2, player3, player4);
	return true;
}

bool RoomManager::InsertRunningRoom(std::string& roomId, std::wstring& roomName, std::map<ROLE, int>& PlayerInfo)
{
	std::lock_guard<std::mutex> lg{ m_runningRoomListLock };
	if (m_RunningRoomList.count(roomId) > 0)return false;
	m_RunningRoomList.try_emplace(roomId, roomId, roomName);
	m_RunningRoomList[roomId].InsertInGamePlayer(PlayerInfo);
	return true;
}

std::vector<Room> RoomManager::GetRecruitingRoomList()
{
	std::vector<Room> resRoom;
	{
		std::lock_guard<std::mutex> lg{ m_RecruitRoomListLock };
		for (const auto& r : m_RecruitingRoomList) {
			resRoom.push_back(r.second);
		}
	}
	return resRoom;
}

std::vector<Room> RoomManager::GetRunningRoomList()
{
	std::vector<Room> resRoom;
	{
		std::lock_guard<std::mutex> lg{ m_runningRoomListLock };
		for (const auto& r : m_RunningRoomList) {
			resRoom.push_back(r.second);
		}
	}
	return resRoom;
}

std::vector<std::string> RoomManager::GetRunningRoomIdList()
{
	std::vector<std::string> resRoom;
	{
		std::lock_guard<std::mutex> lg{ m_runningRoomListLock };
		for (const auto& r : m_RunningRoomList) {
			resRoom.push_back(r.first);
		}
	}
	return resRoom;
}

std::vector<std::string> RoomManager::GetRecruitingRoomIdList()
{
	std::vector<std::string> resRoom;
	{
		std::lock_guard<std::mutex> lg{ m_RecruitRoomListLock };
		for (const auto& r : m_RecruitingRoomList) {
			resRoom.push_back(r.first);
		}
	}
	return resRoom;
}

bool RoomManager::IsExistRecruitRoom(std::string& roomId)
{
	return m_RecruitingRoomList.count(roomId);	
}

bool RoomManager::IsExistRunningRoom(std::string& roomId)
{
	return m_RunningRoomList.count(roomId);	
}

Room& RoomManager::GetRunningRoom(std::string& roomId)
{
	return m_RunningRoomList[roomId];
}

Room& RoomManager::GetRecuritRoom(std::string& roomId)
{
	return m_RecruitingRoomList[roomId];
}

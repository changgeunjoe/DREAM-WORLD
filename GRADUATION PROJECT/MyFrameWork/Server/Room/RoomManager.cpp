#include "stdafx.h"
#include "RoomManager.h"

RoomManager::RoomManager()
{
}

RoomManager::~RoomManager()
{
}

bool RoomManager::InsertRecruitingRoom(std::string& roomId, std::wstring& roomName, int createPlayerId)
{
	std::lock_guard<std::mutex> lg{ m_RecruitRoomListLock };
	if (m_RecruitingRoomList.count(roomId) > 0)return false;
	m_RecruitingRoomList.try_emplace(roomId, roomId, roomName, createPlayerId);
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

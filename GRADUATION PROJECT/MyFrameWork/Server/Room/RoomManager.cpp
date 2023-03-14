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
	std::lock_guard<std::mutex> lg{ m_roomListLock };
	if (m_RecruitingRoomList.count(roomId) > 0)return false;
	m_RecruitingRoomList.try_emplace(roomId, roomId, roomName, createPlayerId);
	return true;
}

Room& RoomManager::GetRecruitingRoom(std::string roomId)
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_RecruitingRoomList[roomId];

}

std::vector<Room> RoomManager::GetRecruitingRoomList()
{
	std::vector<Room> resRoom;
	{
		std::lock_guard<std::mutex> lg{ m_roomListLock };
		for (const auto& r : m_RecruitingRoomList) {
			resRoom.push_back(r.second);
		}
	}
	return resRoom;
}

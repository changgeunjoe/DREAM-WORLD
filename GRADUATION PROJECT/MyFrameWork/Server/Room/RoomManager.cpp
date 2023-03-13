#include "stdafx.h"
#include "RoomManager.h"

RoomManager::RoomManager()
{
}

RoomManager::~RoomManager()
{
}

void RoomManager::InsertRecruitingRoom(std::string& roomId, std::wstring& roomName, int createPlayerId)
{
	std::lock_guard<std::mutex> lg{ m_roomListLock };
	m_RecruitingRoomList.try_emplace(roomId, roomId, roomName, createPlayerId);
}

Room& RoomManager::GetRecruitingRoom(std::string roomId)
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_RecruitingRoomList[roomId];

}

std::vector<Room> RoomManager::GetRecruitingRoomList()
{
	std::vector<Room> resRoom;
	std::lock_guard<std::mutex> lg{ m_roomListLock };
	for (const auto& r : m_RecruitingRoomList) {
		resRoom.push_back(r.second);
	}
	return resRoom;
}

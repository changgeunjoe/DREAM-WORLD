#include "stdafx.h"
#include "RoomManager.h"
#include "Room.h"

void RoomManager::EraseRoom(std::shared_ptr<Room>& roomRef)
{
	std::lock_guard<std::mutex> runningLg(m_runningRoomLock);
	m_runningRooms.erase(roomRef);
}

std::shared_ptr<Room> RoomManager::MakeRunningRoom(std::vector<std::shared_ptr<UserSession>>& userRefVec)
{
	auto roomRef = std::make_shared<Room>(userRefVec);
	std::lock_guard<std::mutex> runningLg(m_runningRoomLock);
	m_runningRooms.insert(roomRef);
	return roomRef;
}

std::shared_ptr<Room> RoomManager::MakeRunningRoomAloneMode(std::shared_ptr<UserSession>& userRef)
{
	auto roomRef = std::make_shared<Room>(userRef);
	std::lock_guard<std::mutex> runningLg(m_runningRoomLock);
	m_runningRooms.insert(roomRef);
	return roomRef;
}

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
	return std::make_shared<Room>(userRefVec);
}

std::shared_ptr<Room> RoomManager::MakeRunningRoomAloneMode(std::shared_ptr<UserSession>& userRef)
{
	return std::make_shared<Room>(userRef);
}

#pragma once
#include "Room.h"

class RoomManager
{
public:
	RoomManager();
	~RoomManager();
private:
	std::unordered_map<std::string, Room> m_roomList; // string: 방 ID(고유성 가져야 됨), Room: 룸 정보를 가짐
};


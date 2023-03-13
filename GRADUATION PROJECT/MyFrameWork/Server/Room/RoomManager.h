#pragma once
#include "Room.h"

class RoomManager
{
public:
	RoomManager();
	~RoomManager();
private:
	std::unordered_map<std::string, Room> m_roomList; // string: �� ID(������ ������ ��), Room: �� ������ ����
};


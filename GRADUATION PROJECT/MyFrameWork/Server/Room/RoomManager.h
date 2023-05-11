#pragma once
#ifdef _DEBUG
#include "../../PCH/stdafx.h"
#endif
#include "Room.h"

class Logic;
class RoomManager
{
public:
	RoomManager();
	~RoomManager();
private:	
	std::array<Room, MAX_USER / 4> m_roomArr;
	std::mutex m_currentLastRoomIdLock;
	Concurrency::concurrent_queue<int> m_restRoomId;
	int m_currentLastRoomId = 0;
private:	
	std::mutex m_runningRoomSetLock;
	std::set<int> m_runningRoomIdSet;
public:
	int GetRoomId();
	Room& GetRunningRoomRef(int id);
	void RoomDestroy(int roomId);
public:
	friend Logic;
};


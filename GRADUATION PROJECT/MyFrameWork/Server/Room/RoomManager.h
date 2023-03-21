#pragma once
#include "Room.h"

class Logic;
class RoomManager
{
public:
	RoomManager();
	~RoomManager();
private:
	std::mutex m_RecruitRoomListLock;
	std::mutex m_runningRoomListLock;
	// string: �� ID(������ ������ ��), Room: �� ������ ����
	std::unordered_map<std::string, Room> m_RecruitingRoomList;
	std::unordered_map<std::string, Room> m_RunningRoomList;
public:
	bool InsertRecruitingRoom(std::string& roomId, std::wstring& roomName, int createPlayerId);
public:
	std::vector<Room> GetRecruitingRoomList();
public:
	friend Logic;
};


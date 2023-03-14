#pragma once
#include "Room.h"

class RoomManager
{
public:
	RoomManager();
	~RoomManager();
private:
	std::mutex m_roomListLock;
	// string: �� ID(������ ������ ��), Room: �� ������ ����
	std::unordered_map<std::string, Room> m_RecruitingRoomList;
	std::unordered_map<std::string, Room> m_RunningRoomList;
	
public:
	bool InsertRecruitingRoom(std::string& roomId, std::wstring& roomName, int createPlayerId);
	void DeleteRoom();

public:
	Room& GetRecruitingRoom(std::string roomId);
	Room& GetRunningRoom(std::string roomId);
	std::vector<Room> GetRecruitingRoomList();
};


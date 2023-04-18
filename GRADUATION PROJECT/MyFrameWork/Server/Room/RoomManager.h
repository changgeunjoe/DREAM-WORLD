#pragma once
#include "../PCH/stdafx.h"
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
	// string: ¹æ ID(°íÀ¯¼º °¡Á®¾ß µÊ), Room: ·ë Á¤º¸¸¦ °¡Áü
	std::unordered_map<std::string, Room> m_RecruitingRoomList;
	std::unordered_map<std::string, Room> m_RunningRoomList;
private:
	Room InvalidRoom;
public:
	void InsertRunningRoom(Room& recruitRoom);
	bool InsertRunningRoom(std::string& roomId, int player1, int player2, int player3, int player4);
	bool InsertRunningRoom(std::string& roomId, std::wstring& roomName, std::map<ROLE, int>& PlayerInfo);
	bool InsertRecruitingRoom(std::string& roomId, std::wstring& roomName, int createPlayerId, ROLE r);
	void ChangeRecruitToRunning(std::string& roomId);
public:
	std::vector<Room> GetRecruitingRoomList();
	std::vector<Room> GetRunningRoomList();
public:
	std::vector<std::string> GetRunningRoomIdList();
	std::vector<std::string> GetRecruitingRoomIdList();

public:
	bool IsExistRecruitRoom(std::string& roomId);
	bool IsExistRunningRoom(std::string& roomId);
	Room& GetRunningRoom(std::string& roomId);
	Room& GetRecuritRoom(std::string& roomId);
public:
	friend Logic;
};


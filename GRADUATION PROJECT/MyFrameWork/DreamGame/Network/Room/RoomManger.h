#pragma once
#include "Room.h"
class RoomManger // 클라이언트가 가지고 있는 룸 정보들(서버에서 리스트들 리퀘스트 요청시)
{
public:
	RoomManger() {}
	~RoomManger() {}
private:
	mutex m_roomLock;
	unordered_map<string, Room> m_roomList;
public:
	bool m_bFirstUpdate = true;//첫 업데이트라면
public:
	void DeleteRoomList() { m_roomList.clear(); }
	void InsertRoom(string& roomId, wstring& roomName, wstring* playerName, char* roleArr);
	std::chrono::high_resolution_clock::time_point m_lastUpdateTime = std::chrono::high_resolution_clock::now();
};

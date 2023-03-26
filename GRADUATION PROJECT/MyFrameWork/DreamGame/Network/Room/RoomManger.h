#pragma once
#include "Room.h"
class RoomManger // Ŭ���̾�Ʈ�� ������ �ִ� �� ������(�������� ����Ʈ�� ������Ʈ ��û��)
{
public:
	RoomManger() {}
	~RoomManger() {}
private:
	mutex m_roomLock;
	unordered_map<string, Room> m_roomList;
public:
	bool m_bFirstUpdate = true;//ù ������Ʈ���
public:
	void DeleteRoomList() { m_roomList.clear(); }
	void InsertRoom(string& roomId, wstring& roomName, wstring* playerName, char* roleArr);
	std::chrono::high_resolution_clock::time_point m_lastUpdateTime = std::chrono::high_resolution_clock::now();
};

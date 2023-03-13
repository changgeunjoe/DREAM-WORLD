#pragma once

class Room
{
public:
	Room();
	Room(std::string& roomId, std::wstring& roomName, int onwerId);
	Room(std::string roomId);
	Room(const Room& rhs);
	~Room();
public:
	Room& operator=(Room& rhs);
private:
	std::wstring m_roomName;
	std::string m_roomId;
	int m_roomOwnerId = -1;// �� ������ ���� ID
private:
	//���� �����ϴ� �÷��̾���� ��� ������
	//set<pair> // �׷��⿣... PlayerObj�� ROle�� �־���. // Set<int> : Player ID(server��)���� �ϸ� �ɱ�
	std::set<int> m_Players;
public:
	const std::set<int>& GetPlayerSet() { return m_Players; }
	const std::string& GetRoomId() { return m_roomId; }
	const std::wstring& GetRoomName() { return m_roomName; }
	const int roomOwner() { return m_roomOwnerId; }	
};

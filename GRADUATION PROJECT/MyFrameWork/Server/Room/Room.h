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
	std::mutex m_LockPlayerSet;
	std::set<int> m_Players;
public:
	const std::set<int> GetPlayerSet() { 
		std::set<int> pSet;
		{
			std::lock_guard<std::mutex> lg{ m_LockPlayerSet };
			pSet = m_Players;
		}
		return pSet;
	}
	const std::string& GetRoomId() { return m_roomId; }
	const std::wstring& GetRoomName() { return m_roomName; }
	const int roomOwner() { return m_roomOwnerId; }	
};

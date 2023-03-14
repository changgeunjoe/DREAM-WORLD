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
	int m_roomOwnerId = -1;// 룸 생성한 자의 ID
private:
	//현재 존재하는 플레이어들을 어떻게 담을까
	//set<pair> // 그러기엔... PlayerObj에 ROle을 넣었다. // Set<int> : Player ID(server상)으로 하면 될까
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

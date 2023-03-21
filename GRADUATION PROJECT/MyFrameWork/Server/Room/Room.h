#pragma once

class Room
{
public:
	Room();
	Room(std::string& roomId, std::wstring& roomName, int onwerId);
	Room(std::string& roomId, int player1, int player2, int player3, int player4);
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
	//set<pair> // 그러기엔... PlayerObj에 Role을 넣었다. // Set<int> : Player ID(server상)으로 하면 될까
	std::mutex m_LockPlayerSet;
	std::set<std::pair<ROLE, int>> m_Players;

	std::mutex m_LockWaitPlayerSet;
	std::set<std::pair<ROLE, int>> m_WaitPlayers;

public://Get
	const std::set<std::pair<ROLE, int>> GetPlayerSet() {
		std::set<std::pair<ROLE, int>> playerSet;
		{
			std::lock_guard<std::mutex> lg{ m_LockPlayerSet };
			playerSet = m_Players;
		}
		return playerSet;
	}
	const std::string& GetRoomId() { return m_roomId; }
	const std::wstring& GetRoomName() { return m_roomName; }
	const int roomOwner() { return m_roomOwnerId; }
public:
	void InsertPlayer(ROLE r, int playerId);
	void InsertWaitPlayer(ROLE r, int playerId);
	void DeletePlayer(int playerId);
};

#pragma once
#include "../Session/Session.h"
class Room
{
public:
	Room();
	Room(std::string& roomId, std::wstring& roomName, int onwerId, ROLE r);
	Room(std::string& roomId, int player1, int player2, int player3, int player4); // 메칭용 룸 생성자
	Room(std::string roomId);
	Room(std::string roomId, std::wstring roomName);
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

	std::mutex m_lockInGamePlayers;
	std::map<ROLE, int> m_inGamePlayers;
	//std::set<std::pair<ROLE, int>> m_Players;

	std::mutex m_lockWaitPlayers;
	std::map<ROLE, int> m_waitPlayers;

private:
	std::vector<Session> m_monsters;
	Session m_boss;
public://Get
	std::map<ROLE, int> GetInGamePlayerMap() {
		std::map<ROLE, int> playerMap;
		{
			std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
			playerMap = m_inGamePlayers;
		}
		return playerMap;
	}
	const std::string& GetRoomId() { return m_roomId; }
	const std::wstring& GetRoomName() { return m_roomName; }
	const int roomOwner() { return m_roomOwnerId; }
public:
	void InsertInGamePlayer(std::map<ROLE, int>& matchPlayer);
	void InsertInGamePlayer(ROLE r, int playerId);
	void DeleteInGamePlayer(int playerId);
public:
	void InsertWaitPlayer(ROLE r, int playerId);
	void DeleteWaitPlayer(int playerId);
};

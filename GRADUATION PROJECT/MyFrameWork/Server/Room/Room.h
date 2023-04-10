#pragma once
#include "../Session/Session.h"
class Room
{
public:
	Room();
	Room(std::string& roomId, std::wstring& roomName, int onwerId, ROLE r);
	Room(std::string& roomId, int player1, int player2, int player3, int player4); // ��Ī�� �� ������
	Room(std::string roomId);
	Room(std::string roomId, std::wstring roomName);
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
	//set<pair> // �׷��⿣... PlayerObj�� Role�� �־���. // Set<int> : Player ID(server��)���� �ϸ� �ɱ�

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

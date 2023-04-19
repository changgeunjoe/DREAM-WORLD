#pragma once
#include "../PCH/stdafx.h"
#include "../Session/SessionObject/ShootingSessionObject.h"
#include "../Session/SessionObject/MonsterSessionObject.h"

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
	std::mutex m_lockInGamePlayers;
	std::map<ROLE, int> m_inGamePlayers;

	std::mutex m_lockWaitPlayers;
	std::map<ROLE, int> m_waitPlayers;
	std::array<ShootingSessionObject, 10> m_arrows;
	std::array<ShootingSessionObject, 10> m_balls;
	std::mutex m_arrowLock;
	std::mutex m_ballLock;
	std::list<int> m_shootingBall;
	std::list<int> m_shootingArrow;
	Concurrency::concurrent_queue<int> m_restArrow;
	Concurrency::concurrent_queue<int> m_restBall;
private:
	//std::vector<Session> m_monsters;
	MonsterSessionObject m_boss;
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
public:
	void CreateBossMonster();
	MonsterSessionObject& GetBoss();
public:
	void ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed);
	void ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed);
};

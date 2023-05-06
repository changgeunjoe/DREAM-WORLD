#pragma once
#include "../PCH/stdafx.h"
#include "../Session/SessionObject/ShootingSessionObject.h"
#include "../Session/SessionObject/MonsterSessionObject.h"

class Room
{
public:
	Room();
	Room(std::string& roomId, std::wstring& roomName, int onwerId, ROLE r);
	Room(std::string& roomId, int player1, int player2, int player3, int player4); // 五暢遂 結 持失切
	Room(std::string roomId);
	Room(std::string roomId, std::wstring roomName);
	Room(const Room& rhs);
	~Room();
public:
	Room& operator=(Room& rhs);
private:
	std::wstring m_roomName;
	std::string m_roomId;
	int m_roomOwnerId = -1;// 結 持失廃 切税 ID
	int m_arrowCount = 0;
	int m_ballCount = 0;
private:
	std::mutex m_lockInGamePlayers;
	std::map<ROLE, int> m_inGamePlayers;

	std::mutex m_lockWaitPlayers;
	std::map<ROLE, int> m_waitPlayers;
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
	int GetInGamePlayerNum() {
		std::lock_guard<std::mutex> lg{ m_lockInGamePlayers };
		return m_inGamePlayers.size();
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
	std::array<ShootingSessionObject, 10> m_arrows;
	std::array<ShootingSessionObject, 10> m_balls;
	Concurrency::concurrent_queue<int> m_restArrow;
	Concurrency::concurrent_queue<int> m_restBall;
	void ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed);
	void ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed);
public:
	Concurrency::concurrent_queue<short> m_bossDamagedQueue;
	bool MeleeAttack(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 pos);
	void GameStart();
	void GameRunningLogic();
	void GameEnd();
};

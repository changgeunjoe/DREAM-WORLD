#pragma once
#ifdef _DEBUG
#include "../PCH/stdafx.h"
#endif
#include "../Session/SessionObject/ShootingSessionObject.h"
#include "../Session/SessionObject/MonsterSessionObject.h"

class Room
{
public:
	Room();
	Room(const Room& rhs);
	~Room();
public:
	Room& operator=(Room& rhs);
private:
	bool m_isAlive = false;
	std::wstring m_roomName;
	int m_roomOwnerId = -1;// 룸 생성한 자의 ID

//Player Session	
private:
	std::mutex m_lockInGamePlayers;
	std::map<ROLE, int> m_inGamePlayers;
public:
	void InsertInGamePlayer(std::map<ROLE, int>& matchPlayer);
	void InsertInGamePlayer(ROLE r, int playerId);
	void DeleteInGamePlayer(int playerId);
	std::map<ROLE, int> GetPlayerMap();
	int GetPlayerNum() { return m_inGamePlayers.size(); }

//Monster Session
private:
	MonsterSessionObject m_boss;
public:
	void CreateBossMonster();
	MonsterSessionObject& GetBoss();
public:
	int m_arrowCount = 0;
	int m_ballCount = 0;
	std::array<ShootingSessionObject, 10> m_arrows;
	std::array<ShootingSessionObject, 10> m_balls;
	Concurrency::concurrent_queue<int> m_restArrow;
	Concurrency::concurrent_queue<int> m_restBall;
	void ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed);
	void ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed);
public:
	Concurrency::concurrent_queue<short> m_bossDamagedQueue;
	bool MeleeAttack(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 pos);

//Game Session
public:
	void GameStart();
	void GameRunningLogic();
	void GameEnd();
};

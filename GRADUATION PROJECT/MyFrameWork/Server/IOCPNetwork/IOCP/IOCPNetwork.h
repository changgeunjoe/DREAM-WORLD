#pragma once
#include "../../Session/UserSession.h"
#include "../../PCH/stdafx.h" //Áö¿ö¾ßµÊ

#define PORT 9000

static std::random_device rd;
static std::default_random_engine dre(rd());
static std::uniform_int_distribution<> aggroRandomPlayer(0, 3);//inclusive
static std::uniform_int_distribution<> bossRandAttack(0, (int)BOSS_ATTACK::ATTACK_COUNT - 1);

class IOCPNetwork
{
private:
	HANDLE m_hIocp;
	std::vector<std::thread> m_workerThread;

private:
	SOCKET m_listenSocket;
	SOCKET m_clientSocket;
	SOCKADDR_IN server_addr;

	ExpOver* m_acceptOver;

	volatile bool b_isRunning;
private:
	int		m_currentClientId;
	concurrency::concurrent_queue<int> m_restClientId;
public:
	std::array<UserSession, MAX_USER> m_session;
public:

	IOCPNetwork();
	~IOCPNetwork();

	void Start();
	void Destroy();

private:
	void Initialize();
	void WorkerThread();
	int GetUserId();
public:
	void DisconnectClient(int id);
public:
	int GetCurrentId() { return m_currentClientId; };
	HANDLE& GetIocpHandle() {
		return m_hIocp;
	};
};

#pragma once
#include "../../PCH/stdafx.h"
#include "../ExpOver/ExpOver.h"
#define PORT 9000


//static std::random_device rd;
//static std::default_random_engine dre(rd());
//static std::uniform_int_distribution<> aggroRandomPlayer(0, 3);//inclusive
//static std::uniform_int_distribution<> bossRandAttack(0, (int)BOSS_ATTACK::ATTACK_COUNT - 1);

class IOCP
{
private:
	HANDLE m_hIocp;
	std::vector<std::thread> m_workerThread;

private:
	SOCKET m_listenSocket;
	SOCKET m_clientSocket;
	

	ExpOver* m_acceptOver;
	char m_acceptBuffer[MAX_BUF_SIZE];	
private:	
	concurrency::concurrent_queue<int> m_restClientId;
public:

	IOCP();
	~IOCP();

	void Start();
	void Destroy();

private:
	void Initialize();
	void WorkerThread();
	//int GetUserId();

public:
	HANDLE& GetIocpHandle() {
		return m_hIocp;
	};
};

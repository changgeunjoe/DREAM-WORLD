#pragma once
#include "../../Session/Session.h"
#include "../../PCH/stdafx.h" //�����ߵ�

#define PORT 9000

static std::random_device rd;
static std::default_random_engine dre(rd());
static std::uniform_int_distribution<> aggroRandomPlayer(0, 4);

class ExpOver;
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

public:
	std::array<Session, MAX_USER> m_session;
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
	int GetCurrentId() { return m_currentClientId; };
	HANDLE& GetIocpHandle() {
		return m_hIocp;
	};
};

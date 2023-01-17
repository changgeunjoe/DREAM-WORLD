#pragma once
#include "../../Session/Session.h"

#define PORT 9000
#define MAX_USER 100

class ExpOver;
class IOCPNetwork
{
private:
	HANDLE m_hIocp;	
	std::vector<std::thread> m_workerThread;

private:
	SOCKET m_listenSocket;
	SOCKET m_clientSocket;

	ExpOver* m_acceptOver;

	volatile bool b_isRunning;

private:
	std::array<Session, MAX_USER> m_session;
	int		m_currentClientId;
public:

	IOCPNetwork();
	~IOCPNetwork();

	void Start();

private:
	void Initialize();
	void Destroy();
	void WorkerThread();
	int GetUserId();

public:

};

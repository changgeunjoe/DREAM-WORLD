#pragma once

#define PORT 9000

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
public:

	IOCPNetwork();
	~IOCPNetwork();

	void Start();

private:
	void Initialize();
	void Destroy();
	void WorkerThread();
};

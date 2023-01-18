#pragma once
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")

#define SERVER_PORT 9000
#define MAX_BUF_SIZE 1024


class NetworkHelper {
private:
	SOCKET m_clientSocket;

	bool m_bIsRunnung = false;
public:
	NetworkHelper();
	~NetworkHelper();

public:
	bool TryConnect();
	void Start();
	void RunThread();
	void Send();
	void Recv();
};
#pragma once
#include "../stdafx.h"
#include <thread>
#include <WS2tcpip.h>
#include "../../Server/IOCPNetwork/protocol/protocol.h"


#pragma comment(lib, "WS2_32.lib")

#define SERVER_PORT 9000
#define MAX_BUF_SIZE 1024

class NetworkHelper {
private:
	SOCKET m_clientSocket;
private:
	char m_buffer[MAX_BUF_SIZE];
	int m_prevPacketSize = 0;
private:
	bool m_bIsRunnung = false;
	std::thread m_runThread;
public:
	NetworkHelper();
	~NetworkHelper();

public:
	bool TryConnect();
	void Start();
	void RunThread();
public:
	void SendMovePacket(DIRECTION d);
	void SendStopPacket(const DirectX::XMFLOAT3& position); // , const DirectX::XMFLOAT3& rotate
	void SendRotatePacket(ROTATE_AXIS axis, float angle);
	void SendKeyUpPacket(DIRECTION d);
	void SendLoginData(char* loginId, char* pw);
private:
	void ConstructPacket(int ioByte);
private:
	void Destroy();
};
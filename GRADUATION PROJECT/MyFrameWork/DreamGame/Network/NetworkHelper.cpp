#include "NetworkHelper.h"
#include <iostream>
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

NetworkHelper::NetworkHelper()
{
	std::wcout.imbue(std::locale("korean"));
	WSADATA WSAData;

	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "WSAAStartUp Err" << std::endl;
	}
	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_clientSocket == INVALID_SOCKET) {
		std::cout << "client Socket Invalid" << std::endl;
	}
	m_bIsRunnung = false;
}
NetworkHelper::~NetworkHelper()
{

}

bool NetworkHelper::TryConnect()
{
	sockaddr_in sockaddrIn;
	sockaddrIn.sin_family = AF_INET;
	sockaddrIn.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &sockaddrIn.sin_addr.s_addr);
	if (connect(m_clientSocket, reinterpret_cast<sockaddr*>(&sockaddrIn), sizeof(sockaddrIn))) {
		std::cout << "connect Fail" << std::endl;
		return false;
	}
	return true;
}
void NetworkHelper::Start()
{

}
void NetworkHelper::RunThread()
{
	while (m_bIsRunnung) {

	}
}
void NetworkHelper::Send()
{

}
void NetworkHelper::Recv()
{

}

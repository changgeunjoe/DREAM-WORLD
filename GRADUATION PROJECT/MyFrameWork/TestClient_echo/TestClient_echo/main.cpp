#include<iostream>
#include<fstream>
#include<string>
#include<WS2tcpip.h>
#include "../../Server/IOCPNetwork/protocol/protocol.h"


#pragma comment(lib, "ws2_32")

#define SERVER_PORT 9000
#define MAX_BUF_SIZE 1024

using namespace std;

void display_Err(int Errcode);

int main(int argv, char** argc)
{
	wcout.imbue(std::locale("korean"));
	WSADATA WSAData;

	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		display_Err(WSAGetLastError());
		return 1;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		display_Err(WSAGetLastError());
		return 1;
	}

	sockaddr_in sockaddrIn;
	sockaddrIn.sin_family = AF_INET;
	sockaddrIn.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, argc[1], &sockaddrIn.sin_addr.s_addr);

	if (connect(clientSocket, reinterpret_cast<sockaddr*>(&sockaddrIn), sizeof(sockaddrIn))) {
		display_Err(WSAGetLastError());
		return 1;
	}

	char buf[MAX_BUF_SIZE] = { 0 };

	ZeroMemory(buf, MAX_BUF_SIZE);

	while (true) {

	}

	closesocket(clientSocket);
	WSACleanup();

}

void display_Err(int Errcode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, Errcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf, 0, NULL);
	wcout << "ErrorCode: " << Errcode << " - " << (WCHAR*)lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}
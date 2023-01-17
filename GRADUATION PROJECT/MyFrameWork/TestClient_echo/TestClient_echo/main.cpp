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
void constructPacket(int ioByte);
void processPacket(char*);

int prevPacket = 0;
char buf[MAX_BUF_SIZE] = { 0 };

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
	inet_pton(AF_INET, "127.0.0.1", &sockaddrIn.sin_addr.s_addr);

	if (connect(clientSocket, reinterpret_cast<sockaddr*>(&sockaddrIn), sizeof(sockaddrIn))) {
		display_Err(WSAGetLastError());
		return 1;
	}
	/*u_long nonBlockingMode = 1;
	ioctlsocket(clientSocket, FIONBIO, &nonBlockingMode);*/
	ZeroMemory(buf, MAX_BUF_SIZE);
	while (true) {
		cout << "send: ";
		CS_TEST_CHAT_PACKET sendPacket;
		ZeroMemory(sendPacket.message, 100);
		cin >> sendPacket.message;
		sendPacket.type = CS_TEST_CHAT;
		sendPacket.size = sizeof(CS_TEST_CHAT_PACKET);
		send(clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);

		int res = recv(clientSocket, buf + prevPacket, MAX_BUF_SIZE, 0);
		if (res > 0) {
			constructPacket(res);
		}
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

void constructPacket(int ioByte)
{
	int remain_data = ioByte;
	if (prevPacket > 0) {
		int remain_data = ioByte + prevPacket;
	}
	char* p = buf;
	while (remain_data > 0) {
		int packet_size = p[0];
		if (packet_size <= remain_data) {
			processPacket(p);
			p = p + packet_size;
			remain_data = remain_data - packet_size;
		}
		else break;
	}
	prevPacket = remain_data;
	if (remain_data > 0) {
		std::memcpy(buf, p, prevPacket);
		ZeroMemory(p + prevPacket, MAX_BUF_SIZE - prevPacket);
	}
}

void processPacket(char* p)
{
	switch (p[1])
	{
	case SC_TEST_CHAT:
	{
		SC_TEST_CHAT_PACKET* sendPacket = reinterpret_cast<SC_TEST_CHAT_PACKET*>(p);
		cout << "recv: " << sendPacket->message << endl;
	}
	break;

	default:
		break;
	}
}

#include "stdafx.h"
#include <iostream>
#include "Logic/Logic.h"
#include "NetworkHelper.h"

extern clientNet::Logic g_Logic;

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
	/*u_long isNonBlock = 1;
	ioctlsocket(m_clientSocket, FIONBIO, &isNonBlock);*/
	m_bIsRunnung = false;
	m_prevPacketSize = 0;
}
NetworkHelper::~NetworkHelper()
{
	Destroy();
	closesocket(m_clientSocket);
}

bool NetworkHelper::TryConnect()
{
	sockaddr_in sockaddrIn;
	sockaddrIn.sin_family = AF_INET;
	sockaddrIn.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &sockaddrIn.sin_addr.s_addr);
	int retVal = connect(m_clientSocket, reinterpret_cast<sockaddr*>(&sockaddrIn), sizeof(sockaddrIn));
	if (retVal) {
		std::cout << "connect Fail" << std::endl;
		return false;
	}
	return true;
}
void NetworkHelper::Start()
{
	m_bIsRunnung = true;
	m_runThread = std::thread([this]() {RunThread(); });

}
void NetworkHelper::RunThread()
{
	while (m_bIsRunnung) {
		int ioByte = recv(m_clientSocket, m_buffer + m_prevPacketSize, MAX_BUF_SIZE, 0);
		if (ioByte == 0) {
			//Server Disconnect
		}
		else if (ioByte > 0) {
			ConstructPacket(ioByte);
		}
		else {
			//error
		}
	}
}

void NetworkHelper::ConstructPacket(int ioByte)
{
	int remain_data = ioByte + m_prevPacketSize;
	char* p = m_buffer;
	while (remain_data > 0) {
		int packet_size = p[0];
		if (packet_size <= remain_data) {
			g_Logic.ProcessPacket(p);
			p = p + packet_size;
			remain_data = remain_data - packet_size;
		}
		else break;
	}
	m_prevPacketSize = remain_data;
	if (remain_data > 0) {
		std::memcpy(m_buffer, p, remain_data);
		ZeroMemory(m_buffer + remain_data, MAX_BUF_SIZE - remain_data);
	}
}

void NetworkHelper::Destroy()
{
	m_bIsRunnung = false;
	if (m_runThread.joinable())
		m_runThread.join();
}

void NetworkHelper::SendMovePacket(DIRECTION d)
{
	CLIENT_PACKET::MovePacket sendPacket;
	sendPacket.direction = d;
	sendPacket.type = CLIENT_PACKET::MOVE_KEY_DOWN;
	sendPacket.size = sizeof(CLIENT_PACKET::MovePacket);
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendStopPacket(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotate)
{
	CLIENT_PACKET::StopPacket sendPacket;
	sendPacket.type = CLIENT_PACKET::STOP;
	sendPacket.size = sizeof(CLIENT_PACKET::StopPacket);
	sendPacket.position = position;
	sendPacket.rotate = rotate;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}
void NetworkHelper::SendRotatePacket(ROTATE_AXIS axis, float angle)
{
	CLIENT_PACKET::RotatePacket sendPacket;
	sendPacket.type = CLIENT_PACKET::ROTATE;
	sendPacket.size = sizeof(CLIENT_PACKET::RotatePacket);
	sendPacket.angle = angle;
	sendPacket.axis = axis;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendKeyUpPacket(DIRECTION d)
{
	CLIENT_PACKET::MovePacket sendPacket;
	sendPacket.direction = d;
	sendPacket.type = CLIENT_PACKET::MOVE_KEY_UP;
	sendPacket.size = sizeof(CLIENT_PACKET::MovePacket);
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendLoginData(char* loginId, char* pw)
{
	CLIENT_PACKET::LoginPacket sendPacket;
	sendPacket.type = CLIENT_PACKET::LOGIN;
	sendPacket.size = sizeof(sendPacket);
	ZeroMemory(sendPacket.id, 0);
	ZeroMemory(sendPacket.pw, 0);
	memcpy(sendPacket.id, loginId, strlen(loginId) + 1);
	memcpy(sendPacket.pw, pw, strlen(pw) + 1);
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

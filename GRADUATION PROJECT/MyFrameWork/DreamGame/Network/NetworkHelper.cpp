#include "stdafx.h"
#include <iostream>
#include "Logic/Logic.h"
#include "NetworkHelper.h"
#include "Room/RoomManger.h"

extern Logic g_Logic;
extern RoomManger g_RoomManager;

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
	inet_pton(AF_INET, SERVER_IP, &sockaddrIn.sin_addr.s_addr);
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
			cout << "recvByte: " << ioByte << endl;
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
		short packet_size = 0;
		memcpy(&packet_size, p, 2);
		if ((int)packet_size <= remain_data) {
			g_Logic.ProcessPacket(p);
			p = p + packet_size;
			remain_data = remain_data - (int)packet_size;
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

void NetworkHelper::SendStopPacket(const DirectX::XMFLOAT3& position) // , const DirectX::XMFLOAT3& rotate
{
	CLIENT_PACKET::StopPacket sendPacket;
	sendPacket.type = CLIENT_PACKET::STOP;
	sendPacket.size = sizeof(CLIENT_PACKET::StopPacket);
	sendPacket.position = position;
	// sendPacket.rotate = rotate;
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

void NetworkHelper::SendMouseStatePacket(bool LClickedButton, bool RClickedButton)
{
	CLIENT_PACKET::MouseInputPacket sendPacket;
	sendPacket.LClickedButton = LClickedButton;
	sendPacket.RClickedButton = RClickedButton;
	sendPacket.type = CLIENT_PACKET::MOUSE_INPUT;
	sendPacket.size = sizeof(CLIENT_PACKET::MouseInputPacket);
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendCreateRoomPacket(ROLE r, wstring roomName)
{
	CLIENT_PACKET::CreateRoomPacket sendPacket;
	sendPacket.Role = r;
	memcpy(sendPacket.roomName, roomName.c_str(), roomName.size() * 2);
	sendPacket.roomName[roomName.size()] = 0;
	sendPacket.size = sizeof(CLIENT_PACKET::CreateRoomPacket);
	sendPacket.type = CLIENT_PACKET::CREATE_ROOM;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendRequestRoomList()
{
	auto durationTime = chrono::high_resolution_clock::now() - g_RoomManager.m_lastUpdateTime;
	if (g_RoomManager.m_bFirstUpdate || durationTime.count() > 30000) { // 30√ ?
		CLIENT_PACKET::RequestRoomListPacket sendPacket;
		sendPacket.size = sizeof(CLIENT_PACKET::RequestRoomListPacket);
		sendPacket.type = CLIENT_PACKET::REQUEST_ROOM_LIST;
		send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
		g_RoomManager.m_lastUpdateTime = chrono::high_resolution_clock::now();
	}
}

void NetworkHelper::SendMatchRequestPacket(ROLE r)
{
	CLIENT_PACKET::MatchPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::MatchPacket);
	sendPacket.type = CLIENT_PACKET::MATCH;
	sendPacket.Role = (char)r;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendArrowAttackPacket(const XMFLOAT3& pos, const XMFLOAT3& dir, float speed)
{
	CLIENT_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::ShootingObject);
	sendPacket.type = CLIENT_PACKET::SHOOTING_ARROW;
	sendPacket.pos = pos;
	sendPacket.dir = dir;
	sendPacket.speed = speed;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

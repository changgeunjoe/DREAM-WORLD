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
		int ioByte = recv(m_clientSocket, m_buffer + m_prevPacketSize, MAX_BUF_SIZE - m_prevPacketSize, 0);
		if (ioByte == 0) {
			//Server Disconnect
		}
		else if (ioByte > 0) {
			ConstructPacket(ioByte);
			//cout << "recvByte: " << ioByte << endl;
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
	while (remain_data > 1) {
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
	if (m_prevPacketSize > 0) {
		std::memcpy(m_buffer, p, m_prevPacketSize);
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
	sendPacket.role = g_Logic.GetMyRole();
	sendPacket.time = std::chrono::utc_clock::now();
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendStopPacket(const DirectX::XMFLOAT3& position) // , const DirectX::XMFLOAT3& rotate
{
	CLIENT_PACKET::StopPacket sendPacket;
	sendPacket.type = CLIENT_PACKET::STOP;
	sendPacket.size = sizeof(CLIENT_PACKET::StopPacket);
	sendPacket.position = position;
	sendPacket.role = g_Logic.GetMyRole();
	sendPacket.time = std::chrono::utc_clock::now();
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

void NetworkHelper::Send_SkillExecute_Q()
{
	CLIENT_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::NotifyPacket);
	sendPacket.type = CLIENT_PACKET::SKILL_INPUT_Q;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::Send_SkillExecute_E()
{
	CLIENT_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::NotifyPacket);
	sendPacket.type = CLIENT_PACKET::SKILL_INPUT_E;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendMatchRequestPacket()
{
	CLIENT_PACKET::MatchPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::MatchPacket);
	sendPacket.type = CLIENT_PACKET::MATCH;
	sendPacket.Role = (char)g_Logic.GetMyRole();
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

void NetworkHelper::SendBallAttackPacket(const XMFLOAT3& pos, const XMFLOAT3& dir, float speed)
{
	CLIENT_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::ShootingObject);
	sendPacket.type = CLIENT_PACKET::SHOOTING_BALL;
	sendPacket.pos = pos;
	sendPacket.dir = dir;
	sendPacket.speed = speed;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendMeleeAttackPacket(const XMFLOAT3& dir)
{
	//CLIENT_PACKET::MeleeAttackPacket sendPacket;
	//sendPacket.size = sizeof(CLIENT_PACKET::MeleeAttackPacket);
	//sendPacket.type = CLIENT_PACKET::MELEE_ATTACK;
	//sendPacket.dir = dir;
	//send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendTestGameEndPacket()
{
	CLIENT_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::NotifyPacket);
	sendPacket.type = CLIENT_PACKET::TEST_GAME_END;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendTestGameEndOKPacket()
{
	CLIENT_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::NotifyPacket);
	sendPacket.type = CLIENT_PACKET::GAME_END_OK;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendOnPositionTriggerBox1()
{
	CLIENT_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::NotifyPacket);
	sendPacket.type = CLIENT_PACKET::TRIGGER_BOX_ON;
	//else sendPacket.type = CLIENT_PACKET::TRIGGER_BOX_OUT;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendSkipNPCCommunicate()
{
	CLIENT_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::NotifyPacket);
	sendPacket.type = CLIENT_PACKET::SKIP_NPC_COMMUNICATION;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendChangeStage_BOSS()
{
	CLIENT_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::NotifyPacket);
	sendPacket.type = CLIENT_PACKET::STAGE_CHANGE_BOSS;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendCommonAttack(const XMFLOAT3& attackDirection, int power)
{
	CLIENT_PACKET::PlayerCommonAttackPacket sendPacket;
	sendPacket.size = sizeof(CLIENT_PACKET::PlayerCommonAttackPacket);
	sendPacket.type = CLIENT_PACKET::PLAYER_COMMON_ATTACK;
	sendPacket.role = g_Logic.GetMyRole();
	sendPacket.dir = attackDirection;
	sendPacket.power = power;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);

}

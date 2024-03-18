#include "stdafx.h"
#include <iostream>
#include "Logic/Logic.h"
#include "NetworkHelper.h"
#include "Room/RoomManger.h"
#include "../../Server/Network/protocol/protocol.h"

extern Logic g_Logic;

NetworkHelper::NetworkHelper()
{
	std::wcout.imbue(std::locale("korean"));
	std::cout << "NetworkHelper::NetworkHelper() - Constructor" << endl;
	WSADATA WSAData;

	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "WSAAStartUp Err" << std::endl;
	}
	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_clientSocket == INVALID_SOCKET) {
		std::cout << "client Socket Invalid" << std::endl;
	}
	u_long isNonBlock = 1;
	ioctlsocket(m_clientSocket, FIONBIO, &isNonBlock);

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
	if (SOCKET_ERROR == retVal) {
		std::cout << "connect Fail" << std::endl;
		int errorCode = WSAGetLastError();
		if (WSAEISCONN == errorCode) return true;
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
	cout << "Start Recv" << endl;
	while (true) {
		int ioByte = recv(m_clientSocket, m_buffer + m_prevPacketSize, MAX_RECV_BUF_SIZE - m_prevPacketSize, 0);
		if (ioByte == 0) {
			std::cout << "Server Disconnected" << endl;
			//Server Disconnect
		}
		else if (ioByte > 0) {
			ConstructPacket(ioByte);
			//cout << "recvByte: " << ioByte << endl;
		}
		else {
			int ErrorCode = WSAGetLastError();
			if (WSAEWOULDBLOCK != ErrorCode) {
				std::cout << "errorCode: " << ErrorCode << endl;
			}
		}
		if (g_Logic.IsReadySendSync()) {
			SendTimeSyncPacket();
		}
	}
}

void NetworkHelper::ConstructPacket(const int& ioByte)
{
	int remainSize = ioByte + m_prevPacketSize;
	char* bufferPosition = m_buffer;
	while (remainSize > sizeof(PacketHeader::size)) {
		PacketHeader* currentPacket = reinterpret_cast<PacketHeader*>(bufferPosition);
		if (currentPacket->size > remainSize) {
			//완성된 패킷이 만들어지지 않음.
			break;
		}
		//완성된 패킷
		g_Logic.ProcessPacket(currentPacket);
		//남은 퍼버 크기 최신화, 현재 버퍼 위치 다음 패킷 시작 위치로
		remainSize -= currentPacket->size;
		bufferPosition = bufferPosition += currentPacket->size;
	}
	//현재 남은 데이터 크기 저장
	m_prevPacketSize = remainSize;
	//남은 패킷 데이터가 있다면, 맨 앞으로 당기기
	if (remainSize > 0)
		std::memcpy(m_buffer, bufferPosition, remainSize);
}

void NetworkHelper::Destroy()
{
	m_bIsRunnung = false;
	if (m_runThread.joinable())
		m_runThread.join();
}

void NetworkHelper::SendMovePacket(DIRECTION d)
{
	CLIENT_PACKET::MovePacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::MOVE_KEY_DOWN));
	sendPacket.direction = d;

	std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
	sendPacket.time = t + std::chrono::microseconds(g_Logic.GetDiffTime());
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendStopPacket(const DirectX::XMFLOAT3& position) // , const DirectX::XMFLOAT3& rotate
{
	CLIENT_PACKET::StopPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::STOP));
	sendPacket.position = position;

	sendPacket.time = std::chrono::steady_clock::now() + std::chrono::microseconds(g_Logic.GetDiffTime());
	// sendPacket.rotate = rotate;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}
void NetworkHelper::SendRotatePacket(ROTATE_AXIS axis, float angle)
{
	CLIENT_PACKET::RotatePacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::ROTATE));
	sendPacket.angle = angle;
	sendPacket.axis = axis;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendKeyUpPacket(DIRECTION d)
{
	CLIENT_PACKET::MovePacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::MOVE_KEY_UP));
	sendPacket.direction = d;
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendLoginData(char* loginId, char* pw)
{
	CLIENT_PACKET::LoginPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::LOGIN));
	ZeroMemory(sendPacket.id, 0);
	ZeroMemory(sendPacket.pw, 0);
	memcpy(sendPacket.id, loginId, strlen(loginId) + 1);
	memcpy(sendPacket.pw, pw, strlen(pw) + 1);
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendMouseStatePacket(bool LClickedButton, bool RClickedButton)
{
	CLIENT_PACKET::MouseInputPacket sendPacket(static_cast<unsigned char>(LClickedButton, RClickedButton, CLIENT_PACKET::TYPE::MOUSE_INPUT));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::Send_SkillInput_Q()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::SKILL_INPUT_Q));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::Send_SkillInput_E()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::SKILL_INPUT_E));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::Send_SkillExecute_Q(XMFLOAT3& dirOrPosition)
{
	CLIENT_PACKET::SkillAttackPacket sendPacket(dirOrPosition, static_cast<unsigned char>(CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::Send_SkillExecute_E(const XMFLOAT3& dirOrPosition)
{
	CLIENT_PACKET::SkillAttackPacket sendPacket(dirOrPosition, static_cast<unsigned char>(CLIENT_PACKET::TYPE::SKILL_EXECUTE_E));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendMatchRequestPacket()
{
	CLIENT_PACKET::MatchPacket sendPacket(g_Logic.GetMyRole(), static_cast<unsigned char>(CLIENT_PACKET::TYPE::MATCH));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendArrowAttackPacket(const XMFLOAT3& pos, const XMFLOAT3& dir, float speed)
{
	//CLIENT_PACKET::ShootingObject sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::SHOOTING_ARROW));
	//sendPacket.pos = pos;
	//sendPacket.dir = dir;
	//sendPacket.speed = speed;
	//send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendBallAttackPacket(const XMFLOAT3& pos, const XMFLOAT3& dir, float speed)
{
	//CLIENT_PACKET::ShootingObject sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::SHOOTING_BALL));
	//sendPacket.pos = pos;
	//sendPacket.dir = dir;
	//sendPacket.speed = speed;
	//send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
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
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::TEST_GAME_END));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendTestGameEndOKPacket()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::GAME_END_OK));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendOnPositionTriggerBox1()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::TRIGGER_BOX_ON));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendSkipNPCCommunicate()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::SKIP_NPC_COMMUNICATION));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendChangeStage_BOSS()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::STAGE_CHANGE_BOSS));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendCommonAttackExecute(const XMFLOAT3& attackDirection, int power)
{
	CLIENT_PACKET::PlayerCommonAttackPacket sendPacket(attackDirection, power, static_cast<unsigned char>(CLIENT_PACKET::TYPE::PLAYER_COMMON_ATTACK_EXECUTE));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);

}

void NetworkHelper::SendCommonAttackStart()
{
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::PLAYER_COMMON_ATTACK));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendPlayerPosition(XMFLOAT3& position)
{
	//CLIENT_PACKET::PlayerPositionPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::PLAYER_POSITION_STATE));
	//sendPacket.position = position;
	//sendPacket.t = std::chrono::steady_clock::now();
	//sendPacket.t += std::chrono::microseconds(g_Logic.GetDiffTime());
	//send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
}

void NetworkHelper::SendTimeSyncPacket()
{
	g_Logic.SetReadySync(false);
	CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(CLIENT_PACKET::TYPE::TIME_SYNC_REQUEST));
	send(m_clientSocket, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);
	g_Logic.SetrequestTime();
}


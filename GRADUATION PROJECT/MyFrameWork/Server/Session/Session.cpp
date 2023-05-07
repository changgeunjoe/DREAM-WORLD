#include "stdafx.h"
#include "Session.h"
#include "SessionObject/PlayerSessionObject.h"
#include "../Logic/Logic.h"

extern Logic g_logic;


Session::Session()
{
	m_id = -1;
	Initialize();
}

Session::~Session()
{
	m_id = -1;
	if (m_sessionObject != nullptr)
		delete m_sessionObject;
	m_sessionObject = nullptr;
}

void Session::Initialize()
{
	m_playerState = PLAYER_STATE::FREE;
	m_sessionObject = nullptr;
	m_prevBufferSize = 0;
	ZeroMemory(&m_exOver.m_overlap, sizeof(m_exOver.m_overlap));
}

void Session::Recv()
{
	DWORD recv_flag = 0;
	memset(&m_exOver.m_overlap, 0, sizeof(m_exOver.m_overlap));
	m_exOver.m_wsaBuf.len = MAX_BUF_SIZE - m_prevBufferSize;
	m_exOver.m_wsaBuf.buf = m_exOver.m_buffer + m_prevBufferSize;
	WSARecv(m_socket, &m_exOver.m_wsaBuf, 1, 0, &recv_flag, &m_exOver.m_overlap, 0);
}

void Session::Send(void* p)
{
	DWORD sendByte = 0;
	ExpOver* sendOverlap = new ExpOver(reinterpret_cast<char*>(p));
	WSASend(m_socket, &sendOverlap->m_wsaBuf, 1, &sendByte, 0, &sendOverlap->m_overlap, 0);
	//std::cout << "sendByte: " << sendByte << std::endl;
}

void Session::ConstructPacket(int ioByte)
{
	int remain_data = ioByte + m_prevBufferSize;
	char* p = m_exOver.m_buffer;
	while (remain_data > 1) {
		short packet_size;
		memcpy(&packet_size, p, 2);
		if ((int)packet_size <= remain_data) {
			g_logic.ProcessPacket(static_cast<int>(m_id), p);
			p = p + (int)packet_size;
			remain_data = remain_data - (int)packet_size;
		}
		else break;
	}
	m_prevBufferSize = remain_data;
	if (remain_data > 0) {
		std::memcpy(m_exOver.m_buffer, p, remain_data);
	}
	Recv();
}

void Session::RegistPlayer(SOCKET& sock, int id)
{
	m_socket = sock;
	m_id = id;
	Recv();
}

PlayerSessionObject* Session::SetPlaySessionObject(ROLE r)
{
	m_sessionObject = new PlayerSessionObject(m_id, r);
	return m_sessionObject;
}

void Session::ResetPlayerToLobbyState()
{
	m_roomId.clear();
	{
		std::lock_guard < std::mutex>lg{ m_playerStateLock };
		m_playerState = PLAYER_STATE::IN_GAME;
	}
	delete m_sessionObject;
	m_sessionObject = nullptr;
}

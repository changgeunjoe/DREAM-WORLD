#include "stdafx.h"
#include "PlayerSessionObject.h"
#include "../Session.h"
#include "../../Logic/Logic.h"

extern  Logic g_logic;

PlayerSessionObject::PlayerSessionObject(Session* session, SOCKET& sock) : SessionObject(session), m_socket(sock)
{
	m_level = 0;
	m_exp = 0;
	m_hp = 0;
	m_maxHp = 0;
	m_attackDamage = 0;
	m_prevBufferSize = 0;
	ZeroMemory(&m_exOver.m_overlap, sizeof(m_exOver.m_overlap));
}

PlayerSessionObject::~PlayerSessionObject()
{
}

void PlayerSessionObject::Recv()
{
	DWORD recv_flag = 0;
	memset(&m_exOver.m_overlap, 0, sizeof(m_exOver.m_overlap));
	m_exOver.m_wsaBuf.len = MAX_BUF_SIZE - m_prevBufferSize;
	m_exOver.m_wsaBuf.buf = m_exOver.m_buffer + m_prevBufferSize;
	WSARecv(m_socket, &m_exOver.m_wsaBuf, 1, 0, &recv_flag, &m_exOver.m_overlap, 0);
}

void PlayerSessionObject::Send(void* p)
{
	ExpOver* sendOverlap = new ExpOver(reinterpret_cast<char*>(p));
	WSASend(m_socket, &sendOverlap->m_wsaBuf, 1, 0, 0, &sendOverlap->m_overlap, 0);
}

void PlayerSessionObject::ConstructPacket(int ioByte)
{
	int remain_data = ioByte + m_prevBufferSize;
	char* p = m_exOver.m_buffer;
	while (remain_data > 0) {
		int packet_size = p[0];
		if (packet_size <= remain_data) {
			g_logic.ProcessPacket(static_cast<int>(m_session->GetId()), p);
			p = p + packet_size;
			remain_data = remain_data - packet_size;
		}
		else break;
	}
	m_prevBufferSize = remain_data;
	if (remain_data > 0) {
		std::memcpy(m_exOver.m_buffer, p, remain_data);
	}
	Recv();
}

#include "stdafx.h"
#include "ExpOver.h"
#include "../PacketManager/PacketManager.h"
#include "ExpOverManager.h"

void RecvExpOverBuffer::DoRecv(SOCKET& socket)
{
	DWORD recv_flag = 0;
	ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));
	m_wsaBuf.len = MAX_BUF_SIZE - m_remainData;
	m_wsaBuf.buf = m_buffer + m_remainData;
	WSARecv(socket, &m_wsaBuf, 1, 0, &recv_flag, &m_overlapped, 0);
}

void RecvExpOverBuffer::RecvPacket(const int& id, const int& roomId, const unsigned long& ioByte)
{
	m_remainData = PacketManager::ProccessPacket(id, roomId, ioByte, m_remainData, m_buffer);
}

void RecvExpOverBuffer::Clear()
{
	ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));
	m_remainData = 0;
	m_wsaBuf.len = MAX_BUF_SIZE;
	m_wsaBuf.buf = m_buffer;
}

void ExpOverWsaBuffer::DoSend(SOCKET& socket)
{
	WSASend(socket, &m_wsaBuf, 1, 0, 0, &m_overlapped, 0);
}


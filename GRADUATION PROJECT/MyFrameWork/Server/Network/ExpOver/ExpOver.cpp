#include "stdafx.h"
#include "ExpOver.h"
#include "../IocpEvent/IocpEventBase.h"

ExpOver::~ExpOver()
{
}

void ExpOver::Execute(const DWORD& ioByte, const ULONG_PTR& key)
{
	m_iocpEvent->Execute(this, ioByte, key);
}

//void RecvExpOverBuffer::DoRecv(SOCKET& socket)
//{
//	DWORD recv_flag = 0;
//	ZeroMemory(this, sizeof(WSAOVERLAPPED));
//	m_wsaBuf.len = MAX_BUF_SIZE - m_remainData;
//	m_wsaBuf.buf = m_buffer + m_remainData;
//	WSARecv(socket, &m_wsaBuf, 1, 0, &recv_flag, this, 0);
//}
//
//void RecvExpOverBuffer::RecvPacket(const int& id, const int& roomId, const unsigned long& ioByte)
//{
//	m_remainData = PacketManager::ProccessPacket(id, roomId, ioByte, m_remainData, m_buffer);
//}
//
//void RecvExpOverBuffer::Clear()
//{
//	ZeroMemory(this, sizeof(WSAOVERLAPPED));
//	m_remainData = 0;
//	m_wsaBuf.len = MAX_BUF_SIZE;
//	m_wsaBuf.buf = m_buffer;
//}
//
//void RecvExpOverBuffer::Execute(ULONG_PTR& completionKey)
//{
//	//UserManager::GetInstance().RecvPacket(completionKey, ioByte);
//}
//
//void ExpOverWsaBuffer::DoSend(SOCKET& socket)
//{
//	WSASend(socket, &m_wsaBuf, 1, 0, 0, this, 0);
//}
//
//void ExpOverWsaBuffer::Execute(ULONG_PTR& completionKey)
//{
//	ExpOverManager::GetInstance().DeleteExpOver(this);
//}
//
//void ExpOverSocket::Execute(ULONG_PTR& completionKey)
//{
//	//UserSession俊 家南 殿废
//	UserManager::GetInstance().AcceptPlayer(IOCP::m_hIocp, std::move(m_socket));
//	//货 家南 积己
//	m_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//	ResetOverlapped();
//	int addr_size = sizeof(SOCKADDR_IN);
//	AcceptEx(completionKey, m_socket, m_buffer, 0, addr_size + 16, addr_size + 16, 0, this);
//}
//
//void ExpOver::Execute(ULONG_PTR& completionKey)
//{
//	ExpOverManager::GetInstance().DeleteExpOver(this);
//}
//
//void ExpOverBuffer::Execute(ULONG_PTR& completionKey)
//{
//	ExpOverManager::GetInstance().DeleteExpOver(this);
//}



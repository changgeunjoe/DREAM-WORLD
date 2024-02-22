#include "stdafx.h"
#include "SendEvent.h"
#include "../ExpOver/ExpOver.h"
#include "../protocol/protocol.h"
#include "../IocpEvent/IocpEventManager.h"

IOCP::SendEvent::SendEvent()
{
	ZeroMemory(m_buffer, MAX_SEND_BUF_SIZE);
	m_wsabuf.buf = m_buffer;
	m_wsabuf.len = 0;
}


void IOCP::SendEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	//over��ü ��ȯ
	IocpEventManager::GetInstance().DeleteExpOver(over);
	//IocpEvent��ü ��ȯ
	//IOCP::EventBase�� enable_shared_from_this<>����̱⶧����, shared_from_this()�� shared_ptr<IOCP::EventBase>�� ��.
	IocpEventManager::GetInstance().DeleteSendEvent(std::static_pointer_cast<IOCP::SendEvent>(shared_from_this() ) );
}

void IOCP::SendEvent::Send(SOCKET sock, ExpOver* over, const PacketHeader* packetHeader)
{
	errno_t errorCode = memcpy_s(m_buffer, MAX_SEND_BUF_SIZE, packetHeader, packetHeader->size);
	if (!errorCode)//memcpy�� �߸��ƴٸ�
		assert(errorCode);
	int sendRes = WSASend(sock, &m_wsabuf, 1, nullptr, 0, over, nullptr);
	//send Error
	if (!sendRes) {
		int errorInfo = WSAGetLastError();
		if (errorInfo != WSA_IO_PENDING) {
			//error...
		}
	}
}

#include "stdafx.h"
#include "IocpSendEvent.h"
#include "../ExpOver/ExpOver.h"
#include "../protocol/protocol.h"
#include "../IocpEvent/IocpEventManager.h"

IocpSendEvent::IocpSendEvent()
{
	ZeroMemory(m_buffer, MAX_BUF_SIZE);
	m_wsabuf.buf = m_buffer;
	m_wsabuf.len = 0;
}


void IocpSendEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	//over°´Ã¼ ¹ÝÈ¯
	IocpEventManager::GetInstance().DeleteExpOver(over);
	//IocpEvent°´Ã¼ ¹ÝÈ¯
	IocpEventManager::GetInstance().DeleteSendEvent(this);
}

void IocpSendEvent::Send(SOCKET sock, ExpOver* over, const PacketHeader* packetHeader)
{
	errno_t errorCode = memcpy_s(m_buffer, MAX_BUF_SIZE, packetHeader, packetHeader->size);
	if (!errorCode)//memcpy°¡ Àß¸øµÆ´Ù¸é
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

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
	const auto& currentOpCode = over->GetOpCode();
	//IocpEvent객체 반환
	//IOCP::EventBase가 enable_shared_from_this<>상속이기때문에, shared_from_this()는 shared_ptr<IOCP::EventBase>가 됨.
	IocpEventManager::GetInstance().DeleteSendEvent(std::static_pointer_cast<IOCP::SendEvent>(shared_from_this()));
	//over객체 반환
	IocpEventManager::GetInstance().DeleteExpOver(over);
}

void IOCP::SendEvent::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	spdlog::info("IOCP::SendEvent::Fail()");
	IocpEventManager::GetInstance().DeleteExpOver(over);
}

void IOCP::SendEvent::Send(SOCKET sock, ExpOver* over, const PacketHeader* packetHeader)
{
	m_wsabuf.len = packetHeader->size;
	errno_t errorCode = memcpy_s(m_buffer, MAX_SEND_BUF_SIZE, packetHeader, packetHeader->size);
	if (errorCode)//memcpy가 잘못됐다면
		assert(errorCode);
	int sendRes = WSASend(sock, &m_wsabuf, 1, nullptr, 0, over, nullptr);
	//send Error
	if (SOCKET_ERROR == sendRes) {
		int errorInfo = WSAGetLastError();
		if (errorInfo != WSA_IO_PENDING) {
			//error...
		}
	}
}

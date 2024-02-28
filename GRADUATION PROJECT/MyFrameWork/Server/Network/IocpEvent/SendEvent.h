#pragma once
#include "IocpEventBase.h"

class PacketHeader;
namespace IOCP
{
	class SendEvent : public IOCP::EventBase
	{
	public:
		SendEvent();
	public:
		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
		virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

		void Send(SOCKET sock, ExpOver* over, const PacketHeader* packetHeader);
	protected:
		char m_buffer[MAX_SEND_BUF_SIZE];
		WSABUF m_wsabuf;
	};
}


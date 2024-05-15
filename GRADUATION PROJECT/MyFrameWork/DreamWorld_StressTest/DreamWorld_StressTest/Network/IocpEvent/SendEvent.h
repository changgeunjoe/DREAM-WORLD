#pragma once
#include "../../PCH/stdafx.h"
#include "IocpEventBase.h"

struct PacketHeader;
namespace Network
{
	class ExpOver;
	class SendEvent : public IocpEventBase
	{
		static constexpr int MAX_SEND_BUF_SIZE = 514;
	public:
		SendEvent();
	public:
		virtual void Execute(Network::ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
		virtual void Fail(Network::ExpOver* over) override;

		void Send(SOCKET sock, Network::ExpOver* over, const PacketHeader* packetHeader);
	protected:
		char m_buffer[MAX_SEND_BUF_SIZE];
		WSABUF m_wsabuf;
	};
}


#pragma once
#include "IocpEventBase.h"

class PacketHeader;
class IocpSendEvent : public IocpEventBase
{
public:
	IocpSendEvent();
public:
	virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

	void Send(SOCKET sock, ExpOver* over, const PacketHeader* packetHeader);
protected:
	char m_buffer[MAX_BUF_SIZE];
	WSABUF m_wsabuf;
};


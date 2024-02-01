#pragma once
#include "../../SingletonBase.h"


class ExpOver;

class IocpEventBase;

class PacketHeader;
class IocpSendEvent;

class IocpEventManager : public SingletonBase< IocpEventManager>
{
	friend SingletonBase;
	//send, DB, Event overlapped包访 磊丰备炼 包府
private:
	IocpEventManager();
	~IocpEventManager();

public:
	void Initailize();

private:
	IocpSendEvent* CreateSendEvent();
public:
	ExpOver* CreateExpOver(const IOCP_OP_CODE& opCode, IocpEventBase* iocpEvent);
	void Send(SOCKET sock, const PacketHeader* packetHeader);

	void DeleteExpOver(ExpOver* delOver);
	void DeleteSendEvent(IocpSendEvent* delEvent);

private:
	tbb::concurrent_queue<ExpOver*> m_expOverQueue;

	tbb::concurrent_queue<IocpEventBase*> m_eventQueue;
	//tbb::concurrent_queue<IocpBufferEvent*> m_bufferEventQueue;
	tbb::concurrent_queue<IocpSendEvent*> m_sendEventQueue;
};

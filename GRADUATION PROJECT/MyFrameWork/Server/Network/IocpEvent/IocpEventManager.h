#pragma once
#include "../../SingletonBase.h"


class ExpOver;

namespace IOCP
{
	class EventBase;
	class SendEvent;
	class RoomEvent;
}
struct PacketHeader;

class IocpEventManager : public SingletonBase<IocpEventManager>
{
	friend SingletonBase;
	//send, DB, Event overlapped包访 磊丰备炼 包府
private:
	IocpEventManager();
	~IocpEventManager();

public:
	void Initailize();

private:
	std::shared_ptr<IOCP::SendEvent> CreateSendEvent();
public:
	ExpOver* CreateExpOver(const IOCP_OP_CODE& opCode, std::shared_ptr<IOCP::EventBase> iocpEvent);
	void Send(SOCKET sock, const PacketHeader* packetHeader);

	void DeleteExpOver(ExpOver* delOver);
	void DeleteSendEvent(std::shared_ptr<IOCP::SendEvent> delEvent);

private:
	tbb::concurrent_queue<ExpOver*> m_expOverQueue;

	//tbb::concurrent_queue<std::shared_ptr<IOCP::EventBase>> m_eventQueue;
	//tbb::concurrent_queue<IocpBufferEvent*> m_bufferEventQueue;
	tbb::concurrent_queue<std::shared_ptr<IOCP::SendEvent>> m_sendEventQueue;
};

#include "stdafx.h"
#include "IocpEventManager.h"
#include "../ExpOver/ExpOver.h"
#include "../IocpEvent/SendEvent.h"
#include "../Room/RoomEvent.h"

IocpEventManager::IocpEventManager()
{
}

IocpEventManager::~IocpEventManager()
{
	//tbb::concurrent_queue<> 소멸자에서 clear()하여 따로 처리 안해도 됨(하지만 not thread-safe)
	spdlog::info("IocpEventManager::~IocpEventManager()");
}

void IocpEventManager::Initailize()
{
	spdlog::info("IocpEventManager::Initialize");
	for (int i = 0; i < 200; ++i) {
		m_expOverQueue.push(new ExpOver());
		m_sendEventQueue.push(std::make_shared<IOCP::SendEvent>());
		//m_eventQueue.push(new IocpEventBase());
		/*m_bufferEventQueue.push(new IocpBufferEvent());
		*/
	}
}

std::shared_ptr<IOCP::SendEvent> IocpEventManager::CreateSendEvent()
{

	std::shared_ptr<IOCP::SendEvent> iocpEvent = nullptr;
	bool success = m_sendEventQueue.try_pop(iocpEvent);
	if (!success)
		iocpEvent = std::make_shared<IOCP::SendEvent>();
	return iocpEvent;
}

ExpOver* IocpEventManager::CreateExpOver(const IOCP_OP_CODE& opCode, std::shared_ptr<IOCP::EventBase> iocpEvent)
{
	ExpOver* overlapObject = nullptr;
	bool success = m_expOverQueue.try_pop(overlapObject);
	if (!success)//실패했다면 새로 생성
		overlapObject = new ExpOver(opCode, iocpEvent);
	else overlapObject->SetData(opCode, iocpEvent);

	return overlapObject;
}

void IocpEventManager::Send(SOCKET sock, const PacketHeader* packetHeader)
{
	std::shared_ptr<IOCP::SendEvent> iocpEventObject = CreateSendEvent();
	auto expOver = CreateExpOver(OP_SEND, iocpEventObject);
	iocpEventObject->Send(sock, expOver, packetHeader);
}

void IocpEventManager::DeleteExpOver(ExpOver* delOver)
{
	delOver->ResetOverlapped();
	delOver->ResetEvent();
	m_expOverQueue.push(delOver);
}

void IocpEventManager::DeleteSendEvent(std::shared_ptr<IOCP::SendEvent> delEvent)
{
	m_sendEventQueue.push(delEvent);
}

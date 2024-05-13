#include "stdafx.h"
#include "IocpEventManager.h"
#include "../ExpOver/ExpOver.h"
#include "SendEvent.h"
#include "IocpEventBase.h"


Network::IocpEventManager::IocpEventManager()
{
}

Network::IocpEventManager::~IocpEventManager()
{
	//tbb::concurrent_queue<> 소멸자에서 clear()하여 따로 처리 안해도 됨(하지만 not thread-safe)
}

void Network::IocpEventManager::Initailize()
{
	for (int i = 0; i < 200; ++i) {
		m_expOverQueue.push(new Network::ExpOver());
		m_sendEventQueue.push(std::make_shared<Network::SendEvent>());
		//m_eventQueue.push(new IocpEventBase());
		/*m_bufferEventQueue.push(new IocpBufferEvent());
		*/
	}
}

std::shared_ptr<Network::SendEvent> Network::IocpEventManager::CreateSendEvent()
{

	std::shared_ptr<Network::SendEvent> iocpEvent = nullptr;
	bool success = m_sendEventQueue.try_pop(iocpEvent);
	if (!success)
		iocpEvent = std::make_shared<Network::SendEvent>();
	return iocpEvent;
}

Network::ExpOver* Network::IocpEventManager::CreateExpOver(const IOCP_OP_CODE& opCode, std::shared_ptr<Network::IocpEventBase> iocpEvent)
{
	ExpOver* overlapObject = nullptr;
	bool success = m_expOverQueue.try_pop(overlapObject);
	if (!success)//실패했다면 새로 생성
		overlapObject = new ExpOver(opCode, iocpEvent);
	else overlapObject->SetData(opCode, iocpEvent);

	return overlapObject;
}

void Network::IocpEventManager::Send(SOCKET sock, const PacketHeader* packetHeader)
{
	std::shared_ptr<Network::SendEvent> iocpEventObject = CreateSendEvent();
	auto expOver = CreateExpOver(IOCP_OP_CODE::OP_SEND, iocpEventObject);
	iocpEventObject->Send(sock, expOver, packetHeader);
}

void Network::IocpEventManager::DeleteExpOver(ExpOver* delOver)
{
	delOver->ResetData();
	m_expOverQueue.push(delOver);
}

void Network::IocpEventManager::DeleteSendEvent(std::shared_ptr<Network::SendEvent> delEvent)
{
	m_sendEventQueue.push(delEvent);
}

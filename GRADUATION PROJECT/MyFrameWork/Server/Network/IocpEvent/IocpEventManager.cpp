#include "stdafx.h"
#include "IocpEventManager.h"
#include "../ExpOver/ExpOver.h"
#include "../IocpEvent/IocpSendEvent.h"

IocpEventManager::IocpEventManager()
{
}

IocpEventManager::~IocpEventManager()
{
	//tbb::concurrent_queue<> �Ҹ��ڿ��� clear()�Ͽ� ���� ó�� ���ص� ��(������ not thread-safe)
}

void IocpEventManager::Initailize()
{
	for (int i = 0; i < 200; ++i) {
		m_expOverQueue.push(new ExpOver());
		m_sendEventQueue.push(new IocpSendEvent());
		//m_eventQueue.push(new IocpEventBase());
		/*m_bufferEventQueue.push(new IocpBufferEvent());
		*/
	}
}

IocpSendEvent* IocpEventManager::CreateSendEvent()
{
	IocpSendEvent* iocpEvent = nullptr;
	bool success = m_sendEventQueue.try_pop(iocpEvent);
	if (!success)
		iocpEvent = new IocpSendEvent();
	return iocpEvent;
}

ExpOver* IocpEventManager::CreateExpOver(const IOCP_OP_CODE& opCode, IocpEventBase* iocpEvent)
{
	ExpOver* overlapObject = nullptr;
	bool success = m_expOverQueue.try_pop(overlapObject);
	if (!success)//�����ߴٸ� ���� ����
		overlapObject = new ExpOver(opCode, iocpEvent);
	else overlapObject->SetData(opCode, iocpEvent);

	return overlapObject;
}

void IocpEventManager::Send(SOCKET sock, const PacketHeader* packetHeader)
{
	IocpSendEvent* iocpEventObject = CreateSendEvent();
	auto expOver = CreateExpOver(OP_SEND, iocpEventObject);
	iocpEventObject->Send(sock, expOver, packetHeader);
}

void IocpEventManager::DeleteExpOver(ExpOver* delOver)
{
	delOver->ResetOverlapped();
	delOver->ResetEvent();
	m_expOverQueue.push(delOver);
}

void IocpEventManager::DeleteSendEvent(IocpSendEvent* delEvent)
{
	m_sendEventQueue.push(delEvent);
}

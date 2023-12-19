#include "stdafx.h"
#include "ExpOverManager.h"
#include "ExpOver.h"

ExpOverManager::ExpOverManager()
{
}

ExpOverManager::~ExpOverManager()
{
}

void ExpOverManager::Initailize()
{
	for (int i = 0; i < 200; ++i) {
		m_expOverQueue.push(new ExpOver(OP_NONE));
		m_bufferQueue.push(new ExpOverBuffer(OP_NONE));
		m_wsaBufferQueue.push(new ExpOverWsaBuffer(OP_NONE));
	}
}

ExpOver* ExpOverManager::CreateExpOver(const IOCP_OP_CODE&& opCode)
{
	ExpOver* over = nullptr;
	bool isSuccess = m_expOverQueue.try_pop(over);
	if (!isSuccess)
		return new ExpOver(opCode);
	over->ResetOverlapped(opCode);
	return over;
}

ExpOver* ExpOverManager::CreateExpOverBuffer(const IOCP_OP_CODE&& opCode, char* data)
{
	ExpOverBuffer* over = nullptr;
	bool isSuccess = m_bufferQueue.try_pop(over);
	if (!isSuccess)
		return new ExpOverBuffer(opCode, data);
	over->ResetOverlapped(opCode);
	over->SetData(data);
	return over;
}

ExpOver* ExpOverManager::CreateExpOverBuffer(const IOCP_OP_CODE&& opCode, char* data, int dataSize)
{
	ExpOverBuffer* over = nullptr;
	bool isSuccess = m_bufferQueue.try_pop(over);
	if (!isSuccess)
		return new ExpOverBuffer(opCode, data, dataSize);
	over->ResetOverlapped(opCode);
	over->SetData(data, dataSize);
	return over;
}

ExpOverWsaBuffer* ExpOverManager::CreateExpOverWsaBuffer(const IOCP_OP_CODE&& opCode, const char* data)
{
	ExpOverWsaBuffer* over = nullptr;
	bool isSuccess = m_wsaBufferQueue.try_pop(over);
	if (!isSuccess)
		return new ExpOverWsaBuffer(opCode, data);
	over->ResetOverlapped(opCode);
	over->SetData(data);
	return over;
}

void ExpOverManager::DeleteExpOver(ExpOver* delExpOver)
{
	if (delExpOver->GetOpCode() == OP_ACCEPT || delExpOver->GetOpCode() == OP_RECV) return;
	m_expOverQueue.push(delExpOver);
}

void ExpOverManager::DeleteExpOverBuffer(ExpOverBuffer* delExpOver)
{
	if (delExpOver->GetOpCode() == OP_ACCEPT || delExpOver->GetOpCode() == OP_RECV) return;
	m_bufferQueue.push(delExpOver);
}

void ExpOverManager::DeleteExpOverWsaBuffer(ExpOverWsaBuffer* delExpOver)
{
	if (delExpOver->GetOpCode() == OP_ACCEPT || delExpOver->GetOpCode() == OP_RECV) return;
	m_wsaBufferQueue.push(delExpOver);
}

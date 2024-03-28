#include "stdafx.h"
#include "IOCP.h"
#include "../UserSession/UserManager.h"
#include "../ExpOver/ExpOver.h"
#include "../IocpEvent/ListenEvent.h"
#include "../ThreadManager/ThreadManager.h"


IOCP::Iocp::Iocp() : m_listener(nullptr)
{
	////iocp객체 생성 - 이때 맨 마지막 인자가 0으로 설정하면 최대 프로세서 수만큼 스레드 허용
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	spdlog::info("Iocp::Iocp() - Create Iocp Handle");
}

IOCP::Iocp::~Iocp()
{

}

void IOCP::Iocp::Start()
{
	int threadNum = std::thread::hardware_concurrency();
	//thread 생성
	ThreadManager& thMgrRef = ThreadManager::GetInstance();
	spdlog::info("Iocp::Start() - Start Worker Threads");
	for (int i = 0; i < threadNum; ++i)
		thMgrRef.CreateThread(std::thread([this]() {WorkerThread(); }));

	m_listener = std::make_shared<IOCP::ListenEvent>(shared_from_this());
	m_listener->StartListen(PORT);

	spdlog::info("IOCP::Iocp::Start() - Accept Start");
	m_listener->Accept();
}

void IOCP::Iocp::WorkerThread()
{
	while (true)
	{
		DWORD ioByte;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(m_hIocp, &ioByte, &key, &over, INFINITE);
		ExpOver* exOver = reinterpret_cast<ExpOver*>(over);
		exOver->Execute(ret, ioByte, key);
	}
}

void IOCP::Iocp::RegistHandle(HANDLE registHandle, int key)
{
	HANDLE retVal = CreateIoCompletionPort(registHandle, m_hIocp, key, 0);
	if (NULL == retVal) {
		//IocpHandle 오류
		spdlog::critical("Iocp::RegistHandle() - ErrorCode: {0}, key: {1}", GetLastError(), key);
	}
}

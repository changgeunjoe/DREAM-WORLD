#include "stdafx.h"
#include "IOCP.h"
#include "../UserSession/UserManager.h"
#include "../ExpOver/ExpOver.h"
#include "../IocpEvent/ListenEvent.h"


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
	for (int i = 0; i < threadNum; ++i)
		m_workerThread.emplace_back([this]() {WorkerThread(); });
	spdlog::info("Iocp::Start() - Start Worker Threads");

	m_listener = std::make_shared<IOCP::ListenEvent>(shared_from_this());
	m_listener->StartListen(PORT);

	spdlog::info("IOCP::Iocp::Start() - Accept Start");
	m_listener->Accept();
}

void IOCP::Iocp::WorkerThreadJoin()
{
	for (auto& th : m_workerThread)
		th.join();
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

		if (FALSE == ret) {
			//if (exOver->GetOpCode() == IOCP_OP_CODE::OP_ACCEPT) {
				//spdlog::info("Accept Error");

				//continue;
			//}
			//spdlog::warn("GQCS Error on client[{0:d}]", key);

			//DisconnectClient(static_cast<int>(key));
			//ExpOverManager::GetInstance().DeleteExpOver(ex_over);
			continue;
		}
		exOver->Execute(ioByte, key);
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

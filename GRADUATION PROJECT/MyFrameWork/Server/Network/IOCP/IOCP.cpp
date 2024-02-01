#include "stdafx.h"
#include "IOCP.h"
#include "../UserSession/UserManager.h"
#include "../ExpOver/ExpOver.h"


Iocp::Iocp()
{
	////iocp��ü ���� - �̶� �� ������ ���ڰ� 0���� �����ϸ� �ִ� ���μ��� ����ŭ ������ ���
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	spdlog::info("Iocp::Iocp() - Create Iocp Handle");
}

Iocp::~Iocp()
{
	Iocp::Destroy();
}

void Iocp::StartWorkerThread()
{
	int threadNum = std::thread::hardware_concurrency();
	//thread ����
	for (int i = 0; i < threadNum; ++i)
		m_workerThread.emplace_back([this]() {WorkerThread(); });
	spdlog::info("Iocp::StartWorkerThread() - Start Worker Tthread");
}

void Iocp::Destroy()
{
	for (auto& th : m_workerThread)
		th.join();
}

void Iocp::WorkerThread()
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

void Iocp::RegistHandle(HANDLE registHandle, int key)
{
	CreateIoCompletionPort(registHandle, m_hIocp, key, 0);
}

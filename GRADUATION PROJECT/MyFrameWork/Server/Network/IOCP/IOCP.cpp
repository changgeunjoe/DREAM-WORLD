#include "stdafx.h"
#include "IOCP.h"

#include "../UserSession/UserManager.h"
#include "../ExpOver/ExpOverManager.h"
#include "../ExpOver/ExpOver.h"
#include "../protocol/protocol.h"


IOCP::IOCP()
{
	Initialize();
	m_acceptOver = new ExpOver(OP_ACCEPT);
}

IOCP::~IOCP()
{
	IOCP::Destroy();
	delete m_acceptOver;
}

void IOCP::Initialize()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "wsaStartUp Error" << std::endl;
		//WSACleanup();
		//return -1;
	}
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	//listen Socket ���ε�
	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	bind(m_listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(m_listenSocket, SOMAXCONN);
	//iocp��ü ���� - �̶� �� ������ ���ڰ� 0���� �����ϸ� �ִ� ���μ��� ����ŭ ������ ���
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	//���� ���� iocp��ü�� ���
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_hIocp, 9999, 0);

	spdlog::info("IOCP::Initialize()");
}

void IOCP::Start()
{
	int threadNum = std::thread::hardware_concurrency();
	//thread ����
	for (int i = 0; i < threadNum; ++i)
		m_workerThread.emplace_back([this]() {WorkerThread(); });

	//accept�� ���� ����
	int addr_size = sizeof(SOCKADDR_IN);
	m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(m_acceptBuffer, MAX_BUF_SIZE);

	AcceptEx(m_listenSocket, m_clientSocket, m_acceptBuffer, 0, addr_size + 16, addr_size + 16, 0, reinterpret_cast<WSAOVERLAPPED*>(m_acceptOver));
}

void IOCP::Destroy()
{
	for (auto& th : m_workerThread)
		th.join();
}

void IOCP::WorkerThread()
{
	while (true)
	{
		DWORD ioByte;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(m_hIocp, &ioByte, &key, &over, INFINITE);
		ExpOver* ex_over = reinterpret_cast<ExpOver*>(over);
		IOCP_OP_CODE currentOpCode = ex_over->GetOpCode();
		if (FALSE == ret) {
			if (ex_over->GetOpCode() == OP_ACCEPT) {
				spdlog::info("Accept Error");
			}
			else {
				spdlog::warn("GQCS Error on client{0:d}]", key);
				//DisconnectClient(static_cast<int>(key));
				ExpOverManager::GetInstance().DeleteExpOver(ex_over);
				continue;
			}
		}
		switch (currentOpCode)
		{
		case OP_ACCEPT:
		{
			//UserSession�� ���� ���
			UserManager::GetInstance().AcceptPlayer(m_hIocp, std::move(m_clientSocket));
			//�� ���� ����
			m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			m_acceptOver->ResetOverlapped();
			int addr_size = sizeof(SOCKADDR_IN);
			//�ٽ� accept
			AcceptEx(m_listenSocket, m_clientSocket, m_acceptBuffer, 0, addr_size + 16, addr_size + 16, 0, reinterpret_cast<WSAOVERLAPPED*>(m_acceptOver));
		}
		break;
		case OP_RECV:
		{
			UserManager::GetInstance().RecvPacket(key, ioByte);
		}
		break;
		case OP_SEND:
		{
			ExpOverManager::GetInstance().DeleteExpOverWsaBuffer(reinterpret_cast<ExpOverWsaBuffer*>(ex_over));
		}
		break;
		//Timer Event Boss
		default: break;
		}
	}
}

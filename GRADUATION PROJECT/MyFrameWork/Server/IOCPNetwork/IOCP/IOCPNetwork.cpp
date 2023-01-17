#include "stdafx.h"
#include "IOCPNetwork.h"
#include "../../Logic/Logic.h"
#include "../../Session/ExpOver.h"

extern  Logic g_logic;

IOCPNetwork::IOCPNetwork()
{
	b_isRunning = false;
	m_currentClientId = 0;
	Initialize();
}

IOCPNetwork::~IOCPNetwork()
{
	IOCPNetwork::Destroy();
}

void IOCPNetwork::Initialize()
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

	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	bind(m_listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(m_listenSocket, SOMAXCONN);
}

void IOCPNetwork::Start()
{
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_hIocp, 9999, 0);

	m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	m_acceptOver = new ExpOver();
	m_acceptOver->m_opCode = OP_ACCEPT;
	AcceptEx(m_listenSocket, m_clientSocket, m_acceptOver->m_buffer, 0, addr_size + 16, addr_size + 16, 0, &m_acceptOver->m_overlap);

	int threadNum = std::thread::hardware_concurrency();
	b_isRunning = true;
	for (int i = 0; i < threadNum; ++i)
		m_workerThread.emplace_back([this]() {WorkerThread(); });
}

void IOCPNetwork::Destroy()
{
	b_isRunning = false;
	for (auto& th : m_workerThread)
		if (th.joinable())
			th.join();
	delete m_acceptOver;
}

void IOCPNetwork::WorkerThread()
{
	while (b_isRunning)
	{
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(m_hIocp, &num_bytes, &key, &over, INFINITE);
		ExpOver* ex_over = reinterpret_cast<ExpOver*>(over);
		if (FALSE == ret) {
			if (ex_over->m_opCode == OP_ACCEPT) {
				std::cout << "Accept Error" << std::endl;
			}
			else {
				std::cout << "GQCS Error on client[" << key << "]" << std::endl;
				//disconnect(static_cast<int>(key));
				if (ex_over->m_opCode == OP_SEND) delete ex_over;
				continue;
			}
		}
		switch (ex_over->m_opCode) {
		case OP_ACCEPT:
		{
			int addr_size = sizeof(SOCKADDR_IN);
			int userId = GetUserId();
			if (userId == -1) {
				std::cout << "Max user" << std::endl;
			}
			else {
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_clientSocket), m_hIocp, userId, 0);
				g_logic.AcceptPlayer(&m_session[userId], userId, m_clientSocket);
				m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				ZeroMemory(&m_acceptOver->m_overlap, sizeof(m_acceptOver->m_overlap));
				AcceptEx(m_listenSocket, m_clientSocket, m_acceptOver->m_buffer, 0, addr_size + 16, addr_size + 16, 0, &m_acceptOver->m_overlap);
			}
		}
		break;
		case OP_RECV:
		{
			
		}
		break;
		case OP_SEND:
		{
			if (ex_over != nullptr)
				delete ex_over;
		}
		break;
		}
		break;
	}
}

int IOCPNetwork::GetUserId()
{
	if (m_currentClientId < MAX_USER) return ++m_currentClientId;
	return -1;
}

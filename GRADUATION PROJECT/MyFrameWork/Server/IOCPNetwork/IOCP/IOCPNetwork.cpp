#include "stdafx.h"
#include "IOCPNetwork.h"
#include "../../Session/ExpOver.h"

IOCPNetwork::IOCPNetwork()
{
	b_isRunning = false;
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
		//cout << "wsaStartUp Error" << endl;
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
	m_acceptOver->m_opCode = ACCEPT;
	AcceptEx(m_listenSocket, m_clientSocket, m_acceptOver->m_buffer, 0, addr_size + 16, addr_size + 16, 0, &m_acceptOver->m_overlap);

	int threadNum = std::thread::hardware_concurrency();
	b_isRunning = true;
	for (int i = 0; i < threadNum; ++i)
		m_workerThread.emplace_back([this]() {WorkerThread(); });
}

void IOCPNetwork::Destroy()
{
	delete m_acceptOver;
	b_isRunning = false;
	for (auto& th : m_workerThread)
		if (th.joinable())
			th.join();
}

void IOCPNetwork::WorkerThread()
{
	while (b_isRunning)
	{
		while (true) {
			DWORD num_bytes;
			ULONG_PTR key;
			WSAOVERLAPPED* over = nullptr;
			BOOL ret = GetQueuedCompletionStatus(m_hIocp, &num_bytes, &key, &over, INFINITE);
			ExpOver* ex_over = reinterpret_cast<ExpOver*>(over);
			if (FALSE == ret) {
				if (ex_over->m_opCode == OP_ACCEPT) {
					//cout << "Accept Error";
				}
				else {
					//cout << "GQCS Error on client[" << key << "]\n";
					//disconnect(static_cast<int>(key));
					if (ex_over->m_opCode == OP_SEND) delete ex_over;
					continue;
				}
			}
			switch (ex_over->m_opCode) {
			case OP_ACCEPT: {
				int client_id = get_new_client_id();
				if (client_id != -1) {
					{
						lock_guard<mutex> ll(clients[client_id]._s_lock);
						clients[client_id]._state = ST_ALLOC;
					}
					clients[client_id].x = 0;
					clients[client_id].y = 0;
					clients[client_id]._id = client_id;
					clients[client_id]._name[0] = 0;
					clients[client_id]._prev_remain = 0;
					clients[client_id]._view_list.clear();
					clients[client_id]._socket = clientSocket;
					CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocpHandle, client_id, 0);
					clients[client_id].do_recv();
					clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				}
				else {
					cout << "Max user exceeded.\n";
				}
				ZeroMemory(&acceptOver._over, sizeof(acceptOver._over));
				int addr_size = sizeof(SOCKADDR_IN);
				AcceptEx(listenSocket, clientSocket, acceptOver._send_buf, 0, addr_size + 16, addr_size + 16, 0, &acceptOver._over);
				break;
			}
			case OP_RECV: {
				int remain_data = num_bytes + clients[key]._prev_remain;
				char* p = ex_over->_send_buf;
				while (remain_data > 0) {
					int packet_size = p[0];
					if (packet_size <= remain_data) {
						process_packet(static_cast<int>(key), p);
						p = p + packet_size;
						remain_data = remain_data - packet_size;
					}
					else break;
				}
				clients[key]._prev_remain = remain_data;
				if (remain_data > 0) {
					std::memcpy(ex_over->_send_buf, p, remain_data);
				}
				clients[key].do_recv();
				break;
			}
			case OP_SEND:
				delete ex_over;
				break;
			}
			break;
		}
	}
}
}

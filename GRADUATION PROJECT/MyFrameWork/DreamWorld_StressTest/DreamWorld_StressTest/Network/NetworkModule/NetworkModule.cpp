#include "stdafx.h"
#include "NetworkModule.h"
#include "../ExpOver/ExpOver.h"
#include "../UserManager/UserManager.h"
#include "../UserSession/UserSession.h"

void Network::NetworkModule::InitializeNetwork()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, MAX_THREAD_NUM);

	for (int i = 0; i < MAX_THREAD_NUM; ++i)
		m_workerThread.emplace_back([this]() {WorkerThread(); });
}

void Network::NetworkModule::RegistHandle(HANDLE& registHandle)
{
	CreateIoCompletionPort(registHandle, m_iocpHandle, reinterpret_cast<ULONG_PTR>(registHandle), 0);
}

void Network::NetworkModule::WorkerThread()
{
	while (true) {
		DWORD ioByte;
		ULONG_PTR key;
		ExpOver* overlapped;
		bool success = GetQueuedCompletionStatus(m_iocpHandle, &ioByte, &key, reinterpret_cast<LPOVERLAPPED*>(&overlapped), INFINITE);
		overlapped->Execute(success, ioByte, key);
	}
}

void DreamWorld::StressTestNetwork::InitializeNetwork()
{
	NetworkModule::InitializeNetwork();
	Network::UserManager::GetInstance().Initialize();

	m_lastConnectTime = Time::now();
	m_connections = 0;
	globalMaxDelay = 0;
	m_connectThread = std::thread([this]() { ConnectThread(); });
}

void DreamWorld::StressTestNetwork::IncreaseActiveClient()
{
	m_activeClientNum++;
}

void DreamWorld::StressTestNetwork::DisconnectClient()
{
	m_activeClientNum--;
	m_connections--;
}

void DreamWorld::StressTestNetwork::ConnectThread()
{
	while (true) {
		AdjustClientNumber();
		Network::UserManager::GetInstance().RunActiveClient(m_activeClientNum);
	}
}

void DreamWorld::StressTestNetwork::AdjustClientNumber()
{
	static int DELAY_MULTIPLIER = 1;
	static int MAX_LIMIT = MAXINT;
	static bool CONNECTION_INCREASING = true;

	static constexpr MS DELAY_LIMIT = MS(100);
	static constexpr MS DELAY_LIMIT2 = MS(150);
	static constexpr MS CONNECTION_DELAY = MS(50);

	if (m_activeClientNum >= MAX_TEST) return;
	if (m_connections >= MAX_CLIENTS) return;

	auto nowTime = Time::now();
	auto lastConnectDurationTime = std::chrono::duration_cast<MS>(nowTime - m_lastConnectTime);

	//(������ Ŀ��Ʈ �ð� - ���� �ð�) < Ŀ��Ʈ ������ => �ʹ� ������ connect �ɸ� ����?
	if (CONNECTION_DELAY * DELAY_MULTIPLIER > lastConnectDurationTime) return;

	//����� Ŭ���̾�Ʈ ��� ������ �ð�
	long long averageDelay = globalDelay;
	if (m_activeClientNum != 0)
		averageDelay /= m_activeClientNum;

	if (DELAY_LIMIT2.count() < averageDelay) {//��� �����̰� �ʹ� Ŭ ��
		if (CONNECTION_INCREASING) {//��� ������ �ð��� ���������� �̻��̶�� Ŀ�ؼ� ���� ����
			MAX_LIMIT = m_activeClientNum;
			CONNECTION_INCREASING = false;
		}
		if (m_activeClientNum < 100) return;
		if (CONNECTION_DELAY * 10 > lastConnectDurationTime) return;
		m_lastConnectTime = nowTime;
		Network::UserManager::GetInstance().ForceDisconnect(m_disconnectClientNum++);
		return;
	}
	else if (DELAY_LIMIT.count() < averageDelay) {//��� �޷��̰� �� Ŭ ��
		// Ŀ��Ʈ ������ �ð��� 10�� �ø��� ��.
		DELAY_MULTIPLIER = 10;
		return;
	}

	//���� ����� Ŭ���̾�Ʈ���� �����̰� Ŭ �� set�� MAX_LIMIT���� 80%���� Ŭ �� => �����̰� Ŭ ��, ���� �ο��� 80%���� ���
	if (float(MAX_LIMIT) * 0.8f < m_activeClientNum) return;

	CONNECTION_INCREASING = true;
	m_lastConnectTime = Time::now();
	SOCKET connectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP.data(), &serverAddr.sin_addr.s_addr);

	int connectResult = WSAConnect(connectSocket, (sockaddr*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);
	if (0 != connectResult) {
		return;
	}
	RegistHandle(reinterpret_cast<HANDLE&>(connectSocket));
	auto connectUserSession = Network::UserManager::GetInstance().GetTryConnectUserSession();
	connectUserSession->Connect(connectSocket);
	m_connections++;
}

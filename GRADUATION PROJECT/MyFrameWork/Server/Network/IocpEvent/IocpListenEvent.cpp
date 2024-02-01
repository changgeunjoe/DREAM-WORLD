#include "stdafx.h"
#include "IocpListenEvent.h"
#include "../UserSession/UserManager.h"
#include "../IOCP/Iocp.h"
#include "../IocpEvent/IocpEventManager.h"

void IocpListenEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	switch (over->GetOpCode())
	{
	case OP_ACCEPT:
	{
		UserManager::GetInstance().AcceptPlayer(std::move(m_clientSocket));
		Accept();
	}
	break;
	default:
		break;
	};
}

void IocpListenEvent::RegisterIocp(Iocp* iocpPtr)
{
	iocp = iocpPtr;
}

void IocpListenEvent::Accept()
{
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(OP_ACCEPT, this);
	m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, NULL, WSA_FLAG_OVERLAPPED);
	int addrInfoSize = sizeof(SOCKADDR_IN);
	/*
		���ð� ������ �ּ��� ũ��� �ּҰ� ���� ����(Internel format)���� ��ϵǱ� ������
		�ݵ�� ������� ���� �������� sockaddr ����ü���� 16����Ʈ �̻� Ŀ���մϴ�.
		�������, sockaddr_in(TCP/IP�� �ּ� ����)�� ũ��� 16����Ʈ�Դϴ�. ���� ,
		�����ּҿ� �����ּҿ� ���� �ּ� 32Bytes�� ����ũ�⸦ �����ؾ� �մϴ�.
	*/
	bool isSuccess = AcceptEx(m_listenSocket, m_clientSocket, &m_connInfo, 0, sizeof(m_connInfo.localInfo), sizeof(m_connInfo.remoteInfo), nullptr, expOver);
	if (!isSuccess) {
		int errorCode = WSAGetLastError();
		if (ERROR_IO_PENDING != errorCode) {
			//Error
		}
	}
}

void IocpListenEvent::StartListen(const unsigned short& port)
{
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(SOCKADDR_IN));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	//listen Socket ����, ���ε�
	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	bind(m_listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(m_listenSocket, SOMAXCONN);

	////���� ���� iocp��ü�� ���
	iocp->RegistHandle(reinterpret_cast<HANDLE>(m_listenSocket), 999999);
}


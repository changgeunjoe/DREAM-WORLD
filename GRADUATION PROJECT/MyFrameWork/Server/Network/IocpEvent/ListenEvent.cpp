#include "stdafx.h"
#include "ListenEvent.h"
#include "../UserSession/UserManager.h"
#include "../IOCP/Iocp.h"
#include "../IocpEvent/IocpEventManager.h"

IOCP::ListenEvent::ListenEvent(std::shared_ptr<IOCP::Iocp> iocp)
{
	spdlog::info("ListenEvent::ListenEvent() - set IocpRef Complete");
	iocpRef = iocp;
}

void IOCP::ListenEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	const auto& currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
	case IOCP_OP_CODE::OP_ACCEPT:
	{
		//m_connInfo로 remote 주소 정보 확인 가능

		spdlog::debug("IOCP::ListenEvent::Execute() - OP_ACCEPT, key: {}", key);
		UserManager::GetInstance().RegistPlayer(m_clientSocket);
		//새로운 클라이언트를 받기 위해 다시 Accept호출
	}
	break;
	default:
		spdlog::critical("ListenEvent::Execute() - UnDefined OP_CODE - {}", static_cast<int>(currentOpCode));
		break;
	};
	Accept();
	IocpEventManager::GetInstance().DeleteExpOver(over);
}

void IOCP::ListenEvent::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	const auto& currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
	case IOCP_OP_CODE::OP_ACCEPT:
	{
		spdlog::debug("IOCP::ListenEvent::Fail() - OP_ACCEPT");
		closesocket(m_clientSocket);
	}
	break;
	default:
		spdlog::critical("ListenEvent::Fail() - UnDefined OP_CODE - {}", static_cast<int>(currentOpCode));
		break;
	};
	Accept();
	IocpEventManager::GetInstance().DeleteExpOver(over);
}

void IOCP::ListenEvent::RegisterIocp(std::shared_ptr<IOCP::Iocp>& iocp)
{
	iocpRef = iocp;
}

void IOCP::ListenEvent::Accept()
{
	while (true) {
		auto expOver = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_ACCEPT, shared_from_this());
		m_clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, NULL, WSA_FLAG_OVERLAPPED);
		int addrInfoSize = sizeof(SOCKADDR_IN);
		/*
			로컬과 원격지 주소의 크기는 주소가 내부 형식(Internel format)으로 기록되기 때문에
			반드시 사용중인 전송 프로토콜 sockaddr 구조체보다 16바이트 이상 커야합니다.
			예를들어, sockaddr_in(TCP/IP의 주소 구조)의 크기는 16바이트입니다. 따라서,
			로컬주소와 원격주소에 대해 최소 32Bytes의 버퍼크기를 지정해야 합니다.
		*/
		bool isSuccess = AcceptEx(m_listenSocket, m_clientSocket, &m_connInfo, 0, sizeof(m_connInfo.localInfo), sizeof(m_connInfo.remoteInfo), nullptr, expOver);
		if (!isSuccess) {
			int errCode = WSAGetLastError();
			if (WSA_IO_PENDING == errCode) {
				return;
			}
			spdlog::critical("ListenEvent::Accept() - Accept Error");
			DisplayWsaGetLastError(errCode);
		}
		else {//즉시 완료
			ListenEvent::Execute(expOver, 0, 0);
			return;
		}
	}
}

void IOCP::ListenEvent::StartListen(const unsigned short& port)
{
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(SOCKADDR_IN));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	//listen Socket 생성, 바인딩
	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	bind(m_listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(m_listenSocket, SOMAXCONN);

	////리슨 소켓 iocp객체에 등록
	iocpRef->RegistHandle(reinterpret_cast<HANDLE>(m_listenSocket), 999999);
}

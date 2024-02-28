#pragma once
#include "../PCH/stdafx.h"
#include "IocpEventBase.h"

/*
	Iocp Accept Listen을 위한 클래스
*/

struct ConnectInfo
{
	char localInfo[sizeof(SOCKADDR) + 16];
	char remoteInfo[sizeof(SOCKADDR) + 16];
	ConnectInfo()
	{
		ZeroMemory(localInfo, sizeof(SOCKADDR) + 16);
		ZeroMemory(remoteInfo, sizeof(SOCKADDR) + 16);
	}
};

namespace IOCP
{
	class Iocp;
}
class ExpOver;
namespace IOCP
{
	class ListenEvent : public EventBase
	{
	public:
		ListenEvent() = default;
		ListenEvent(std::shared_ptr<IOCP::Iocp> iocp);

		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
		virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

		void RegisterIocp(std::shared_ptr< IOCP::Iocp>& iocp);
		void Accept();
		void StartListen(const unsigned short& port);
	private:
		std::shared_ptr< IOCP::Iocp> iocpRef;

		SOCKET m_listenSocket;
		SOCKET m_clientSocket;
		ConnectInfo m_connInfo;
	};
}


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

class Iocp;
class ExpOver;
class IocpListenEvent : public IocpEventBase
{
public:
	IocpListenEvent() = default;
	virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)override;

	void RegisterIocp(Iocp* iocpPtr);
	void Accept();
	void StartListen(const unsigned short& port);
private:
	Iocp* iocp;

	SOCKET m_listenSocket;
	SOCKET m_clientSocket;
	ConnectInfo m_connInfo;
};


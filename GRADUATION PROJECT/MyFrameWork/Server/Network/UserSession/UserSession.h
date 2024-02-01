#pragma once
#include "../../PCH/stdafx.h"
#include "../IocpEvent/IocpEventBase.h"

//UserSession -> recvEvent°´Ã¼
class ExpOver;
class PacketHeader;
class UserSession :public IocpEventBase
{
public:
	UserSession();
	UserSession(int id);
	UserSession(const UserSession& other);
	UserSession(UserSession&& other) noexcept;
	~UserSession();

	void RegistSocket(SOCKET&& socket);
	void ContructPacket(const DWORD& ioSize);
	void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
private:
	void DoSend(const PacketHeader* packetHeader);
	void DoRecv(ExpOver* over);

	void ProccessPacket();
protected:
	SOCKET m_socket;

	//IocpEventBase
	int m_remainDataLength = 0;
	char m_buffer[MAX_BUF_SIZE];
	WSABUF m_wsabuf;

	//UserInfo
	int m_id;
	std::atomic<PLAYER_STATE> m_playerState;
	std::wstring m_loginId;
	std::wstring m_playerName;

};

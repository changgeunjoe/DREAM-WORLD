#pragma once
#include "../../PCH/stdafx.h"
#include "../IocpEvent/IocpEventBase.h"

//UserSession -> recvEvent객체
class ExpOver;
class PacketHeader;
class Room;
//recv, DB(weak_ptr), Room객체 예정
class UserSession :public IOCP::EventBase
{
private:
	struct RecvDataStorage
	{
		WSABUF m_wsabuf;
		int m_remainDataLength = 0;
		char m_buffer[MAX_RECV_BUF_SIZE];
		RecvDataStorage()
		{
			m_remainDataLength = 0;
			ZeroMemory(m_buffer, MAX_RECV_BUF_SIZE);
			m_wsabuf.buf = m_buffer;
			m_wsabuf.len = MAX_RECV_BUF_SIZE;
		}

		void Reset()
		{
			m_remainDataLength = 0;
			ZeroMemory(m_buffer, MAX_RECV_BUF_SIZE);
			m_wsabuf.buf = m_buffer;
			m_wsabuf.len = MAX_RECV_BUF_SIZE;
		}
	};

public:
	UserSession();
	UserSession(int id);
	UserSession(int id, SOCKET sock);
	UserSession(const UserSession& other);
	UserSession(UserSession&& other) noexcept;
	~UserSession();

	void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	void StartRecv();
	void DoSend(const PacketHeader* packetHeader);

	void LoginSucces(const wchar_t* nickName)
	{
		m_playerName = nickName;
	}

	void SetRole(const ROLE& role)
	{
		m_role = role;
	}

	const ROLE GetRole() const
	{
		return m_role;
	}

	void SetRoomRef(std::shared_ptr<Room>& roomRef);

private:
	void DoRecv(ExpOver*& over);

	void ContructPacket(const DWORD& ioSize);
	void ExecutePacket(const PacketHeader* packetHeader);
	void Disconnect();
protected:
	SOCKET m_socket;

	//IocpEventBase -> recv
	RecvDataStorage m_recvDataStorage;

	//UserInfo
	std::atomic<PLAYER_STATE> m_playerState;
	std::wstring m_playerName;

	std::weak_ptr<Room> m_roomWeakRef;//Room이 삭제 됐다면, 플레이어는 방에서 나오게
	ROLE m_role;
};

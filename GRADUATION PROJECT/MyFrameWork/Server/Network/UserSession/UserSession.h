#pragma once
#include "../../PCH/stdafx.h"
#include "../IocpEvent/IocpEventBase.h"

//UserSession -> recvEvent객체
class ExpOver;
class Room;
class CharacterObject;
struct PacketHeader;
//recv, DB(weak_ptr), Room객체 예정
class UserSession :public IOCP::EventBase
{
public:
	enum class CONNECT_STATE
	{
		CONNECTED,
		DISCONNECTED
	};
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
	//void DoSend(const PacketHeader* packetHeader) const;
	void DoSend(const std::shared_ptr<PacketHeader> packetHeader);
	void Reconnect(std::shared_ptr<UserSession> prevDisconnectedUserSession);

	void LoginSuccess(const wchar_t* nickName)
	{
		m_playerName = nickName;
	}

	const std::wstring& GetPlayerName() const
	{
		return m_playerName;
	}

	void SetIngameRole(const ROLE& role)
	{
		m_ingameRole = role;
	}

	const ROLE GetIngameRole() const
	{
		return m_ingameRole;
	}

	const CONNECT_STATE GetConnectState() const
	{
		return m_connectState;
	}

	void SetIngameRef(std::shared_ptr<Room>& roomRef, std::shared_ptr<CharacterObject>& characterRef);

private:
	void DoRecv(ExpOver*& over);
	void RecvComplete(const DWORD& ioByte);
	void ContructPacket(const DWORD& ioSize);
	void ExecutePacket(const PacketHeader* packetHeader);

	void SendExecte();
	void SendComplete(const DWORD& ioByte);

	void Disconnect();
	void ReconnectFail();
protected:
	SOCKET m_socket;

	//IocpEventBase -> recv
	RecvDataStorage m_recvDataStorage;
	ExpOver* m_recvOverlapped;

	//send Scatter-gather
	std::vector<std::shared_ptr<PacketHeader>> m_sendPacketBuffer;
	tbb::concurrent_queue<std::shared_ptr<PacketHeader>> m_prevSendPacketBufferQueue;
	std::atomic_int m_prevSendPacketBufferQueueSize;
	std::atomic_bool m_isAbleSend;
	ExpOver* m_sendOverlapped;

	//UserInfo
	std::atomic<PLAYER_STATE> m_playerState;
	std::atomic<CONNECT_STATE> m_connectState;
	std::wstring m_playerName;

	std::weak_ptr<Room> m_roomWeakRef;//Room이 삭제 됐다면, 플레이어는 방에서 나오게
	std::weak_ptr<CharacterObject> m_possessCharacter;//Room에서 플레이하고 있는 캐릭터

	ROLE m_ingameRole;//실제 인게임 ROLE
	ROLE m_matchedRole;//현재 match 큐에 삽입한 ROLE
};

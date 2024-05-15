#pragma once
#include "../../PCH/stdafx.h"
#include "../IocpEvent/IocpEventBase.h"

struct PacketHeader;
namespace Network {
	class ExpOver;
	class UserSession : public Network::IocpEventBase
	{
		static int GetInitUserId();

		static constexpr int MAX_RECV_BUF_SIZE = 1024;
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
		~UserSession();
		virtual void Execute(Network::ExpOver* expOver, const DWORD& ioByte, const ULONG_PTR& key) override;
		virtual void Fail(ExpOver* expOver) override;
		
		virtual void Connect(SOCKET connectSocket);
		void StartRecv();
		void Send(PacketHeader* sendPacketHeader);
		void Send(std::shared_ptr<PacketHeader> sendPacketHeader);

		const int& GetId() const
		{
			return m_id;
		}
		virtual void Disconnect();
	protected:
		virtual void ExecutePacket(const PacketHeader* executePacketHeader) = 0; 
	private:
		void DoRecv(ExpOver*& expOver);
		void ConstructPacket(const DWORD& ioSize);

	protected:
		std::atomic_bool m_isConnect;
		SOCKET m_socket;
		RecvDataStorage m_recvDataStorage;
		int m_id;
	};

}



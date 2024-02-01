#include "stdafx.h"
#include "UserSession.h"
#include "../IocpEvent/IocpEventManager.h"
#include "../ExpOver/ExpOver.h"


UserSession::UserSession()
{
	m_id = -1;
	spdlog::debug("UserSession::UserSession() - 0x{0:0>16x}", long long(this));
}

UserSession::UserSession(int id) : m_id(id)
{
	spdlog::debug("UserSession::UserSession({0:d}) - 0x{1:0>16x}", id, long long(this));
	m_playerName = L"m_playerName Test";
}

UserSession::UserSession(const UserSession& other)
	: m_id(other.m_id), m_loginId(other.m_loginId), m_playerName(other.m_playerName)//, m_roomId(other.m_roomId)
{
	spdlog::debug("UserSession::UserSession() - copyConstructor 0x{0:0>16x}", long long(this));
	m_socket = NULL;
}

UserSession::UserSession(UserSession&& other) noexcept
	: m_id(other.m_id), m_loginId(other.m_loginId), m_playerName(std::move(other.m_playerName))//, m_roomId(other.m_roomId)
{
	spdlog::debug("UserSession::UserSession() - moveConstructor 0x{0:0>16x}", long long(this));

	m_playerState.store(other.m_playerState);
	m_socket = other.m_socket;
	other.m_socket = NULL;
}

UserSession::~UserSession()
{
	long long ptr = long long(this);
	spdlog::debug("UserSession::~UserSession({0:d}) - 0x{1:0>16x}", m_id, ptr);
	m_id = -1;
	if (NULL != m_socket)
		closesocket(m_socket);
}

void UserSession::RegistSocket(SOCKET&& socket)
{
	m_socket = socket;
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(OP_RECV, this);
	DoRecv(expOver);
}

void UserSession::ContructPacket(const DWORD& ioSize)
{

}

void UserSession::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	auto currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
	case OP_RECV:
		//Construct Packet
		break;
	default:
		break;
	}
	DoRecv(over);
}

void UserSession::DoSend(const PacketHeader* packetHeader)
{
	IocpEventManager::GetInstance().Send(m_socket, packetHeader);
}

void UserSession::DoRecv(ExpOver* over)
{
	int recvRes = WSARecv(m_socket, &m_wsabuf + m_remainDataLength, 1, nullptr, 0, over, nullptr);
	if (recvRes != 0) {
		int errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			//error Check
		}
	}
}

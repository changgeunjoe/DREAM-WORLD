#include "stdafx.h"
#include "UserSession.h"
#include "../PacketManager/PacketManager.h"
#include "../ExpOver/ExpOverManager.h"
#include "../ExpOver/ExpOver.h"


UserSession::UserSession()
{
	m_id = -1;
	m_recvExOver = new RecvExpOverBuffer();
	spdlog::debug("UserSession::UserSession() - 0x{0:0>16x}", long long(this));
}

UserSession::UserSession(int id) : m_id(id)
{
	m_recvExOver = new RecvExpOverBuffer();
	spdlog::debug("UserSession::UserSession({0:d}) - 0x{1:0>16x}", id, long long(this));
	m_playerName = L"m_playerName Test";
}

UserSession::UserSession(const UserSession& other)
	: m_id(other.m_id), m_loginId(other.m_loginId), m_playerName(other.m_playerName), m_roomId(other.m_roomId)
{
	spdlog::debug("UserSession::UserSession() - copyConstructor 0x{0:0>16x}", long long(this));
	m_recvExOver = new RecvExpOverBuffer();
	m_socket = NULL;
}

UserSession::UserSession(UserSession&& other) noexcept
	: m_id(other.m_id), m_loginId(other.m_loginId), m_playerName(std::move(other.m_playerName)), m_roomId(other.m_roomId)
{
	spdlog::debug("UserSession::UserSession() - moveConstructor 0x{0:0>16x}", long long(this));

	m_playerState.store(other.m_playerState);
	m_recvExOver = other.m_recvExOver;
	other.m_recvExOver = nullptr;
	m_socket = other.m_socket;
	other.m_socket = NULL;
}

UserSession::~UserSession()
{
	long long ptr = long long(this);
	spdlog::debug("UserSession::~UserSession({0:d}) - 0x{1:0>16x}", m_id, ptr);
	m_id = -1;
	if (nullptr != m_recvExOver)
		delete m_recvExOver;
	if (NULL != m_socket)
		closesocket(m_socket);
}

void UserSession::RegistSocket(SOCKET&& socket)
{
	m_socket = socket;
	DoRecv();
}

void UserSession::ContructPacket(const DWORD& ioSize)
{
	m_recvExOver->RecvPacket(m_id, m_roomId, ioSize);
	DoRecv();
}

void UserSession::DoSend(const char* data)
{
	ExpOverWsaBuffer* sendOver = ExpOverManager::GetInstance().CreateExpOverWsaBuffer(IOCP_OP_CODE::OP_SEND, data);
	sendOver->DoSend(m_socket);
}

void UserSession::DoRecv()
{
	m_recvExOver->DoRecv(m_socket);
}

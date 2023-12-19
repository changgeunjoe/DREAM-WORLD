#pragma once
#include "../../PCH/stdafx.h"

class RecvExpOverBuffer;
class UserSession
{
public:
	UserSession();
	UserSession(int id);
	UserSession(const UserSession& other);
	UserSession(UserSession&& other) noexcept;
	~UserSession();
private:
	//id state
	int m_id;
	std::atomic<PLAYER_STATE> m_playerState;
	//id.name
	std::wstring m_loginId;
	std::wstring m_playerName;

	//�÷������� ��ID
	int m_roomId;

	//����, recv Overlapped�� ����
	RecvExpOverBuffer* m_recvExOver;
	SOCKET m_socket;

public:
	void RegistSocket(SOCKET&& socket);
	void ContructPacket(const DWORD& ioSize);
private:
	void DoSend(const char* data);
	void DoRecv();
};

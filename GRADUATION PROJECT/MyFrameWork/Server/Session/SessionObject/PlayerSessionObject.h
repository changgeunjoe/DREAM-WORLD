#pragma once
#include "SessionObject.h"
#include "../ExpOver.h"

#define NAME_SIZE	20


class PlayerSessionObject : public SessionObject
{
private:
	char	m_playerID[NAME_SIZE];
	short	m_level;
	short	m_exp;
	short	m_hp;
	short	m_maxHp;
	short	m_attackDamage;

private:
	ExpOver				m_exOver;
	SOCKET				m_socket;
	int					m_prevBufferSize;
private:
	int roomNumber;

public:
	PlayerSessionObject(Session* session, SOCKET& sock);
	virtual	~PlayerSessionObject();

public:
	void Recv();
	void Send(void* p);
	void ConstructPacket(int ioByte);
};

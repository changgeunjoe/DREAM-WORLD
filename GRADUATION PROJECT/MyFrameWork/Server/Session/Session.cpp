#include "stdafx.h"
#include "Session.h"
#include "SessionObject/PlayerSessionObject.h"
#include <iostream>
Session::Session()
{
	m_id = -1;
	Initialize();
}

Session::~Session()
{
	m_id = -1;
	if (m_sessionObject != nullptr)
		delete m_sessionObject;
	m_sessionObject = nullptr;
}

void Session::Initialize()
{
	m_prevPacket = 0;
	m_playerState = PLAYER_STATE::FREE;
	m_sessionObject = nullptr;
}

void Session::RegistPlayer(int id, SOCKET& sock)
{
	{
		std::lock_guard<std::mutex> psLock(m_playerStateLock);
		//m_playerState = PLAYER_STATE::ALLOC;
		m_playerState = PLAYER_STATE::IN_GAME;//임시로 인게임 상태로 전환 -> 에코 서버 테스트
	}
	m_sessionCategory = PLAYER;
	m_id = id;
	PlayerSessionObject* newSessionObjet = new PlayerSessionObject(this, sock);
	m_sessionObject = reinterpret_cast<SessionObject*>(newSessionObjet);
	std::cout << "std::RegistPlayer" << std::endl;
	newSessionObjet->Recv();
}

void Session::Recvpacket()
{
	//m_sessionObject->;
}

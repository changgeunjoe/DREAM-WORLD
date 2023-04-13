#include "stdafx.h"
#include "Session.h"
#include "SessionObject/PlayerSessionObject.h"
#include "SessionObject/MonsterSessionObject.h"
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
	m_playerState = PLAYER_STATE::FREE;
	m_sessionObject = nullptr;
}

void Session::RegistPlayer(int id, SOCKET& sock)
{
	m_sessionCategory = PLAYER;
	m_id = id;
	//PlayerSessionObject* newSessionObjet = new PlayerSessionObject(this, sock);
	m_sessionObject = new PlayerSessionObject(this, sock);
		//= reinterpret_cast<SessionObject*>(newSessionObjet);
	std::cout << "std::RegistPlayer" << std::endl;
	{
		std::lock_guard<std::mutex> psLock(m_playerStateLock);
		m_playerState = PLAYER_STATE::ALLOC;
	}
	m_sessionObject->Recv();
}

void Session::RegistMonster(int id, std::string& roomId)
{
	m_sessionCategory = BOSS;
	m_id = id;
	m_sessionObject = new MonsterSessionObject(this, roomId);
	std::cout << "std::RegistMonster" << std::endl;
	{
		std::lock_guard<std::mutex> psLock(m_playerStateLock);
		m_playerState = PLAYER_STATE::ALLOC;
	}	
}

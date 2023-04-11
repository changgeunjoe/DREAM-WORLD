#include "stdafx.h"
#include "../Session.h"
#include "MonsterSessionObject.h"
#include "../../Logic/Logic.h"

extern Logic g_logic;

MonsterSessionObject::MonsterSessionObject(Session* session) : SessionObject(session)
{
	
}

MonsterSessionObject::MonsterSessionObject(Session* session, std::string& roomId) : SessionObject(session), m_roomId(roomId)
{
	
}

MonsterSessionObject::~MonsterSessionObject()
{	 
	
}

void MonsterSessionObject::Recv()
{
	std::cout << "Monster Can not Recv" << std::endl;
}

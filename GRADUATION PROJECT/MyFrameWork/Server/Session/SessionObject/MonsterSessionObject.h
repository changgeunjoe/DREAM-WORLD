#pragma once
#include "SessionObject.h"
class MonsterSessionObject : public SessionObject
{
public:
	MonsterSessionObject(Session* session);
	~MonsterSessionObject();

private:
	lua_State* m_luaState;
public:
	virtual void Recv() override;
public:
	int API_MonsterChasePlayer(lua_State* L);
};

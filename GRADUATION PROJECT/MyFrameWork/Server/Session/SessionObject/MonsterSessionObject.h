#pragma once
#include "SessionObject.h"
#include "../../Room/Room.h"
class MonsterSessionObject : public SessionObject
{
public:
	MonsterSessionObject(Session* session);
	MonsterSessionObject(Session* session, std::string& roomId);
	~MonsterSessionObject();

private:
	lua_State* m_luaState;
	std::string m_roomId;
public:
	virtual void Recv() override;
public:
	
};

namespace LUA_API_FUNC {
	int API_Test(lua_State* L);
	int API_BOSS_MOVE(lua_State* L);
	int API_AUTO_CHASE(lua_State* L);
}
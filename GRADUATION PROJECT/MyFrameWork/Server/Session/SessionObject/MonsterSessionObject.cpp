#include "stdafx.h"
#include "../Session.h"
#include "MonsterSessionObject.h"
#include "../../Logic/Logic.h"

extern Logic g_logic;

MonsterSessionObject::MonsterSessionObject(Session* session) : SessionObject(session)
{
	m_luaState = luaL_newstate();
	luaL_openlibs(m_luaState);
	int error = luaL_loadfile(m_luaState, "LuaScript/boss.lua");
	if (error) {
		std::cout << "Error: luaL_loadfile: " << lua_tostring(m_luaState, -1);
		lua_pop(m_luaState, 1);
	}
	error = lua_pcall(m_luaState, 0, 0, 0);
	if (error) {
		std::cout << "Error: lua_pcall: " << lua_tostring(m_luaState, -1);
		lua_pop(m_luaState, 1);
	}
	
	
	lua_register(m_luaState, "API_MyTest", LUA_API_FUNC::API_Test);
	lua_getglobal(m_luaState, "Regist_Id");
	lua_pushnumber(m_luaState, session->GetId());
	error = lua_pcall(m_luaState, 1, 0, 0);
	if (error) {
		std::cout << "Error: lua_pcall: " << lua_tostring(m_luaState, -1);
		lua_pop(m_luaState, 1);
	}

	lua_pop(m_luaState, 1);// eliminate set_uid from stack after call
	lua_settop(m_luaState, 0);


}

MonsterSessionObject::MonsterSessionObject(Session* session, std::string& roomId) : SessionObject(session)
{
	m_roomId = roomId;
	m_luaState = luaL_newstate();
	luaL_openlibs(m_luaState);
	int error = luaL_loadfile(m_luaState, "LuaScript/boss.lua");
	if (error) {
		std::cout << "Error: luaL_loadfile: " << lua_tostring(m_luaState, -1);
		lua_pop(m_luaState, 1);
	}
	error = lua_pcall(m_luaState, 0, 0, 0);
	if (error) {
		std::cout << "Error: lua_pcall: " << lua_tostring(m_luaState, -1);
		lua_pop(m_luaState, 1);
	}

	//Regist Api function
	lua_register(m_luaState, "API_MyTest", LUA_API_FUNC::API_Test);
	lua_register(m_luaState, "API_Boss_Move", LUA_API_FUNC::API_BOSS_MOVE);
	lua_register(m_luaState, "API_Boss_Chase", LUA_API_FUNC::API_AUTO_CHASE);

	//call regist ID
	lua_getglobal(m_luaState, "Regist_Id");
	lua_pushnumber(m_luaState, session->GetId());
	error = lua_pcall(m_luaState, 1, 0, 0);
	if (error) {
		std::cout << "Error: lua_pcall: " << lua_tostring(m_luaState, -1);
		lua_pop(m_luaState, 1);
	}

	lua_pop(m_luaState, 1);// eliminate set_uid from stack after call
	lua_settop(m_luaState, 0);
}

MonsterSessionObject::~MonsterSessionObject()
{	 
	lua_close(m_luaState);
}

void MonsterSessionObject::Recv()
{
	std::cout << "Monster Can not Recv" << std::endl;
}

int LUA_API_FUNC::API_Test(lua_State* L)
{
	int user_id = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	std::cout  << "Regist Id Success: " << user_id << std::endl;
	return 0;
}

int LUA_API_FUNC::API_BOSS_MOVE(lua_State* L)
{

	return 0;
}

int LUA_API_FUNC::API_AUTO_CHASE(lua_State* L)
{

	return 0;
}

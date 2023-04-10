#include "stdafx.h"
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
	auto aaa = [&](lua_State* L)-> int {
		return API_MonsterChasePlayer(L);
	};
	lua_register(m_luaState, "API_Test", (lua_CFunction)&aaa);
	error = lua_getglobal(m_luaState, "testEvent");
	//if (error) {
	//	std::cout << "Error: lua_getglobal: " << lua_tostring(m_luaState, -1);
	//	lua_pop(m_luaState, 1);
	//}
	error = lua_pcall(m_luaState, 0, 0, 0);
	if (error) {
		std::cout << "Error: lua_pcall: " << lua_tostring(m_luaState, -1);
		lua_pop(m_luaState, 1);
	}
	lua_pop(m_luaState, 1);
}

MonsterSessionObject::~MonsterSessionObject()
{
	lua_close(m_luaState);
}

void MonsterSessionObject::Recv()
{
	std::cout << "Monster Can not Recv" << std::endl;
}

int MonsterSessionObject::API_MonsterChasePlayer(lua_State* L)
{
	std::cout << "aaa" << std::endl;
	/*int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);
	lua_pop(L, 4);*/
	return 0;
}

#pragma once
#include<sqlext.h>
#include <string>
#include <thread>
class DBObject
{
public:
	//std::queue m_workQueue;
private:
	bool m_bIsRunning = false;
	std::thread m_DBthread;
public:
	DBObject()
	{
		m_bIsRunning = true;		
		m_DBthread = std::thread{ [this]() {RunDBThread(); } };
	}
	~DBObject()
	{
		m_bIsRunning = false;
	}
public:
	void RunDBThread();
	void Destroy();
	void InitializeDBData(SQLHENV& henv, SQLHDBC& hdbc,	SQLHSTMT& hstmt);
public:
	bool GetPlayerInfo(std::wstring PlayerLoginId, std::wstring& outputPlayerName, short& pos_X, short& pos_Y, short& level, short& Exp, short& hp, short& maxHp, short& attackDamage);
	void SavePlayerInfo(std::wstring PlayerLoginId, short& pos_X, short& pos_Y, short& level, short& Exp, short& hp, short& maxHp, short& attackDamage);
	void AddUser(std::wstring PlayerLoginId);
	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
};

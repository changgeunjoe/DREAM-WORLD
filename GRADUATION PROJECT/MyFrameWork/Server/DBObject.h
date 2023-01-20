#pragma once
#include<sqlext.h>
#include <string>
#include <thread>
class DBObject
{
private:
	SQLHENV m_henv;
	SQLHDBC m_hdbc;
	SQLHSTMT m_hstmt;

private:
	bool m_bIsRunning = false;
	std::thread m_DBthread;
public:
	DBObject()
	{
		m_bIsRunning = false;
	}
	~DBObject()
	{
		// Process data  
		SQLCancel(m_hstmt);///종료
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);//리소스 해제
		//disconnet
		SQLDisconnect(m_hdbc);
	}
public:
	void RunDBThread();
	bool GetPlayerInfo(std::wstring PlayerLoginId, std::wstring& outputPlayerName, short& pos_X, short& pos_Y, short& level, short& Exp, short& hp, short& maxHp, short& attackDamage);
	void SavePlayerInfo(std::wstring PlayerLoginId, short& pos_X, short& pos_Y, short& level, short& Exp, short& hp, short& maxHp, short& attackDamage);
	void AddUser(std::wstring PlayerLoginId);
	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
};

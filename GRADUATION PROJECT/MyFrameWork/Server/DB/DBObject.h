#pragma once
#include<sqlext.h>
#include <string>
#include <thread>
#include <concurrent_queue.h>

enum DB_OP_CODE {
	DB_OP_GET_PLAYER_INFO
};

struct DB_EVENT {
	DB_OP_CODE	op;
	int			userId;
	void* Data;
};

namespace DB_STRUCT {

	struct PlayerInfo {
		std::wstring PlayerLoginId;
		std::wstring pw;
	};

}

class DBObject
{
private:
	SQLHENV m_henv;
	SQLHDBC m_hdbc;
	SQLHSTMT m_hstmt;
public:
	Concurrency::concurrent_queue<DB_EVENT> m_workQueue;
private:
	bool m_bIsRunning = false;
	std::thread m_DBthread;
public:
	DBObject()
	{
		m_bIsRunning = true;
		InitializeDBData();
		m_DBthread = std::thread{ [this]() {RunDBThread(); } };
	}
	~DBObject()
	{
		m_bIsRunning = false;
		// Process data  
		SQLCancel(m_hstmt);///종료
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);//리소스 해제

		Destroy();
	}
public:
	void RunDBThread();
	void Destroy();
private:
	void InitializeDBData();
	int print_error(SQLHENV henv, SQLHDBC hdbc, SQLHSTMT hstmt);
public:
	bool GetPlayerInfo(std::wstring PlayerLoginId, std::wstring pw, std::wstring& outputPlayerName);
	void SavePlayerInfo(std::wstring PlayerLoginId, short& pos_X, short& pos_Y, short& level, short& Exp, short& hp, short& maxHp, short& attackDamage);
	void AddUser(std::wstring PlayerLoginId);
	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
};

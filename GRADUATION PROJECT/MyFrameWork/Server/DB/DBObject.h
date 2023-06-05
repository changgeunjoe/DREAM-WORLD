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

	class PlayerInfo {
	public:
		std::wstring PlayerLoginId;
		std::wstring pw;
	public:
		PlayerInfo() {};
		PlayerInfo(std::wstring loginId, std::wstring password)
		{
			PlayerLoginId = loginId;
			pw = password;
		}
		PlayerInfo(std::string loginId, std::string password)
		{
			PlayerLoginId.assign(loginId.begin(), loginId.end());
			pw.assign(password.begin(), password.end());
		}
		PlayerInfo(char* loginId, char* password)
		{
			std::string id{ loginId };
			std::string passWord{ password };
			PlayerLoginId.assign(id.begin(), id.end());
			pw.assign(passWord.begin(), passWord.end());
		}
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

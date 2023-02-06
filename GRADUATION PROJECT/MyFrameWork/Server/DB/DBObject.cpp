#include "stdafx.h"
#include "DBObject.h"

void DBObject::RunDBThread()
{
	SQLHENV m_henv;
	SQLHDBC m_hdbc;
	SQLHSTMT m_hstmt;
	InitializeDBData(m_henv, m_hdbc, m_hstmt);

	while (m_bIsRunning) {

	}

	// Process data  
	SQLCancel(m_hstmt);///종료
	SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);//리소스 해제
	//disconnet
	SQLDisconnect(m_hdbc);
}

void DBObject::Destroy()
{
	for (auto& th : m_DBthread)
		if (th.joinable())
			th.join();
}

void DBObject::InitializeDBData(SQLHENV& henv, SQLHDBC& hdbc, SQLHSTMT& hstmt)
{
	SQLRETURN retcode;
	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"Dream_World_DB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					//cout << "DB Success" << endl;
				}
			}
		}
	}
}

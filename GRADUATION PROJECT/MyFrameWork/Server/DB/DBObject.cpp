#include "stdafx.h"
#include <mutex>
#include "DBObject.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../Session/SessionObject/PlayerSessionObject.h"
#include "../Logic/Logic.h"
#include "../IOCPNetwork/protocol/protocol.h"

extern IOCPNetwork	g_iocpNetwork;
extern Logic		g_logic;

void DBObject::RunDBThread()
{
	while (m_bIsRunning) {
		if (m_workQueue.empty()) {
			Sleep(1);
			continue;
		}
		DB_EVENT currentEvent;
		if (m_workQueue.try_pop(currentEvent)) {
			switch (currentEvent.op)
			{
			case DB_OP_GET_PLAYER_INFO:
			{
				DB_STRUCT::PlayerInfo* pInfo = reinterpret_cast<DB_STRUCT::PlayerInfo*>(currentEvent.Data);
				std::wstring wst_nickName;
				if (GetPlayerInfo(pInfo->PlayerLoginId, pInfo->pw, wst_nickName)) {					
					g_iocpNetwork.m_session[currentEvent.userId].SetInGameState();
					g_iocpNetwork.m_session[currentEvent.userId].SetName(wst_nickName);
					SERVER_PACKET::LoginPacket sendPacket;
					sendPacket.type = SERVER_PACKET::LOGIN_OK;
					sendPacket.size = sizeof(SERVER_PACKET::LoginPacket);
					sendPacket.userID = currentEvent.userId;
					memcpy(sendPacket.name, wst_nickName.c_str(), wst_nickName.size() * 2);
					sendPacket.name[wst_nickName.size()] = 0;
					g_iocpNetwork.m_session[currentEvent.userId].Send(&sendPacket);

					//SERVER_PACKET::AddPlayerPacket myInfoPacket;
					//memcpy(myInfoPacket.name, wst_nickName.c_str(), wst_nickName.size() * 2);
					//myInfoPacket.name[wst_nickName.size()] = 0;
					//myInfoPacket.userId = currentEvent.userId;
					//myInfoPacket.position = pSession->GetPosition();
					//myInfoPacket.rotate = pSession->GetRotation();
					//myInfoPacket.type = SERVER_PACKET::ADD_PLAYER;
					//myInfoPacket.size = sizeof(SERVER_PACKET::AddPlayerPacket);
					//g_logic.MultiCastOtherPlayer(myInfoPacket.userId, &myInfoPacket);

					//for (auto& session : g_iocpNetwork.m_session) {
					//	if (currentEvent.userId == session.GetId() || session.GetId() == -1)
					//		continue;
					//	PlayerSessionObject* otherSession = dynamic_cast<PlayerSessionObject*>(session.m_sessionObject);
					//	char* otherPlayerDataPacket = otherSession->GetPlayerInfo();
					//	//SERVER_PACKET::AddPlayerPacket* otherPlayerDataPacket = reinterpret_cast<SERVER_PACKET::AddPlayerPacket*>(otherSession->GetPlayerInfo());
					//	//char* otherPlayerDataPacket = otherSession->GetPlayerInfo();
					//	pSession->Send(otherPlayerDataPacket);
					//	delete otherPlayerDataPacket;
					//}
				}
				else {
					//login failed
				}
				if (currentEvent.Data != nullptr)
					delete currentEvent.Data;
			}
			break;
			default:
				break;
			}
		}

	}
}

void DBObject::Destroy()
{
	if (m_DBthread.joinable())
		m_DBthread.join();
}

void DBObject::InitializeDBData()
{
	SQLRETURN retcode;
	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);

	// Set the ODBC version environment attribute
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				// Connect to data source  
				retcode = SQLConnect(m_hdbc, (SQLWCHAR*)L"Dream_World_DB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					std::cout << "DB Success" << std::endl;
				}
			}
		}
	}
}

bool DBObject::GetPlayerInfo(std::wstring PlayerLoginId, std::wstring pw, std::wstring& outputPlayerName)
{
	SQLRETURN retcode;

	// Allocate statement handle  
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
	if (retcode == SQL_ERROR) {
		print_error(m_henv, m_hdbc, m_hstmt);
		return false;
	}
	SQLWCHAR szName[NAME_SIZE + 1] = { 0 };
	SQLLEN cbName = 0;

	std::wstring storeProcedure = L"EXEC GET_PLAYER_INFO ";
	storeProcedure.append(PlayerLoginId);
	storeProcedure.append(L", ");
	storeProcedure.append(pw);
	storeProcedure.append(L"\0");
	retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)storeProcedure.c_str(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLBindCol(m_hstmt, 1, SQL_C_WCHAR, szName, NAME_SIZE * 2 + 2, &cbName);

		retcode = SQLFetch(m_hstmt);
		if (retcode == SQL_ERROR) {
			print_error(m_henv, m_hdbc, m_hstmt);
			SQLCancel(m_hstmt);///종료
			SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);//리소스 해제
			return false;
		}
		else if (retcode == SQL_SUCCESS)
		{
			outputPlayerName.append(szName);
			int fRes = outputPlayerName.find(32, 0);
			outputPlayerName.erase(fRes);
		}
		if (retcode == SQL_SUCCESS_WITH_INFO) {
			outputPlayerName.append(szName);
			print_error(m_henv, m_hdbc, m_hstmt);
			int fRes = outputPlayerName.find(32, 0);
			outputPlayerName.erase(fRes);
		}
	}
	if (retcode == SQL_ERROR) {
		print_error(m_henv, m_hdbc, m_hstmt);
	}

	SQLCancel(m_hstmt);///종료
	SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);//리소스 해제

	return true;
}

int DBObject::print_error(SQLHENV    henv, SQLHDBC    hdbc, SQLHSTMT   hstmt)
{
	SQLWCHAR     buffer[SQL_MAX_MESSAGE_LENGTH + 1];
	SQLWCHAR     sqlstate[SQL_SQLSTATE_SIZE + 1];
	SQLINTEGER  sqlcode;
	SQLSMALLINT length;

	while (SQLError(henv, hdbc, hstmt, sqlstate, &sqlcode, buffer,
		SQL_MAX_MESSAGE_LENGTH + 1, &length) == SQL_SUCCESS)
	{
		std::wcout << "\n **** ERROR *****\n";
		std::wcout << "         SQLSTATE: " << sqlstate << std::endl;
		std::wcout << "Native Error Code: " << sqlcode << std::endl;
		std::wcout << "%s" << buffer << std::endl;
	};
	return (0);
}

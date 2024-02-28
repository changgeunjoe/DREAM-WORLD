#include "stdafx.h"
#include "DB.h"
#include "../Network/IocpEvent/IocpEventManager.h"
#include "../Network/IocpEvent/IocpDBEvent.h"
#include "../Network/IOCP/IOCP.h"
#include "../Network/UserSession/UserSession.h"

void DB::EventBase::Execute(HANDLE iocpHandle, SQLHDBC hdbc)
{
	SQLRETURN retCode;
	SQLHSTMT sqlStatement;
	// Allocate statement handle  
	retCode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &sqlStatement);
	if (SQL_SUCCESS != retCode) {
		spdlog::warn("DB::PlayerInfoEvent::Execute() - sqlStatement alloc Error");
		DB::ErrorPrint(SQL_HANDLE_STMT, sqlStatement);
		if (SQL_ERROR == retCode) {
			ExecuteFail();
			SQLCancel(sqlStatement);///종료
			SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);//리소스 해제
			return;
		}
	}

	std::wstring query = GetQuery();
	//SQL문 실행 - stmt, 실행할 SQL문, 실행할 SQL문 길이
	retCode = SQLExecDirect(sqlStatement, (SQLWCHAR*)query.c_str(), query.size());

	if (SQL_ERROR == retCode || SQL_INVALID_HANDLE == retCode) {
		//문제가 있다면 클라이언트에 알림
		spdlog::warn("DB::PlayerInfoEvent::Execute() - SQLExecDirect alloc Error");
		DB::ErrorPrint(SQL_HANDLE_STMT, sqlStatement);
		ExecuteFail();//클라이언트에 다시 시도 알림
		SQLCancel(sqlStatement);///종료
		SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);//리소스 해제
		return;
	}

	//쿼리에 대한 결과 처리
	Proccess(retCode, iocpHandle, sqlStatement);

	//stmt 해제
	SQLCancel(sqlStatement);///종료
	SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);//리소스 해제
}

#pragma region DB_PLAYER_EVENT

DB::PlayerInfoEvent::PlayerInfoEvent(const DB_OP_CODE& opCode, std::shared_ptr<UserSession>& userRef, const char* loginId, const char* pw)
	:EventBase(opCode), m_userRef(userRef)
{
	m_playerLoginId = ConvertStringToWideString(loginId);
	m_password = ConvertStringToWideString(pw);
}

void DB::PlayerInfoEvent::Proccess(SQLRETURN exeResult, HANDLE iocpHandle, SQLHSTMT hstmt)
{
	auto userRef = m_userRef.lock();
	//유저가 로그인 하기전에 나감
	if (nullptr == userRef) return;

	//플레이어 정보가 존재
	SQLRETURN retCode;
	SQLWCHAR playerName[DB_NAME_SIZE] = { 0 };
	SQLLEN dataLength = SQL_NULL_DATA;
	//namelength
	//SQL_NULL_DATA
	//SQL_NO_TOTAL

	//BufferLegth는 wchar_t: 2바이트 이기때문에 * 2 계산
	retCode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, playerName, DB_NAME_SIZE * 2, &dataLength);
#ifdef _DEBUG
	if (SQL_SUCCESS != retCode) {
		spdlog::warn("PlayerInfoEvent::Proccess() - SQLBindCol Raise Error");
		ErrorPrint(SQL_HANDLE_STMT, hstmt);
	}
#endif // _DEBUG

	retCode = SQLFetch(hstmt);
	//플레이어 정보가 없음
	if (SQL_NO_DATA == retCode) {
		NonExist(iocpHandle, userRef);
		//stmt종료는 함수 외부에서 수행
		return;
	}

#ifdef _DEBUG
	else if (SQL_SUCCESS != retCode) {
		spdlog::warn("PlayerInfoEvent::Proccess() - SQLFetch Raise Error");
		ErrorPrint(SQL_HANDLE_STMT, hstmt);
	}
#endif // _DEBUG

	auto playerInfoEvent = std::make_shared<IOCP::DBGetPlayerInfoEvent>(playerName, userRef);
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_SUCCESS_GET_PLAYER_INFO, playerInfoEvent);
	PostQueuedCompletionStatus(iocpHandle, 0, 99999, expOver);

	/*
		SQL_SUCCESS_WITH_INFO
		SQL_NEED_DATA - 아마 발생 안함
		SQL_STILL_EXECUTING - 비동기 일때 생기는 에러코드
	*/
	//SQL_SUCCESS가 아니라면 문제가 있다는 의미 - 로그 출력
	if (SQL_SUCCESS != exeResult) {
		spdlog::warn("DB::PlayerInfoEvent::Execute() - Success with Info");
		DB::ErrorPrint(SQL_HANDLE_STMT, hstmt);
	}
}

std::wstring DB::PlayerInfoEvent::GetQuery()
{
	std::wstring query = L"EXEC GET_PLAYER_INFO ";
	query.append(m_playerLoginId);
	query.append(L", ");
	query.append(m_password);
	query.append(L"\0");
	return query;
}

void DB::PlayerInfoEvent::ExecuteFail()
{
	//다시 시도 알림

}

void DB::PlayerInfoEvent::NonExist(HANDLE iocpHandle, std::shared_ptr<UserSession>& userRef)
{
	//성공했으나, 해당하는 정보가 존재 하지 않음.
	//auto playerInfoEvent = std::make_shared<IOCP::DBNotifyEvent>(userRef);
	/*auto expOver = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_FAIL_GET_PLAYER_INFO, playerInfoEvent);
	PostQueuedCompletionStatus(iocpHandle, 0, 99999, expOver);*/
}

void DB::PlayerInfoEvent::SetData(const DB_OP_CODE& opCode, std::shared_ptr<UserSession>& userRef, const char* loginId, const char* pw)
{
	m_opCode = opCode;
	m_userRef = userRef;
	m_playerLoginId = ConvertStringToWideString(loginId);
	m_password = ConvertStringToWideString(pw);
}
//DB_EVENT
#pragma endregion 

DB::DBConnector::DBConnector()
{
}

DB::DBConnector::~DBConnector()
{
}

void DB::DBConnector::DBConnectThread()
{
	spdlog::info("DB::DBConnector::DBConnectThread() - start Db Thread");
	while (true) {
		if (m_DBEventQueue.empty()) {
			//DB 이벤트 수행할게 없어 양보
			std::this_thread::yield();
			continue;
		}
		std::shared_ptr<DB::EventBase> dbEvent;
		bool isSuccess = m_DBEventQueue.try_pop(dbEvent);
		if (!isSuccess) {//이벤트를 못 가져왔다면 양보
			std::this_thread::yield();
			continue;
		}
		dbEvent->Execute(iocpRef->GetIocpHandle(), m_hdbc);
	}
}

void DB::DBConnector::RegistIocp(std::shared_ptr<IOCP::Iocp> iocp)
{
	iocpRef = iocp;
}

void DB::DBConnector::Connect()
{
	SQLRETURN retCode;
	// Allocate environment handle
	//SQLAllocHandle() - ODBC 핸들 변수 할당
	retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);

	// Set the ODBC version environment attribute
	if (SQL_SUCCESS == retCode) {
		//env설정
		retCode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (SQL_SUCCESS == retCode) {
			//dbc 할당
			retCode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);

			if (SQL_SUCCESS == retCode) {
				// Set login timeout to 5 second
				retCode = SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				if (SQL_SUCCESS == retCode) {
					// Connect to data source
					//SQL 연결, dbc, DB이름, 사용자 이름, 비밀번호...
					retCode = SQLConnect(m_hdbc, (SQLWCHAR*)L"Dream_World_DB", SQL_NTS, (SQLWCHAR*)NULL, SQL_NTS, NULL, SQL_NTS);
					if (SQL_SUCCESS == retCode) {
						spdlog::info("DB Connect Success");
						m_DBthread = std::jthread([this]() {DBConnectThread(); });
					}
					else {
						if (SQL_SUCCESS_WITH_INFO == retCode) {
							spdlog::info("DB Connect Success With Info");
							ErrorPrint(SQL_HANDLE_DBC, m_hdbc);
							m_DBthread = std::jthread([this]() {DBConnectThread(); });
						}
						else {
							spdlog::critical("DB::DBConnector::Connect() - dbc ConnectError");
							spdlog::critical("DB Fail");
							ErrorPrint(SQL_HANDLE_DBC, m_hdbc);
							//assert
						}
					}
				}
				else {
					spdlog::critical("DB::DBConnector::Connect() - dbc SetAttrError");
					ErrorPrint(SQL_HANDLE_DBC, m_hdbc);
					if (SQL_ERROR == retCode) {
						//assert?
					}
				}
			}
			else {
				spdlog::critical("DB::DBConnector::Connect() - dbc AllocError");
				ErrorPrint(SQL_HANDLE_DBC, m_hdbc);
				if (SQL_ERROR == retCode) {
					//assert?
				}
			}
		}
		else {
			spdlog::critical("DB::DBConnector::Connect() - env SetAttrError");
			ErrorPrint(SQL_HANDLE_ENV, m_henv);
			if (SQL_ERROR == retCode) {
				//assert?
			}
		}
	}
	else {
		spdlog::critical("DB::DBConnector::Connect() - env AllocError");
		ErrorPrint(SQL_HANDLE_ENV, m_henv);
		if (SQL_ERROR == retCode) {
			//assert?
		}
	}
}

void DB::DBConnector::InsertDBEvent(std::shared_ptr<DB::EventBase>& dbEvent)
{
	m_DBEventQueue.push(dbEvent);
}

void DB::ErrorPrint(const SQLSMALLINT& handleType, SQLHANDLE handle)
{
	//오류 저장 버퍼
	SQLWCHAR sqlState[SQL_SQLSTATE_SIZE + 1]{ 0 };
	SQLWCHAR sqlMessage[SQL_MAX_MESSAGE_LENGTH + 1]{ 0 };
	SQLINTEGER sqlNativeError = 0;
	SQLSMALLINT sqlErrorLength = 0;

	int recNumber = 1;
	while (true) {
		SQLRETURN sqlRetVal = SQLGetDiagRec(handleType, handle, recNumber, sqlState, &sqlNativeError, sqlMessage, SQL_MAX_MESSAGE_LENGTH + 1, &sqlErrorLength);
		switch (sqlRetVal)
		{
		case SQL_SUCCESS: //진단 정보 존재
		{
			spdlog::info("SQL State: {0}", ConvertWideStringToString(sqlState));
			spdlog::info("SQL Error Message: {0}", ConvertWideStringToString(sqlMessage));

			ZeroMemory(sqlState, SQL_SQLSTATE_SIZE + 1);
			ZeroMemory(sqlMessage, sqlErrorLength + 1);
		}
		break;

		case SQL_SUCCESS_WITH_INFO: // 버퍼가 작을 수 있음
		{
			spdlog::warn("SQL State: {0}", ConvertWideStringToString(sqlState));
			spdlog::warn("SQL Error Message: {0}", ConvertWideStringToString(sqlMessage));
			if (sqlErrorLength > SQL_MAX_MESSAGE_LENGTH) {
				spdlog::warn("DB::DBConnector::ErrorPrint() - SQL_SUCCESS_WITH_INFO");
			}
			ZeroMemory(sqlState, SQL_SQLSTATE_SIZE + 1);
			ZeroMemory(sqlMessage, sqlErrorLength + 1);
		}
		break;

		case SQL_INVALID_HANDLE:	// 핸들 또는 핸들 타입이 잘못 됨.
		{
			spdlog::critical("DB::DBConnector::ErrorPrint() - SQL_INVALID_HANDLE");
			//asssert?
		}
		break;

		case SQL_ERROR:				// recNumber <= 0 또는 bufferLength < 0 또는 비동기 알림을 사용하면 비동기 작업이 완료 되지 않음.
		{
			if (recNumber <= 0) {
				spdlog::critical("DB::DBConnector::ErrorPrint() - SQL_ERROR");
			}
			//bufferLength는 항상 존재 하기때문에 0미만이 되진 않음.
			//비동기 작업을 수행하지 않음.
		}
		break;

		case SQL_NO_DATA:			// recNumber가 handle에 지정된 핸들에 대해 존재했던 진단 레코드 수 보다 큰 경우 또는
			//Handle에 대한 진단 레코드가 없을 때 recNumber는 양수 + SQL_NO_DATA 반환
		{
			if (recNumber > 0) return;
		}
		break;
		default:
			break;
		}
		++recNumber;
	}

}

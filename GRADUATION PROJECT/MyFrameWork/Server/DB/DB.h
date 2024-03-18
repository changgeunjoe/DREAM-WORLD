#pragma once
#include "../PCH/stdafx.h"
#include "../SingletonBase.h"

namespace IOCP
{
	class Iocp;
}
class UserSession;
namespace DB
{
	enum DB_OP_CODE
	{
		DB_OP_GET_PLAYER_INFO
	};

	class EventBase : public std::enable_shared_from_this<EventBase>
	{
	public:
		EventBase() = default;
		EventBase(const DB_OP_CODE& opCode)
			:m_opCode(opCode)
		{}

		void Execute(HANDLE iocpHandle, SQLHDBC hdbc);

	protected:
		virtual void Proccess(SQLRETURN exeResult, HANDLE iocpHandle, SQLHSTMT hstmt) = 0;
		virtual std::wstring GetQuery() = 0;
		virtual void ExecuteFail() = 0;

	protected:
		DB_OP_CODE	m_opCode;
	};

	class PlayerInfoEvent : public EventBase
	{
	public:
		PlayerInfoEvent() = default;
		PlayerInfoEvent(const DB_OP_CODE& opCode, std::shared_ptr<UserSession>& userRef, const char* loginId, const char* pw);

		void SetData(const DB_OP_CODE& opCode, std::shared_ptr<UserSession>& userRef, const char* loginId, const char* pw);

	protected:
		virtual void Proccess(SQLRETURN exeResult, HANDLE iocpHandle, SQLHSTMT hstmt) override;
		virtual std::wstring GetQuery() override;
		virtual void ExecuteFail() override;

	private:
		void NonExist(HANDLE iocpHandle, std::shared_ptr<UserSession>& userRef);

	protected:
		std::wstring m_playerLoginId;
		std::wstring m_password;
		std::weak_ptr<UserSession> m_userRef;
	};

	class DBConnector : public SingletonBase<DBConnector>
	{
		friend SingletonBase;
	private:
		DBConnector();
		~DBConnector();

		void DBConnectThread();

	public:
		void RegistIocp(std::shared_ptr<IOCP::Iocp> iocp);
		void Connect();

		void InsertDBEvent(std::shared_ptr<DB::EventBase>& dbEvent);

	private:
		//ODBC핸들 - SQLHANDLE
		//환경 핸들
		SQLHENV m_henv;
		//연결 핸들 DB Connection핸들
		SQLHDBC m_hdbc;

		//명령 핸들 statement 핸들
		//SQLHSTMT m_hstmt;

		tbb::concurrent_queue<std::shared_ptr<DB::EventBase>> m_DBEventQueue;
		std::shared_ptr<IOCP::Iocp> iocpRef;
	};

	void ErrorPrint(const SQLSMALLINT& handleType, SQLHANDLE handle);
};

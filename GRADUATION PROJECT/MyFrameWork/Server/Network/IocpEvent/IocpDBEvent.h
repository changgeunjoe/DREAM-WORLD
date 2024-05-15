#pragma once
#include "IocpEventBase.h"

constexpr short DB_NAME_SIZE = 20;
class UserSession;
namespace IOCP
{
	class DBNotifyEvent : public EventBase
	{
	public:
		DBNotifyEvent() = delete;
		DBNotifyEvent(std::shared_ptr<UserSession>& userRef);
		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
		virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

	private:
		std::weak_ptr<UserSession> m_userRef;
	};

	class DBGetPlayerInfoEvent : public EventBase
	{
	public:
		DBGetPlayerInfoEvent() = delete;
		DBGetPlayerInfoEvent(const wchar_t* name, std::shared_ptr<UserSession>& userRef);
		DBGetPlayerInfoEvent(const std::wstring& name, std::shared_ptr<UserSession>& userRef);
		~DBGetPlayerInfoEvent() = default;

		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
		virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	private:
		wchar_t m_buffer[DB_NAME_SIZE];//playerName 정보 저장
		std::weak_ptr<UserSession> m_userRef;
	};
}


#pragma once
#include "IocpEventBase.h"

namespace IOCP
{
	class DBNotifyEvent : public EventBase
	{
		DBNotifyEvent() = default;
		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	};

	class DBGetPlayerInfoEvent : public EventBase
	{
	public:
		DBGetPlayerInfoEvent() = default;
		DBGetPlayerInfoEvent(const wchar_t* name);
		DBGetPlayerInfoEvent(const std::wstring& name);
		~DBGetPlayerInfoEvent() = default;

		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;

	private:
		wchar_t m_buffer[NAME_SIZE];//playerName 정보 저장
	};
}


#pragma once
#include "../../PCH/stdafx.h"

class ExpOver;
/*
	Iocp 이벤트 인터페이스 class
*/
namespace IOCP
{
	class EventBase : public std::enable_shared_from_this<EventBase>
	{
	public:
		EventBase() = default;
	public:
		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) = 0;
		virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) = 0;

		std::shared_ptr<EventBase> GetSharedPtr()
		{
			return shared_from_this();
		}

		std::weak_ptr<EventBase> GetWeakPtr()
		{
			return weak_from_this();
		}
	};
}

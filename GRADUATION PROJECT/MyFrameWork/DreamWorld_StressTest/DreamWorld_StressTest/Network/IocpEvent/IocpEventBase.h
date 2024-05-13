#pragma once
#include "../../PCH/stdafx.h"
namespace Network
{
	class ExpOver;
	class IocpEventBase : public std::enable_shared_from_this<IocpEventBase>
	{
	public:
		virtual void Execute(ExpOver* expOver, const DWORD& ioByte, const ULONG_PTR& key) = 0;
		virtual void Fail(ExpOver* expOver) = 0;
	};
}


#pragma once
#include "../../PCH/stdafx.h"

class ExpOver;
/*
	Iocp �̺�Ʈ �������̽� class
*/
class IocpEventBase
{
public:
	IocpEventBase() = default;
public:
	virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) = 0;
};

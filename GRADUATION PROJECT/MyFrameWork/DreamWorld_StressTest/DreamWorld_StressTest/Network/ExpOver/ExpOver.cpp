#include "stdafx.h"
#include "ExpOver.h"
#include "../IocpEvent/IocpEventBase.h"

void Network::ExpOver::Execute(const bool& success, const DWORD& ioByte, const ULONG_PTR& key)
{
	if (success) {
		m_iocpEventRef->Execute(this, ioByte, key);
	}
	else {
		m_iocpEventRef->Fail(this);
	}
}

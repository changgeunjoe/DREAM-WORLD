#include "stdafx.h"
#include "IocpDBEvent.h"
#include "IocpEventManager.h"
#include "../ExpOver/ExpOver.h"


void IOCP::DBNotifyEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	switch (over->GetOpCode())
	{
		//DB - login Error, SQL_ERROR ...
	case OP_FAIL_GET_PLAYER_INFO:
	{

	}
	break;
	case OP_DB_ERROR:
	{

	}
	break;
	default:
		break;
	}
}

IOCP::DBGetPlayerInfoEvent::DBGetPlayerInfoEvent(const wchar_t* name)
{
	//DB에서 26글자로 제한 했기때문에 문제 발생 안할거로 예상 됨.
	HRESULT res = StringCchCopyW(m_buffer, NAME_SIZE, name);
	if (S_OK != res) {
		//Error
	}

}

IOCP::DBGetPlayerInfoEvent::DBGetPlayerInfoEvent(const std::wstring& name)
{
	HRESULT res = StringCchCopyW(m_buffer, NAME_SIZE, name.c_str());
	if (S_OK != res) {
		//Error
	}
}

void IOCP::DBGetPlayerInfoEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	switch (over->GetOpCode())
	{
	case OP_SUCCESS_GET_PLAYER_INFO:
	{
		//userSession에 이름 저장
		//클라이언트에 이름 전송
	}
	break;
	default:
		break;
	}
	//expOver는 반환, 자기 자신은 delete
	IocpEventManager::GetInstance().DeleteExpOver(over);
	delete this;
}

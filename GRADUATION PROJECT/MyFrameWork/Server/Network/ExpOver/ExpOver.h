#pragma once
#include "../../PCH/stdafx.h"

//Event를 처리할 객체를 포인터로 가짐

/*
	가상 함수를 사용하게 되면, class의 주소가 vtable의 주소가 되기 때문에
	IocpEvent를 멤버로 가져 execute하여 처리
*/

/*
	IOCP_OP_CODE: 완료포트 통지 시, 할 행동
	IocpEvent: 실제 행위는 IocpEvent::Execute()에서
*/
namespace IOCP
{
	class EventBase;
}

//overlapped구조체가 맨 앞에(다른건 상속하면 안될듯?) ex) enable_shared_from_this<>
class ExpOver : public WSAOVERLAPPED
{
public:
	ExpOver() :m_opCode(IOCP_OP_CODE::OP_NONE), m_iocpEvent(nullptr)
	{
		ResetOverlapped();
	}

	ExpOver(const IOCP_OP_CODE& opCode) :m_opCode(opCode), m_iocpEvent(nullptr)
	{
		ResetOverlapped();
	}

	ExpOver(const IOCP_OP_CODE& opCode, std::shared_ptr<IOCP::EventBase>& iocpEvent) : m_opCode(opCode), m_iocpEvent(iocpEvent)
	{
		ResetOverlapped();
	}

	~ExpOver();

public:
	void SetData(const IOCP_OP_CODE& opCode, std::shared_ptr<IOCP::EventBase>& iocpEvent)
	{
		ResetOverlapped();
		m_opCode = opCode;
		m_iocpEvent = iocpEvent;
	}

	void ResetEvent()
	{
		m_iocpEvent = nullptr;
	}

	const IOCP_OP_CODE& GetOpCode() const
	{
		return m_opCode;
	}

	void Execute(const DWORD& ioByte, const ULONG_PTR& key);

	//오버랩 객체 초기화
	void ResetOverlapped()
	{
		ZeroMemory(this, sizeof(WSAOVERLAPPED));
	}
private:
	IOCP_OP_CODE m_opCode;
	std::shared_ptr<IOCP::EventBase> m_iocpEvent;
};

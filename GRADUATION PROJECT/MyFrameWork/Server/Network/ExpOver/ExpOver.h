#pragma once
#include "../../PCH/stdafx.h"

//Event�� ó���� ��ü�� �����ͷ� ����

/*
	���� �Լ��� ����ϰ� �Ǹ�, class�� �ּҰ� vtable�� �ּҰ� �Ǳ� ������
	IocpEvent�� ����� ���� execute�Ͽ� ó��
*/

/*
	IOCP_OP_CODE: �Ϸ���Ʈ ���� ��, �� �ൿ
	IocpEvent: ���� ������ IocpEvent::Execute()����
*/
namespace IOCP
{
	class EventBase;
}

//overlapped����ü�� �� �տ�(�ٸ��� ����ϸ� �ȵɵ�?) ex) enable_shared_from_this<>
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

	//������ ��ü �ʱ�ȭ
	void ResetOverlapped()
	{
		ZeroMemory(this, sizeof(WSAOVERLAPPED));
	}
private:
	IOCP_OP_CODE m_opCode;
	std::shared_ptr<IOCP::EventBase> m_iocpEvent;
};

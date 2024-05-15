#pragma once
#include "../../PCH/stdafx.h"

namespace Network {
	class IocpEventBase;
	class ExpOver : public WSAOVERLAPPED
	{
	public:
		ExpOver() : m_opCode(IOCP_OP_CODE::OP_NONE), m_iocpEventRef(nullptr) {}
		ExpOver(const IOCP_OP_CODE& opCode, std::shared_ptr<IocpEventBase> eventRef) : m_opCode(opCode), m_iocpEventRef(eventRef) {}
		void Execute(const bool& success, const DWORD& ioByte, const ULONG_PTR& key);

		void ResetEvent()
		{
			m_iocpEventRef = nullptr;
		}

		void ResetOverlapped()
		{
			m_opCode = IOCP_OP_CODE::OP_NONE;
		}

		void ResetData()
		{
			ResetEvent();
			ResetOverlapped();
		}

		void SetData(const IOCP_OP_CODE& opCode, std::shared_ptr<IocpEventBase> eventRef)
		{
			m_opCode = opCode;
			m_iocpEventRef = eventRef;
		}

		const IOCP_OP_CODE& GetOpCode() const
		{
			return m_opCode;
		}
	private:
		IOCP_OP_CODE m_opCode;
		std::shared_ptr<IocpEventBase> m_iocpEventRef;
	};
}

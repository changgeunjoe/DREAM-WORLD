#pragma once
#include "../../SingletonBase.h"


struct PacketHeader;

namespace Network
{
	class ExpOver;
	class IocpEventBase;
	class SendEvent;
	class RoomEvent;

	class IocpEventManager : public SingletonBase<IocpEventManager>
	{
		friend SingletonBase;
		//send, DB, Event overlapped包访 磊丰备炼 包府
	private:
		IocpEventManager();
		~IocpEventManager();

	public:
		void Initailize();

	public:
		ExpOver* CreateExpOver(const IOCP_OP_CODE& opCode, std::shared_ptr<Network::IocpEventBase> iocpEvent);
		void Send(SOCKET sock, const PacketHeader* packetHeader);

		void DeleteExpOver(ExpOver* delOver);
		void DeleteSendEvent(std::shared_ptr<Network::SendEvent> delEvent);

	private:
		std::shared_ptr<Network::SendEvent> CreateSendEvent();
	private:
		tbb::concurrent_queue<ExpOver*> m_expOverQueue;
		tbb::concurrent_queue<std::shared_ptr<Network::SendEvent>> m_sendEventQueue;
	};
}

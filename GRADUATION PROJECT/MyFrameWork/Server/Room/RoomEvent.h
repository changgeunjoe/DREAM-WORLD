#pragma once
#include "../Timer/TimerEventBase.h"
#include "../Network/IocpEvent/IocpEventBase.h"

namespace TIMER
{
	class RoomEvent : public TIMER::EventBase
	{
	public:
		RoomEvent(const TIMER_EVENT_TYPE& eventId, const std::chrono::milliseconds& afterTime, const int& roomId);
		RoomEvent(const TIMER_EVENT_TYPE& eventId, const std::chrono::seconds& afterTime, const int& roomId);

		virtual void Execute(HANDLE iocpHandle) override;
	private:
		int m_roomId;
	};
}

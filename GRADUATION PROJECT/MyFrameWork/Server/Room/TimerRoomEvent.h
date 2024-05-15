#pragma once
#include "../Timer/TimerEventBase.h"
#include "../Network/IocpEvent/IocpEventBase.h"

class Room;
namespace TIMER
{
	class RoomEvent : public TIMER::EventBase
	{
	public:
		RoomEvent(const TIMER_EVENT_TYPE& eventId, const std::chrono::milliseconds& afterTime, std::shared_ptr<Room> roomRef);
		RoomEvent(const TIMER_EVENT_TYPE& eventId, const std::chrono::seconds& afterTime, std::shared_ptr<Room> roomRef);

		virtual void Execute(HANDLE iocpHandle) override;
	protected:
		//int m_roomId;
		//Room이 없어졌다면, weak_ptr이 없다면 더 이상 호출x
		std::weak_ptr<Room> m_roomWeakRef;
	};
}

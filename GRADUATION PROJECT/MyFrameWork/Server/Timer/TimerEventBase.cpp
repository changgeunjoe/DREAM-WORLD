#include "stdafx.h"
#include "TimerEventBase.h"

TIMER::EventBase::EventBase(const TIMER_EVENT_TYPE& eventId, const std::chrono::milliseconds& afterTime)
	:m_eventId(eventId)
{
	m_wakeupTime = std::chrono::system_clock::now() + afterTime;
}

TIMER::EventBase::EventBase(const TIMER_EVENT_TYPE& eventId, const std::chrono::seconds& afterTime)
	:m_eventId(eventId)
{
	m_wakeupTime = std::chrono::system_clock::now() + afterTime;
}

bool TIMER::EventBase::IsReady() const
{
	return m_wakeupTime <= std::chrono::system_clock::now();
}

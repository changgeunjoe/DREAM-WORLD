#include "stdafx.h"
#include "TimerEventBase.h"

TIMER::EventBase::EventBase(const TIMER_EVENT_TYPE& eventId, const std::chrono::milliseconds& afterTime)
	:m_eventId(eventId)
{
	m_wakeupTime = std::chrono::high_resolution_clock::now() + afterTime;
}

TIMER::EventBase::EventBase(const TIMER_EVENT_TYPE& eventId, const std::chrono::seconds& afterTime)
	:m_eventId(eventId)
{
	m_wakeupTime = std::chrono::high_resolution_clock::now() + afterTime;
}

bool TIMER::EventBase::IsReady() const
{
	auto laterTime = GetRestTimeForReady().count();
	if (laterTime > 3) return false;
	/*if (laterTime < -19) {
		spdlog::warn("Timer LaterTime: {}", laterTime);
		spdlog::critical("laterTime > 19");
	}*/
	return true;
	//return m_wakeupTime <= std::chrono::high_resolution_clock::now();
	return laterTime <= 0;
}

const TIMER::MS TIMER::EventBase::GetRestTimeForReady() const
{
	return std::chrono::duration_cast<TIMER::MS>(m_wakeupTime - std::chrono::high_resolution_clock::now());
}

#include "stdafx.h"
#include "Timer.h"
#include "TimerEventBase.h"
#include "../Network/IOCP/IOCP.h"

bool TIMER::TimerQueueComp(std::shared_ptr<TIMER::EventBase>& l, std::shared_ptr<TIMER::EventBase>& r)
{
	return *l < *r;
}

TIMER::Timer::~Timer()
{
	m_TimerThread.join();
	spdlog::info("Timer::~Timer() - thread join");

}

void TIMER::Timer::TimerThreadFunc()
{
	while (true) {
		if (m_timerQueue.empty()) {
			//타이머 이벤트 수행할게 없어 양보
			std::this_thread::yield();
			continue;
		}
		std::shared_ptr<TIMER::EventBase> currentEvent = nullptr;
		bool isSuccess = m_timerQueue.try_pop(currentEvent);
		if (!isSuccess) {//이벤트를 못 가져왔다면 양보
			std::this_thread::yield();
			continue;
		}

		if (currentEvent->IsReady()) {//수행할 시간이 됐다면 수행
			currentEvent->Execute(iocpRef->GetIocpHandle());
		}
		else {//아니라면 다시 삽입
			m_timerQueue.push(currentEvent);
		}
	}
}

void TIMER::Timer::StartTimer()
{
	m_TimerThread = std::jthread([this]() {TimerThreadFunc(); });
	spdlog::info("Timer::StartTimer() - Timer Start");
}

void TIMER::Timer::RegisterIocp(std::shared_ptr<IOCP::Iocp>& iocp)
{
	iocpRef = iocp;
}

void TIMER::Timer::InsertTimerEvent(std::shared_ptr<TIMER::EventBase>& timer)
{
	m_timerQueue.push(timer);
}

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
			//Ÿ�̸� �̺�Ʈ �����Ұ� ���� �纸
			std::this_thread::yield();
			continue;
		}
		std::shared_ptr<TIMER::EventBase> currentEvent = nullptr;
		bool isSuccess = m_timerQueue.try_pop(currentEvent);
		if (!isSuccess) {//�̺�Ʈ�� �� �����Դٸ� �纸
			std::this_thread::yield();
			continue;
		}

		if (currentEvent->IsReady()) {//������ �ð��� �ƴٸ� ����
			currentEvent->Execute(iocpRef->GetIocpHandle());
		}
		else {//�ƴ϶�� �ٽ� ����
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

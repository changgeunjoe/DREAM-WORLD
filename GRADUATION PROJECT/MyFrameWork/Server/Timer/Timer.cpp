#include "stdafx.h"
#include "Timer.h"
#include "../ThreadManager/ThreadManager.h"
#include "TimerEventBase.h"
#include "../Network/IOCP/IOCP.h"

bool TIMER::TimerQueueComp(const std::shared_ptr<TIMER::EventBase>& l, const std::shared_ptr<TIMER::EventBase>& r)
{
	return *l < *r;
}

TIMER::Timer::Timer() : iocpRef(nullptr),
m_timerQueue(tbb::concurrent_priority_queue<std::shared_ptr<TIMER::EventBase>, decltype(&TIMER::TimerQueueComp)>(
	[](const std::shared_ptr<TIMER::EventBase>& l, const std::shared_ptr<TIMER::EventBase>& r) {
		return TIMER::TimerQueueComp(l, r);
	}))
{
	spdlog::info("Timer::Timer() - Timer Constructor");
}

TIMER::Timer::~Timer()
{
	spdlog::info("Timer::~Timer()");
}

//이전 타이머 코드
void TIMER::Timer::TimerThreadFunc()
{
	while (true) {
		if (m_timerQueue.empty()) {
			std::this_thread::yield();
		}
		std::shared_ptr<TIMER::EventBase> currentEvent = nullptr;
		bool isSuccess = m_timerQueue.try_pop(currentEvent);
		if (!isSuccess) {
			std::this_thread::yield();
			continue;
		}

		if (currentEvent->IsReady()) {
			currentEvent->Execute(iocpRef->GetIocpHandle());
		}
		else {
			m_timerQueue.push(currentEvent);
		}
	}
}

//void TIMER::Timer::TimerThreadFunc()
//{
//	/*
//		얼마 남지 않은 타이머 이벤트에 대해서 임시 타이머 큐에 저장
//		concurrency_priority_queue에 다시 삽입하는거보다는 임시로 저장하고
//		곧 수행되기 때문에, 다음 수행 때 바로 실행에 가까움
//		top으로 우선순위 높은 객체 볼 수 있음.
//	*/
//	std::priority_queue<std::shared_ptr<TIMER::EventBase>> immediateTimer;
//	constexpr TIMER::MS PUSH_IMMEDIATE_TIMER_QUEUE_TIME = TIMER::MS(4);//어느정도 시간이 적당할지는 생각해야할듯...
//	while (true) {
//		while (!immediateTimer.empty()) {
//			//임시큐에서 빼지 않고 최우선 타이머 이벤트를 볼 수 있음
//			auto& immediateTimerEvent = immediateTimer.top();
//			if (!immediateTimerEvent->IsReady())//즉시 수행 불가능이라면 임시 타이머 큐 객체는 종료
//				break;
//			//임시 타이머 객체니 수행 가능
//			immediateTimerEvent->Execute(iocpRef->GetIocpHandle());
//			immediateTimer.pop();
//		}
//
//		if (m_timerQueue.empty()) {
//			//타이머 이벤트 수행할게 없어 다른 쓰레드에 양보
//			Sleep(1);
//			//std::this_thread::yield();
//			continue;
//		}
//		while (true) {
//			std::shared_ptr<TIMER::EventBase> currentEvent = nullptr;
//			bool isSuccess = m_timerQueue.try_pop(currentEvent);
//			if (!isSuccess) {//이벤트를 못 가져왔다면 다른 쓰레드에 양보
//				Sleep(1);
//				//std::this_thread::yield();
//				break;
//			}
//
//			if (currentEvent->IsReady()) {//수행할 시간이 됐다면 수행
//				currentEvent->Execute(iocpRef->GetIocpHandle());
//				continue;
//			}
//			//아니라면 다시 삽입
//			TIMER::MS restTime = currentEvent->GetRestTimeForReady();
//			if (restTime <= PUSH_IMMEDIATE_TIMER_QUEUE_TIME)//기준 시간보다 적은 시간이 남았다면, 임시 큐에 삽입
//				immediateTimer.push(currentEvent);
//			else m_timerQueue.push(currentEvent);//아니라면 concurrent에 삽입
//			Sleep(1);
//			break;
//		}
//	}
//}

void TIMER::Timer::StartTimer()
{

	spdlog::info("Timer::StartTimer() - Timer Start");
	ThreadManager::GetInstance().CreateThread(std::thread([this]() {TimerThreadFunc(); }));
}

void TIMER::Timer::RegisterIocp(std::shared_ptr<IOCP::Iocp>& iocp)
{
	iocpRef = iocp;
}

void TIMER::Timer::InsertTimerEvent(std::shared_ptr<TIMER::EventBase>& timer)
{
	m_timerQueue.push(timer);
}

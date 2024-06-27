#pragma once
#include "../PCH/stdafx.h"
#include "../SingletonBase.h"

namespace IOCP
{
	class Iocp;
}
namespace TIMER
{
	class EventBase;
	struct TimerQueueComp
	{
		bool operator()(const std::shared_ptr<TIMER::EventBase>& l, const std::shared_ptr<TIMER::EventBase>& r);

	};
	class Timer : public SingletonBase<Timer>
	{
		friend SingletonBase;
	private:
		Timer();// : iocpRef(nullptr) {}
		~Timer();
	private:
		void TimerThreadFunc();
	public:

		void StartTimer();
		void RegisterIocp(std::shared_ptr<IOCP::Iocp>& iocp);
		void InsertTimerEvent(std::shared_ptr<TIMER::EventBase>& timer);
	private:
		//decltype에서 그냥 함수를 넘겨도 되지만, 명시적으로 함수의 주소를 가져올 수 있음(이건 좀 알아봐야 할듯)
		tbb::concurrent_priority_queue<std::shared_ptr<TIMER::EventBase>, TimerQueueComp> m_timerQueue;
		//tbb::concurrent_priority_queue<std::shared_ptr<TIMER::EventBase>, decltype(&TIMER::TimerQueueComp)> m_timerQueue;
		//std::priority_queue <TimerEvent*> m_timerQueue;
		//std::mutex m_timerQueueLock;

		std::shared_ptr<IOCP::Iocp> iocpRef;
	};
}


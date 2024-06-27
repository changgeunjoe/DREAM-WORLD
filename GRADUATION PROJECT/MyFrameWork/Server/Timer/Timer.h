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
		//decltype���� �׳� �Լ��� �Ѱܵ� ������, ��������� �Լ��� �ּҸ� ������ �� ����(�̰� �� �˾ƺ��� �ҵ�)
		tbb::concurrent_priority_queue<std::shared_ptr<TIMER::EventBase>, TimerQueueComp> m_timerQueue;
		//tbb::concurrent_priority_queue<std::shared_ptr<TIMER::EventBase>, decltype(&TIMER::TimerQueueComp)> m_timerQueue;
		//std::priority_queue <TimerEvent*> m_timerQueue;
		//std::mutex m_timerQueueLock;

		std::shared_ptr<IOCP::Iocp> iocpRef;
	};
}


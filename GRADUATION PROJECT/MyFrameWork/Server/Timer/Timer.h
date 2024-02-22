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
	bool TimerQueueComp(EventBase* l, EventBase* r);
	class Timer : public SingletonBase<Timer>
	{
		friend SingletonBase;
	private:
		Timer() : iocpRef(nullptr) {}
		~Timer();
	private:
		void TimerThreadFunc();
	public:

		void StartTimer();
		void RegisterIocp(std::shared_ptr<IOCP::Iocp>& iocp);
		//void InsertTimerEvent(TIMER_EVENT ev);
	private:
		std::thread m_TimerThread;
		//decltype���� �׳� �Լ��� �Ѱܵ� ������, ��������� �Լ��� �ּҸ� ������ �� ����(�̰� �� �˾ƺ��� �ҵ�)
		tbb::concurrent_priority_queue<TIMER::EventBase*, decltype(&TIMER::TimerQueueComp)> m_timerQueue;
		//std::priority_queue <TimerEvent*> m_timerQueue;
		//std::mutex m_timerQueueLock;

		std::shared_ptr<IOCP::Iocp> iocpRef;
	};
}


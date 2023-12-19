#pragma once
#include "../PCH/stdafx.h"
#include "../SingletonBase.h"
class Timer : public SingletonBase<Timer>
{
	friend SingletonBase;
private:
	Timer() :m_running(false) {}
	~Timer();
private:
	volatile bool m_running;
	std::thread m_TimerThread;
	std::priority_queue <TIMER_EVENT> m_timerQueue;
	std::mutex m_timerQueueLock;
private:
	void TimerThreadFunc();
public:
	void StartTimer();
	//void InsertTimerEvent(TIMER_EVENT ev);
};

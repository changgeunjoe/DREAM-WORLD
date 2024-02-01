#pragma once
#include "../PCH/stdafx.h"
#include "../SingletonBase.h"

class Iocp;
class Timer : public SingletonBase<Timer>
{
	friend SingletonBase;
private:
	Timer() :m_running(false) {}
	~Timer();
private:
	void TimerThreadFunc();
public:

	void StartTimer();
	void RegisterIocp(Iocp* iocpPtr);
	//void InsertTimerEvent(TIMER_EVENT ev);
private:
	volatile bool m_running;
	std::thread m_TimerThread;

	std::priority_queue <TIMER_EVENT> m_timerQueue;
	std::mutex m_timerQueueLock;

	Iocp* iocp;
};

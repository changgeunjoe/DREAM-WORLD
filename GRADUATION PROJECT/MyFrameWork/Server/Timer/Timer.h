#pragma once

class Timer
{
public:
	Timer();
	~Timer();
private:
	bool isRunning = false;
	std::thread m_TimerThread;
public:
	concurrency::concurrent_priority_queue<TIMER_EVENT> m_TimerQueue;
private:
	void TimerThreadFunc();
};

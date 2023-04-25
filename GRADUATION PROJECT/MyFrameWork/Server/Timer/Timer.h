#pragma once

class Timer
{
public:
	Timer();
	~Timer();
private:
	bool isRunning = false;
	std::thread m_TimerThread;
private:
	std::priority_queue <TIMER_EVENT> m_TimerQueue;
	std::mutex m_TimerQueueLock;
private:
	void TimerThreadFunc();
public:
	void InsertTimerQueue(TIMER_EVENT ev);
};

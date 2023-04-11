#include "stdafx.h"
#include "Timer.h"
#include "../Session/ExpOver.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../Logic/Logic.h"

extern IOCPNetwork g_iocpNetwork;
extern Logic g_logic;
Timer::Timer()
{
	isRunning = true;
	m_TimerThread = std::thread{ [&]() {TimerThreadFunc(); } };
}

Timer::~Timer()
{

}

void Timer::TimerThreadFunc()
{
	while (isRunning)
	{
		auto current_time = std::chrono::system_clock::now();
		TIMER_EVENT ev;
		auto tempTimerQueue = m_TimerQueue;//타이머 큐를 복사
		if (true == tempTimerQueue.try_pop(ev)) { //복사한 객체로부터 try_pop()
			if (ev.wakeupTime > current_time) {
				//m_TimerQueue.push(ev);		// 최적화 필요
				// timer_queue에 다시 넣지 않고 처리해야 한다.
				std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 실행시간이 아직 안되었으므로 잠시 대기
				continue;
			}
			m_TimerQueue.try_pop(ev);
			switch (ev.eventId) {
			case EV_RANDOM_MOVE:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_TIMER_TEST;
				memcpy(ov->m_buffer, ev.roomId.c_str(), ev.roomId.size());//exOver의 cchar*버퍼에 roomId를 담는다면?
				ov->m_buffer[ev.roomId.size()] = 0;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, -1, &ov->m_overlap);
				TIMER_EVENT new_ev{ std::chrono::system_clock::now()+ std::chrono::seconds(1), ev.roomId, -1,EV_RANDOM_MOVE };
				m_TimerQueue.push(new_ev);
			}
			break;
			default: break;
			}
			continue;		// 즉시 다음 작업 꺼내기
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));   // timer_queue가 비어 있으니 잠시 기다렸다가 다시 시작
	}
}

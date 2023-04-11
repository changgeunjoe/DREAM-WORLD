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
		auto tempTimerQueue = m_TimerQueue;//Ÿ�̸� ť�� ����
		if (true == tempTimerQueue.try_pop(ev)) { //������ ��ü�κ��� try_pop()
			if (ev.wakeupTime > current_time) {
				//m_TimerQueue.push(ev);		// ����ȭ �ʿ�
				// timer_queue�� �ٽ� ���� �ʰ� ó���ؾ� �Ѵ�.
				std::this_thread::sleep_for(std::chrono::milliseconds(1));  // ����ð��� ���� �ȵǾ����Ƿ� ��� ���
				continue;
			}
			m_TimerQueue.try_pop(ev);
			switch (ev.eventId) {
			case EV_RANDOM_MOVE:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_TIMER_TEST;
				memcpy(ov->m_buffer, ev.roomId.c_str(), ev.roomId.size());//exOver�� cchar*���ۿ� roomId�� ��´ٸ�?
				ov->m_buffer[ev.roomId.size()] = 0;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, -1, &ov->m_overlap);
				TIMER_EVENT new_ev{ std::chrono::system_clock::now()+ std::chrono::seconds(1), ev.roomId, -1,EV_RANDOM_MOVE };
				m_TimerQueue.push(new_ev);
			}
			break;
			default: break;
			}
			continue;		// ��� ���� �۾� ������
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));   // timer_queue�� ��� ������ ��� ��ٷȴٰ� �ٽ� ����
	}
}

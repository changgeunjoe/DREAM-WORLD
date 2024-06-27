#include "stdafx.h"
#include "Timer.h"
#include "../ThreadManager/ThreadManager.h"
#include "TimerEventBase.h"
#include "../Network/IOCP/IOCP.h"

bool TIMER::TimerQueueComp::operator()(const std::shared_ptr<TIMER::EventBase>& l, const std::shared_ptr<TIMER::EventBase>& r)
{
	//������ Ÿ�̸� �̺�Ʈ ���ο��� ���۷���������
	return *l < *r;
}

TIMER::Timer::Timer() : iocpRef(nullptr)
//m_timerQueue(tbb::concurrent_priority_queue<std::shared_ptr<TIMER::EventBase>, decltype(&TIMER::TimerQueueComp)>(&TIMER::TimerQueueComp))
{
	spdlog::info("Timer::Timer() - Timer Constructor");
}

TIMER::Timer::~Timer()
{
	spdlog::info("Timer::~Timer()");
}

���� Ÿ�̸� �ڵ�
void timer::timer::timerthreadfunc()
{
	while (true) {
		if (m_timerqueue.empty()) {
			std::this_thread::yield();
		}
		std::shared_ptr<timer::eventbase> currentevent = nullptr;
		bool issuccess = m_timerqueue.try_pop(currentevent);
		if (!issuccess) {
			std::this_thread::yield();
			continue;
		}

		if (currentevent->isready()) {
			currentevent->execute(iocpref->getiocphandle());
		}
		else {
			m_timerqueue.push(currentevent);
		}
	}
}

//void TIMER::Timer::TimerThreadFunc()
//{
//	/*
//		�� ���� ���� Ÿ�̸� �̺�Ʈ�� ���ؼ� �ӽ� Ÿ�̸� ť�� ����
//		concurrency_priority_queue�� �ٽ� �����ϴ°ź��ٴ� �ӽ÷� �����ϰ�
//		�� ����Ǳ� ������, ���� ���� �� �ٷ� ���࿡ �����
//		top���� �켱���� ���� ��ü �� �� ����.
//	*/
//	std::priority_queue<std::shared_ptr<TIMER::EventBase>, std::vector<std::shared_ptr<TIMER::EventBase>>, TimerQueueComp> immediateTimer;
//	constexpr TIMER::MS PUSH_IMMEDIATE_TIMER_QUEUE_TIME = TIMER::MS(4);//������� �ð��� ���������� �����ؾ��ҵ�...
//	while (true) {
//		while (!immediateTimer.empty()) {
//			//�ӽ�ť���� ���� �ʰ� �ֿ켱 Ÿ�̸� �̺�Ʈ�� �� �� ����
//			auto& immediateTimerEvent = immediateTimer.top();
//			if (!immediateTimerEvent->IsReady())//��� ���� �Ұ����̶�� �ӽ� Ÿ�̸� ť ��ü�� ����
//				break;
//			//�ӽ� Ÿ�̸� ��ü�� ���� ����
//			immediateTimerEvent->Execute(iocpRef->GetIocpHandle());
//			immediateTimer.pop();
//		}
//
//		if (m_timerQueue.empty()) {
//			//Ÿ�̸� �̺�Ʈ �����Ұ� ���� �ٸ� �����忡 �纸
//			std::this_thread::yield();
//			continue;
//		}
//		while (true) {
//			std::shared_ptr<TIMER::EventBase> currentEvent = nullptr;
//			bool isSuccess = m_timerQueue.try_pop(currentEvent);
//			if (!isSuccess) {//�̺�Ʈ�� �� �����Դٸ� �ٸ� �����忡 �纸
//				//Sleep(1);
//				std::this_thread::yield();
//				break;
//			}
//
//			if (currentEvent->IsReady()) {//������ �ð��� �ƴٸ� ����
//				currentEvent->Execute(iocpRef->GetIocpHandle());
//				continue;
//			}
//			//�ƴ϶�� �ٽ� ����
//			TIMER::MS restTime = currentEvent->GetRestTimeForReady();
//			if (restTime <= PUSH_IMMEDIATE_TIMER_QUEUE_TIME)//���� �ð����� ���� �ð��� ���Ҵٸ�, �ӽ� ť�� ����
//				immediateTimer.push(currentEvent);
//			else m_timerQueue.push(currentEvent);//�ƴ϶�� concurrent�� ����
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

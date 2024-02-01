#include "stdafx.h"
#include "Timer.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../Logic/Logic.h"
#include "../Room/RoomManager.h"
#include "../Session/SessionObject/MonsterSessionObject.h"


extern IOCPNetwork g_iocpNetwork;
extern Logic g_logic;
extern RoomManager g_RoomManager;

Timer::Timer()
{
	isRunning = true;
	m_TimerThread = std::thread{ [&]() {TimerThreadFunc(); } };
}

Timer::~Timer()
{
	isRunning = false;
	if (m_TimerThread.joinable())
		m_TimerThread.join();
}

void Timer::TimerThreadFunc()
{
	while (isRunning)
	{
		m_TimerQueueLock.lock();
		if (!m_TimerQueue.empty()) {
			auto current_time = std::chrono::system_clock::now();
			TIMER_EVENT ev = m_TimerQueue.top();
			if (ev.wakeupTime > current_time) {
				m_TimerQueueLock.unlock();
				//std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 실행시간이 아직 안되었으므로 잠시 대기
				std::this_thread::yield();
				continue;
			}
			m_TimerQueue.pop();
			m_TimerQueueLock.unlock();
			switch (ev.eventId) {
			case EV_FIND_PLAYER:
			{

				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_FIND_PLAYER;
				//memcpy(ov->m_buffer, ev.roomId.c_str(), ev.roomId.size());
				//ov->m_buffer[ev.roomId.size()] = 0;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
				//if (g_RoomManager.IsExistRunningRoom(ev.roomId)) {
				//	Room& room = g_RoomManager.GetRunningRoom(ev.roomId);
				//	/*if (room.GetBoss().isMove) {
				//		TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), ev.roomId, -1,EV_BOSS_MOVE_SEND };
				//		{
				//			std::lock_guard<std::mutex> timer_lg{ m_TimerQueueLock };
				//			m_TimerQueue.push(new_ev);
				//		}
				//	}*/
				//}

			}
			break;
			case EV_BOSS_STATE:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_BOSS_STATE;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
			}
			break;
			case EV_GAME_STATE_B_SEND:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_GAME_STATE_B_SEND;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
				//TIMER_EVENT new_ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(500), ev.roomId, -1,EV_GAME_STATE_SEND };
				//{
				//	std::lock_guard<std::mutex> timer_lg{ m_TimerQueueLock };
				//	m_TimerQueue.push(new_ev);
				//}
			}
			break;
			case EV_GAME_STATE_S_SEND:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_GAME_STATE_S_SEND;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
			}
			break;
			case EV_BOSS_ATTACK:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_BOSS_ATTACK_EXECUTE;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
			}
			break;
			case EV_SM_UPDATE:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_UPDATE_SMALL_MONSTER;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
			}
			break;
			case EV_HEAL:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_PLAYER_HEAL;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
			}
			break;
			case EV_TANKER_SHIELD_START:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_SET_BARRIER;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
			}
			break;
			case EV_SKY_ARROW_ATTACK:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_SKY_ARROW_ATTACK;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, ev.targetId, &ov->m_overlap);
			}
			break;
			case EV_SYNC_TIME:
			{
				ExpOver* ov = new ExpOver();
				ov->m_opCode = OP_SYNC_TIME;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, 0, &ov->m_overlap);
			}
			break;
			default: break;
			}
			continue;		// 즉시 다음 작업 꺼내기
		}
		else {
			m_TimerQueueLock.unlock();
			std::this_thread::yield();
		}
	}
}
void Timer::InsertTimerQueue(TIMER_EVENT ev)
{
	std::lock_guard<std::mutex> timer_lg{ m_TimerQueueLock };
	m_TimerQueue.push(ev);
}

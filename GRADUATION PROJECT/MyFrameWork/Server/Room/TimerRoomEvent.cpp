#include "stdafx.h"
#include "TimerRoomEvent.h"
#include "../Network/ExpOver/ExpOver.h"
#include "../Network/IocpEvent/IocpEventManager.h"
#include "../Room/Room.h"

TIMER::RoomEvent::RoomEvent(const TIMER_EVENT_TYPE& eventId, const std::chrono::milliseconds& afterTime, std::shared_ptr<Room>& roomRef)
	:EventBase(eventId, afterTime), m_roomRef(roomRef)
{
}

TIMER::RoomEvent::RoomEvent(const TIMER_EVENT_TYPE& eventId, const std::chrono::seconds& afterTime, std::shared_ptr<Room>& roomRef)
	:EventBase(eventId, afterTime), m_roomRef(roomRef)
{
}

void TIMER::RoomEvent::Execute(HANDLE iocpHandle)
{
	auto roomRef = m_roomRef.lock();
	if (nullptr == roomRef) return;
	IOCP_OP_CODE currentOpCode = IOCP_OP_CODE::OP_NONE;
	//이벤트에 따른 OP_CODE 설정
	switch (m_eventId)
	{
	case TIMER_EVENT_TYPE::EV_SEND_NPC_MOVE:
	{
		currentOpCode = IOCP_OP_CODE::OP_SEND_NPC_MOVE;
	}
	break;
	case TIMER_EVENT_TYPE::EV_ROOM_UPDATE:
	{
		currentOpCode = IOCP_OP_CODE::OP_ROOM_UPDATE;
	}
	break;
	case TIMER_EVENT_TYPE::EV_SEND_GAME_STATE:
	{
		currentOpCode = IOCP_OP_CODE::OP_GAME_STATE_SEND;
	}
	break;
	case TIMER_EVENT_TYPE::EV_HEAL_START:
	{
		currentOpCode = IOCP_OP_CODE::OP_PLAYER_HEAL_START;
	}
	break;
	case TIMER_EVENT_TYPE::EV_HEAL:
	{
		currentOpCode = IOCP_OP_CODE::OP_PLAYER_HEAL;
	}
	break;
	case TIMER_EVENT_TYPE::EV_HEAL_END:
	{
		currentOpCode = IOCP_OP_CODE::OP_PLAYER_HEAL_END;
	}
	break;
	case TIMER_EVENT_TYPE::EV_APPLY_SHIELD:
	{
		currentOpCode = IOCP_OP_CODE::OP_PLAYER_APPLY_SIELD;
	}
	break;
	case TIMER_EVENT_TYPE::EV_REMOVE_SHIELD:
	{
		currentOpCode = IOCP_OP_CODE::OP_PLAYER_REMOVE_SIELD;
	}
	break;
	case TIMER_EVENT_TYPE::EV_RAIN_ARROW_ATTACK:
	{
		currentOpCode = IOCP_OP_CODE::OP_RAIN_ARROW_ATTACK;
	}
	break;
	default:
		break;
	}
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(currentOpCode, roomRef);
	PostQueuedCompletionStatus(iocpHandle, 1, NULL, expOver);
}

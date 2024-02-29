#include "stdafx.h"
#include "RoomEvent.h"
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
	case TIMER_EVENT_TYPE::EV_ROOM_UPDATE:
	{
		currentOpCode = IOCP_OP_CODE::OP_ROOM_UPDATE;
	}
	break;
	case TIMER_EVENT_TYPE::EV_GAME_STATE_SEND:
	{
		currentOpCode = IOCP_OP_CODE::OP_GAME_STATE_SEND;
	}
	break;

	case TIMER_EVENT_TYPE::EV_FIND_PLAYER:
	{
		currentOpCode = IOCP_OP_CODE::OP_FIND_PLAYER;
	}
	break;
	case TIMER_EVENT_TYPE::EV_BOSS_ATTACK:
	{
		currentOpCode = IOCP_OP_CODE::OP_BOSS_ATTACK_EXECUTE;
	}
	break;

	case TIMER_EVENT_TYPE::EV_HEAL:
	{
		currentOpCode = IOCP_OP_CODE::OP_PLAYER_HEAL;
	}
	break;
	case TIMER_EVENT_TYPE::EV_TANKER_SHIELD_END:
	{
		//currentOpCode = IOCP_OP_CODE::OP_tank;
	}
	break;
	case TIMER_EVENT_TYPE::EV_SKY_ARROW_ATTACK:
	{
		currentOpCode = IOCP_OP_CODE::OP_SKY_ARROW_ATTACK;
	}
	break;
	default:
		break;
	}
	auto expOver = IocpEventManager::GetInstance().CreateExpOver(currentOpCode, roomRef);
	PostQueuedCompletionStatus(iocpHandle, 1, NULL, expOver);
}

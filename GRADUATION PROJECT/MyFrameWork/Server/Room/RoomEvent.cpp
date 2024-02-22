#include "stdafx.h"
#include "RoomEvent.h"
#include "../Network/ExpOver/ExpOver.h"
#include "../Network/IocpEvent/IocpEventManager.h"

TIMER::RoomEvent::RoomEvent(const TIMER_EVENT_TYPE& eventId, const std::chrono::milliseconds& afterTime, const int& roomId)
	:EventBase(eventId, afterTime), m_roomId(roomId)
{
}

TIMER::RoomEvent::RoomEvent(const TIMER_EVENT_TYPE& eventId, const std::chrono::seconds& afterTime, const int& roomId)
	:EventBase(eventId, afterTime), m_roomId(roomId)
{
}

void TIMER::RoomEvent::Execute(HANDLE iocpHandle)
{
	//IocpEventManager::GetInstance().CreateExpOver();
	//IOCP_OP_CODE opCode = IOCP_OP_CODE::
	switch (m_eventId)
	{
	case EV_ROOM_UPDATE:
	{

	}
	break;
	case EV_GAME_STATE_SEND:
	{

	}
	break;

	case EV_FIND_PLAYER:
	{

	}
	break;
	case EV_BOSS_ATTACK:
	{

	}
	break;

	case EV_HEAL:
	{

	}
	break;
	case EV_TANKER_SHIELD_END:
	{

	}
	break;
	case EV_SKY_ARROW_ATTACK:
	{

	}
	break;
	default:
		break;
	}
}

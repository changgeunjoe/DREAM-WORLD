#include "stdafx.h"
#include "Room.h"
#include "../Network/ExpOver/ExpOver.h"
#include "../Network/UserSession/UserSession.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../Timer/Timer.h"
#include "RoomEvent.h"


Room::Room(std::vector<std::shared_ptr<UserSession>>& userRefVec)
{
	m_roomState = ROOM_STATE::ROOM_COMMON;
	for (auto& userRef : userRefVec) {
		m_user.insert(std::make_pair(userRef, userRef->GetRole()));
	}
	//Character Initialize

	//Timer Event Start
	Update(std::chrono::milliseconds(1));
	GameStateSend(std::chrono::milliseconds(1));
}

Room::Room(std::shared_ptr<UserSession>& userRef)
{
	spdlog::warn("Room::Room() - make Room for Alone Test");
	m_roomState = ROOM_STATE::ROOM_COMMON;
	m_user.insert(std::make_pair(userRef, userRef->GetRole()));
	//Character Initialize

	//Timer Event Start
	Update(std::chrono::milliseconds(1));
	GameStateSend(std::chrono::milliseconds(1));
}

Room::~Room()
{
}

void ::Room::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	switch (over->GetOpCode())
	{
		//Room - update
	case IOCP_OP_CODE::OP_ROOM_UPDATE:
	{
		Update();
	}
	break;
	case IOCP_OP_CODE::OP_GAME_STATE_SEND:
	{
		//tbb::parallel_for(m_user.begin(), m_user.end(), );
		GameStateSend();
	}
	break;

	//Room - Boss
	case IOCP_OP_CODE::OP_FIND_PLAYER:
	{

	}
	break;
	case IOCP_OP_CODE::OP_BOSS_ATTACK_SELECT:
	{

	}
	break;
	case IOCP_OP_CODE::OP_BOSS_ATTACK_EXECUTE:
	{

	}
	break;

	//Room - player skill
	case IOCP_OP_CODE::OP_PLAYER_HEAL:
	{

	}
	break;
	case IOCP_OP_CODE::OP_SKY_ARROW_ATTACK:
	{

	}
	break;
	default:
		break;
	}
}

void Room::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
}

void Room::Update(const std::chrono::milliseconds& updateTick)
{
	//auto timerEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_ROOM_UPDATE, updateTick, shared_from_this());
	//TIMER::Timer::GetInstance().InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(timerEvent));
}

void Room::GameStateSend(const std::chrono::milliseconds& updateTick)
{
	//auto timerEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_GAME_STATE_SEND, updateTick, shared_from_this());
	//TIMER::Timer::GetInstance().InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(timerEvent));
}

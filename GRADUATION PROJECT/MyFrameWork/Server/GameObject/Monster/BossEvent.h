#pragma once
#include "../../Network/IocpEvent/IocpEventBase.h"
#include "../Timer/TimerEventBase.h"

class Room;
class CharacterObject;
class ExpOver;
namespace IOCP
{
	class BossEventBase : public IOCP::EventBase
	{
	public:
		BossEventBase(std::shared_ptr<Room> roomRef) : m_roomWeakRef(roomRef) {}
	protected:
		std::weak_ptr<Room> m_roomWeakRef;
	};

	class BossCalculateRoadEvent : public IOCP::BossEventBase
	{
	public:
		BossCalculateRoadEvent(std::shared_ptr<Room> roomRef, const XMFLOAT3& destinationPosition, const XMFLOAT3& startPosition)
			: BossEventBase(roomRef), m_destination(destinationPosition), m_startPosition(startPosition) {}
		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
		virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	protected:
		XMFLOAT3 m_destination;
		XMFLOAT3 m_startPosition;
	};

	class BossAggroEvent : public IOCP::BossCalculateRoadEvent
	{
	public:
		BossAggroEvent(std::shared_ptr<Room> roomRef, const XMFLOAT3& destinationPosition, const XMFLOAT3& startPosition, std::shared_ptr<CharacterObject> aggroCharacter)
			:BossCalculateRoadEvent(roomRef, destinationPosition, startPosition), m_aggroCharacter(aggroCharacter) {}
		virtual void Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
		virtual void Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key) override;
	private:
		std::weak_ptr<CharacterObject> m_aggroCharacter;
	};
}

namespace TIMER
{
	class BossCalculateRoadEvent : public TIMER::EventBase
	{
	public:
		BossCalculateRoadEvent(std::shared_ptr<Room> roomRef, const XMFLOAT3& destination, const XMFLOAT3& startPosition)
			: TIMER::EventBase(TIMER_EVENT_TYPE::EV_BOSS_RESEARCH_ROAD, std::chrono::milliseconds(1))
			, m_roomWeakRef(roomRef), m_destination(destination), m_startPosition(startPosition) {}

		virtual void Execute(HANDLE iocpHandle) override;
	protected:
		XMFLOAT3 m_destination;
		XMFLOAT3 m_startPosition;
		std::weak_ptr<Room> m_roomWeakRef;
	};

	class BossAggroEvent : public TIMER::BossCalculateRoadEvent
	{
	public:
		BossAggroEvent(std::shared_ptr<Room> roomRef, const XMFLOAT3& destination, const XMFLOAT3& startPosition, std::shared_ptr<CharacterObject> aggroCharacter)
			: TIMER::BossCalculateRoadEvent(roomRef, destination, startPosition), m_aggroCharacter(aggroCharacter) {}

		virtual void Execute(HANDLE iocpHandle) override;
	protected:
		std::weak_ptr<CharacterObject> m_aggroCharacter;
	};
};

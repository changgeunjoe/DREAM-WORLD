#pragma once
#include "../PCH/stdafx.h"

namespace TIMER {

	/*
		���� ���� ����
		EV_ROOM_UPDATE: ��ġ ������Ʈ - (60fps = 1000/60ms = ��16.67ms -> 17ms�� ������Ʈ)
			targetId: �� ��ȣ
		EV_GAME_STATE_SEND: ���� ��Ŀ���� ���� ���� ��ǥ send - (100ms���� ����)
			targetid: �� ��ȣ

		//���� ����
		EV_FIND_PLAYER: ���� �÷��̾� �� Ž��
			targetId: �� ��ȣ
		EV_BOSS_ATTACK: ���� ����
			targetId: �� ��ȣ

		//�÷��̾� ����
		EV_HEAL: ������ ��
			targetId: �� ��ȣ
		EV_TANKER_SHIELD_END: ��Ŀ ���� ���� �� ����
			targetId: �� ��ȣ
		EV_SKY_ARROW_ATTACK: n���Ŀ� ȭ�� ��ġ�� ���� �ǰ�
			targetId: ���ȣ

	*/

	class EventBase : std::enable_shared_from_this<TIMER::EventBase>
	{
	public:
		//ms�ִٰ� ������ �۾�
		EventBase(const TIMER_EVENT_TYPE& eventId, const std::chrono::milliseconds& afterTime);
		//sec�ִٰ� ������ �۾�
		EventBase(const TIMER_EVENT_TYPE& eventId, const std::chrono::seconds& afterTime);

		//priorityQueue���� �켱 ���� �Ǵ��� ���� operator
		constexpr bool operator < (const EventBase& L) const
		{
			return (m_wakeupTime > L.m_wakeupTime);
		}

		bool IsReady() const;
		virtual void Execute(HANDLE iocpHandle) = 0;
	protected:
		std::chrono::system_clock::time_point m_wakeupTime;
		TIMER_EVENT_TYPE m_eventId = TIMER_EVENT_TYPE::EV_NONE;
	};
}


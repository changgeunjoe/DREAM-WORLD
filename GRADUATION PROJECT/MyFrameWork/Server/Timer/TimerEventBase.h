#pragma once
#include "../PCH/stdafx.h"

namespace TIMER {

	/*
		게임 상태 관련
		EV_ROOM_UPDATE: 위치 업데이트 - (60fps = 1000/60ms = 약16.67ms -> 17ms로 업데이트)
			targetId: 룸 번호
		EV_GAME_STATE_SEND: 데드 레커닝을 위한 서버 좌표 send - (100ms마다 수행)
			targetid: 룸 번호

		//보스 관련
		EV_FIND_PLAYER: 보스 플레이어 길 탐색
			targetId: 룸 번호
		EV_BOSS_ATTACK: 보스 공격
			targetId: 룸 번호

		//플레이어 관련
		EV_HEAL: 힐러가 힐
			targetId: 룸 번호
		EV_TANKER_SHIELD_END: 탱커 쉴드 끝날 때 제거
			targetId: 룸 번호
		EV_SKY_ARROW_ATTACK: n초후에 화살 위치에 몬스터 피격
			targetId: 룸번호

	*/

	class EventBase : std::enable_shared_from_this<TIMER::EventBase>
	{
	public:
		//ms있다가 시작할 작업
		EventBase(const TIMER_EVENT_TYPE& eventId, const std::chrono::milliseconds& afterTime);
		//sec있다가 시작할 작업
		EventBase(const TIMER_EVENT_TYPE& eventId, const std::chrono::seconds& afterTime);

		//priorityQueue에서 우선 순위 판단을 위한 operator
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


#pragma once
#include "stdafx.h"

class Character;
namespace CharacterEvent
{
	enum class INTERPOLATE_STATE : char {
		NON_APPLY,
		INTERPOALTE,
		SET_POSITION
	};
	class InterpolateData;
	class InterpolateEvent : public std::enable_shared_from_this<InterpolateEvent>
	{
	public:
		InterpolateEvent() = default;
		InterpolateEvent(const XMFLOAT3& position, const std::chrono::high_resolution_clock::time_point& time);

		bool Execute(Character* character, shared_ptr<InterpolateData>& interpolateData, const std::chrono::high_resolution_clock::time_point& currentTime) const;
		void SetData(const XMFLOAT3& position, const std::chrono::high_resolution_clock::time_point& time);
	private:
		XMFLOAT3 m_position;
		std::chrono::high_resolution_clock::time_point m_time;
	};

	class InterpolateData : public std::enable_shared_from_this<InterpolateData>
	{
	public:
		InterpolateData();
		void ProcessEvents(Character* character);

		const XMFLOAT3 GetInterpolatePosition() const
		{
			return m_interpolatePosition;
		}

		const INTERPOLATE_STATE GetInterpolateState() const
		{
			return m_interpolateState;
		}

		std::pair<float, XMFLOAT3> GetInterpolateData() const;


		void SetData(const INTERPOLATE_STATE& needInterpolate)
		{
			m_interpolateState = needInterpolate;
		}

		void SetData(const INTERPOLATE_STATE& needInterpolate, const XMFLOAT3& interpolatePosition)
		{
			m_interpolateState = needInterpolate;
			m_interpolatePosition = interpolatePosition;
		}


		void SetData(const float& distance, const XMFLOAT3 interpolateVector);

		void InsertEvent(const XMFLOAT3& position, const std::chrono::high_resolution_clock::time_point& serverTime);
		void DeleteEvent(std::shared_ptr<InterpolateEvent>& delEvent);
	private:
		std::shared_ptr<InterpolateEvent> CreateInterpolateEvent(const XMFLOAT3& position, const std::chrono::high_resolution_clock::time_point& serverTime);
	private:
		INTERPOLATE_STATE m_interpolateState = INTERPOLATE_STATE::NON_APPLY;
		float m_interpolationDistance = 0.0f;
		XMFLOAT3 m_interpolationVector = XMFLOAT3{ 0,0,0 };
		XMFLOAT3 m_interpolatePosition = XMFLOAT3{ 0,0,0 };

		concurrency::concurrent_queue<shared_ptr<InterpolateEvent>> m_eventQueue;
		concurrency::concurrent_queue<shared_ptr<InterpolateEvent>> m_restEventQueue;
		atomic_short m_currentEventCnt;
	};
}


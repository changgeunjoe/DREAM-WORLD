#include "stdafx.h"
#include "CharacterEvent.h"
#include "Character.h"
#include "Network/Logic/Logic.h"

extern Logic g_Logic;

CharacterEvent::InterpolateEvent::InterpolateEvent(const XMFLOAT3& position, const std::chrono::high_resolution_clock::time_point& time)
	:m_position(position), m_time(time)
{
}

bool CharacterEvent::InterpolateEvent::Execute(Character* character, shared_ptr<InterpolateData>& interpolateData, const std::chrono::high_resolution_clock::time_point& currentTime) const
{
	if (m_time > currentTime) return false;
	//interpolateData->SetData(INTERPOLATE_STATE::SET_POSITION, m_position);
	//return true;
	auto characterPosition = character->GetPosition();

	auto toInterpolatePositionVector = character->GetToVector(m_position);
	float distance = Vector3::Length(toInterpolatePositionVector);
	toInterpolatePositionVector = Vector3::Normalize(toInterpolatePositionVector);
	long long durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_time).count();
	durationTime += g_Logic.GetDiffTime();

	float differentTime = (float)durationTime / 1000'000.0f;

	//if (differentTime > g_Logic.GetRTT()) {
	//	std::cout << "Time Diff bing" << endl;
	//}
	float interpolateSize = distance - differentTime * 50.0f;//length - v*t
	interpolateSize = abs(interpolateSize);
	if (interpolateSize < 2.0f) {
		interpolateData->SetData(INTERPOLATE_STATE::NON_APPLY);
	}
	else if (interpolateSize > 8.0f) {
		interpolateData->SetData(INTERPOLATE_STATE::SET_POSITION, m_position);
		//std::cout << "Server PositionSetPosition, differentTime: " << endl;
		//std::cout << "client Position: " << characterPosition.x << ", " << characterPosition.y << ", " << characterPosition.z << endl;
		//std::cout << "server Position: " << m_position.x << ", " << m_position.y << ", " << m_position.z << endl;

	}
	else {
		interpolateData->SetData(interpolateSize, toInterpolatePositionVector);
	}
	return true;
}

void CharacterEvent::InterpolateEvent::SetData(const XMFLOAT3& position, const std::chrono::high_resolution_clock::time_point& time)
{
	m_position = position;
	m_time = time;
}

CharacterEvent::InterpolateData::InterpolateData()
	:m_interpolationDistance(0.0f), m_interpolationVector(XMFLOAT3()), m_currentEventCnt(0), m_interpolateState(INTERPOLATE_STATE::NON_APPLY)
{
	for (int i = 0; i < 3; ++i) {
		m_restEventQueue.push(std::make_shared<InterpolateEvent>());
	}
}

void CharacterEvent::InterpolateData::ProcessEvents(Character* character)
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	short currentEventCnt = m_currentEventCnt;
	for (short processCnt = 0; processCnt < currentEventCnt; ++processCnt) {
		shared_ptr<InterpolateEvent> currentEvent;
		bool isSuccess = m_eventQueue.try_pop(currentEvent);
		if (!isSuccess) {
			currentEventCnt = processCnt;
			break;
		}
		if (!currentEvent->Execute(character, shared_from_this(), currentTime)) {
			currentEventCnt = processCnt;
		}
		DeleteEvent(currentEvent);
	}
	m_currentEventCnt -= currentEventCnt;
}

std::pair<float, XMFLOAT3> CharacterEvent::InterpolateData::GetInterpolateData() const
{
	return std::pair<float, XMFLOAT3>(m_interpolationDistance, m_interpolationVector);
}

void CharacterEvent::InterpolateData::SetData(const float& distance, const XMFLOAT3 interpolateVector)
{
	m_interpolateState = INTERPOLATE_STATE::INTERPOALTE;
	m_interpolationVector = interpolateVector;
	m_interpolationDistance = distance;
}

void CharacterEvent::InterpolateData::InsertEvent(const XMFLOAT3& position, const std::chrono::high_resolution_clock::time_point& serverTime)
{
	m_eventQueue.push(CreateInterpolateEvent(position, serverTime));
	++m_currentEventCnt;
}

void CharacterEvent::InterpolateData::DeleteEvent(std::shared_ptr<InterpolateEvent>& delEvent)
{
	m_restEventQueue.push(delEvent);
}

std::shared_ptr<CharacterEvent::InterpolateEvent> CharacterEvent::InterpolateData::CreateInterpolateEvent(const XMFLOAT3& position, const std::chrono::high_resolution_clock::time_point& serverTime)
{
	std::shared_ptr<CharacterEvent::InterpolateEvent> currentEvent;
	bool isSuccess = m_restEventQueue.try_pop(currentEvent);
	if (isSuccess) {
		currentEvent->SetData(position, serverTime);
		return currentEvent;
	}
	return std::make_shared<CharacterEvent::InterpolateEvent>(position, serverTime);
}

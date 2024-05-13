#include "stdafx.h"
#include "EventController.h"
#include "CoolDownEventBase.h"
#include "DurationEvent.h"

const bool EventController::InsertCoolDownEventData(const std::string_view eventName, const MS& coolTime)
{
	if (m_eventDatas.count(eventName.data())) return false;
	auto coolDownEvent = std::make_shared<CoolDownEventBase>(coolTime);
	m_eventDatas.emplace(eventName, coolDownEvent);
	return true;
}

const bool EventController::InsertDurationEventData(const std::string_view eventName, const MS& coolTime, const MS& durationTime)
{
	if (m_eventDatas.count(eventName.data())) return false;
	auto durationEvent = std::make_shared<DurationEvent>(coolTime, durationTime);
	m_eventDatas.emplace(eventName, durationEvent);
	return true;
}

std::shared_ptr<CoolDownEventBase> EventController::GetEventData(const std::string_view eventName)
{
	//존재 하지 않는 이벤트일 때
	if (!m_eventDatas.count(eventName.data())) return nullptr;
	return m_eventDatas[eventName.data()];
}

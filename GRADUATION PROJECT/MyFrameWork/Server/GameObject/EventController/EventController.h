#pragma once
#include "../PCH/stdafx.h"

class CoolDownEventBase;

class EventController
{
public:
	using MS = std::chrono::milliseconds;
	using SEC = std::chrono::seconds;
	using MIN = std::chrono::minutes;

	const bool InsertCoolDownEventData(const std::string_view eventName, const MS& coolTime);
	const bool InsertDurationEventData(const std::string_view eventName, const MS& coolTime, const MS& durationTime);
	std::shared_ptr<CoolDownEventBase> GetEventData(const std::string_view eventName);
private:
	std::unordered_map<std::string, std::shared_ptr<CoolDownEventBase>> m_eventDatas;
};

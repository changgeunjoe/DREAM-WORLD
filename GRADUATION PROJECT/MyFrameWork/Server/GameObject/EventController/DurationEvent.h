#pragma once
#include "../PCH/stdafx.h"

#include "CoolDownEventBase.h"
class DurationEvent : public CoolDownEventBase
{
public:
	DurationEvent() = delete;
	DurationEvent(const MS& coolTime, const MS& durationTime)
		:CoolDownEventBase(coolTime), m_durationTime(durationTime)
	{}

	const MS& GetDurationTIme() const
	{
		return m_durationTime;
	}
private:
	MS m_durationTime;
};


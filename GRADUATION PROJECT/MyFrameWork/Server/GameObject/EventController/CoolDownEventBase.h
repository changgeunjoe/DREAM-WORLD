#pragma once
#include "../PCH/stdafx.h"

class CoolDownEventBase
{
public:
	using MS = std::chrono::milliseconds;
	using TIME = std::chrono::high_resolution_clock::time_point;

public:
	CoolDownEventBase() = delete;
	CoolDownEventBase(const MS& coolTime) : m_coolTime(coolTime), m_lastExecTime(std::chrono::high_resolution_clock::now() - coolTime) {}

	const bool IsAbleExecute();
	void ForceExecute();
	const TIME& GetLastExeTime() const;
	void ResetCoolTime();
private:
	MS m_coolTime;
	TIME m_lastExecTime;
};


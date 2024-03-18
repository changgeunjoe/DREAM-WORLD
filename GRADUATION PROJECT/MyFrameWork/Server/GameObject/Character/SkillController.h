#pragma once
#include "../PCH/stdafx.h"

class SkillController
{
public:
	SkillController(const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime);

	const bool IsExecutable(const int& idx) const;
	std::optional<const std::chrono::seconds> GetDurationTime(const int& idx) const;
	void ExecuteSkill(const int& idx);

private:
	//지속 시간
	std::vector<std::chrono::seconds> m_durationTime;
	//쿨 타임
	std::vector<std::chrono::seconds> m_coolTime;
	//마지막 스킬 입력 시간
	std::vector<std::chrono::high_resolution_clock::time_point> m_lastExecTime;
};


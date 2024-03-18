#include "stdafx.h"
#include "SkillController.h"

SkillController::SkillController(const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime)
{
	if (durationTime.size() != coolTime.size()) {
		spdlog::critical("SkillControiller::SkillControiller() - Skill Time not Equal");
		exit(-1);
		return;
	}
	m_durationTime = durationTime;
	m_coolTime = coolTime;
	m_lastExecTime = std::vector<std::chrono::high_resolution_clock::time_point>(durationTime.size(), std::chrono::high_resolution_clock::now());
}

const bool SkillController::IsExecutable(const int& idx) const
{
	if (idx >= m_lastExecTime.size()) return false;
	auto lastSkillExecuteDurationTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_lastExecTime[idx]);
	//쿨 타임보다 (마지막 시간 - 지금)이 더 큰 값 수행 가능
	if (m_coolTime[idx] <= lastSkillExecuteDurationTime) return true;
	return false;
}

std::optional<const std::chrono::seconds> SkillController::GetDurationTime(const int& idx) const
{
	if (idx >= m_lastExecTime.size()) return std::nullopt;
	return m_durationTime[idx];
}

void SkillController::ExecuteSkill(const int& idx)
{
	if (idx >= m_lastExecTime.size()) return;
	m_lastExecTime[idx] = std::chrono::high_resolution_clock::now();
}

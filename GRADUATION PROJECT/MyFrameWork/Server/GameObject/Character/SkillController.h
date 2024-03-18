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
	//���� �ð�
	std::vector<std::chrono::seconds> m_durationTime;
	//�� Ÿ��
	std::vector<std::chrono::seconds> m_coolTime;
	//������ ��ų �Է� �ð�
	std::vector<std::chrono::high_resolution_clock::time_point> m_lastExecTime;
};


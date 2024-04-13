#include "stdafx.h"
#include "CoolDownEventBase.h"

const bool CoolDownEventBase::IsAbleExecute()
{
	auto nowTime = std::chrono::high_resolution_clock::now();
	auto lastExecuteDurationTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - m_lastExecTime);
	//�� Ÿ�Ӻ��� (������ �ð� - ����)�� �� ū �� ���� ����
	if (m_coolTime <= lastExecuteDurationTime) {
		m_lastExecTime = nowTime;
		return true;
	}
	return false;
}

const CoolDownEventBase::TIME& CoolDownEventBase::GetLastExeTime() const
{
	return m_lastExecTime;
	// TODO: ���⿡ return ���� �����մϴ�.
}

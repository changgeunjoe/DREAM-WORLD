#include "stdafx.h"
#include "CoolDownEventBase.h"

const bool CoolDownEventBase::IsAbleExecute()
{
	auto nowTime = std::chrono::high_resolution_clock::now();
	auto lastExecuteDurationTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - m_lastExecTime);
	//쿨 타임보다 (마지막 시간 - 지금)이 더 큰 값 수행 가능
	if (m_coolTime <= lastExecuteDurationTime) {
		m_lastExecTime = nowTime;
		return true;
	}
	return false;
}

const CoolDownEventBase::TIME& CoolDownEventBase::GetLastExeTime() const
{
	return m_lastExecTime;
	// TODO: 여기에 return 문을 삽입합니다.
}

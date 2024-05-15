#pragma once
#include "../PCH/stdafx.h"

class PlayerSkillBase : public std::enable_shared_from_this<PlayerSkillBase>
{
public:
	virtual void Execute() = 0;
};

using CommonSkill = PlayerSkillBase;

class CommonDurationSkill_MILSEC : public PlayerSkillBase
{
public:
	using DURATION_TIME_RATIO = std::chrono::milliseconds;
	CommonDurationSkill_MILSEC(const DURATION_TIME_RATIO& durationTime) : durationTime(durationTime) {}
protected:
	DURATION_TIME_RATIO durationTime;
};

class DirectionSkillBase : public PlayerSkillBase
{
public:
	DirectionSkillBase(const XMFLOAT3& direction) : direction(direction) {}
protected:
	XMFLOAT3 direction;
};

class PositionSkillBase : public PlayerSkillBase
{
public:
	PositionSkillBase(const XMFLOAT3& position) : position(position) {}
protected:
	XMFLOAT3 position;
};



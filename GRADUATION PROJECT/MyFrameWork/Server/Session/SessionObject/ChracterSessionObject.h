#pragma once
#ifdef _DEBUG
#include "../../PCH/stdafx.h"
#endif
#include "SessionObject.h"


class ChracterSessionObject : public SessionObject
{
private:
	bool	m_leftmouseInput;
	bool	m_rightmouseInput;
protected:
	std::array<std::chrono::seconds, 2> m_skillDuration;
	std::array<std::chrono::seconds, 2> m_skillCoolTime;
	std::array<std::chrono::high_resolution_clock::time_point, 2> m_skillInputTime;
protected:
	float	m_defensivePower;
	float	m_Shield;
	bool	m_ShieldActivation = false;
private:
	ROLE m_InGameRole = ROLE::NONE_SELECT;
public:
	DIRECTION						m_inputDirection = DIRECTION::IDLE;
	DIRECTION						m_prevDirection = DIRECTION::IDLE;
private:
	ROOM_STATE						m_roomState = ROOM_STATE::ROOM_BOSS;
public:
	ChracterSessionObject(ROLE r);
	virtual	~ChracterSessionObject();
protected:
	virtual void SetPosition(DirectX::XMFLOAT3& pos) override {
		m_position = pos;
		m_SPBB.Center = pos;
		m_SPBB.Center.y = m_fBoundingSize;
	}

public:
	bool AdjustPlayerInfo(DirectX::XMFLOAT3& position); // , DirectX::XMFLOAT3& rotate
public:
	//virtual void AutoMove() override;
	virtual void StartMove(DIRECTION d);
	virtual void StopMove();
	virtual void ChangeDirection(DIRECTION d);
	virtual void SetShield(bool active);
	virtual void AttackedHp(short damage) override;
public:
	bool Move(float elapsedTime) override;
	void SetDirection(DIRECTION d);
	void SetMouseInput(bool LmouseInput, bool RmouseInput);

public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	virtual void SetStage_1Position() = 0;
	virtual void SetBossStagePosition() = 0;
	void SetRole(ROLE r) {
		m_InGameRole = r;
	}
	//void ResetRole() { m_InGameRole = ROLE::NONE_SELECT; }
	ROLE GetRole() {
		return m_InGameRole;
	}
	bool GetLeftAttack() { return m_leftmouseInput; }
	void SetRoomState(ROOM_STATE rState) { m_roomState = rState; }
	std::chrono::seconds GetSkillDuration(int i) { return m_skillDuration[i]; }
	std::chrono::high_resolution_clock::time_point GetSkillInputTime(int i) { return m_skillInputTime[i]; }
	float GetDefencePower() { return m_defensivePower; }
	float GetShield() { return m_Shield; }
	bool GetShieldActivation() { return m_ShieldActivation; }
protected:
	std::pair<float, XMFLOAT3> GetNormalVectorSphere(XMFLOAT3& point);
public:
	virtual void Skill_1() = 0;
	virtual void Skill_2() = 0;
protected:
	virtual std::pair<bool, XMFLOAT3> CheckCollisionMap(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual std::pair<bool, XMFLOAT3> CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual std::pair<bool, XMFLOAT3> CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual bool CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
};


class WarriorSessionObject : public ChracterSessionObject
{
public:
	WarriorSessionObject(int id) :ChracterSessionObject(ROLE::WARRIOR)
	{
		SetStage_1Position();
	}
	~WarriorSessionObject() {}
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;

	void Skill_1()override;
	void Skill_2()override;
};

class MageSessionObject : public ChracterSessionObject
{
public:
	MageSessionObject(int id) :ChracterSessionObject(ROLE::PRIEST)
	{
		SetStage_1Position();
	}
	~MageSessionObject() {}
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;
	void Skill_1()override;
	void Skill_2()override;
};

class TankerSessionObject : public ChracterSessionObject
{
public:
	TankerSessionObject(int id) :ChracterSessionObject(ROLE::TANKER)
	{
		SetStage_1Position();
	}
	~TankerSessionObject() {}
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;
	void Skill_1()override;
	void Skill_2()override;
};

class ArcherSessionObject : public ChracterSessionObject
{
public:
	ArcherSessionObject(int id) :ChracterSessionObject(ROLE::ARCHER)
	{
		SetStage_1Position();
	}
	~ArcherSessionObject() {}	
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;
	void Skill_1()override;
	void Skill_2()override;
};
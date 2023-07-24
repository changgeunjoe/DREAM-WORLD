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
	std::array<std::chrono::high_resolution_clock::time_point, 2> m_prevSkillInputTime;
protected:
	std::chrono::seconds m_CommonAttackCoolTime = std::chrono::seconds(1);
	std::chrono::high_resolution_clock::time_point m_prevCommonAttackTime;
protected:
	float	m_Shield = 0.0f;
	float	m_damageRedutionRate = 0.0f;
private:
	ROLE m_InGameRole = ROLE::NONE_SELECT;
public:
	DIRECTION						m_inputDirection = DIRECTION::IDLE;
	DIRECTION						m_applyDirection = DIRECTION::IDLE;
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
	virtual void StartMove(DIRECTION d, XMFLOAT3& clientPosition);
	virtual void StopMove();
	virtual void ChangeDirection(DIRECTION d);
	virtual void SetShield(bool active);
	virtual void AttackedHp(float damage) override;
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
	float GetShield() { return m_Shield; }
protected:
	std::pair<float, XMFLOAT3> GetNormalVectorSphere(XMFLOAT3& point);
public:
	virtual void Skill_1() = 0;
	virtual void Skill_2() = 0;
	bool IsDurationEndTimeSkill_1();
	bool IsDurationEndTimeSkill_2();
	virtual void ExecuteCommonAttack(XMFLOAT3& attackDir, int power) = 0;
protected://collision
	virtual std::pair<bool, XMFLOAT3> CheckCollisionMap_Boss(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual std::pair<bool, XMFLOAT3> CheckCollisionMap_Stage(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
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
	virtual void ExecuteCommonAttack(XMFLOAT3& attackDir, int power) override;
};

class MageSessionObject : public ChracterSessionObject
{
public:
	MageSessionObject(int id) :ChracterSessionObject(ROLE::PRIEST)
	{
		m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(7) };
		m_skillDuration = { std::chrono::seconds(9), std::chrono::seconds(3) };
		m_prevSkillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0],
			std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
		m_CommonAttackCoolTime = std::chrono::seconds(1);
		m_prevCommonAttackTime = std::chrono::high_resolution_clock::now() - m_CommonAttackCoolTime;
		SetStage_1Position();
	}
	~MageSessionObject() {}
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;
	void Skill_1()override;
	void Skill_2()override;
	virtual void ExecuteCommonAttack(XMFLOAT3& attackDir, int power) override;
};

class TankerSessionObject : public ChracterSessionObject
{
public:
	TankerSessionObject(int id) :ChracterSessionObject(ROLE::TANKER)
	{
		m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(0) };
		m_skillDuration = { std::chrono::seconds(5), std::chrono::seconds(0) };
		m_prevSkillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0],
			std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
		m_CommonAttackCoolTime = std::chrono::seconds(1);
		m_prevCommonAttackTime = std::chrono::high_resolution_clock::now() - m_CommonAttackCoolTime;
		SetStage_1Position();
	}
	~TankerSessionObject() {}
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;
	void Skill_1()override;
	void Skill_2()override;
	virtual void ExecuteCommonAttack(XMFLOAT3& attackDir, int power) override;
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
	virtual void ExecuteCommonAttack(XMFLOAT3& attackDir, int power) override;
};
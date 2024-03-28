#pragma once
#include "Character.h"
class Player : public Character
{
public:
	Player();
	Player(ROLE r);
	~Player();
protected:
	XMFLOAT3	m_xmf3RotateAxis;	// XMFLOAT3(0, 0, 1)로부터 회전한 각도	
	bool		m_bQSkillClicked = false;
	bool		m_bESkillClicked = false;
	bool		m_bShieldActive = false;
	float		m_fShield = false;
	bool		m_bCanAttack = true;

protected:
	std::array<std::chrono::seconds, 2> m_skillDuration;
	std::array<std::chrono::seconds, 2> m_skillCoolTime;
	std::array<std::chrono::high_resolution_clock::time_point, 2> m_skillInputTime;
public:
	virtual void Reset();
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	virtual void FirstSkillDown() { m_bQSkillClicked = true; }
	virtual void SecondSkillDown() { m_bESkillClicked = true; };
	virtual void StartEffect(int nSkillNum) {};
	virtual void EndEffect(int nSkillNum) {};
	virtual bool CanMove() { return true; }
	void VisualizeSkillCoolTime();

protected:
	void ChangeAnimation(pair< CharacterAnimation, CharacterAnimation> nextAnimation);
	float GetAnimationProgressRate(CharacterAnimation nAnimation);
	virtual void SetLookDirection() = 0;

public:
	bool GetQSkillState() { return m_bQSkillClicked; }
	bool GetESkillState() { return m_bESkillClicked; }
	bool GetOnAttack() { return m_bOnAttack; }
	bool GetShieldActive() { return m_bShieldActive; }
	float GetShield() { return m_fShield; }
	XMFLOAT3& GetRotateAxis() { return m_xmf3RotateAxis; }

public:
	void AddDirection(DIRECTION d) {
		m_currentDirection = (DIRECTION)(m_currentDirection | d);
		SetLookDirection();
	}
	void RemoveDIrection(DIRECTION d) {
		m_currentDirection = (DIRECTION)(m_currentDirection ^ d);
		SetLookDirection();
	}
	void SetStopDirection() {
		m_currentDirection = DIRECTION::IDLE;
	}

public:
	void SetRotateAxis(XMFLOAT3& xmf3RotateAxis)
	{
		m_xmf3RotateAxis = xmf3RotateAxis;
		SetLookDirection();
	}
	void SetShieldActive(bool bActive) { m_bShieldActive = bActive; }
	void SetShield(float fShield) { m_fShield = fShield; }
};


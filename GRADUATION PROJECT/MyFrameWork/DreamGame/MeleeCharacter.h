#pragma once
#include "Player.h"

class MeleeCharacter : public Player
{
public:
	MeleeCharacter();
	MeleeCharacter(ROLE r);
	~MeleeCharacter();

protected:
	virtual void Move(float fTimeElapsed);
	virtual void SetLookDirection();
};

class Warrior : public MeleeCharacter
{
private:
	int		m_iAttackType = 0;
	bool	m_bAnimationLock = false;
	bool	m_bComboAttack = false;
	CharacterAnimation m_attackAnimation = CharacterAnimation::CA_ATTACK;
	CharacterAnimation m_nextAnimation = CharacterAnimation::CA_NOTHING;

public:
	Warrior();
	virtual ~Warrior();
	virtual void Reset();
	virtual void Attack();
	virtual void Animate(float fTimeElapsed) override;
	virtual void SetLButtonClicked(bool bLButtonClicked);
	virtual void FirstSkillDown();
	virtual bool CanMove() { return !m_bQSkillClicked; }

	virtual void RecvFirstSkill(const high_resolution_clock::time_point& serverTime) override;
	virtual void RecvSecondSkill(const high_resolution_clock::time_point& serverTime) override;
public:
	void SetStage1Position();
	void SetBossStagePostion();
};

class Tanker : public MeleeCharacter
{
private:
	bool m_CanActiveQSkill = false;

public:
	Tanker();
	virtual ~Tanker();
	virtual void Reset();
	virtual void Attack();
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void FirstSkillDown();
	virtual void SecondSkillDown();
	virtual void Animate(float fTimeElapsed);
	virtual void SetSkillBall(Projectile* pBall);
	virtual void StartEffect(int nSkillNum);
	virtual void EndEffect(int nSkillNum);
	virtual bool CanMove() { return !m_bQSkillClicked && !m_bESkillClicked; }

	virtual void RecvFirstSkill(const high_resolution_clock::time_point& serverTime) override;
	virtual void RecvSecondSkill(const high_resolution_clock::time_point& serverTime) override;
public:
	void SetStage1Position();
	void SetBossStagePostion();
};

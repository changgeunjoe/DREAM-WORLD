#pragma once
#include "Character.h"

class Monster : public Character
{
public:
	XMFLOAT3 m_desDirecionVec = XMFLOAT3(0, 0, 1);
	XMFLOAT3 m_serverDesDirecionVec = XMFLOAT3(0, 0, 1);

public:
	std::list<int> m_BossRoute;
	std::mutex m_lockBossRoute;

public:
	GameObject* m_pSkillRange{ nullptr };
	float		m_fSkillTime{ 0.0f };
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };
	int m_astarIdx = -1;

public:
	Monster();
	virtual ~Monster();
	virtual void Animate(float fTimeElapsed);
	virtual void SetSkillRangeObject(GameObject* obj) { m_pSkillRange = obj; }
	virtual void Move(float fTimeElapsed)override;
	virtual void MoveForward(int forwardDirection = 1, float ftimeElapsed = 0.01768f) override;
	//void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)override;
};

class NormalMonster : public Character
{
private:
	bool	m_bHaveTarget{ false };
	int		m_iTargetID{ -1 };
	XMFLOAT3 m_desPos = XMFLOAT3(0, 0, 0);

public:
	int		m_nID = -1;
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };

public:
	NormalMonster();
	virtual ~NormalMonster();
	void SetAnimation();
	void SetDesPos(const XMFLOAT3& desPos) { m_desPos = desPos; }

public:
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed) override;
	//void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)override;
};

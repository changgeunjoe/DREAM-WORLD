#pragma once
#include "player.h"
class RangedCharacter : public Player
{
public:
	RangedCharacter();
	RangedCharacter(ROLE role);
	~RangedCharacter();

public:
	virtual void Move(float fTimeElapsed);
	virtual void SetLookDirection();
};

class Arrow;
class Archer : public RangedCharacter
{
private:
	XMFLOAT3 m_CameraLook;
	bool m_bZoomInState;
	float m_fZoomInStartTime = 0.0f;

public:
	array<Arrow*, 3> m_ppArrowForQSkill;
	array<Arrow*, 15> m_ppArrowForESkill;
	XMFLOAT3 m_xmf3TargetPos;

public:
	Archer();
	virtual ~Archer();
	virtual void Reset();
	virtual void Attack();
	virtual void SetArrow(Projectile** pArrow);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void ZoomInCamera();
	virtual void FirstSkillDown();
	virtual void SecondSkillDown();
	virtual void ShootArrow();
	virtual void ShootArrow(const XMFLOAT3& xmf3Direction);
	virtual void SetLButtonClicked(bool bLButtonClicked);

public:
	void SetAdditionArrowForQSkill(Arrow** ppArrow);
	void SetAdditionArrowForESkill(Arrow** ppArrow);
	void ResetArrow();

public:
	void SetStage1Position();
	void SetBossStagePostion();
};

class Priest : public RangedCharacter
{
private:
	GameObject* m_pHealRange{ nullptr };
	float		m_fHealTime{ 0.0f };

public:
	Priest();
	virtual ~Priest();
	virtual void Reset();
	virtual void Attack();
	virtual void Attack(const XMFLOAT3& xmf3Direction);
	virtual void SetProjectile(Projectile** pEnergyBall);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void FirstSkillDown();
	virtual void SecondSkillDown();
	virtual void SetSkillRangeObject(GameObject* obj) { m_pHealRange = obj; }
	virtual void StartEffect(int nSkillNum);
	virtual void EndEffect(int nSkillNum);

public:
	void UpdateEffect();
	void SetStage1Position();
	void SetBossStagePostion();
};
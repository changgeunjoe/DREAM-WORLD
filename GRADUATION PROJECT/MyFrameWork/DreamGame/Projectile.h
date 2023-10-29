#pragma once
#include "GameObject.h"

class Projectile : public GameObject
{
public:
	XMFLOAT3	m_xmf3startPosition;
	XMFLOAT3	m_xmf3direction;
	XMFLOAT4X4	m_xmf4x4Transform;
	float		m_fSpeed;
	float		m_Angle;
	bool		m_bActive;
	ROLE		m_HostRole{ ROLE::NONE_SELECT };
public:
	Projectile(entity_id eid = UNDEF_ENTITY);
	virtual ~Projectile();
	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void SetRButtonClicked(bool clicked) {};
	virtual bool GetRButtonClicked() { return false; }
	virtual void Move(XMFLOAT3 dir, float fDistance);
	virtual void SetActive(bool bActive) { m_bActive = bActive; }
	virtual void SetHostRole(ROLE r) { m_HostRole = r; }
	virtual void SetSpeed(float speed) { m_fSpeed = speed; }
	virtual void Reset();
};

class Arrow : public Projectile
{
public:
	bool		m_RButtonClicked;
	int			m_iArrowType;
	float		m_fArrowPos;
	XMFLOAT3	m_xmf3TargetPos;
	XMFLOAT3	m_xmf3Offset;
public:
	Arrow();
	virtual ~Arrow();
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual bool GetRButtonClicked() { return m_RButtonClicked; }
	virtual void SetActive(bool bActive);
	virtual void Reset();
};

class IceLance : public Projectile
{
private:
	float m_fDistance = 0.0f;
	float m_fTime = 0.0f;
public:
	IceLance();
	virtual ~IceLance();
	virtual void Animate(float fTimeElapsed);
};

class EnergyBall : public Projectile
{
public:
	float		m_fProgress;
	ROLE		m_Target;
public:
	EnergyBall();
	virtual ~EnergyBall();
	virtual void Animate(float fTimeElapsed);
	void SetTarget(ROLE r) { m_Target = r; }
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void Reset();
};

class RockSpike : public Projectile
{
public:
	GameObject* m_pAttackedArea = nullptr;
public:
	RockSpike();
	virtual ~RockSpike();
	virtual void Move(XMFLOAT3 dir, float fDistance);
	virtual void Animate(float fTimeElapsed);
	virtual void Reset();
};

#pragma once
#include "GameObject.h"

class Character : public GameObject
{
public:
	Character();
	virtual ~Character();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	//virtual void Move(DIRECTION direction, float fDistance);
};

class Warrior : public Character
{
public:
	Warrior();
	virtual ~Warrior();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void Move(DIRECTION direction, float fDsitance) override;
	virtual void Animate(float fTimeElapsed);
};

class Archer : public Character
{
public:
	Archer();
	virtual ~Archer();
	virtual void Attack();
	virtual void SetArrow(Arrow* pArrow);
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void Move(DIRECTION direction, float fDsitance) override;
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
};

class Arrow : public GameObject
{
public:
	XMFLOAT3 m_xmf3startPosition;
	XMFLOAT3 m_xmf3direction;
	bool m_bActive = false;
public:
	Arrow();
	virtual ~Arrow();
	virtual void Animate(float fTimeElapsed);
	void ShootArrow(const XMFLOAT3& xmf3StartPos, const XMFLOAT3& xmf3direction);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
};

class Tanker : public Character
{
public:
	Tanker();
	virtual ~Tanker();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void Move(DIRECTION direction, float fDsitance) override;
	virtual void Animate(float fTimeElapsed);
};

class Priest : public Character
{
public:
	Priest();
	virtual ~Priest();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void Move(DIRECTION direction, float fDsitance) override;
	virtual void Animate(float fTimeElapsed);
};

class Monster : public Character
{
public:
	Monster();
	virtual ~Monster();
	virtual void Animate(float fTimeElapsed);
};
#pragma once
#include "GameObject.h"

class Character : public GameObject
{
protected:
	XMFLOAT3	m_xmf3RotateAxis;	// XMFLOAT3(0, 0, 1)로부터 회전한 각도	
	bool m_bQSkillClicked;
	bool m_bESkillClicked;
	bool m_bOnAttack;
	bool m_bOnSkill = false;
public:
	Character();
	virtual ~Character();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	virtual void Reset();
	virtual void FirstSkillDown() { m_bQSkillClicked = true; }
	virtual void FirstSkillUp() {};
	virtual void SecondSkillDown() { m_bESkillClicked = true; };
	virtual void SecondSkillUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f }) {};
	virtual void MoveObject();
	bool CheckAnimationEnd(int nAnimation);
public:
	virtual void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos);
public:
	bool GetQSkillState() { return m_bQSkillClicked; }
	bool GetESkillState() { return m_bESkillClicked; }
	bool GetOnAttack() { return m_bOnAttack; }
	void SetRotateAxis(XMFLOAT3& xmf3RotateAxis) { m_xmf3RotateAxis = xmf3RotateAxis; }
	XMFLOAT3& GetRotateAxis() { return m_xmf3RotateAxis; }
public:
	virtual void Move(float fDsitance) = 0;
protected:
	DIRECTION m_currentDirection = DIRECTION::IDLE;
public:
	void AddDirection(DIRECTION d)
	{
		m_currentDirection = (DIRECTION)(m_currentDirection | d);
	}
	void RemoveDIrection(DIRECTION d)
	{
		m_currentDirection = (DIRECTION)(m_currentDirection ^ d);
	}
	void SetStopDirection() {
		m_currentDirection = DIRECTION::IDLE;
	}
	//DIRECTION m_prevDirection = DIRECTION::IDLE;
	//virtual void Move(DIRECTION direction, float fDistance);
};

class Warrior : public Character
{
public:
	Warrior();
	virtual ~Warrior();
	virtual void Attack(float fSpeed = 150.0f);
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void Move(float fDsitance)override;
	virtual void Animate(float fTimeElapsed) override;
};

class Archer : public Character
{
private:
	XMFLOAT3 m_CameraLook;
public:
	Archer();
	virtual ~Archer();
	virtual void Attack(float fSpeed = 150.0f);
	virtual void SetArrow(Projectile* pArrow);
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	virtual void Move(float fDsitance)override;
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void MoveObject();
	virtual void FirstSkillDown();
	virtual void FirstSkillUp();
	virtual void SecondSkillDown();
	virtual void SecondSkillUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	virtual void ShootArrow();
	// virtual void ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent);
};

class Tanker : public Character
{
public:
	Tanker();
	virtual ~Tanker();
	virtual void Attack(float fSpeed = 150.0f);
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void Move(float fDsitance)override;
	virtual void Animate(float fTimeElapsed);
};

class Priest : public Character
{
private:
	GameObject* m_pHealRange{ nullptr };
	float		m_fHealTime{ 0.0f };
public:
	Priest();
	virtual ~Priest();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void Attack(float fSpeed = 150.0f);
	virtual void SetEnergyBall(Projectile* pEnergyBall);
	virtual void Move(float fDsitance)override;
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void MoveObject();
	virtual void FirstSkillDown();
	virtual void FirstSkillUp();
	virtual void SecondSkillDown() {};
	virtual void SecondSkillUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f }) {};
	virtual void SetSkillRangeObject(GameObject* obj) { m_pHealRange = obj; }
	// virtual void ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent);
};

class Monster : public Character
{
public:
	GameObject* m_pSkillRange{ nullptr };
	float		m_fSkillTime{ 0.0f };
public:
	Monster();
	virtual ~Monster();
	virtual void Animate(float fTimeElapsed);
	virtual void SetSkillRangeObject(GameObject* obj) { m_pSkillRange = obj; }
	virtual void Move(float fDsitance)override;
	void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos)override;
public:
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };
};

class NormalMonster : public Character
{
private:
	bool	m_bHaveTarget{ false };
	bool	m_bCanActive{ false };
	int		m_iTargetID{ -1 };
	XMFLOAT3 m_desPos = XMFLOAT3(0, 0, 0);	
public:
	void SetDesPos(XMFLOAT3& desPos) { m_desPos = desPos; }
public:
	int		m_nID = -1;
	NormalMonster();
	virtual ~NormalMonster();
	virtual void Animate(float fTimeElapsed) override;
	virtual void Move(float fDsitance)override;
	void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos)override;
public:
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };
private:
	std::pair<float, XMFLOAT3> GetNormalVectorSphere(const XMFLOAT3& point);
};

class Projectile : public GameObject
{
public:
	XMFLOAT3	m_xmf3startPosition;
	XMFLOAT3	m_xmf3direction;
	XMFLOAT4X4	m_xmf4x4Transform;
	float		m_fSpeed;
	bool		m_bActive;
	bool		m_RAttack;
	float		m_Angle;

public:
	Projectile(entity_id eid = UNDEF_ENTITY);
	virtual ~Projectile();
	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void SetRButtonClicked(bool clicked) {};
	virtual bool GetRButtonClicked() { return false; }
};

class Arrow : public Projectile
{
public:
	int			m_ArrowType;
	float		m_ArrowPos;
	bool		m_RButtonClicked;
	XMFLOAT3	m_xmf3TargetPos;
public:
	Arrow();
	virtual ~Arrow();
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void SetRButtonClicked(bool clicked) { m_RButtonClicked = clicked; }
	virtual bool GetRButtonClicked() { return m_RButtonClicked; }
};

class EnergyBall : public Projectile
{
public:
	EnergyBall();
	virtual ~EnergyBall();
	virtual void Animate(float fTimeElapsed);
	void Move(XMFLOAT3 dir, float fDistance);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
};

class TrailObject : public GameObject
{
public:

	float		m_Angle;
public:
	TrailObject(entity_id eid = UNDEF_ENTITY);
	virtual ~TrailObject();
	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};
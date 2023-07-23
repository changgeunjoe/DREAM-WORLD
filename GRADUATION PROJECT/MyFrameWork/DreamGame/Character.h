#pragma once
#include "GameObject.h"

class Character : public GameObject
{
protected:
	XMFLOAT3	m_xmf3RotateAxis;	// XMFLOAT3(0, 0, 1)로부터 회전한 각도	
	bool m_bQSkillClicked;
	bool m_bESkillClicked;
	bool m_bOnAttack = false;
	bool m_bOnSkill = false;
	bool m_bShieldActive = false;
	float m_fShield = false;
protected:
	std::array<std::chrono::seconds, 2> m_skillDuration;
	std::array<std::chrono::seconds, 2> m_skillCoolTime;
	std::array<std::chrono::high_resolution_clock::time_point, 2> m_skillInputTime;
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
	virtual void StartEffect(int nSkillNum) {};
	virtual void EndEffect(int nSkillNum) {};
	bool CheckAnimationEnd(int nAnimation);
protected:

public://move
	virtual void SetLookDirection();
	virtual void Move(float fTimeElapsed) = 0;
	virtual void MoveForward(int forwardDirection = 1, float ftimeElapsed = 0.01768f) override;
	virtual void MoveStrafe(int rightDirection = 1, float ftimeElapsed = 0.01768f)override;
	virtual void MoveDiagonal(int fowardDirection, int rightDirection, float ftimeElapsed = 0.01768f)override;
	virtual void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos);

protected://collision check
	virtual std::pair<bool, XMFLOAT3> CheckCollisionMap(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual std::pair<bool, XMFLOAT3> CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual std::pair<bool, XMFLOAT3> CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual bool CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
public:
	virtual void ExecuteSkill_Q();
	virtual void ExecuteSkill_E();
	bool GetQSkillState() { return m_bQSkillClicked; }
	bool GetESkillState() { return m_bESkillClicked; }
	bool GetOnAttack() { return m_bOnAttack; }
	bool GetShieldActive() { return m_bShieldActive; }
	float GetShield() { return m_fShield; }
	void SetOnAttack(bool onAttack) { m_bOnAttack = onAttack; }	
	XMFLOAT3& GetRotateAxis() { return m_xmf3RotateAxis; }
public:
	void SetRotateAxis(XMFLOAT3& xmf3RotateAxis) { m_xmf3RotateAxis = xmf3RotateAxis; }
	void SetShieldActive(bool bActive) { m_bShieldActive = bActive; }
	void SetShield(float fShield) { m_fShield = fShield; }
//movedir
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
protected:
	std::pair<float, XMFLOAT3> GetNormalVectorSphere(const XMFLOAT3& point);
};

class Warrior : public Character
{
public:
	Warrior();
	virtual ~Warrior();
	virtual void Attack();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed) override;
public:
	void SetStage1Position();
	void SetBossStagePostion();

//attack
public:
	virtual void ExecuteSkill_Q();
	virtual void ExecuteSkill_E();
};

class Archer : public Character
{
private:
	XMFLOAT3 m_CameraLook;
public:
	Archer();
	virtual ~Archer();
	virtual void Attack();
	virtual void SetArrow(Projectile* pArrow);
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void SetLookDirection() override;
	virtual void FirstSkillDown();
	virtual void FirstSkillUp();
	virtual void SecondSkillDown();
	virtual void SecondSkillUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	virtual void ShootArrow();
	virtual void ShootArrow(const XMFLOAT3& xmf3StartPos, const XMFLOAT3& xmf3Direction, const float fSpeed);
	// virtual void ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent);
public:
	void SetStage1Position();
	void SetBossStagePostion();
	//attack
public:
	virtual void ExecuteSkill_Q();
	virtual void ExecuteSkill_E();
};

class Tanker : public Character
{
public:
	Tanker();
	virtual ~Tanker();
	virtual void Attack();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void FirstSkillDown();
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed);
	virtual void SetSkillBall(Projectile* pBall);
	virtual void StartEffect(int nSkillNum);
	virtual void EndEffect(int nSkillNum);
public:
	void SetStage1Position();
	void SetBossStagePostion();
	//attack
public:
	virtual void ExecuteSkill_Q();
	virtual void ExecuteSkill_E();
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
	virtual void Attack();
	virtual void Attack(const XMFLOAT3& xmf3StartPos, const XMFLOAT3& xmf3Direction, const float fSpeed);
	virtual void SetEnergyBall(Projectile* pEnergyBall);
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void SetLookDirection() override;
	virtual void FirstSkillDown();
	virtual void FirstSkillUp();
	virtual void SecondSkillDown() {};
	virtual void SecondSkillUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f }) {};
	virtual void SetSkillRangeObject(GameObject* obj) { m_pHealRange = obj; }
	virtual void StartEffect(int nSkillNum);
	virtual void EndEffect(int nSkillNum);
	void UpdateEffect();
	// virtual void ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent);
public:
	void SetStage1Position();
	void SetBossStagePostion();
	//attack
public:
	virtual void ExecuteSkill_Q();
	virtual void ExecuteSkill_E();
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
	virtual void Move(float fTimeElapsed)override;
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
	virtual void Move(float fTimeElapsed)override;
	void SetAnimation();
	void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos)override;
public:
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };
private:
	virtual std::pair<bool, XMFLOAT3> CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual std::pair<bool, XMFLOAT3> CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f) override;
	virtual bool CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f) override;
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
	virtual void Move(XMFLOAT3 dir, float fDistance);
};

class Arrow : public Projectile
{
public:
	bool		m_RButtonClicked;
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
	float		m_fProgress;
	ROLE		m_Target;
	ROLE		m_HostRole{ ROLE::NONE_SELECT };
public:
	EnergyBall();
	virtual ~EnergyBall();
	virtual void Animate(float fTimeElapsed);
	void SetHostRole(ROLE r) { m_HostRole = r; }
	void SetTarget(ROLE r) { m_Target = r; }
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
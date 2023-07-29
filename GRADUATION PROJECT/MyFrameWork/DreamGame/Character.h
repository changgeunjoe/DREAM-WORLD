#pragma once
#include "GameObject.h"

class Character : public GameObject
{
protected:
	XMFLOAT3	m_xmf3RotateAxis;	// XMFLOAT3(0, 0, 1)로부터 회전한 각도	
	bool		m_bQSkillClicked;
	bool		m_bESkillClicked;
	bool		m_bOnAttack = false;
	bool		m_bOnSkill = false;
	bool		m_bShieldActive = false;
	float		m_fShield = false;
	bool		m_bCanAttack = true;
protected:
	std::array<std::chrono::seconds, 2> m_skillDuration;
	std::array<std::chrono::seconds, 2> m_skillCoolTime;
	std::array<std::chrono::high_resolution_clock::time_point, 2> m_skillInputTime;
public:
	Character();
	virtual ~Character();
	virtual GameObject* GetHpBar() { return m_pHPBarObject; };
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	virtual void Reset();
	virtual void FirstSkillDown() { m_bQSkillClicked = true; }
	virtual void FirstSkillUp() {};
	virtual void SecondSkillDown() { m_bESkillClicked = true; };
	virtual void SecondSkillUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f }) {};
	virtual void StartEffect(int nSkillNum) {};
	virtual void EndEffect(int nSkillNum) {};
protected:
	bool CheckAnimationEnd(int nAnimation);
	void ChangeAnimation(pair< CharacterAnimation, CharacterAnimation> nextAnimation);
	float GetAnimationProgressRate(CharacterAnimation nAnimation);

public://move
	virtual void SetLookDirection();
	virtual void Move(float fTimeElapsed) = 0;
	virtual void MoveForward(int forwardDirection = 1, float ftimeElapsed = 0.01768f) override;
	virtual void MoveStrafe(int rightDirection = 1, float ftimeElapsed = 0.01768f)override;
	virtual void MoveDiagonal(int fowardDirection, int rightDirection, float ftimeElapsed = 0.01768f)override;
	virtual void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec);

protected://collision check
	virtual std::pair<bool, XMFLOAT3> CheckCollisionMap_Boss(XMFLOAT3& normalVector, XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual std::pair<bool, XMFLOAT3> CheckCollisionMap_Stage(XMFLOAT3& normalVector, XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
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
public:
	GameObject* m_pHPBarObject{ NULL };
};

class Warrior : public Character
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
	virtual void Attack();
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed) override;
	virtual void SetLButtonClicked(bool bLButtonClicked);
	virtual void FirstSkillDown();
public:
	void SetStage1Position();
	void SetBossStagePostion();

	//attack
public:
	virtual void ExecuteSkill_Q();
	virtual void ExecuteSkill_E();
};

class Arrow;
class Archer : public Character
{
private:
	XMFLOAT3 m_CameraLook;
	bool m_bZoomInState;
public:
	array<Arrow*, 3> m_ppArrowForQSkill;
	array<Arrow*, 15> m_ppArrowForESkill;
	XMFLOAT3 m_xmf3TargetPos;

public:
	Archer();
	virtual ~Archer();
	virtual void Attack();
	virtual void RbuttonClicked(float fTimeElapsed) {};
	virtual void SetArrow(Projectile** pArrow);
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void SetLookDirection() override;
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
private:
	bool m_CanActiveQSkill = false;
public:
	Tanker();
	virtual ~Tanker();
	virtual void Attack();
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void FirstSkillDown();
	virtual void SecondSkillDown();
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
	virtual void Attack();
	virtual void Attack(const XMFLOAT3& xmf3Direction);
	virtual void SetProjectile(Projectile** pEnergyBall);
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void SetLookDirection() override;
	virtual void FirstSkillDown();
	virtual void SecondSkillDown();
	virtual void SetSkillRangeObject(GameObject* obj) { m_pHealRange = obj; }
	virtual void StartEffect(int nSkillNum);
	virtual void EndEffect(int nSkillNum);
public:
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
	XMFLOAT3 m_desDirecionVec = XMFLOAT3(0, 0, 1);
	XMFLOAT3 m_serverDesDirecionVec = XMFLOAT3(0, 0, 1);
public:
	std::list<int> m_BossRoute;
	std::mutex m_lockBossRoute;
public:
	GameObject* m_pSkillRange{ nullptr };
	float		m_fSkillTime{ 0.0f };
	int m_astarIdx = -1;
public:
	Monster();
	virtual ~Monster();
	virtual void Animate(float fTimeElapsed);
	virtual void SetSkillRangeObject(GameObject* obj) { m_pSkillRange = obj; }
	virtual void Move(float fTimeElapsed)override;
	virtual void MoveForward(int forwardDirection = 1, float ftimeElapsed = 0.01768f) override;
	void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)override;
public:
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };
};

class NormalMonster : public Character
{
private:
	bool	m_bHaveTarget{ false };
	bool	m_bIsAlive{ true };
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
	void SetAliveState(bool bAlive) { m_bIsAlive = bAlive; }
	bool GetAliveState() { return m_bIsAlive; }
	void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)override;
public:
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };
private:
	virtual std::pair<bool, XMFLOAT3> CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual std::pair<bool, XMFLOAT3> CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f) override;
	virtual bool CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f) override;
};

class NpcCharacter : public Character
{
public:
	virtual void Move(float fTimeElapsed) {};
};

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
};

class IceLance : public Projectile
{
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
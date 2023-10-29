#pragma once
#include "GameObject.h"

class Character : public GameObject
{
protected:
	bool		m_bOnAttack = false;
	bool		m_bOnSkill = false;
public:
	GameObject* m_pHPBarObject{ NULL };
public:
	Character();
	Character(ROLE r);
	virtual ~Character();
	virtual GameObject* GetHpBar() { return m_pHPBarObject; };
	virtual void Reset();
protected:
	std::atomic<DIRECTION> m_currentDirection = DIRECTION::IDLE;
	ROLE m_role;
public://move
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
	std::pair<bool, XMFLOAT3> CheckCollisionBoss(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	virtual bool CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
	bool CheckAnimationEnd(int nAnimation);
public:
	void SetOnAttack(bool onAttack) { m_bOnAttack = onAttack; }
protected:
	std::pair<float, XMFLOAT3> GetNormalVectorSphere(const XMFLOAT3& point);
};

class NpcCharacter : public Character
{
public:
	virtual void Move(float fTimeElapsed) {};
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
#pragma once
#include "GameObject.h"

namespace CharacterEvent {
	class InterpolateData;
}
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
public://move
	virtual void Move(float fTimeElapsed) = 0;
	virtual void MoveForward(int forwardDirection = 1, float ftimeElapsed = 0.01768f) override;
	virtual void MoveStrafe(int rightDirection = 1, float ftimeElapsed = 0.01768f)override;
	virtual void MoveDiagonal(int fowardDirection, int rightDirection, float ftimeElapsed = 0.01768f)override;
	virtual void SetInterpolateData(const chrono::high_resolution_clock::time_point& recvTime, const XMFLOAT3& recvPos);

	void SetAliveState(bool bAlive) { m_bIsAlive = bAlive; }
	bool GetAliveState() { return m_bIsAlive; }
protected:
	virtual std::optional<std::pair<bool, XMFLOAT3>> CheckCollisionMap_Boss(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, float ftimeElapsed = 0.01768f);
	virtual std::optional<std::pair<bool, XMFLOAT3>> CheckCollisionMap_Stage(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, float ftimeElapsed = 0.01768f);

	virtual std::optional<std::pair<bool, XMFLOAT3>> CheckCollisionCharacterObject(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, const bool& isApplySlidingVector, float ftimeElapsed = 0.01768f);
	std::optional < std::pair<bool, XMFLOAT3>> CheckCollisionBoss(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, float ftimeElapsed = 0.01768f);

	void UpdateInterpolateData();
	bool CheckAnimationEnd(int nAnimation);
public:
	void SetOnAttack(bool onAttack) { m_bOnAttack = onAttack; }
	const float GetDistance(const XMFLOAT3& otherPosition) const;
	const XMFLOAT3 GetToVector(const XMFLOAT3& otherPosition) const;
	const XMFLOAT3 GetFromVector(const XMFLOAT3& otherPosition) const;
protected:
	std::atomic<DIRECTION> m_currentDirection = DIRECTION::IDLE;
	ROLE m_role;
	bool	m_bIsAlive{ true };
	std::shared_ptr<CharacterEvent::InterpolateData> m_interpolateData;
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
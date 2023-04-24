#pragma once
#include "SessionObject.h"
#include "../../PCH/stdafx.h" // Áö¿ö¾ßµÊ

class MonsterSessionObject : public SessionObject
{
public:
	MonsterSessionObject();
	MonsterSessionObject(std::string& roomId);
	virtual ~MonsterSessionObject();

private:	
	DirectX::XMFLOAT3 m_DestinationPos = { 0,0,0 };
	std::mutex m_restRotateAngleLock;
	DirectX::XMFLOAT3 m_RestRotateAngle = { 0,0,0 };
private:
	int m_aggroPlayerId = -1;	
public:
	bool isMove = false;
	bool isAttack = false;
	BOSS_ATTACK currentAttack = BOSS_ATTACK::ATTACK_COUNT;
	DirectX::BoundingSphere m_SPBB = BoundingSphere(DirectX::XMFLOAT3(0.0f, 22.5f, 0.0f), 22.5f);
public:
	virtual void AutoMove() override;
	virtual void StartMove(DIRECTION d) override;
	virtual void StopMove() override;
	virtual void ChangeDirection(DIRECTION d) override;
public:
	virtual const DirectX::XMFLOAT3 GetPosition() override;
	virtual const DirectX::XMFLOAT3 GetRotation() override;
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	void SetDirection(DIRECTION d);
	void Move(float fDistance, float elapsedTime);
public:
	void SetDestinationPos(DirectX::XMFLOAT3 des) {		
		m_DestinationPos = des;
	}
	void SetRestRotateAngle(ROTATE_AXIS axis, float angle) {
		switch (axis)
		{
		case X:
			m_RestRotateAngle.x = angle;
			break;
		case Y:
			m_RestRotateAngle.y = angle;
			break;
		case Z:
			m_RestRotateAngle.z = angle;
			break;
		default:
			break;
		}
	}
public:
	DirectX::XMFLOAT3 GetLook() { return m_directionVector; }
public:
	void SetAggroPlayerId(int id);
	int GetAggroPlayerId() { return m_aggroPlayerId; }
	void AttackPlayer();
};

#pragma once
#ifdef _DEBUG
#include "../../PCH/stdafx.h"
#endif
class SessionObject
{
protected:
	std::atomic<float>	m_hp;
	float	m_maxHp;
	float	m_attackDamage;
	float m_speed = 50.0f;
	int m_roomId = -1;
protected:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotateAngle = { 0,0,0 };
	DirectX::XMFLOAT3 m_directionVector = DirectX::XMFLOAT3{ 0,0,1 };
	DirectX::XMFLOAT3 m_rightVector = DirectX::XMFLOAT3{ 1,0,0 };

	DirectX::XMFLOAT4X4 m_worldMatrix = Matrix4x4::Identity();
	std::chrono::high_resolution_clock::time_point m_lastMoveTime;

	float                           m_fBoundingSize{ 8.0f };
	BoundingSphere					m_SPBB = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), m_fBoundingSize);
public:
	SessionObject();
	SessionObject(float boundingSize);
	virtual ~SessionObject();
public:
	BoundingSphere& GetSpbb() { return m_SPBB; }
	float GetHp() { return m_hp; }
	float GetMaxHp() { return m_maxHp; }
	XMFLOAT3& GetPos() { return m_position; }
	XMFLOAT3& GetRot() { return m_rotateAngle; }
	XMFLOAT3& GetDirectionVector() { return m_directionVector; }

	void HealHp(float heal) { if (m_hp < m_maxHp) m_hp += heal; }
	float GetAttackDamage() { return m_attackDamage; }
public:
	void AutoMove();
	virtual bool Move(float elapsedTime) = 0;
	virtual void AttackedHp(float damage) { m_hp -= damage; }
public:
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) = 0;
protected:
	void CalcRightVector()
	{
		m_rightVector = Vector3::CrossProduct(DirectX::XMFLOAT3(0, 1, 0), m_directionVector);
	}
protected:
	virtual void SetPosition(DirectX::XMFLOAT3& pos)
	{
		m_position = pos;
		m_SPBB.Center = pos;
		m_SPBB.Center.y = m_fBoundingSize;
	}
public:
	void SetInitPosition(DirectX::XMFLOAT3& pos)
	{
		m_position = pos;
		m_SPBB.Center = pos;
		m_SPBB.Center.y = m_fBoundingSize;
	}
	void SetRoomId(int roomId) { m_roomId = roomId; }
};

#pragma once
#ifdef _DEBUG
#include "../../PCH/stdafx.h"
#endif
class SessionObject
{
protected:
	short	m_hp;
	short	m_maxHp;
	short	m_attackDamage;
	int		m_roomId;
protected:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotateAngle = { 0,0,0 };
	DirectX::XMFLOAT3 m_directionVector = DirectX::XMFLOAT3{ 0,0,1 };
	DirectX::XMFLOAT3 m_rightVector = DirectX::XMFLOAT3{ 1,0,0 };

	DirectX::XMFLOAT4X4 m_worldMatrix = Matrix4x4::Identity();
	std::chrono::high_resolution_clock::time_point m_lastMoveTime;
public:
	SessionObject();
	SessionObject(int roomId) : m_roomId(roomId) {};
	virtual ~SessionObject();
public:
	short GetHp() { return m_hp; }
	XMFLOAT3& GetPos() { return m_position; }
	XMFLOAT3& GetRot() { return m_rotateAngle; }
	XMFLOAT3& GetDirectionVector() { return m_directionVector; }

	void AttackedHp(short damage) { m_hp -= damage; };
	short GetAttackDamage() { return m_attackDamage; }
public:
	virtual void AutoMove() = 0;
	virtual void StartMove(DIRECTION d) = 0;
	virtual void StopMove() = 0;
	virtual void ChangeDirection(DIRECTION d) = 0;
public:
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) = 0;
protected:
	void CalcRightVector()
	{
		m_rightVector = Vector3::CrossProduct(DirectX::XMFLOAT3(0, 1, 0), m_directionVector);
	}
protected:
	virtual void SetPosition(DirectX::XMFLOAT3 pos) { m_position = pos; }
	virtual void SetPosition(DirectX::XMFLOAT3& pos) { m_position = pos; }
public:
	void SetRoomId(int& roomId)
	{
		m_roomId = roomId;
	}
	int GetRoomId() { return m_roomId; }
};

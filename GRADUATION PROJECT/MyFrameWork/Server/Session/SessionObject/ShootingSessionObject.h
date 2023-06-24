#pragma once
#ifdef _DEBUG
#include "../../PCH/stdafx.h"
#endif
#include "SessionObject.h"

class Session;
class MonsterSessionObject;

class ShootingSessionObject : public SessionObject
{
private:
	int m_id = -1;
	float m_distance = 0.0f;
	BoundingSphere m_SPBB;
public:
	bool m_active = false;
	ShootingSessionObject();
	ShootingSessionObject(int& roomId);
	virtual ~ShootingSessionObject();
private:	
	float m_speed;
public:
	virtual void AutoMove();
	virtual void StartMove(DIRECTION d);
	virtual void StopMove();
	virtual void ChangeDirection(DIRECTION d);
	virtual void Rotate(ROTATE_AXIS axis, float angle);
public:
	virtual const DirectX::XMFLOAT3 GetPosition();
	virtual const DirectX::XMFLOAT3 GetRotation();
	int DetectCollision(MonsterSessionObject* m_bossSession);
	void SetSpeed(float speed);
	void SetStart(XMFLOAT3& dir, XMFLOAT3& srcPos, float speed);
	void Move(float distance);
public:
	void SetInfo(int& roomId, int idx)
	{
		m_id = idx;
		m_roomId = roomId;
	}
	int GetId() { return m_id; }
};


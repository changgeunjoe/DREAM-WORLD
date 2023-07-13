#pragma once
#ifdef _DEBUG
#include "../../PCH/stdafx.h"
#endif
#include "SessionObject.h"

class UserSession;
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
	virtual ~ShootingSessionObject();
	void Rotate(ROTATE_AXIS axis, float angle) override {}
public:
	virtual const DirectX::XMFLOAT3 GetPosition();
	virtual const DirectX::XMFLOAT3 GetRotation();
	int DetectCollision(MonsterSessionObject* m_bossSession);
	void SetSpeed(float speed);
	void SetStart(XMFLOAT3& dir, XMFLOAT3& srcPos, float speed);
	void Move(float elapsedTime) override;
public:
	void SetInfo(int idx)
	{
		m_id = idx;
	}
	int GetId() { return m_id; }
};


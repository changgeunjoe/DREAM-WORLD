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
	float m_damage = 50.0f;
	ROLE m_OwnerRole = ROLE::NONE_SELECT;
	bool m_isSkill = false;
public:
	bool m_active = false;

public:
	ShootingSessionObject();
	virtual ~ShootingSessionObject();
	void Rotate(ROTATE_AXIS axis, float angle) override {}
public:
	virtual const DirectX::XMFLOAT3 GetPosition();
	virtual const DirectX::XMFLOAT3 GetRotation();
	int DetectCollision(MonsterSessionObject* m_bossSession);
	void SetSpeed(float speed);
	void SetStart(XMFLOAT3& dir, XMFLOAT3& srcPos, float speed);
	bool Move(float elapsedTime) override;
public:
	void SetDamage(float damage) { m_damage = damage; }
	void SetOwnerRole(ROLE r) {
		m_OwnerRole = r;
	}
	void SetUseSkill(bool b) { m_isSkill = b; }
public:
	void SetInfo(int idx)
	{
		m_id = idx;
	}
	int GetId() { return m_id; }
};


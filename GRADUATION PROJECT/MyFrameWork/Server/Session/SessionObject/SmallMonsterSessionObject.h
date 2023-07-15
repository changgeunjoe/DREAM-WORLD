#pragma once
#include "SessionObject.h"
#include "../../PCH/stdafx.h" // �����ߵ�

class SmallMonsterSessionObject : public SessionObject
{
public:
	SmallMonsterSessionObject();
	SmallMonsterSessionObject(int roomId);
	virtual ~SmallMonsterSessionObject();
private:
	int m_roomId = -1;
	XMFLOAT3 m_desPos = XMFLOAT3(0, 0, 0);
	//float m_desDis = 0.0f;
	DirectX::BoundingSphere m_SPBB = BoundingSphere(DirectX::XMFLOAT3(0.0f, 15.0f, 0.0f), 15.0f);
public:
	std::chrono::high_resolution_clock::time_point m_lastAttackTime = std::chrono::high_resolution_clock::now();
	bool isMove = false;
public:
	void SetDestinationPos(XMFLOAT3* posArr);
	void StartMove();
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	bool Move(float elapsedTime) override;
public:
	DirectX::XMFLOAT3 GetLook() { return m_directionVector; }
public:
	void SetZeroHp() { m_hp = 0; }
	void SetRoomId(int roomId) { m_roomId = roomId; }
};

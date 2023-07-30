#pragma once
#include "SessionObject.h"
#include "../../PCH/stdafx.h" // Áö¿ö¾ßµÊ

class MonsterSessionObject : public SessionObject
{
public:
	MonsterSessionObject();
	MonsterSessionObject(int roomId);
	virtual ~MonsterSessionObject();
private:
	DirectX::XMFLOAT3 m_DestinationPos = { 0,0,0 };
	std::mutex m_restRotateAngleLock;
	DirectX::XMFLOAT3 m_RestRotateAngle = { 0,0,0 };
private:
	ROLE m_aggroPlayerRole = ROLE::NONE_SELECT;
	ROLE m_newAggroPlayerRole = ROLE::NONE_SELECT;
	std::mutex m_reserveRoadLock;
	std::list<int> m_ReserveRoad;
	std::atomic_bool m_changeRoad = false;
	std::atomic_int m_onIdx = -1;
public:
	XMFLOAT3 m_desVector = XMFLOAT3(0, 0, 1);
	std::atomic_bool  isMove = false;
	std::atomic_bool isAttack = false;
	std::atomic_bool isPhaseChange = false;

	BOSS_ATTACK currentAttack = BOSS_ATTACK::ATTACK_COUNT;
public:
	std::chrono::high_resolution_clock::time_point m_lastAttackTime = std::chrono::high_resolution_clock::now();
public:
	void StartMove();
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	bool Move(float elapsedTime) override;
public:
	void SetOnidx(int idx) { m_onIdx = idx; }
	int GetOnIdx() { return m_onIdx; }
	void SetDestinationPos();
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
	void ReserveAggroPlayerRole(ROLE r);
	void SetAggroPlayerRole();
	ROLE GetAggroPlayerRole() { return m_aggroPlayerRole; }
	ROLE GetNewAggroPlayerRole() { return m_newAggroPlayerRole; }
	void AttackTimer();
	void AttackPlayer();
	bool StartAttack();
	///////////////
	bool isBossDie = false;
	void SetZeroHp() { m_hp = 0; }
	void SetBossStagePosition();
};

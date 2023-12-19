//#pragma once
//#include "../SessionObject.h"
//#include "../../PCH/stdafx.h"
//
//class SmallMonsterSessionObject : public SessionObject
//{
//public:
//	SmallMonsterSessionObject();
//	SmallMonsterSessionObject(int roomId);
//	virtual ~SmallMonsterSessionObject();
//private:
//	int m_id = -1;
//	bool m_isAlive = true;
//	XMFLOAT3 m_desPos = XMFLOAT3(0, 0, 0);
//public:
//	std::chrono::high_resolution_clock::time_point m_lastAttackTime = std::chrono::high_resolution_clock::now();
//	bool isMove = false;
//public:
//	void SetDestinationPos(XMFLOAT3* posArr);
//	bool StartAttack();
//	void StartMove();
//public:
//	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
//public:
//	bool Move(float elapsedTime) override;
//public:
//	DirectX::XMFLOAT3 GetLook() { return m_directionVector; }
//public:
//	void SetZeroHp() { m_hp = 0; }
//	void SetId(int id) { m_id = id; }
//	XMFLOAT3 GetDesPos() { return m_desPos; }
//	bool IsAlive() { return m_isAlive; }
//	void UpdateMonsterState();
//	float GetDistance(const XMFLOAT3& point);
//	std::pair<float, XMFLOAT3> GetNormalVectorSphere(XMFLOAT3& point);
//protected:
//	virtual std::pair<bool, XMFLOAT3> CheckCollisionMap_Boss(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
//	virtual std::pair<bool, XMFLOAT3> CheckCollisionMap_Stage(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);	
//	virtual std::pair<bool, XMFLOAT3> CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
//	virtual std::pair<bool, XMFLOAT3> CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
//	virtual bool CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed = 0.01768f);
//};

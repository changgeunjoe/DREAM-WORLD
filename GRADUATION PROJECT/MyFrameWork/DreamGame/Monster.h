#pragma once
#include "Character.h"

enum class BOSS_STATE {
	IDLE,
	MOVE,
	MOVE_AGGRO,
	ATTACK
};

class Monster;
class BossEventBase : std::enable_shared_from_this<BossEventBase>
{
public:
	virtual void Execute(Monster* monster) = 0;
};

class BossSetDestinationPositionEvent : public BossEventBase
{
public:
	BossSetDestinationPositionEvent(const XMFLOAT3& destination) : destinationPosition(destination) {}
	virtual void Execute(Monster* monster) override;
private:
	XMFLOAT3 destinationPosition;
};

class BossSetAggroPositionEvent : public BossEventBase
{
public:
	BossSetAggroPositionEvent(const XMFLOAT3& aggroPosition) : aggroPosition(aggroPosition) {}
	virtual void Execute(Monster* monster) override;
private:
	XMFLOAT3 aggroPosition;
};

class BossAttackEventBase : public BossEventBase
{
public:
	virtual void Execute(Monster* monster) override;
};

class BossDirectionAttackEvent : public BossAttackEventBase
{
public:
	BossDirectionAttackEvent(const XMFLOAT3& direction) : direction(direction) {}
protected:
	XMFLOAT3 direction;
};

class BossFireAttackEvent : public BossAttackEventBase
{
public:
	virtual void Execute(Monster* monster) override;
};

class BossSpinAttackEvent : public BossAttackEventBase
{
public:
	virtual void Execute(Monster* monster) override;
};

class BossMeteorAttackEvent : public BossAttackEventBase
{
public:
	virtual void Execute(Monster* monster) override;
};

class BossKickAttackEvent : public BossDirectionAttackEvent
{
public:
	BossKickAttackEvent(const XMFLOAT3& direction) :BossDirectionAttackEvent(direction) {}
	virtual void Execute(Monster* monster) override;
};

class BossPunchAttackEvent : public BossDirectionAttackEvent
{
public:
	BossPunchAttackEvent(const XMFLOAT3& direction) :BossDirectionAttackEvent(direction) {}
	virtual void Execute(Monster* monster) override;
};

class Monster : public Character
{
	friend BossSetDestinationPositionEvent;
	friend BossSetAggroPositionEvent;
public:
	Monster();
	virtual ~Monster();
	virtual void Animate(float fTimeElapsed);
	virtual void SetSkillRangeObject(GameObject* obj) { m_pSkillRange = obj; }
	virtual void Move(float fTimeElapsed)override;
	virtual void MoveForward(int forwardDirection = 1, float ftimeElapsed = 0.01768f) override;



	//void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)override;
//public:
//	XMFLOAT3 m_desDirecionVec = XMFLOAT3(0, 0, 1);
//	XMFLOAT3 m_serverDesDirecionVec = XMFLOAT3(0, 0, 1);

//public:
//	std::list<int> m_BossRoute;
//	std::mutex m_lockBossRoute;

	void InsertEvent(std::shared_ptr<BossEventBase> bossEvent);
	void ChangeBossState(const BOSS_STATE& bossState);
private:
	void ProcessPrevEvent();
	void MoveAggro(float ftimeElapsed);
public:
	GameObject* m_pSkillRange{ nullptr };
	float		m_fSkillTime{ 0.0f };
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };
	//int m_astarIdx = -1;
private:
	XMFLOAT3 m_destinationPosition;
	XMFLOAT3 m_aggroPosition;
	BOSS_STATE m_bossState;

	Concurrency::concurrent_queue<std::shared_ptr< BossEventBase>> m_prevEventQueue;
	atomic_int m_prevEventQueueSize = 0;
};

class NormalMonster : public Character
{
private:
	bool	m_bHaveTarget{ false };
	int		m_iTargetID{ -1 };
	std::mutex m_destinationPositionLock;
	XMFLOAT3 m_desPos = XMFLOAT3(0, 0, 0);
public:
	int		m_nID = -1;
	XMFLOAT3 m_xmf3rotateAngle = XMFLOAT3{ 0,0,0 };

public:
	NormalMonster();
	virtual ~NormalMonster();
	void SetAnimation();
	void SetDesPos(const XMFLOAT3& desPos)
	{
		std::lock_guard<std::mutex> lg(m_destinationPositionLock);
		m_desPos = desPos;
	}
	const XMFLOAT3 GetDesPosition()
	{
		std::lock_guard<std::mutex> lg(m_destinationPositionLock);
		return m_desPos;
	}
public:
	virtual void Move(float fTimeElapsed)override;
	virtual void Animate(float fTimeElapsed) override;
	//void InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)override;
};

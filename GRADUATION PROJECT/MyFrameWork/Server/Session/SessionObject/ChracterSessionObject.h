#pragma once
#ifdef _DEBUG
#include "../../PCH/stdafx.h"
#endif
#include "SessionObject.h"


class ChracterSessionObject : public SessionObject
{
private:
	bool	m_leftmouseInput;
	bool	m_rightmouseInput;
private:
	ROLE m_InGameRole = ROLE::NONE_SELECT;
public:
	DIRECTION						m_inputDirection = DIRECTION::IDLE;
	DIRECTION						m_prevDirection = DIRECTION::IDLE;
private:
	ROOM_STATE						m_roomState = ROOM_STATE::ROOM_BOSS;
public:
	ChracterSessionObject(ROLE r);
	virtual	~ChracterSessionObject();
protected:
	virtual void SetPosition(DirectX::XMFLOAT3& pos) override {
		m_position = pos;
		m_SPBB.Center = pos;
		m_SPBB.Center.y = m_fBoundingSize;
	}
public:
	bool AdjustPlayerInfo(DirectX::XMFLOAT3& position); // , DirectX::XMFLOAT3& rotate
public:
	//virtual void AutoMove() override;
	virtual void StartMove(DIRECTION d);
	virtual void StopMove();
	virtual void ChangeDirection(DIRECTION d);
public:
	bool Move(float elapsedTime) override;
	void SetDirection(DIRECTION d);
	void SetMouseInput(bool LmouseInput, bool RmouseInput);
	bool CheckMove_Boss(float fDistance);
	bool CheckMove_Stage1(float fDistance);
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	virtual void SetStage_1Position() = 0;
	virtual void SetBossStagePosition() = 0;
	void SetRole(ROLE r) {
		m_InGameRole = r;
	}
	//void ResetRole() { m_InGameRole = ROLE::NONE_SELECT; }
	ROLE GetRole() {
		return m_InGameRole;
	}
	bool GetLeftAttack() { return m_leftmouseInput; }
	void SetRoomState(ROOM_STATE rState) { m_roomState = rState; }
protected:
	std::pair<float, XMFLOAT3> GetNormalVectorSphere(XMFLOAT3& point);
public:
	virtual void Skill_1() = 0;
	virtual void Skill_2() = 0;
};


class WarriorSessionObject : public ChracterSessionObject
{
public:
	WarriorSessionObject(int id) :ChracterSessionObject(ROLE::WARRIOR)
	{
		SetStage_1Position();
	}
	~WarriorSessionObject() {}
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;

	void Skill_1()override;
	void Skill_2()override;
};

class MageSessionObject : public ChracterSessionObject
{
public:
	MageSessionObject(int id) :ChracterSessionObject(ROLE::PRIEST)
	{
		SetStage_1Position();
	}
	~MageSessionObject() {}
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;
	void Skill_1()override;
	void Skill_2()override;
};

class TankerSessionObject : public ChracterSessionObject
{
public:
	TankerSessionObject(int id) :ChracterSessionObject(ROLE::TANKER)
	{
		SetStage_1Position();
	}
	~TankerSessionObject() {}
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;
	void Skill_1()override;
	void Skill_2()override;
};

class ArcherSessionObject : public ChracterSessionObject
{
public:
	ArcherSessionObject(int id) :ChracterSessionObject(ROLE::ARCHER)
	{
		SetStage_1Position();
	}
	~ArcherSessionObject() {}	
public:
	void SetStage_1Position()override;
	void SetBossStagePosition()override;
	void Skill_1()override;
	void Skill_2()override;
};
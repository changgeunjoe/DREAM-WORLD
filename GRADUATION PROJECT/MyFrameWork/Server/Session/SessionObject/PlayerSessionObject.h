#pragma once
#ifdef _DEBUG
#include "../../PCH/stdafx.h"
#endif
#include "SessionObject.h"


class PlayerSessionObject : public SessionObject
{
private:	
	bool	m_leftmouseInput;
	bool	m_rightmouseInput;
private:
	int m_id;
private:

	ROLE m_InGameRole = ROLE::NONE_SELECT;
public:
	DIRECTION m_inputDirection = DIRECTION::IDLE;
	DIRECTION m_prevDirection = DIRECTION::IDLE;
public:
	PlayerSessionObject(int id, ROLE r);
	virtual	~PlayerSessionObject();



public:
	bool AdjustPlayerInfo(DirectX::XMFLOAT3& position); // , DirectX::XMFLOAT3& rotate
public:
	virtual void AutoMove() override;
	virtual void StartMove(DIRECTION d) override;
	virtual void StopMove() override;
	virtual void ChangeDirection(DIRECTION d) override;
public:
	void Move(float fDistance);
	void SetDirection(DIRECTION d);
	void SetMouseInput(bool LmouseInput, bool RmouseInput);
	bool CanGo(const XMFLOAT3& nextPos);
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	/*virtual const DirectX::XMFLOAT3 GetPosition() override;
	virtual const DirectX::XMFLOAT3 GetRotation() override;*/
public:
	char* GetPlayerInfo();
public:
	void SetRole(ROLE r) {
		m_InGameRole = r;
		switch (r)
		{
		case WARRIOR:
		{
			SetPosition(XMFLOAT3(260, 0, 50));
			m_maxHp = m_hp = 400;
			m_attackDamage = 150;
		}
		break;
		case PRIEST:
		{
			SetPosition(XMFLOAT3(-270, 0, 40));
			m_maxHp = m_hp = 480;
			m_attackDamage = 30;
		}
		break;
		case TANKER:
		{
			SetPosition(XMFLOAT3(150, 0, -60));
			m_maxHp = m_hp = 600;
			m_attackDamage = 60;
		}
		break;
		case ARCHER:
		{
			SetPosition(XMFLOAT3(-200, 0, -40));
			m_maxHp = m_hp = 250;
			m_attackDamage = 200;
		}
		break;
		default:
			break;
		}
	}
	void ResetRole() { m_InGameRole = ROLE::NONE_SELECT; }
	ROLE GetRole() {
		return m_InGameRole;
	}
	bool GetLeftAttack() { return m_leftmouseInput;	}
};

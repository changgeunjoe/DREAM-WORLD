#pragma once

class GameObject;

class Session
{
public:
	Session() {}
	~Session() {}
public:
	int m_id = -1;
public:
	bool m_isVisible = false;
	DIRECTION m_currentDirection = DIRECTION::IDLE;
	DIRECTION m_prevDirection = DIRECTION::IDLE;
public:
	XMFLOAT3 m_rotateAngle = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_ownerRotateAngle = XMFLOAT3(0, 0, 0); // 마우스 회전을 위한 객체
private:
	std::wstring name;
	ROLE m_role = ROLE::NONE_SELECT;
private:
	short	m_hp = 0;
	short	m_maxHp = 0;
	short	m_attackDamage = 0;
public:
	GameObject* m_currentPlayGameObject = nullptr;
public:
	void SetGameObject(GameObject* gObj);
	void UnuseGameObject()
	{
		m_currentPlayGameObject = nullptr;
	}
	void SetName(wstring& n) { name = n; }
	void SetName(wchar_t* n) { name = n; }
	void SetRole(ROLE r) {
		m_role = r;
		switch (r)
		{
		case WARRIOR:
		{			
			m_maxHp = m_hp = 400;
			m_attackDamage = 150;
		}
		break;
		case PRIEST:
		{		
			m_maxHp = m_hp = 480;
			m_attackDamage = 30;
		}
		break;
		case TANKER:
		{			
			m_maxHp = m_hp = 600;
			m_attackDamage = 60;
		}
		break;
		case ARCHER:
		{		
			m_maxHp = m_hp = 250;
			m_attackDamage = 200;
		}
		break;
		default:
			break;
		}
	}
	ROLE GetRole() { return m_role; }
	const wstring getName() { return name; }
};

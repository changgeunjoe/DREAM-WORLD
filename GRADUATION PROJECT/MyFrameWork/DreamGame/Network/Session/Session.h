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
	XMFLOAT3 m_ownerRotateAngle = XMFLOAT3(0, 0, 0); // ���콺 ȸ���� ���� ��ü
private:
	std::wstring name;
	ROLE m_role = ROLE::NONE_SELECT;
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
	void SetRole(ROLE r) { m_role = r; }
	ROLE GetRole() { return m_role; }
	const wstring getName() { return name; }
};

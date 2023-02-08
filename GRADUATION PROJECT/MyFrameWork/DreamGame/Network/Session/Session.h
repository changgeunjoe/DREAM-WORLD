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
private:
	std::wstring name;
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
	const wstring getName() { return name; }
};

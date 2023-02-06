#pragma once

class GameObject;
class Session
{
public:
	Session() {}
	~Session() {}
public:
	int m_id = -1;
private:
	std::wstring name;
public:
	GameObject* m_currentPlayGameObject = nullptr;
public:
	void SetGameObject(GameObject* gObj)
	{
		m_currentPlayGameObject = gObj;
	}
	void UnuseGameObject()
	{
		m_currentPlayGameObject = nullptr;
	}
	void SetName(wstring& n) { name = n; }
	const wstring getName() { return name; }
};

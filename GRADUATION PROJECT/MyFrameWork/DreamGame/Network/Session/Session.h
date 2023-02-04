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

};

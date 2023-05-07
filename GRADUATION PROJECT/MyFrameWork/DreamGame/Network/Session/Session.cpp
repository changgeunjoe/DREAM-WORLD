#include "stdafx.h"
#include "Session.h"
#include "../../GameObject.h"

void Session::Reset()
{
	m_isVisible = false;
	m_currentDirection = DIRECTION::IDLE;
	m_prevDirection = DIRECTION::IDLE;
	m_rotateAngle = XMFLOAT3(0, 0, 0);
	m_ownerRotateAngle = XMFLOAT3(0, 0, 0);
	m_role = ROLE::NONE_SELECT;
	m_hp = 0;
	m_maxHp = 0;
	m_attackDamage = 0;
	m_currentPlayGameObject = nullptr;
	m_id = -1;
}

void Session::SetGameObject(GameObject* gObj)
{
	m_currentPlayGameObject = gObj;
	m_isVisible = true;
}
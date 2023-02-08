#include "stdafx.h"
#include "Session.h"
#include "../../GameObject.h"

void Session::SetGameObject(GameObject* gObj)
{
	m_currentPlayGameObject = gObj;
	m_isVisible = true;
}
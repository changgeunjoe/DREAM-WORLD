#include "stdafx.h"
#include "SessionObject.h"
#include "../Session.h"
SessionObject::SessionObject(Session* session) : m_session{ session }
{
	m_position = XMFLOAT3(0, 0, 0);
	m_rotateAngle = XMFLOAT3(0, 0, 0);
}

SessionObject::~SessionObject()
{

}

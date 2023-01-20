#include "stdafx.h"
#include "SessionObject.h"
#include "../Session.h"
SessionObject::SessionObject(Session* session) : m_session{ session }
{
	m_position = DirectX::XMFLOAT3(0, 0, 0);
	m_rotateAngle = DirectX::XMFLOAT3(0, 0, 0);
}

SessionObject::~SessionObject()
{

}

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

void SessionObject::AutoMove() // /1000.0f해서 이동하자
{
	auto currentTime = std::chrono::steady_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastMoveTime).count();
	m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, (double)durationTime / 1000.0f));
	m_lastMoveTime = currentTime;
}

void SessionObject::StartMove()
{
	m_isMove = true;
	m_lastMoveTime = std::chrono::steady_clock::now();
}

void SessionObject::StopMove()
{
	m_isMove = false;
}

const DirectX::XMFLOAT3 SessionObject::GetPosition()
{
	return m_position;
}

const DirectX::XMFLOAT3 SessionObject::GetRotation()
{
	return m_rotateAngle;
}

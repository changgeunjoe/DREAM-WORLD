#include "stdafx.h"
#include "ShootingSessionObject.h"
#include "../Session.h"

ShootingSessionObject::ShootingSessionObject()
{

}

ShootingSessionObject::ShootingSessionObject(std::string& roomId): m_roomId(roomId)
{

}

ShootingSessionObject::~ShootingSessionObject()
{

}

void ShootingSessionObject::AutoMove()
{
	CalcRightVector();
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;
	Move(((float)durationTime / 1000.0f) * m_speed);
	m_lastMoveTime = currentTime;
}

void ShootingSessionObject::StartMove(DIRECTION d)
{
}

void ShootingSessionObject::StopMove()
{
}

void ShootingSessionObject::ChangeDirection(DIRECTION d)
{
}

void ShootingSessionObject::Rotate(ROTATE_AXIS axis, float angle)
{
}

const DirectX::XMFLOAT3 ShootingSessionObject::GetPosition()
{
	return DirectX::XMFLOAT3();
}

const DirectX::XMFLOAT3 ShootingSessionObject::GetRotation()
{
	return DirectX::XMFLOAT3();
}

void ShootingSessionObject::SetSpeed(float speed)
{
	m_speed;
}

void ShootingSessionObject::SetStart(XMFLOAT3& dir, XMFLOAT3& srcPos, float speed)//초당 이동 속도
{
	m_position = srcPos;
	m_directionVector = dir;
	m_speed = speed;
}

void ShootingSessionObject::Move(float distance)
{
	m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, distance));
}

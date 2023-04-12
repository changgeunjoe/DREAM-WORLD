#include "stdafx.h"
#include "../Session.h"
#include "MonsterSessionObject.h"
#include "../../Logic/Logic.h"

extern Logic g_logic;

MonsterSessionObject::MonsterSessionObject(Session* session) : SessionObject(session)
{

}

MonsterSessionObject::MonsterSessionObject(Session* session, std::string& roomId) : SessionObject(session), m_roomId(roomId)
{

}

MonsterSessionObject::~MonsterSessionObject()
{

}

void MonsterSessionObject::Recv()
{
	std::cout << "Monster Can not Recv" << std::endl;
}

void MonsterSessionObject::AutoMove()
{
	CalcRightVector();
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;
	Move(((float)durationTime / 1000.0f) * 50.0f);
	m_lastMoveTime = currentTime;
	std::cout << "current Position " << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;
	std::cout << "rotate angle" << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

void MonsterSessionObject::StartMove(DIRECTION d)
{
	if (m_inputDirection == DIRECTION::IDLE)
		m_lastMoveTime = std::chrono::high_resolution_clock::now();
	m_inputDirection = (DIRECTION)(m_inputDirection | d);
	SetDirection(m_inputDirection);
}

void MonsterSessionObject::StopMove()
{
	PrintCurrentTime();
	std::cout << "PlayerSessionObject::StopMove() - Look Dir: " << m_directionVector.x << ", " << m_directionVector.y << ", " << m_directionVector.z << std::endl;
	// m_prevDirection = m_inputDirection;
	m_inputDirection = DIRECTION::IDLE;
}

void MonsterSessionObject::ChangeDirection(DIRECTION d)
{
	m_inputDirection = (DIRECTION)(m_inputDirection ^ d);
	SetDirection(m_inputDirection);
}

const DirectX::XMFLOAT3 MonsterSessionObject::GetPosition()
{
	return m_position;
}

const DirectX::XMFLOAT3 MonsterSessionObject::GetRotation()
{
	return m_rotateAngle;
}

void MonsterSessionObject::Rotate(ROTATE_AXIS axis, float angle)
{
	DirectX::XMFLOAT3 upVec = DirectX::XMFLOAT3(0, 1, 0);
	switch (axis)
	{
	case X:
	{

	}
	break;
	case Y:
	{
		m_rotateAngle.y += angle;
		SetDirection(m_inputDirection);
	}
	break;
	case Z:
	{

	}
	break;
	default:
		break;
	}
}

void MonsterSessionObject::SetDirection(DIRECTION d)
{
	DirectX::XMFLOAT3 xmf3Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	DirectX::XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&xmf3Up), DirectX::XMConvertToRadians(m_rotateAngle.y));
	DirectX::XMFLOAT3 xmf3Look = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);

	DirectX::XMFLOAT3 xmf3Rev = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fRotateAngle = -1.0f;

	if (d != DIRECTION::IDLE)
	{
		if (((d & DIRECTION::LEFT) == DIRECTION::LEFT) &&
			((d & DIRECTION::RIGHT) == DIRECTION::RIGHT))
		{
			d = (DIRECTION)(d ^ DIRECTION::LEFT);
			d = (DIRECTION)(d ^ DIRECTION::RIGHT);
		}
		if (((d & DIRECTION::FRONT) == DIRECTION::FRONT) &&
			((d & DIRECTION::BACK) == DIRECTION::BACK))
		{
			d = (DIRECTION)(d ^ DIRECTION::FRONT);
			d = (DIRECTION)(d ^ DIRECTION::BACK);
		}
		switch (d)
		{
		case DIRECTION::FRONT:						fRotateAngle = 0.0f;	break;
		case DIRECTION::LEFT | DIRECTION::FRONT:	fRotateAngle = 45.0f;	break;
		case DIRECTION::LEFT:						fRotateAngle = 90.0f;	break;
		case DIRECTION::BACK | DIRECTION::LEFT:		fRotateAngle = 135.0f;	break;
		case DIRECTION::BACK:						fRotateAngle = 180.0f;	break;
		case DIRECTION::RIGHT | DIRECTION::BACK:	fRotateAngle = 225.0f;	break;
		case DIRECTION::RIGHT:						fRotateAngle = 270.0f;	break;
		case DIRECTION::FRONT | DIRECTION::RIGHT:	fRotateAngle = 315.0f;	break;
		}

		fRotateAngle = fRotateAngle * (3.14159265359f / 180.0f);
		xmf3Rev.x = xmf3Look.x * cos(fRotateAngle) - xmf3Look.z * sin(fRotateAngle);
		xmf3Rev.z = xmf3Look.x * sin(fRotateAngle) + xmf3Look.z * cos(fRotateAngle);
		xmf3Rev = Vector3::Normalize(xmf3Rev);

		m_directionVector = xmf3Rev;
	}
}

void MonsterSessionObject::Move(float fDistance)
{
	DIRECTION tespDIR = m_inputDirection;
	if (((tespDIR & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tespDIR & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::LEFT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::RIGHT);
	}
	if (((tespDIR & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tespDIR & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::FRONT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::BACK);
	}

	if (m_inputDirection != DIRECTION::IDLE)
	{
		switch (tespDIR)
		{
		case DIRECTION::FRONT:
		case DIRECTION::FRONT | DIRECTION::RIGHT:
		case DIRECTION::RIGHT:
		case DIRECTION::BACK | DIRECTION::RIGHT:
		case DIRECTION::BACK:
		case DIRECTION::BACK | DIRECTION::LEFT:
		case DIRECTION::LEFT:
		case DIRECTION::FRONT | DIRECTION::LEFT:
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, fDistance));
		default: break;
		}
	}
}

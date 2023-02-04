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
	CalcRightVector();
	auto currentTime = std::chrono::steady_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastMoveTime).count();
	if (DIRECTION::FRONT == (m_inputDirection & DIRECTION::FRONT)) {
		if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) {
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, (double)durationTime / 2000.0f));
			m_position = Vector3::Subtract(m_position, Vector3::ScalarProduct(m_rightVector, (double)durationTime / 2000.0f));
		}
		else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT)) {
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, (double)durationTime / 2000.0f));
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_rightVector, (double)durationTime / 2000.0f));
		}
		else m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, (double)durationTime / 1000.0f));
	}
	else if (DIRECTION::BACK == (m_inputDirection & DIRECTION::BACK)) {
		if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) {
			m_position = Vector3::Subtract(m_position, Vector3::ScalarProduct(m_directionVector, (double)durationTime / 2000.0f));
			m_position = Vector3::Subtract(m_position, Vector3::ScalarProduct(m_rightVector, (double)durationTime / 2000.0f));
		}
		else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT)) {
			m_position = Vector3::Subtract(m_position, Vector3::ScalarProduct(m_directionVector, (double)durationTime / 2000.0f));
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_rightVector, (double)durationTime / 2000.0f));
		}
		else m_position = Vector3::Subtract(m_position, Vector3::ScalarProduct(m_directionVector, (double)durationTime / 1000.0f));
	}
	else {
		if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) 	
			m_position = Vector3::Subtract(m_position, Vector3::ScalarProduct(m_rightVector, (double)durationTime / 1000.0f));
		else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT))
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_rightVector, (double)durationTime / 1000.0f));
	}
	m_lastMoveTime = currentTime;
}

void SessionObject::StartMove(DIRECTION d)
{
	m_inputDirection = (DIRECTION)(m_inputDirection | d);
	m_lastMoveTime = std::chrono::steady_clock::now();
}

void SessionObject::StopMove()
{
	m_inputDirection = DIRECTION::IDLE;
}

void SessionObject::ChangeDirection(DIRECTION d)
{
}

const DirectX::XMFLOAT3 SessionObject::GetPosition()
{
	return m_position;
}

const DirectX::XMFLOAT3 SessionObject::GetRotation()
{
	return m_rotateAngle;
}

void SessionObject::Rotate(ROTATE_AXIS axis, float angle)
{
}

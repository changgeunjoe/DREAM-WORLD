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
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;
	if (DIRECTION::FRONT == (m_inputDirection & DIRECTION::FRONT)) {
		if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) {
			DirectX::XMFLOAT3 resDir = Vector3::Normalize(Vector3::Subtract(m_directionVector, m_rightVector));
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resDir, ((double)durationTime / 1000.0f) * 50.0f));
			//std::cout << "diagonal dir: " << resDir.x << ", " << resDir.z << "distance: " << ((double)durationTime / 1000.0f) * 50.0f << std::endl;
			//std::cout << "LF" << std::endl;
		}
		else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT)) {
			DirectX::XMFLOAT3 resDir = Vector3::Normalize(Vector3::Add(m_directionVector, m_rightVector));
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resDir, ((double)durationTime / 1000.0f) * 50.0f));
			//std::cout << "diagonal dir: " << resDir.x << ", " << resDir.z << std::endl;
			//std::cout << "RF" << std::endl;
		}
		else {
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, ((double)durationTime / 1000.0f) * 50.0f));
			std::cout << "F" << std::endl;
		}
	}
	else if (DIRECTION::BACK == (m_inputDirection & DIRECTION::BACK)) {
		if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) {
			DirectX::XMFLOAT3 resDir = Vector3::Normalize(Vector3::Add(m_directionVector, m_rightVector));
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resDir, ((double)durationTime / 1000.0f) * -50.0f));
			//std::cout << "diagonal dir: " << resDir.x << ", " << resDir.z << std::endl;
			//std::cout << "LB" << std::endl;
		}
		else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT)) {
			DirectX::XMFLOAT3 resDir = Vector3::Normalize(Vector3::Subtract(m_rightVector, m_directionVector));
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resDir, ((double)durationTime / 1000.0f) * 50.0f));
			//std::cout << "diagonal dir: " << resDir.x << ", " << resDir.z << std::endl;
			//std::cout << "RB" << std::endl;
		}
		else {
			m_position = Vector3::Subtract(m_position, Vector3::ScalarProduct(m_directionVector, ((double)durationTime / 1000.0f) * 50.0f));
			std::cout << "B" << std::endl;
		}
	}
	else {
		if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) {
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_rightVector, ((double)durationTime / 1000.0f) * -50.0f));
			std::cout << "L" << std::endl;
		}
		else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT)) {
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_rightVector, ((double)durationTime / 1000.0f) * 50.0f));
			std::cout << "R" << std::endl;
		}
	}
	m_lastMoveTime = currentTime;
	std::cout << "current Position " << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;
}

void SessionObject::StartMove(DIRECTION d)
{
	if (m_inputDirection == DIRECTION::IDLE)
		m_lastMoveTime = std::chrono::high_resolution_clock::now();
	m_inputDirection = (DIRECTION)(m_inputDirection | d);
}

void SessionObject::StopMove()
{
	m_inputDirection = DIRECTION::IDLE;
}

void SessionObject::ChangeDirection(DIRECTION d)
{
	m_inputDirection = (DIRECTION)(m_inputDirection ^ d);
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
	std::cout << "rotate axis: " << (int)axis << " angle: " << angle << std::endl;
	DirectX::XMFLOAT3 upVec = DirectX::XMFLOAT3(0, 1, 0);
	switch (axis)
	{
	case X:
	{

	}
	break;
	case Y:
	{
		DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&upVec), DirectX::XMConvertToRadians(angle));
		m_worldMatrix = Matrix4x4::Multiply(mtxRotate, m_worldMatrix);
		/*std::cout << "matrix\n"
			<< m_worldMatrix._11 << "   " << m_worldMatrix._12 << "   " << m_worldMatrix._13 << std::endl
			<< m_worldMatrix._21 << "   " << m_worldMatrix._22 << "   " << m_worldMatrix._23 << std::endl
			<< m_worldMatrix._31 << "   " << m_worldMatrix._32 << "   " << m_worldMatrix._33 << std::endl;*/

		m_directionVector = Vector3::Normalize(XMFLOAT3(m_worldMatrix._31, m_worldMatrix._32, m_worldMatrix._33));

		m_rotateAngle.y += angle;
	}
	break;
	case Z:
	{

	}
	break;
	default:
		break;
	}
	std::cout << "current direction " << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

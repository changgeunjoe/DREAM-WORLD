#include "stdafx.h"
#include "PlayerSessionObject.h"
#include "../Session.h"
#include "../../Logic/Logic.h"
#include "../../IOCPNetwork/protocol/protocol.h"

extern  Logic g_logic;

PlayerSessionObject::PlayerSessionObject(int id, ROLE r) : SessionObject()
{
	m_id = id;
	/*m_level = 0;
	m_exp = 0;*/
	m_hp = 0;
	m_maxHp = 0;
	m_attackDamage = 0;
	m_InGameRole = r;
	SetRole(r);
}

PlayerSessionObject::~PlayerSessionObject()
{
}


bool PlayerSessionObject::AdjustPlayerInfo(DirectX::XMFLOAT3& position) // , DirectX::XMFLOAT3& rotate
{
	//m_rotateAngle = rotate;
	if (Vector3::Length(Vector3::Subtract(m_position, position)) < 0.3f) { // ���� �� �̸��̶�� ���� ����, �̻��̶�� ���ǵ� ��?���� �����ϰ� ��ġ ���� ��ġ�� ��ȯ
		m_position = position;
		return true;
	}
	return false;
}

void PlayerSessionObject::AutoMove()
{
	CalcRightVector();
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;	
	Move(((float)durationTime / 1000.0f) * 50.0f);
	m_lastMoveTime = currentTime;
	// std::cout << "current Position " << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;	
	// std::cout << "rotate angle" << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

void PlayerSessionObject::SetDirection(DIRECTION d)
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

void PlayerSessionObject::SetMouseInput(bool LmouseInput, bool RmouseInput)
{
	m_leftmouseInput = LmouseInput;
	m_rightmouseInput = RmouseInput;
}

void PlayerSessionObject::StartMove(DIRECTION d)
{
	//std::cout << "PlayerSessionObject::StartMove()" << std::endl;
	if (m_inputDirection == DIRECTION::IDLE)
		m_lastMoveTime = std::chrono::high_resolution_clock::now();

	m_inputDirection = (DIRECTION)(m_inputDirection | d);
	SetDirection(m_inputDirection);
}

void PlayerSessionObject::StopMove()
{
	PrintCurrentTime();
	//std::cout << "PlayerSessionObject::StopMove() - Look Dir: " << m_directionVector.x << ", " << m_directionVector.y << ", " << m_directionVector.z << std::endl;
	//std::cout << "PlayerSessionObject::StopMove() - Pos: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
	// m_prevDirection = m_inputDirection;
	m_inputDirection = DIRECTION::IDLE;
}

void PlayerSessionObject::ChangeDirection(DIRECTION d)
{
	m_inputDirection = (DIRECTION)(m_inputDirection ^ d);
	SetDirection(m_inputDirection);
}

void PlayerSessionObject::Move(float fDistance)
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

void PlayerSessionObject::Rotate(ROTATE_AXIS axis, float angle)
{
	//std::cout << "rotate axis: " << (int)axis << " angle: " << angle << std::endl;
	DirectX::XMFLOAT3 upVec = DirectX::XMFLOAT3(0, 1, 0);
	switch (axis)
	{
	case X:
	{

	}
	break;
	case Y:
	{
		//DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&upVec), DirectX::XMConvertToRadians(angle));
		//m_worldMatrix = Matrix4x4::Multiply(mtxRotate, m_worldMatrix);
		///*std::cout << "matrix\n"
		//	<< m_worldMatrix._11 << "   " << m_worldMatrix._12 << "   " << m_worldMatrix._13 << std::endl
		//	<< m_worldMatrix._21 << "   " << m_worldMatrix._22 << "   " << m_worldMatrix._23 << std::endl
		//	<< m_worldMatrix._31 << "   " << m_worldMatrix._32 << "   " << m_worldMatrix._33 << std::endl;*/

		//m_directionVector = Vector3::Normalize(XMFLOAT3(m_worldMatrix._31, m_worldMatrix._32, m_worldMatrix._33));

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
	//std::cout << "current direction " << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
	//std::cout << "rotate angle" << m_rotateAngle.x << " " << m_rotateAngle.y << " " << m_rotateAngle.z << std::endl;
}

const DirectX::XMFLOAT3 PlayerSessionObject::GetPosition()
{
	return m_position;
}

const DirectX::XMFLOAT3 PlayerSessionObject::GetRotation()
{
	return m_rotateAngle;
}

char* PlayerSessionObject::GetPlayerInfo()
{
	SERVER_PACKET::AddPlayerPacket* playerInfo = new SERVER_PACKET::AddPlayerPacket;
	playerInfo->userId = m_id;
	//memcpy(playerInfo->name, m_playerName.c_str(), m_playerName.size() * 2);
	//playerInfo->name[m_playerName.size()] = 0;
	playerInfo->position = m_position;
	playerInfo->rotate = m_rotateAngle;
	playerInfo->type = SERVER_PACKET::ADD_PLAYER;
	playerInfo->role = m_InGameRole;
	playerInfo->size = sizeof(SERVER_PACKET::AddPlayerPacket);
	return reinterpret_cast<char*>(playerInfo);
}

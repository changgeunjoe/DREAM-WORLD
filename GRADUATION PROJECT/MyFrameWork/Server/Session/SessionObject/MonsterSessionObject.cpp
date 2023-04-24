#include "stdafx.h"
#include "../Session.h"
#include "MonsterSessionObject.h"
#include "../../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../../Logic/Logic.h"
#include "../../Timer/Timer.h"

extern Logic g_logic;
extern Timer g_Timer;
extern IOCPNetwork g_iocpNetwork;

MonsterSessionObject::MonsterSessionObject() : SessionObject()
{
	m_maxHp = m_hp = 2500;
}

MonsterSessionObject::MonsterSessionObject(std::string& roomId) : SessionObject()
{
	m_roomId = roomId;
	m_maxHp = m_hp = 2500;
}

MonsterSessionObject::~MonsterSessionObject()
{

}

void MonsterSessionObject::AutoMove()
{
	CalcRightVector();
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;
	Move(((float)durationTime / 1000.0f) * 50.0f, ((float)durationTime / 1000.0f));
	m_lastMoveTime = currentTime;
	//std::cout << "Boss current Position " << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;
	//std::cout << "Boss Dir Vector" << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

void MonsterSessionObject::StartMove(DIRECTION d)
{
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	isMove = true;
}

void MonsterSessionObject::StopMove()
{

}

void MonsterSessionObject::ChangeDirection(DIRECTION d)
{

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
	switch (axis)
	{
	case X:
	{

	}
	break;
	case Y:
	{
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
	//std::cout << "Rotate angle: " << angle << std::endl;//
	DirectX::XMFLOAT3 xmf3Rev = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	xmf3Rev.x = m_directionVector.x * cos(XMConvertToRadians(-angle)) - m_directionVector.z * sin(XMConvertToRadians(-angle));
	xmf3Rev.z = m_directionVector.x * sin(XMConvertToRadians(-angle)) + m_directionVector.z * cos(XMConvertToRadians(-angle));
	xmf3Rev = Vector3::Normalize(xmf3Rev);
	m_directionVector = xmf3Rev;
	//std::cout << "Boss Dir Vector" << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

void MonsterSessionObject::SetDirection(DIRECTION d)
{

}

void MonsterSessionObject::Move(float fDistance, float elapsedTime)
{
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 des = Vector3::Subtract(m_DestinationPos, m_position);	// ¸ñÀûÁö¶û À§Ä¡¶û º¤ÅÍ	
	CalcRightVector();
	bool OnRight = (Vector3::DotProduct(m_rightVector, Vector3::Normalize(des)) > 0) ? true : false;	// ¸ñÀûÁö°¡ ¿Ã´ÀÂÊ ¿Þ
	float ChangingAngle = Vector3::Angle(Vector3::Normalize(des), m_directionVector);	
	if (Vector3::Length(des) < 10.0f) {		
		TIMER_EVENT bossAttackEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), m_roomId, -1,EV_BOSS_ATTACK_ORDER };
		g_Timer.m_TimerQueue.push(bossAttackEvent);
	}
	else
	{
		if (ChangingAngle > 0.5f)
		{
			if (OnRight)
				Rotate(ROTATE_AXIS::Y, 45.0f * elapsedTime);
			else if (!OnRight)
				Rotate(ROTATE_AXIS::Y, -45.0f * elapsedTime);
		}
		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, fDistance));//Æ½¸¶´Ù ¿òÁ÷ÀÓ
		m_SPBB = BoundingSphere(DirectX::XMFLOAT3(m_position.x, m_position.y + 12.5f, m_position.z), 22.5f);
	}
	//std::cout << "BossPos: " << m_position.x << "0, " << m_position.z << std::endl;
}

void MonsterSessionObject::SetAggroPlayerId(int id)
{
	m_aggroPlayerId = id;
}

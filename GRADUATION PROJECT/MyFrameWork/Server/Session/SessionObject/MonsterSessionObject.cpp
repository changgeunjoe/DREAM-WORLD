#include "stdafx.h"
#include "../Session.h"
#include "MonsterSessionObject.h"
#include "../../Logic/Logic.h"
#include "../../Timer/Timer.h"

extern Logic g_logic;
extern Timer g_Timer;

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
	Move(((float)durationTime / 1000.0f) * 50.0f, ((float)durationTime / 1000.0f));
	m_lastMoveTime = currentTime;
	std::cout << "current Position " << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;
	std::cout << "rotate angle" << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

void MonsterSessionObject::StartMove(DIRECTION d)
{

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

}

void MonsterSessionObject::Move(float fDistance, float elapsedTime)
{
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 des = Vector3::Subtract(m_DestinationPos, m_position);	// ¸ñÀûÁö¶û À§Ä¡¶û º¤ÅÍ	
	CalcRightVector();
	bool OnRight = (Vector3::DotProduct(m_rightVector, Vector3::Normalize(des)) > 0) ? true : false;	// ¸ñÀûÁö°¡ ¿Ã´ÀÂÊ ¿Þ
	float ChangingAngle = Vector3::Angle(Vector3::Normalize(des), GetLook());	// 

	if (Vector3::Length(des) < DBL_EPSILON)
		m_position = m_DestinationPos;
	else
	{
		if (ChangingAngle > 1.0f)
		{
			if (OnRight)
				Rotate(ROTATE_AXIS::Y, 10.0f * elapsedTime);
			else if (!OnRight)
				Rotate(ROTATE_AXIS::Y, -10.0f * elapsedTime);
		}
		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, fDistance));//Æ½¸¶´Ù ¿òÁ÷ÀÓ		
	}
}

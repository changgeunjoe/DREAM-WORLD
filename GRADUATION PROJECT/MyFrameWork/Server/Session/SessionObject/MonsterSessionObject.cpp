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
	TIMER_EVENT firstEv{ std::chrono::system_clock::now() + std::chrono::seconds(3), m_roomId, -1,EV_RANDOM_MOVE };
	g_Timer.m_TimerQueue.push(firstEv);
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

void MonsterSessionObject::Move(float fDistance)
{
	
}

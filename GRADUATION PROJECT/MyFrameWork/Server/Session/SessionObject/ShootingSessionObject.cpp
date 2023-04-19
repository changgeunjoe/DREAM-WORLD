#include "stdafx.h"
#include "ShootingSessionObject.h"
#include "MonsterSessionObject.h"
#include "../Session.h"

ShootingSessionObject::ShootingSessionObject() : SessionObject()
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
	m_distance += ((float)durationTime / 1000.0f) * m_speed;
	if (m_distance > 250.0f) m_active = false;	// 추후 수정 //생명주기 관리에 사용
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

void ShootingSessionObject::DetectCollision(MonsterSessionObject* m_bossSession)
{
	if (m_SPBB.Intersects(m_bossSession->m_SPBB))
	{
		// 보스 체력 -
		std::cout << "충돌체크 완료" << std::endl;
		m_active = false;	// 추후 생명주기 관리에 사용
	}
}

void ShootingSessionObject::SetStart(XMFLOAT3& dir, XMFLOAT3& srcPos, float speed)//초당 이동 속도
{
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	m_active = true;
	m_position = srcPos;
	m_directionVector = dir;
	m_speed = speed;
	m_SPBB = BoundingSphere(XMFLOAT3(srcPos.x, srcPos.y + 0.5f, srcPos.z), 0.5f);
}

void ShootingSessionObject::Move(float distance)
{
	m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, distance));
	m_SPBB = BoundingSphere(XMFLOAT3(m_position.x, m_position.y + 0.2f, m_position.z), 0.2f);
}

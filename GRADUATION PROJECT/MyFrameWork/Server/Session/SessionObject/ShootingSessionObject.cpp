#include "stdafx.h"
#include "ShootingSessionObject.h"
#include "MonsterSessionObject.h"
#include "../UserSession.h"

ShootingSessionObject::ShootingSessionObject() : SessionObject(4.0f)
{
	m_speed = 100.0f;
}

ShootingSessionObject::~ShootingSessionObject()
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

int ShootingSessionObject::DetectCollision(MonsterSessionObject* m_bossSession)
{
	//if (m_SPBB.Intersects(m_bossSession->m_SPBB))
	//{
	//	// ���� ü�� -
	//	std::cout << "�浹üũ �Ϸ�" << std::endl;
	//	m_active = false;	// ���� �����ֱ� ������ ���
	//	return m_id;
	//}
	return -1;
}

void ShootingSessionObject::SetStart(XMFLOAT3& dir, XMFLOAT3& srcPos, float speed)//�ʴ� �̵� �ӵ�
{
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	m_active = true;
	m_position = srcPos;
	m_directionVector = dir;
	m_speed = speed;
	m_SPBB.Center = srcPos;
}

bool ShootingSessionObject::Move(float elapsedTime)
{
	m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, elapsedTime * m_speed));
	m_SPBB.Center = m_position;
	//���� �����Լ� Move�� �߰����� �ؿ� �κ�
	m_distance += elapsedTime * m_speed;
	if (m_distance > 250.0f) m_active = false;	// ���� ���� //�����ֱ� ������ ���	
	return true;
}

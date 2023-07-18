#include "stdafx.h"
#include "SmallMonsterSessionObject.h"

SmallMonsterSessionObject::SmallMonsterSessionObject() :SessionObject()
{
	m_speed = 30.0f;
	m_maxHp = m_hp = 100;
}

SmallMonsterSessionObject::SmallMonsterSessionObject(int roomId) :SessionObject(15.0f), m_roomId(roomId)
{
	m_speed = 30.0f;
	m_maxHp = m_hp = 100;
}

SmallMonsterSessionObject::~SmallMonsterSessionObject()
{

}

void SmallMonsterSessionObject::SetDestinationPos(XMFLOAT3* posArr)
{

	std::pair<float, int> minDis = std::make_pair(-1.0f, -1);
	for (int i = 0; i < 4; i++) {
		if (minDis.first < 0.0f) {
			float dis = Vector3::Length(Vector3::Subtract(m_position, posArr[i]));
			minDis.first = dis;
			minDis.second = i;
		}
		else {
			float dis = Vector3::Length(Vector3::Subtract(m_position, posArr[i]));
			if (dis < minDis.first) {
				minDis.first = dis;
				minDis.second = i;
			}
		}
	}
	m_desPos = posArr[minDis.second];
	//m_desDis = minDis.first;
}

void SmallMonsterSessionObject::StartMove()
{
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	isMove = true;
}

void SmallMonsterSessionObject::Rotate(ROTATE_AXIS axis, float angle)
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
	//PrintCurrentTime();
	//std::cout << std::endl << "Rotate angle: " << m_rotateAngle.y << std::endl;//
	DirectX::XMFLOAT3 xmf3Rev = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 currentVector = m_directionVector;
	xmf3Rev.x = currentVector.x * cos(XMConvertToRadians(-angle)) - currentVector.z * sin(XMConvertToRadians(-angle));
	xmf3Rev.z = currentVector.x * sin(XMConvertToRadians(-angle)) + currentVector.z * cos(XMConvertToRadians(-angle));
	xmf3Rev.y = 0;
	xmf3Rev = Vector3::Normalize(xmf3Rev);
	m_directionVector = xmf3Rev;
}

bool SmallMonsterSessionObject::Move(float elapsedTime)
{
	CalcRightVector();
	XMFLOAT3 desVector = Vector3::Subtract(m_desPos, m_position);	
	//std::cout << "Look: " << m_directionVector.x << ", " << m_directionVector.y << ", " << m_directionVector.z << std::endl;
	float desDis = Vector3::Length(desVector);
	desVector = Vector3::Normalize(desVector);
	float frontDotRes = Vector3::DotProduct(desVector, m_directionVector);
	//std::cout << "desVector: " << desVector.x << ", " << desVector.y << ", " << desVector.z << std::endl;

	if (desDis >= 120.0f) {
		//멀리 있음
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨 - 방향이 맞지 않음
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, elapsedTime * 90.0f);
			}
			else {
				Rotate(ROTATE_AXIS::Y, elapsedTime * -90.0f);
			}
		}
		return true;
	}

	if (desDis <= 25.0f) {//근접 했을때
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨 - 방향이 맞지 않음
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, elapsedTime * 90.0f);
			}
			else {
				Rotate(ROTATE_AXIS::Y, elapsedTime * -90.0f);
			}
			return true;
		}
		auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_lastAttackTime);
		if (durationTime > std::chrono::seconds(1)) {
			isMove = false;
			//공격 가능
			m_lastAttackTime = std::chrono::high_resolution_clock::now();
			return true;
		}
		return true;
	}
	else {
		//멀리와 근접 사이에 움직여야함
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, elapsedTime * 90.0f);
			}
			else {
				Rotate(ROTATE_AXIS::Y, elapsedTime * -90.0f);
			}
		}
		//std::cout << "SmallMonsterSessionObject::Move() - elapsedTime: " << elapsedTime << std::endl;
		//PrintCurrentTime();
		//std::cout << "Look: " << m_directionVector.x << ", " << m_directionVector.y << ", " << m_directionVector.z << std::endl << std::endl;

		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
		m_SPBB.Center = m_position;
		m_SPBB.Center.y += m_fBoundingSize;
		return true;
	}
	return true;
}

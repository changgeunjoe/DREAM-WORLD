#include "stdafx.h"
#include "ChracterSessionObject.h"
#include "../UserSession.h"
#include "../../Logic/Logic.h"
#include "../../IOCPNetwork/protocol/protocol.h"
#include "../../MapData/MapData.h"
#include "../../Room/Room.h"
#include "../../Timer/Timer.h"

extern Logic g_logic;
extern MapData g_bossMapData;
extern MapData g_stage1MapData;
extern Timer g_Timer;

ChracterSessionObject::ChracterSessionObject(ROLE r) : SessionObject()
{
	m_hp = 0;
	m_maxHp = 0;
	m_attackDamage = 0;
	m_speed = 50.0f;
	SetRole(r);
}

ChracterSessionObject::~ChracterSessionObject()
{
}


bool ChracterSessionObject::AdjustPlayerInfo(DirectX::XMFLOAT3& position) // , DirectX::XMFLOAT3& rotate
{
	//m_rotateAngle = rotate;
	if (Vector3::Length(Vector3::Subtract(m_position, position)) < 0.3f) { // ���� �� �̸��̶�� ���� ����, �̻��̶�� ���ǵ� ��?���� �����ϰ� ��ġ ���� ��ġ�� ��ȯ
		m_position = position;

		return true;
	}
	return false;
}

void ChracterSessionObject::SetDirection(DIRECTION d)
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

void ChracterSessionObject::SetMouseInput(bool LmouseInput, bool RmouseInput)
{
	m_leftmouseInput = LmouseInput;
	m_rightmouseInput = RmouseInput;
}

bool ChracterSessionObject::CheckMove_Boss(float fDistance)
{
	auto& allCollisionVec = g_bossMapData.GetCollideData();
	for (auto& collide : allCollisionVec)
	{
		if (collide.GetObb().Intersects(m_SPBB))
		{
			//XMFLOAT3 CollidePolygonNormalVector = p.CalSlidingVector(m_position, m_directionVector);
			//m_position = Vector3::Add(m_position, Vector3::ScalarProduct(CollidePolygonNormalVector, 0.5f * fDistance));
			std::vector<int>& relationIdxsVector = collide.GetRelationCollisionIdxs();
			int secondCollide = -1;
			for (auto& otherCol : relationIdxsVector) {
				if (allCollisionVec[otherCol].GetObb().Intersects(m_SPBB)) {
					//CollidePolygonNormalVector = allCollisionVec[otherCol].CalSlidingVector(m_position, m_directionVector);
					//m_position = Vector3::Add(m_position, Vector3::ScalarProduct(CollidePolygonNormalVector, 0.5f * fDistance));
					secondCollide = otherCol;
					break;
				}
			}
			if (secondCollide == -1) {
				auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, m_position, m_directionVector);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//노말 벡터
				XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//슬라이딩 벡터
				float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
				float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//슬라이딩 벡터와 무브 벡터 내적 값
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut * fDistance));
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector, normalVectorDotProductReslut * fDistance));
			}
			else {
				auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, m_position, m_directionVector);
				auto CollidePolygonNormalVector2 = allCollisionVec[secondCollide].CalSlidingVector(m_SPBB, m_position, m_directionVector);

				XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//노말 벡터
				XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//슬라이딩 벡터
				float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);
				float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//슬라이딩 벡터와 무브 벡터 내적 값

				XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//노말 벡터
				XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//슬라이딩 벡터
				float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);
				float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//슬라이딩 벡터와 무브 벡터 내적 값

				XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, allCollisionVec[secondCollide].GetObb().Center));
				resultSlidingVector.y = 0.0f;
				float dotRes = Vector3::DotProduct(resultSlidingVector, m_directionVector);
				if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);

				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resultSlidingVector, fDistance));
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector1, normalVectorDotProductResult1 * fDistance));
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector2, normalVectorDotProductResult2 * fDistance));

			}
			std::cout << " Player Get Collision " << std::endl;
			m_SPBB.Center = m_position;
			return false;
		}
	}
	return true;
}

void ChracterSessionObject::StartMove(DIRECTION d)
{
	//std::cout << "ChracterSessionObject::StartMove()" << std::endl;
	if (m_inputDirection == DIRECTION::IDLE)
		m_lastMoveTime = std::chrono::high_resolution_clock::now();

	m_inputDirection = (DIRECTION)(m_inputDirection | d);
	SetDirection(m_inputDirection);
}

bool ChracterSessionObject::CheckMove_Stage1(float fDistance)
{
	auto& allCollisionVec = g_stage1MapData.GetCollideData();
	for (auto& collide : allCollisionVec)
	{
		if (collide.GetObb().Intersects(m_SPBB))
		{
			//XMFLOAT3 CollidePolygonNormalVector = p.CalSlidingVector(m_position, m_directionVector);
			//m_position = Vector3::Add(m_position, Vector3::ScalarProduct(CollidePolygonNormalVector, 0.5f * fDistance));
			std::vector<int>& relationIdxsVector = collide.GetRelationCollisionIdxs();
			int secondCollide = -1;
			for (auto& otherCol : relationIdxsVector) {
				if (allCollisionVec[otherCol].GetObb().Intersects(m_SPBB)) {
					//CollidePolygonNormalVector = allCollisionVec[otherCol].CalSlidingVector(m_position, m_directionVector);
					//m_position = Vector3::Add(m_position, Vector3::ScalarProduct(CollidePolygonNormalVector, 0.5f * fDistance));
					secondCollide = otherCol;
					break;
				}
			}
			if (secondCollide == -1) {
				auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, m_position, m_directionVector);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//노말 벡터
				XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//슬라이딩 벡터
				float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
				float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//슬라이딩 벡터와 무브 벡터 내적 값
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut * fDistance));
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector, normalVectorDotProductReslut * fDistance));
			}
			else {
				auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, m_position, m_directionVector);
				auto CollidePolygonNormalVector2 = allCollisionVec[secondCollide].CalSlidingVector(m_SPBB, m_position, m_directionVector);

				XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//노말 벡터
				XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//슬라이딩 벡터
				float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);
				float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//슬라이딩 벡터와 무브 벡터 내적 값

				XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//노말 벡터
				XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//슬라이딩 벡터
				float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);
				float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//슬라이딩 벡터와 무브 벡터 내적 값

				XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, allCollisionVec[secondCollide].GetObb().Center));
				resultSlidingVector.y = 0.0f;
				float dotRes = Vector3::DotProduct(resultSlidingVector, m_directionVector);
				if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);

				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resultSlidingVector, fDistance));
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector1, normalVectorDotProductResult1 * fDistance));
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector2, normalVectorDotProductResult2 * fDistance));

			}
			std::cout << " Player Get Collision " << std::endl;
			m_SPBB.Center = m_position;
			return false;
		}
	}
	return true;
}

void ChracterSessionObject::StopMove()
{
	//PrintCurrentTime();
	//std::cout << "ChracterSessionObject::StopMove() - Look Dir: " << m_directionVector.x << ", " << m_directionVector.y << ", " << m_directionVector.z << std::endl;
	//std::cout << "ChracterSessionObject::StopMove() - Pos: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
	// m_prevDirection = m_inputDirection;
	m_inputDirection = DIRECTION::IDLE;
}

void ChracterSessionObject::ChangeDirection(DIRECTION d)
{
	m_inputDirection = (DIRECTION)(m_inputDirection ^ d);
	SetDirection(m_inputDirection);
}

bool ChracterSessionObject::Move(float elapsedTime)
{
	if (m_inputDirection != DIRECTION::IDLE)
	{
#ifdef _DEBUG
		std::cout << "character::Move() - elapsedTime: " << elapsedTime << std::endl;
#endif 		 
		if (m_roomState == ROOM_BOSS) {
			if (CheckMove_Boss(elapsedTime * m_speed)) {
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, elapsedTime * m_speed));
				m_SPBB.Center = m_position;
			}
			return true;
		}
		else {
			if (CheckMove_Stage1(elapsedTime * m_speed)) {
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, elapsedTime * m_speed));
				m_SPBB.Center = m_position;
			}
			return true;
		}
	}
	return false;
}

void ChracterSessionObject::Rotate(ROTATE_AXIS axis, float angle)
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

void WarriorSessionObject::Skill_1()
{

}

void WarriorSessionObject::Skill_2()
{

}

void WarriorSessionObject::SetStage_1Position()
{
	SetPosition(XMFLOAT3(-1290.0f, 0, -1470.0f));
	m_maxHp = m_hp = 400;
	m_attackDamage = 150;
}

void WarriorSessionObject::SetBossStagePosition()
{
	SetPosition(XMFLOAT3(0, 0, -211.0f));
	m_maxHp = m_hp = 400;
	m_attackDamage = 150;
}

void MageSessionObject::Skill_1()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_skillInputTime[0] = currentTime;
	TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now(), m_roomId ,EV_MAGE_HEAL };
	g_Timer.InsertTimerQueue(gameStateEvent);
}

void MageSessionObject::Skill_2()
{

}

void MageSessionObject::SetStage_1Position()
{
	SetPosition(XMFLOAT3(-1370.45, 0, -1450.89f));
	m_maxHp = m_hp = 480;
	m_attackDamage = 30;

	m_skillCoolTime[0] = std::chrono::seconds(20);
	m_skillCoolTime[1] = std::chrono::seconds(10);
	m_skillDuration[0] = std::chrono::seconds(10);
	m_skillDuration[1] = std::chrono::seconds(10);
	m_skillInputTime[0] = std::chrono::high_resolution_clock::now();
	m_skillInputTime[1] = std::chrono::high_resolution_clock::now();
}

void MageSessionObject::SetBossStagePosition()
{
	SetPosition(XMFLOAT3(20, 0, -285));
	m_maxHp = m_hp = 480;
	m_attackDamage = 30;
}

void TankerSessionObject::Skill_1()
{

}

void TankerSessionObject::Skill_2()
{

}

void TankerSessionObject::SetStage_1Position()
{
	SetPosition(XMFLOAT3(-1260.3f, 0, -1510.7f));
	m_maxHp = m_hp = 600;
	m_attackDamage = 60;
}

void TankerSessionObject::SetBossStagePosition()
{
	SetPosition(XMFLOAT3(82, 0, -223.0f));
	m_maxHp = m_hp = 600;
	m_attackDamage = 60;
}

void ArcherSessionObject::Skill_1()
{

}

void ArcherSessionObject::Skill_2()
{

}

void ArcherSessionObject::SetStage_1Position()
{
	SetPosition(XMFLOAT3(-1340.84f, 0, -1520.93f));
	m_maxHp = m_hp = 250;
	m_attackDamage = 200;
}

void ArcherSessionObject::SetBossStagePosition()
{
	SetPosition(XMFLOAT3(123, 0, -293));
	m_maxHp = m_hp = 250;
	m_attackDamage = 200;
}

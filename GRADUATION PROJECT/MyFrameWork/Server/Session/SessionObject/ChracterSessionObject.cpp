#include "stdafx.h"
#include "ChracterSessionObject.h"
#include "../UserSession.h"
#include "../../Logic/Logic.h"
#include "../../IOCPNetwork/protocol/protocol.h"
#include "../../MapData/MapData.h"
#include "../../Room/RoomManager.h"
#include "../../Timer/Timer.h"

extern Logic g_logic;
extern MapData g_bossMapData;
extern MapData g_stage1MapData;
extern RoomManager g_RoomManager;
extern Timer g_Timer;

//#define CHARCTER_MOVE_LOG 1

ChracterSessionObject::ChracterSessionObject(ROLE r) : SessionObject(8.0f)
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

void ChracterSessionObject::AttackedHp(float damage)
{
	if (m_Shield > FLT_EPSILON) {
		m_Shield -= damage * (1.0f - m_damageRedutionRate);
		if (m_Shield < 0.0f) {
			m_hp += m_Shield;
			m_Shield = 0.0f;
		}
		return;
	}
	m_hp -= damage * (1.0f - m_damageRedutionRate);
}

bool ChracterSessionObject::AdjustPlayerInfo(DirectX::XMFLOAT3& position) // , DirectX::XMFLOAT3& rotate
{
	//m_rotateAngle = rotate;
	if (Vector3::Length(Vector3::Subtract(m_position, position)) < 2.0f) { // ���� �� �̸��̶�� ���� ����, �̻��̶�� ���ǵ� ��?���� �����ϰ� ��ġ ���� ��ġ�� ��ȯ
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
		if (
			((d & DIRECTION::LEFT) == DIRECTION::LEFT)
			&&
			((d & DIRECTION::RIGHT) == DIRECTION::RIGHT)
			)
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
		m_applyDirection = d;
		fRotateAngle = fRotateAngle * (3.14159265359f / 180.0f);
		xmf3Rev.x = xmf3Look.x * cos(fRotateAngle) - xmf3Look.z * sin(fRotateAngle);
		xmf3Rev.z = xmf3Look.x * sin(fRotateAngle) + xmf3Look.z * cos(fRotateAngle);
		xmf3Rev = Vector3::Normalize(xmf3Rev);

		m_directionVector = xmf3Rev;
	}
	else
		m_applyDirection = d;
}

void ChracterSessionObject::SetMouseInput(bool LmouseInput, bool RmouseInput)
{
	m_leftmouseInput = LmouseInput;
	m_rightmouseInput = RmouseInput;
}

void ChracterSessionObject::StartMove(DIRECTION d, std::chrono::utc_clock::time_point& recvTime)
{
	//std::cout << "ChracterSessionObject::StartMove()" << std::endl;
	if (m_inputDirection == DIRECTION::IDLE) {
		m_lastMoveTime = std::chrono::high_resolution_clock::now();
		m_inputDirection = (DIRECTION)(m_inputDirection | d);
		SetDirection(m_inputDirection);
		auto serverUtcTime = std::chrono::utc_clock::now();
		double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(serverUtcTime - recvTime).count();
		durationTime = (double)durationTime / 1000.0f;//microseconds to mill
		durationTime = (double)durationTime / 1000.0f;//milliseconds to sec
		XMFLOAT3 pos = m_position = Vector3::Add(m_position, m_directionVector, durationTime * 50.0f);
		std::cout << "ChracterSessionObject::StartMove() - m_position:" << m_position.x << ", "
			<< m_position.y << ", " << m_position.z << std::endl;
		SERVER_PACKET::MovePacket sendPacket;
		sendPacket.direction = d;
		sendPacket.role = m_InGameRole;
		sendPacket.type = SERVER_PACKET::MOVE_KEY_DOWN;
		sendPacket.time = serverUtcTime;
		sendPacket.position = pos;
		sendPacket.moveVec = m_directionVector;
		sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
		g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, m_InGameRole, &sendPacket);
		return;
	}
	m_inputDirection = (DIRECTION)(m_inputDirection | d);
	SetDirection(m_inputDirection);
	SERVER_PACKET::MovePacket sendPacket;
	sendPacket.direction = d;
	sendPacket.role = m_InGameRole;
	sendPacket.type = SERVER_PACKET::MOVE_KEY_DOWN;
	auto serverUtcTime = std::chrono::utc_clock::now();
	sendPacket.time = serverUtcTime;
	sendPacket.position = m_position;
	sendPacket.moveVec = m_directionVector;
	sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, m_InGameRole, &sendPacket);
}

void ChracterSessionObject::StopMove()
{
	//PrintCurrentTime();
	//std::cout << "ChracterSessionObject::StopMove() - Look Dir: " << m_directionVector.x << ", " << m_directionVector.y << ", " << m_directionVector.z << std::endl;
	//std::cout << "ChracterSessionObject::StopMove() - Pos: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
	// m_prevDirection = m_inputDirection;
	m_applyDirection = m_inputDirection = DIRECTION::IDLE;
}

void ChracterSessionObject::ChangeDirection(DIRECTION d)
{
	m_inputDirection = (DIRECTION)(m_inputDirection ^ d);
	SetDirection(m_inputDirection);
}

void ChracterSessionObject::SetShield(bool active)
{
	if (active) {
		m_Shield = 200.0f;
		m_damageRedutionRate = 0.2f;
		return;
	}
	m_Shield = 0.0f;
	m_damageRedutionRate = 0.0f;
	//(0.4f * active - 0.2f);	// active : 0.2f 데미지 20% 감소 // inactive : -0.2f 원상복구

}

bool ChracterSessionObject::Move(float elapsedTime)
{
	if (m_position.y < 0.0f) {
		std::cout << "이상" << std::endl;
	}
	if (m_applyDirection != DIRECTION::IDLE)
	{
#ifdef _DEBUG
		//std::cout << "character::Move() - elapsedTime: " << elapsedTime << std::endl;
#endif 		 
		if (!CheckCollision(m_directionVector, elapsedTime)) {
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, elapsedTime * m_speed));
			SetPosition(m_position);
		}
		return true;
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

std::pair<float, XMFLOAT3> ChracterSessionObject::GetNormalVectorSphere(XMFLOAT3& point)
{
	XMFLOAT3 normalVec = Vector3::Subtract(m_position, point);
	float normalSize = Vector3::Length(normalVec);
	normalVec = Vector3::Normalize(normalVec);
	return std::pair<float, XMFLOAT3>(normalSize, normalVec);
}

bool ChracterSessionObject::IsDurationEndTimeSkill_1()
{
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_prevSkillInputTime[0]);
	if (m_skillDuration[0] < durationTime) return true;
	return false;
}

bool ChracterSessionObject::IsDurationEndTimeSkill_2()
{
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_prevSkillInputTime[1]);
	if (m_skillDuration[1] < durationTime) return true;
	return false;
}

std::pair<bool, XMFLOAT3> ChracterSessionObject::CheckCollisionMap_Boss(XMFLOAT3& normalVector, XMFLOAT3& moveDirection, float ftimeElapsed)
{
	std::vector<MapCollide>& Collides = g_bossMapData.GetCollideData();
	for (auto& collide : Collides) {
		if (collide.GetObb().Intersects(m_SPBB)) {

			auto& relationIdxsVector = collide.GetRelationCollisionIdxs();
			int secondCollide = -1;
			for (auto& otherCol : relationIdxsVector) {
				if (Collides[otherCol].GetObb().Intersects(m_SPBB)) {
					secondCollide = otherCol;
					break;
				}
			}
			if (secondCollide == -1) {//m_SPBB								
				auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, m_position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//노말 벡터
				XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//슬라이딩 벡터
				float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
				float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//슬라이딩 벡터와 무브 벡터 내적 값				
				collideSlidingVector = Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut, false);
				normalVector = collideNormalVector;
				collideNormalVector = Vector3::ScalarProduct(collideNormalVector, 0.06f * normalVectorDotProductReslut, false);
				//return std::pair<bool, XMFLOAT3>(true, Vector3::Add(collideSlidingVector, collideNormalVector));
				return std::pair<bool, XMFLOAT3>(true, collideSlidingVector);
			}
			else {
				auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, m_position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//노말 벡터
				//XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//슬라이딩 벡터
				float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);//노말 벡터가 가져야할 크기(충돌 위치 조정을 위한)
				float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//슬라이딩 벡터와 룩 벡터 내적 값				

				auto CollidePolygonNormalVector2 = Collides[secondCollide].CalSlidingVector(m_SPBB, m_position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//노말 벡터
				//XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//슬라이딩 벡터
				float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);//노말 벡터가 가져야할 크기(충돌 위치 조정을 위한)
				float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//슬라이딩 벡터와 룩 벡터 내적 값

				XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, Collides[secondCollide].GetObb().Center));
				resultSlidingVector.y = 0.0f;
				float dotRes = Vector3::DotProduct(resultSlidingVector, moveDirection);
				if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);
				normalVector = Vector3::Normalize(Vector3::Add(collideNormalVector1, collideNormalVector2));
				collideNormalVector1 = Vector3::ScalarProduct(collideNormalVector1, 0.3f * normalVectorDotProductResult1, false);
				collideNormalVector2 = Vector3::ScalarProduct(collideNormalVector2, 0.3f * normalVectorDotProductResult2, false);
				return std::pair<bool, XMFLOAT3>(true, Vector3::Add(resultSlidingVector, Vector3::Add(collideNormalVector1, collideNormalVector2)));
			}
		}
	}
	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
}

std::pair<bool, XMFLOAT3> ChracterSessionObject::CheckCollisionMap_Stage(XMFLOAT3& normalVector, XMFLOAT3& moveDirection, float ftimeElapsed)
{
	std::vector<MapCollide>& Collides = g_stage1MapData.GetCollideData();
	for (auto& collide : Collides) {
		if (collide.GetObb().Intersects(m_SPBB)) {

			auto& relationIdxsVector = collide.GetRelationCollisionIdxs();
			int secondCollide = -1;
			for (auto& otherCol : relationIdxsVector) {
				if (Collides[otherCol].GetObb().Intersects(m_SPBB)) {
					secondCollide = otherCol;
					break;
				}
			}
			if (secondCollide == -1) {//m_SPBB								
				auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, m_position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//노말 벡터
				XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//슬라이딩 벡터
				float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
				float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//슬라이딩 벡터와 무브 벡터 내적 값				
				collideSlidingVector = Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut, false);
				normalVector = collideNormalVector;
				collideNormalVector = Vector3::ScalarProduct(collideNormalVector, 0.06f * normalVectorDotProductReslut, false);
				return std::pair<bool, XMFLOAT3>(true, collideSlidingVector);
				//return std::pair<bool, XMFLOAT3>(true, Vector3::Add(collideSlidingVector, collideNormalVector));
			}
			else {
				auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, m_position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//노말 벡터
				//XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//슬라이딩 벡터
				float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);//노말 벡터가 가져야할 크기(충돌 위치 조정을 위한)
				float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//슬라이딩 벡터와 룩 벡터 내적 값				

				auto CollidePolygonNormalVector2 = Collides[secondCollide].CalSlidingVector(m_SPBB, m_position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//노말 벡터
				//XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//슬라이딩 벡터
				float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);//노말 벡터가 가져야할 크기(충돌 위치 조정을 위한)
				float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//슬라이딩 벡터와 룩 벡터 내적 값

				XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, Collides[secondCollide].GetObb().Center));
				resultSlidingVector.y = 0.0f;
				float dotRes = Vector3::DotProduct(resultSlidingVector, moveDirection);
				if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);
				normalVector = Vector3::Normalize(Vector3::Add(collideNormalVector1, collideNormalVector2));
				collideNormalVector1 = Vector3::ScalarProduct(collideNormalVector1, 0.3f * normalVectorDotProductResult1, false);
				collideNormalVector2 = Vector3::ScalarProduct(collideNormalVector2, 0.3f * normalVectorDotProductResult2, false);
				return std::pair<bool, XMFLOAT3>(true, Vector3::Add(resultSlidingVector, Vector3::Add(collideNormalVector1, collideNormalVector2)));
			}
		}
	}
	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
}

std::pair<bool, XMFLOAT3> ChracterSessionObject::CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	int collideCnt = 0;
	std::vector<std::pair<XMFLOAT3, XMFLOAT3> >  collideCharacterData;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	for (auto& playCharacter : roomRef.GetPlayCharacters()) {
		if (m_InGameRole != playCharacter.first)
		{
			auto normalVecRes = GetNormalVectorSphere(playCharacter.second->GetPos());
			if (normalVecRes.first <= m_SPBB.Radius + 8.0f) {
				normalVecRes.second;
				collideCnt++;
				XMFLOAT3 normalVec = normalVecRes.second;
				XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
				float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
				if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
				collideCharacterData.emplace_back(normalVec, slidingVec);
			}
		}
	}
	if (collideCnt == 0)
		return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
	if (collideCnt == 3) {
		return std::pair<bool, XMFLOAT3>(true, XMFLOAT3(0, 0, 0));
	}
	XMFLOAT3 resultNormal = XMFLOAT3(0, 0, 0);
	XMFLOAT3 resultSliding = XMFLOAT3(0, 0, 0);
	for (auto& character : collideCharacterData) {
		resultNormal = Vector3::Add(resultNormal, character.first);
		resultSliding = Vector3::Add(resultSliding, character.second);
	}
	resultNormal = Vector3::Normalize(resultNormal);
	resultSliding = Vector3::Normalize(resultSliding);
	resultNormal = Vector3::ScalarProduct(resultNormal, 0.3f, false);
	//std::cout <<"moveDir"
	return std::pair<bool, XMFLOAT3>(true, resultSliding);
}

std::pair<bool, XMFLOAT3> ChracterSessionObject::CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	SmallMonsterSessionObject* monsterArr = roomRef.GetStageMonsterArr();
	std::vector < std::pair<XMFLOAT3, XMFLOAT3> > myCollideData_NPC;
	myCollideData_NPC.reserve(4);

	int collideCnt = 0;
	for (int i = 0; i < 15; i++) {
		if (!monsterArr[i].IsAlive())continue;
		auto normalVecRes = GetNormalVectorSphere(monsterArr[i].GetPos());
		if (normalVecRes.first >= m_SPBB.Radius + 8.0f)continue;
		if (collideCnt == 3) {
			return std::pair<bool, XMFLOAT3>(true, XMFLOAT3(0, 0, 0));
		}
		collideCnt++;
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		myCollideData_NPC.emplace_back(normalVec, slidingVec);
	}
	if (collideCnt == 0)	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));

	XMFLOAT3 normalVecResult = XMFLOAT3(0, 0, 0);
	XMFLOAT3 slidingVecResult = XMFLOAT3(0, 0, 0);

	for (auto& collideResult : myCollideData_NPC) {
		normalVecResult = Vector3::Add(normalVecResult, collideResult.first);
		slidingVecResult = Vector3::Add(slidingVecResult, collideResult.second);
	}
	if (collideCnt) {
		normalVecResult = Vector3::Normalize(normalVecResult);
		normalVecResult = Vector3::ScalarProduct(normalVecResult, 0.02f);
		slidingVecResult = Vector3::Normalize(slidingVecResult);
	}
	XMFLOAT3 collideNPCMoveDir = Vector3::Normalize(Vector3::Add(normalVecResult, slidingVecResult));
	return std::pair<bool, XMFLOAT3>(true, slidingVecResult);
}

bool ChracterSessionObject::CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	std::pair<bool, XMFLOAT3> mapCollideResult;
	XMFLOAT3 mapNormalVector;
	if (roomRef.GetRoomState() == ROOM_STAGE1) {
		mapCollideResult = CheckCollisionMap_Stage(mapNormalVector, moveDirection, ftimeElapsed);
	}
	else if (roomRef.GetRoomState() == ROOM_BOSS) {
		mapCollideResult = CheckCollisionMap_Boss(mapNormalVector, moveDirection, ftimeElapsed);
	}
	auto CharacterCollide = CheckCollisionCharacter(moveDirection, ftimeElapsed);
	if (CharacterCollide.first && std::abs(CharacterCollide.second.x) < DBL_EPSILON && std::abs(CharacterCollide.second.z) < DBL_EPSILON) {//캐릭터 콜리전으로 인해 아예 못움직임
#ifdef CHARCTER_MOVE_LOG
		std::cout << "character no Move" << std::endl;
#endif
		return true;
	}
	if (mapCollideResult.first) {//맵에 충돌 됨
		if (CharacterCollide.first) {//캐릭터가 충돌 됨		
			float dotRes = Vector3::DotProduct(Vector3::Normalize(mapCollideResult.second), Vector3::Normalize(CharacterCollide.second));
			if (dotRes > 0.2f) {//충돌 벡터가 방향이 비슷함
				auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
				if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//노말 몬스터 콜리전으로 인해 아예 못움직임
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "normalMonster no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
				XMFLOAT3 moveDir = Vector3::Normalize(Vector3::Add(mapCollideResult.second, CharacterCollide.second));
				if (normalMonsterCollide.first) {//노말 몬스터 충돌 됨
					float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), moveDir);
					if (dotRes > 0.2f) {//방향이 맞음						
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, moveDir)), 0.9f * m_speed * ftimeElapsed));
						SetPosition(m_position);
#ifdef CHARCTER_MOVE_LOG
						PrintCurrentTime();
						std::cout << "normalMonster &char & map Move" << std::endl;
						std::cout << std::endl;
#endif
						return true;
					}
					else {//움직일 수가 없음
#ifdef CHARCTER_MOVE_LOG
						PrintCurrentTime();
						std::cout << "normalMonster &char & map Move" << std::endl;
						std::cout << std::endl;
#endif
						return true;
					}
				}
				else {//노말 몬스터와 충돌하지 않음					
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(Vector3::Normalize(moveDir), 0.9f * m_speed * ftimeElapsed));
					SetPosition(m_position);
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "char & map Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
			}
			else {//아무것도 충돌하지 암ㅎ음
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & char no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			return true;
		}
		//캐릭터가 충돌하진 않았지만 노말 몬스터 체크
		auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
		if (normalMonsterCollide.first) {//노말 몬스터와 충돌함
			if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//노말 몬스터 콜리전으로 인해 아예 못움직임
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), Vector3::Normalize(mapCollideResult.second));
			if (dotRes > 0.2f) {//맵과 노말 몬스터 충돌 벡터 방향이 비슷함
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, mapCollideResult.second)), 0.9f * m_speed * ftimeElapsed));
				SetPosition(m_position);
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & monster Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			else {//아예 다름 -> 움직임 x
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & monster no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
		}
		else {//노말 몬스터와 충돌하지 않음 => 맵만 충돌
			float dotResult = Vector3::DotProduct(mapNormalVector, moveDirection);
			if (dotResult > 0.121) {
				return false;
			}
#ifdef CHARCTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "map Move" << std::endl;
			std::cout << "prev collision position: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
#endif 
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(mapCollideResult.second, m_speed * ftimeElapsed, false));
			SetPosition(m_position);
#ifdef CHARCTER_MOVE_LOG
			std::cout << "after collision position: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
#endif
			return true;
		}
	}
	//맵 충돌 하지 않음
	if (CharacterCollide.first) {//캐릭터 와 충돌
		auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
		if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//캐릭터 콜리전으로 인해 아예 못움직임
#ifdef CHARCTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "monster no Move" << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
		if (normalMonsterCollide.first) {//노말 몬스터와 충돌
			float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), Vector3::Normalize(CharacterCollide.second));
			if (dotRes > 0.2f) {//캐릭터 노말 몬스터 벡터
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, CharacterCollide.second)), 0.9f * m_speed * ftimeElapsed));
				SetPosition(m_position);
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster & character Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			else {//벡터가 달라서 움직이지 못함
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster & character no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
		}
		else {// 노말 몬스터와 충돌하지 않음 -> 캐릭터만 충돌
			XMFLOAT3 moveVec = Vector3::ScalarProduct(CharacterCollide.second, 0.9f * m_speed * ftimeElapsed);
#ifdef CHARCTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "character Move" << std::endl;
			std::cout << "prev collision position: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
#endif
			m_position = Vector3::Add(m_position, moveVec);
			SetPosition(m_position);
			//PrintCurrentTime();
			//std::cout << "elapsedTime: " << ftimeElapsed << std::endl;
			//std::cout << "speed: " << m_speed << std::endl;
			//std::cout << "position: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
			//std::cout << "moveVec: " << moveVec.x << ", " << moveVec.y << ", " << moveVec.z << std::endl << std::endl;
#ifdef CHARCTER_MOVE_LOG
			std::cout << "after collision position: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
			std::cout << "character MoveDirecion: " << moveDirection.x << ", " << moveDirection.y << ", " << moveDirection.z << std::endl;
			std::cout << "slidingVec: " << moveVec.x << ", " << moveVec.y << ", " << moveVec.z << std::endl;
			std::cout << "slidingVec Size: " << Vector3::Length(moveVec) << std::endl << std::endl;
#endif
			return true;
		}
	}
	//캐릭터와 충돌하지 않음
	auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
	if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//노말 몬스터 콜리전으로 인해 아예 못움직임
#ifdef CHARCTER_MOVE_LOG
		PrintCurrentTime();
		std::cout << "monster no Move" << std::endl;
		std::cout << std::endl;
#endif
		return true;
	}
	if (normalMonsterCollide.first) {//노말 몬스터와 충돌함
		XMFLOAT3 slidingVec = Vector3::ScalarProduct(normalMonsterCollide.second, 0.9f * m_speed * ftimeElapsed);
#ifdef CHARCTER_MOVE_LOG
		PrintCurrentTime();
		std::cout << "monster Move" << std::endl;
		std::cout << "collision slidingVec: " << normalMonsterCollide.second.x << ", " << normalMonsterCollide.second.y << ", " << normalMonsterCollide.second.z << std::endl;
		std::cout << "collision slidingSize: " << Vector3::Length(slidingVec) << std::endl;
		std::cout << "collision prev position: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
#endif
		m_position = Vector3::Add(m_position, slidingVec);
		SetPosition(m_position);
#ifdef CHARCTER_MOVE_LOG
		std::cout << "collision after position: " << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl << std::endl;
		std::cout << std::endl;
#endif
		return true;
	}
	return false;
}


void WarriorSessionObject::Skill_1(XMFLOAT3& posOrDir)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	roomRef.ExecuteLongSwordAttack(posOrDir, m_position);
}

void WarriorSessionObject::Skill_2(XMFLOAT3& posOrDir)
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
}

void WarriorSessionObject::ExecuteCommonAttack(XMFLOAT3& attackDir, int power)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.MeleeAttack(ROLE::WARRIOR, attackDir, m_position, power);
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

void MageSessionObject::Skill_1(XMFLOAT3& posOrDir)
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	g_RoomManager.GetRunningRoomRef(m_roomId).StartHealPlayerCharacter();
}

void MageSessionObject::Skill_2(XMFLOAT3& posOrDir)
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteMageThunder(posOrDir);
}

void MageSessionObject::ExecuteCommonAttack(XMFLOAT3& attackDir, int power)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	//offset 적용 안됨
	roomRef.ShootBall(attackDir, m_position);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_BALL;
	sendPacket.dir = attackDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::PRIEST, &sendPacket);
}

void MageSessionObject::SetStage_1Position()
{
	SetPosition(XMFLOAT3(-1370.45, 0, -1450.89f));
	m_maxHp = m_hp = 480;
	m_hp = 100.0f;
	m_attackDamage = 30;
}

void MageSessionObject::SetBossStagePosition()
{
	SetPosition(XMFLOAT3(20, 0, -285));
	m_maxHp = m_hp = 480;
	m_attackDamage = 30;
}

void TankerSessionObject::Skill_1(XMFLOAT3& posOrDir)
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(2400), m_roomId ,EV_TANKER_SHIELD_START };	//애니메이션 진행 시간 1.4초 + 공 날아가는 시간 1초
	g_Timer.InsertTimerQueue(gameStateEvent);
	SERVER_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	sendPacket.type = SERVER_PACKET::SHIELD_START;
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);
}

void TankerSessionObject::Skill_2(XMFLOAT3& posOrDir)
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteHammerAttack(posOrDir, m_position);
}

void TankerSessionObject::ExecuteCommonAttack(XMFLOAT3& attackDir, int power)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.MeleeAttack(ROLE::TANKER, attackDir, m_position, 0);
	//공격 애니메이션 패킷
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

void ArcherSessionObject::Skill_1(XMFLOAT3& posOrDir)
{
	//3갈래 발사
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteThreeArrow(posOrDir, m_position);
}

void ArcherSessionObject::Skill_2(XMFLOAT3& posOrDir)
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.StartSkyArrow(posOrDir);
}

void ArcherSessionObject::ExecuteCommonAttack(XMFLOAT3& attackDir, int power)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	//offset ShootArrow 내부에서 적용함

	if (power == 0) {
		roomRef.ShootArrow(attackDir, m_position, 100.0f, 100.0f);
	}
	else if (power == 1) {
		roomRef.ShootArrow(attackDir, m_position, 140.0f, 140.0f);
	}
	else if (power == 2) {
		roomRef.ShootArrow(attackDir, m_position, 200.0f, 200.0f);
	}

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_ARROW;
	sendPacket.dir = attackDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
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

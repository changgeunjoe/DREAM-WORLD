#include "stdafx.h"
#include "SmallMonsterSessionObject.h"
#include "../../Logic/Logic.h"
#include "../../Room/RoomManager.h"
#include "../../MapData/MapData.h"

extern Logic g_logic;
extern RoomManager g_RoomManager;
extern MapData g_bossMapData;
extern MapData g_stage1MapData;

SmallMonsterSessionObject::SmallMonsterSessionObject() :SessionObject(8.0f)
{
	m_speed = 30.0f;
	m_maxHp = m_hp = 100;
}

SmallMonsterSessionObject::SmallMonsterSessionObject(int roomId) :SessionObject(8.0f)
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
	auto& collisionData = g_stage1MapData.GetCollideData();
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	SmallMonsterSessionObject* monsterArr = roomRef.GetStageMonsterArr();

	std::vector < std::pair<XMFLOAT3, XMFLOAT3> > myCollideData_NPC(4);//normal sliding
	XMFLOAT3 currentPosition = m_position;

	bool isCollideNPC = false;
	int collideCnt = 0;
	for (int i = 0; i < 15; i++) {
		if (i == m_id) continue;
		auto normalVecRes = GetNormalVectorSphere(monsterArr[i].GetPos());
		if (normalVecRes.first >= 16.0f)continue;
		if (collideCnt == 2)break;
		collideCnt++;
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, GetLook());
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		myCollideData_NPC.emplace_back(normalVec, slidingVec);
		//float normalSize = 16.0f - normalVecRes.first;
		//m_position = Vector3::Add(m_position, Vector3::ScalarProduct(slidingVec, std::abs(directionVectorDotslidingVec) * 30.0f * fTimeElapsed, false));//틱마다 움직임
		//m_position = Vector3::Add(m_position, Vector3::ScalarProduct(normalVec, 0.2f * normalSize * fTimeElapsed, false));//틱마다 움직임		
		isCollideNPC = true;
	}
	XMFLOAT3 normalVecResult = XMFLOAT3(0, 0, 0);
	XMFLOAT3 slidingVecResult = XMFLOAT3(0, 0, 0);

	for (auto& collideResult : myCollideData_NPC) {
		normalVecResult = Vector3::Add(normalVecResult, collideResult.first);
		slidingVecResult = Vector3::Add(slidingVecResult, collideResult.second);
	}
	if (collideCnt) {
		normalVecResult = Vector3::Normalize(normalVecResult);
		slidingVecResult = Vector3::Normalize(slidingVecResult);
		currentPosition = Vector3::Add(currentPosition, Vector3::ScalarProduct(normalVecResult, 30.0f * elapsedTime));
		currentPosition = Vector3::Add(currentPosition, Vector3::ScalarProduct(slidingVecResult, 30.0f * elapsedTime));
	}
	XMFLOAT3 collideNPCMoveDir = Vector3::Add(normalVecResult, slidingVecResult);

	if (isCollideNPC) {
		for (auto& collide : collisionData) {
			if (collide.GetObb().Intersects(m_SPBB)) {
				std::vector<int>& relationIdxsVector = collide.GetRelationCollisionIdxs();
				int secondCollide = -1;
				for (auto& otherCol : relationIdxsVector) {
					if (collisionData[otherCol].GetObb().Intersects(m_SPBB)) {
						secondCollide = otherCol;
						break;
					}
				}
				if (secondCollide == -1) {
					auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, currentPosition, GetLook());//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
					XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//노말 벡터
					XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//슬라이딩 벡터
					float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
					float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//슬라이딩 벡터와 무브 벡터 내적 값					
					//NPC Collide Vector, Collision Box Vector DotP
					float dotProductResult = Vector3::DotProduct(Vector3::Add(normalVecResult, slidingVecResult), Vector3::Add(collideSlidingVector, collideNormalVector));
					if (dotProductResult < 0) {
						currentPosition = m_position;
					}
					currentPosition = Vector3::Add(currentPosition, Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut * 30.0f * elapsedTime));
					currentPosition = Vector3::Add(currentPosition, Vector3::ScalarProduct(collideNormalVector, 0.4f * normalVectorDotProductReslut * 30.0f * elapsedTime));
				}
				else {
					auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, currentPosition, GetLook());
					auto CollidePolygonNormalVector2 = collisionData[secondCollide].CalSlidingVector(m_SPBB, currentPosition, GetLook());

					XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//노말 벡터
					XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//슬라이딩 벡터
					float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);
					float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//슬라이딩 벡터와 무브 벡터 내적 값

					XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//노말 벡터
					XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//슬라이딩 벡터
					float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);
					float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//슬라이딩 벡터와 무브 벡터 내적 값

					XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, collisionData[secondCollide].GetObb().Center));
					resultSlidingVector.y = 0.0f;
					float dotRes = Vector3::DotProduct(resultSlidingVector, GetLook());
					if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);

					//NPC Collide Vector, Collision Box Vector DotP
					float dotProductResult = Vector3::DotProduct(Vector3::Add(normalVecResult, slidingVecResult),
						Vector3::Add(resultSlidingVector, Vector3::Add(collideNormalVector1, collideNormalVector2)));
					if (dotProductResult < 0) {
						currentPosition = m_position;
					}
					currentPosition = Vector3::Add(currentPosition, Vector3::ScalarProduct(resultSlidingVector, 30.0f * elapsedTime));
					currentPosition = Vector3::Add(currentPosition, Vector3::ScalarProduct(collideNormalVector1, 0.3f * normalVectorDotProductResult1 * 30.0f * elapsedTime));
					currentPosition = Vector3::Add(currentPosition, Vector3::ScalarProduct(collideNormalVector2, 0.3f * normalVectorDotProductResult2 * 30.0f * elapsedTime));
				}
				break;
			}
		}
		XMFLOAT3 desVector = Vector3::Subtract(m_desPos, currentPosition);
		desVector = Vector3::Normalize(desVector);
		float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
		if (rightDotRes >= 0) {
			Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);			

		}
		else {
			Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
		}
		m_position = currentPosition;
		m_SPBB.Center = currentPosition;
		m_SPBB.Center.y += m_fBoundingSize;
		return true;
	}
	XMFLOAT3 myLook = GetLook();
	XMFLOAT3 desVector = Vector3::Subtract(m_desPos, m_position);
	float desDis = Vector3::Length(desVector);
	desVector = Vector3::Normalize(desVector);
	float frontDotRes = Vector3::DotProduct(desVector, myLook);
	//std::cout << "desVector: " << desVector.x << ", " << desVector.y << ", " << desVector.z << endl;

	if (desDis <= 30.0f) {//근접 했을때
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨 - 방향이 맞지 않음
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
			}
			else {
				Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			}
		}
	}
	else if (desDis <= 80.0f) {
		for (auto& collide : collisionData) {
			if (collide.GetObb().Intersects(m_SPBB)) {
				std::vector<int>& relationIdxsVector = collide.GetRelationCollisionIdxs();
				int secondCollide = -1;
				for (auto& otherCol : relationIdxsVector) {
					if (collisionData[otherCol].GetObb().Intersects(m_SPBB)) {
						secondCollide = otherCol;
						break;
					}
				}
				if (secondCollide == -1) {
					auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, m_position, GetLook());//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
					XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//노말 벡터
					XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//슬라이딩 벡터
					float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
					float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//슬라이딩 벡터와 무브 벡터 내적 값
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut * 30.0f * elapsedTime));
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector, 0.2f * normalVectorDotProductReslut * 30.0f * elapsedTime));
				}
				else {
					auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, m_position, GetLook());
					auto CollidePolygonNormalVector2 = collisionData[secondCollide].CalSlidingVector(m_SPBB, m_position, GetLook());

					XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//노말 벡터
					XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//슬라이딩 벡터
					float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);
					float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//슬라이딩 벡터와 무브 벡터 내적 값

					XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//노말 벡터
					XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//슬라이딩 벡터
					float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);
					float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//슬라이딩 벡터와 무브 벡터 내적 값

					XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, collisionData[secondCollide].GetObb().Center));
					resultSlidingVector.y = 0.0f;
					float dotRes = Vector3::DotProduct(resultSlidingVector, GetLook());
					if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);

					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resultSlidingVector, 30.0f * elapsedTime));
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector1, 0.3f * normalVectorDotProductResult1 * 30.0f * elapsedTime));
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(collideNormalVector2, 0.3f * normalVectorDotProductResult2 * 30.0f * elapsedTime));
				}
				float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
				if (rightDotRes >= 0) {
					Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
				}
				else {
					Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
				}
				m_position = m_position;
				m_SPBB.Center = m_position;
				m_SPBB.Center.y += m_fBoundingSize;
				return true;
			}
		}


		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨			
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
			}
			else {
				Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			}
			myLook = GetLook();
		}
		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(GetLook(), 30.0f * elapsedTime, false));//틱마다 움직임		
	}
	else if (desDis <= 120.0f) {
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨 - 방향이 맞지 않음			
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
			}
			else {
				Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			}
		}
	}
	return true;
}

float SmallMonsterSessionObject::GetDistance(XMFLOAT3& point)
{
	return Vector3::Length(Vector3::Subtract(point, m_position));
}

std::pair<float, XMFLOAT3> SmallMonsterSessionObject::GetNormalVectorSphere(XMFLOAT3& point)
{
	XMFLOAT3 normalVec = Vector3::Subtract(m_position, point);
	float normalSize = Vector3::Length(normalVec);
	normalVec = Vector3::Normalize(normalVec);
	return std::pair<float, XMFLOAT3>(normalSize, normalVec);
}

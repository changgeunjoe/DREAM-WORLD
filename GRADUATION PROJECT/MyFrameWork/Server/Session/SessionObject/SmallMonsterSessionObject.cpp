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
	SmallMonsterSessionObject* monstersArr = roomRef.GetStageMonsterArr();
	//std::set<int> nearM;
	//m_nearMonsterSetLock.lock();
	//nearM = m_nearMonsterSet;
	//m_nearMonsterSetLock.unlock();
	bool isCollideNPC = false;
	for (int i = 0; i < 15; i++) {
		if (i == m_id)continue;
		auto normalVecRes = GetNormalVectorSphere(monstersArr[i].GetPos());
		if (normalVecRes.first >= 16.0f)continue;
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float normalSize = 16.0f - normalVecRes.first;
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, m_directionVector);
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(slidingVec, m_speed * elapsedTime, false));//틱마다 움직임
		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(normalVec, normalSize * elapsedTime, false));//틱마다 움직임
		m_SPBB.Center = m_position;
		m_SPBB.Center.y += m_fBoundingSize;
		isCollideNPC = true;
	}
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
				break;
			}
		}
		m_SPBB.Center = m_position;
		m_SPBB.Center.y += m_fBoundingSize;
		return true;
	}

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
					m_SPBB.Center = m_position;
					m_SPBB.Center.y += m_fBoundingSize;
					return true;
				}
			}
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

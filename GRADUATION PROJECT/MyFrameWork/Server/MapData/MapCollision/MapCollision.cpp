#include "stdafx.h"
//#include "MapCollision.h"
//
//MapCollision::MapCollision(XMFLOAT3& center, XMFLOAT3& extent, XMFLOAT4& quaternion, float& dotRes1, XMFLOAT3& normalVec1, float& dotRes2, XMFLOAT3& normalVec2)
//{
//	m_obb = BoundingOrientedBox(center, extent, quaternion);
//	normalVectorData[0] = std::make_pair(dotRes1, normalVec1);
//	normalVectorData[1] = std::make_pair(dotRes2, normalVec2);
//}
//
//std::tuple<XMFLOAT3, XMFLOAT3, float, float> MapCollision::CalSlidingVector(BoundingSphere& boundingSphere, XMFLOAT3& position, XMFLOAT3& moveDirectionVector)//충돌한 면의 노말 벡터, 계산된 슬라이딩 벡터 반환, 노멀 내적, 슬라이딩 내적 
//{
//	XMFLOAT3 retSlidingVector;
//	XMFLOAT3 retnormalVector;
//	float retNormalVecDotProduct;
//	//float	 retIntersectDistacneNormalVector;
//	float	 retSlidingVecDotProduct;
//	//center->player vector
//	XMFLOAT3 center = m_obb.Center;
//	center.y = 0.0f;
//	/*XMFLOAT3 tempPosition = position;
//	tempPosition = Vector3::Add(position, Vector3::ScalarProduct(moveDirectionVector, -1.0f ));*/
//	XMFLOAT3 centerToPlayerVector = Vector3::Subtract(position, center);//벡터
//	centerToPlayerVector.y = 0.0f;
//	float centerToPlayerDisrtance = Vector3::Length(centerToPlayerVector);//거리
//	centerToPlayerVector = Vector3::Normalize(centerToPlayerVector);
//	//float playerCollisionSphereIntersectDistance = 0.0f;
//	//플레이어가 위치한 면탐색을 위한
//	float forwardDotResult = Vector3::DotProduct(centerToPlayerVector, normalVectorData[0].second);//객체의 center와 플레이어와 normal간의 cos값
//	if (std::abs(forwardDotResult) >= normalVectorData[0].first) {//노말 벡터가 forward인 면 2개 판단		
//		//set normal
//		if (forwardDotResult >= 0) retnormalVector = normalVectorData[0].second;
//		else retnormalVector = Vector3::ScalarProduct(normalVectorData[0].second, -1.0f, true);
//		//set sliding
//		float judgeSlidingVectorDotRes = Vector3::DotProduct(normalVectorData[1].second, moveDirectionVector);//콜리전 객체의 right 벡터와 내적
//		if (judgeSlidingVectorDotRes >= 0) retSlidingVector = normalVectorData[1].second;
//		else retSlidingVector = Vector3::ScalarProduct(normalVectorData[1].second, -1.0f, true);
//		//playerCollisionSphereIntersectDistance = centerToPlayerDisrtance * std::abs(forwardDotResult) - m_obb.Extents.z;
//		retNormalVecDotProduct = std::abs(Vector3::DotProduct(normalVectorData[0].second, moveDirectionVector));
//		retSlidingVecDotProduct = std::abs(judgeSlidingVectorDotRes);
//	}
//	else {
//		float rightDotResult = Vector3::DotProduct(centerToPlayerVector, normalVectorData[1].second);
//		//set normal
//		if (rightDotResult >= 0) retnormalVector = normalVectorData[1].second;
//		else retnormalVector = Vector3::ScalarProduct(normalVectorData[1].second, -1.0f, true);
//		//set sliding
//		float judgeSlidingVectorDotRes = Vector3::DotProduct(normalVectorData[0].second, moveDirectionVector);//콜리전 객체의 right 벡터와 내적
//		if (judgeSlidingVectorDotRes >= 0) retSlidingVector = normalVectorData[0].second;
//		else retSlidingVector = Vector3::ScalarProduct(normalVectorData[0].second, -1.0f, true);
//		//playerCollisionSphereIntersectDistance = centerToPlayerDisrtance * std::abs(rightDotResult) - m_obb.Extents.x;
//		retNormalVecDotProduct = std::abs(Vector3::DotProduct(normalVectorData[1].second, moveDirectionVector));
//		retSlidingVecDotProduct = std::abs(judgeSlidingVectorDotRes);
//	}
//	//retIntersectDistacneNormalVector = boundingSphere.Radius - playerCollisionSphereIntersectDistance;
//	//if (std::abs(retIntersectDistacneNormalVector) < DBL_EPSILON)retIntersectDistacneNormalVector = 0.0f;
//	return { retnormalVector, retSlidingVector, retNormalVecDotProduct, retSlidingVecDotProduct };
//}

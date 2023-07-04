#include "stdafx.h"
#include "MapCollide.h"

MapCollide::MapCollide(XMFLOAT3& center, XMFLOAT3& extent, XMFLOAT4& quaternion, float& dotRes1, XMFLOAT3& normalVec1, float& dotRes2, XMFLOAT3& normalVec2)
{
	m_obb = BoundingOrientedBox(center, extent, quaternion);
	normalVectorData[0] = std::make_pair(dotRes1, normalVec1);
	normalVectorData[1] = std::make_pair(dotRes2, normalVec2);
}

std::pair< XMFLOAT3, XMFLOAT3> MapCollide::CalSlidingVector(XMFLOAT3& position, XMFLOAT3& lookVector)//충돌한 면의 노말 벡터, 계산된 슬라이딩 벡터 반환
{
	XMFLOAT3 Center2PlayerVector = Vector3::Subtract(position, m_obb.Center);
	Center2PlayerVector = Vector3::Normalize(Center2PlayerVector);
	float forwardDotResult = Vector3::DotProduct(Center2PlayerVector, normalVectorData[0].second);
	if (std::abs(forwardDotResult) > normalVectorData[0].first) {
		if (forwardDotResult < 0) {
			XMFLOAT3 retNormalVec = Vector3::ScalarProduct(normalVectorData[0].second, -1.0f, false);			
			return std::make_pair(retNormalVec, Vector3::Normalize(Vector3::Add(lookVector, Vector3::ScalarProduct(retNormalVec, 2.0f, false))));
		}
		return std::make_pair(normalVectorData[0].second, Vector3::Normalize(Vector3::Add(lookVector, Vector3::ScalarProduct(normalVectorData[0].second, 2.0f, false))));
	}
	float rightDotResult = Vector3::DotProduct(Center2PlayerVector, normalVectorData[1].second);
	if (rightDotResult < 0) {
		XMFLOAT3 retNormalVec = Vector3::ScalarProduct(normalVectorData[1].second, -1.0f, false);
		return std::make_pair(retNormalVec, Vector3::Normalize(Vector3::Add(lookVector, Vector3::ScalarProduct(retNormalVec, 2.0f, false))));
	}
	return std::make_pair(normalVectorData[1].second, Vector3::Normalize(Vector3::Add(lookVector, Vector3::ScalarProduct(normalVectorData[1].second, 2.0f, false))));
}

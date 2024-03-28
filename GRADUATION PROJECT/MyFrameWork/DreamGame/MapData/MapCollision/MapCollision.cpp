#include "stdafx.h"
#include "MapCollision.h"
#include "../../Character.h"

MapCollision::MapCollision(const XMFLOAT3& center, const XMFLOAT3& extent, const XMFLOAT4& quaternion, const float& dotRes1, const XMFLOAT3& normalVec1, const float& dotRes2, const XMFLOAT3& normalVec2)
	: m_collision(BoundingOrientedBox(center, extent, quaternion))
{
	normalVectorData[0] = std::make_pair(dotRes1, normalVec1);
	normalVectorData[1] = std::make_pair(dotRes2, normalVec2);
}

void MapCollision::AddRelationCollision(std::shared_ptr<MapCollision>& other)
{
	m_relationCollsions.push_back(other);
}

bool MapCollision::CollideMap(const BoundingSphere& boundingSphere) const
{
	return m_collision.Intersects(boundingSphere);
}

const std::pair<float, XMFLOAT3> MapCollision::GetSlidingVector(Character* gameObject, const XMFLOAT3& moveVector)
{
	XMFLOAT3 toGameObjectVector = gameObject->GetFromVector(m_collision.Center);
	toGameObjectVector.y = 0.0f;
	toGameObjectVector = Vector3::Normalize(toGameObjectVector);

	//ToGameObject벡터와 저장된 normal간의 cos값을 계산
	// => 객체로 가는 벡터 . 벽객체의 노말 방향을 내적하면 어떤 면에서 충돌인지 알 수 있음.
	float forwardDotResult = Vector3::DotProduct(toGameObjectVector, normalVectorData[0].second);
	XMFLOAT3 slidingVector;
	if (abs(forwardDotResult) > normalVectorData[0].first) {// 이 면과 충돌
		slidingVector = normalVectorData[1].second;//적용할 슬라이딩 벡터
	}
	else {
		slidingVector = normalVectorData[0].second;
	}
	XMFLOAT3 currentMoveVector = moveVector;
	//이동할 세기 -> 벽과 수직으로 이동하면, 많이 이동 못하게.
	float moveVectorDotResult = Vector3::DotProduct(currentMoveVector, slidingVector);
	//움지이는 방향과, 슬라이딩 벡터의 방향이 다르다면 * -1
	if (moveVectorDotResult < 0) slidingVector = Vector3::ScalarProduct(slidingVector, -1, true);
	return { abs(moveVectorDotResult), slidingVector };
}

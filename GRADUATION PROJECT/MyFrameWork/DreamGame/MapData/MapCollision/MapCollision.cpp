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

	//ToGameObject���Ϳ� ����� normal���� cos���� ���
	// => ��ü�� ���� ���� . ����ü�� �븻 ������ �����ϸ� � �鿡�� �浹���� �� �� ����.
	float forwardDotResult = Vector3::DotProduct(toGameObjectVector, normalVectorData[0].second);
	XMFLOAT3 slidingVector;
	if (abs(forwardDotResult) > normalVectorData[0].first) {// �� ��� �浹
		slidingVector = normalVectorData[1].second;//������ �����̵� ����
	}
	else {
		slidingVector = normalVectorData[0].second;
	}
	XMFLOAT3 currentMoveVector = moveVector;
	//�̵��� ���� -> ���� �������� �̵��ϸ�, ���� �̵� ���ϰ�.
	float moveVectorDotResult = Vector3::DotProduct(currentMoveVector, slidingVector);
	//�����̴� �����, �����̵� ������ ������ �ٸ��ٸ� * -1
	if (moveVectorDotResult < 0) slidingVector = Vector3::ScalarProduct(slidingVector, -1, true);
	return { abs(moveVectorDotResult), slidingVector };
}

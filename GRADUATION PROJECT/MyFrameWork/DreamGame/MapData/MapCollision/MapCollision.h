#pragma once
#include "../../stdafx.h"

using namespace DirectX;
class Character;
class MapCollision : public std::enable_shared_from_this<MapCollision>
{
public:
	MapCollision() = delete;
	//center, extent, quaternion, normalVec1, normalVec2
	MapCollision(const XMFLOAT3& center, const XMFLOAT3& extent, const XMFLOAT4& quaternion, const  float& dotRes1, const  XMFLOAT3& normalVec1, const  float& dotRes2, const XMFLOAT3& normalVec2);

	const BoundingOrientedBox& GetCollision() const
	{
		return m_collision;
	}
	void AddRelationCollision(std::shared_ptr<MapCollision>& other);

	bool CollideMap(const BoundingSphere& boundingSphere) const;
	const std::pair<float, XMFLOAT3> GetSlidingVector(Character* gameObject, const XMFLOAT3& moveVector);

private:
	BoundingOrientedBox m_collision;
	std::array<std::pair< float, XMFLOAT3>, 2> normalVectorData;//직사각형 콜리전 노말 벡터
	std::vector<std::weak_ptr<MapCollision>> m_relationCollsions;//연결된 콜리전 정보
};

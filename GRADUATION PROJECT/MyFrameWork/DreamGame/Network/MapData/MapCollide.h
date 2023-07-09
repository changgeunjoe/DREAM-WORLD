#pragma once
#include "../../stdafx.h"
using namespace DirectX;
class MapCollide
{
private:
	BoundingOrientedBox m_obb;
	std::pair< float, XMFLOAT3> normalVectorData[2];
	//각 노말에 대한 크기도 넣어야 될듯함 -> 직사각형의 가로와 세로의 절반 값
	std::vector<int> m_relationIdx;
public:
	MapCollide() {}
	MapCollide(XMFLOAT3& center, XMFLOAT3& extent, XMFLOAT4& quaternion, float& dotRes1, XMFLOAT3& normalVec1, float& dotRes2, XMFLOAT3& normalVec2);//center, extent, quaternion, normalVec1, normalVec2
	const BoundingOrientedBox& GetObb() { return m_obb; }
	std::tuple<XMFLOAT3, XMFLOAT3, float, float> CalSlidingVector(BoundingSphere& boundingSphere, XMFLOAT3& position, XMFLOAT3& moveDirectionVector);
	void SetRelationIdx(int idx) { m_relationIdx.push_back(idx); }
	std::vector<int>& GetRelationCollisionIdxs() { return m_relationIdx; }
};

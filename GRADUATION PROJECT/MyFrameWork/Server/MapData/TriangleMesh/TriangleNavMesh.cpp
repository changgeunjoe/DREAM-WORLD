#include "stdafx.h"
#include "TriangleNavMesh.h"

NavMesh::TriangleNavMesh::TriangleNavMesh(const int& triangleId, const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3, const int& idx1, const int& idx2, const int& idx3)
	: m_id(triangleId), m_vertexs(std::array<XMFLOAT3, 3>{v1, v2, v3})
{
	m_vertexIdxSet.insert(idx1);
	m_vertexIdxSet.insert(idx2);
	m_vertexIdxSet.insert(idx3);
	//무게 중심
	m_center = Vector3::ScalarProduct(Vector3::Add(m_vertexs[0], Vector3::Add(m_vertexs[1], m_vertexs[2])), 1.0f / 3.0f, false);
	m_vector01 = Vector3::Subtract(m_vertexs[1], m_vertexs[0]);
	m_vector12 = Vector3::Subtract(m_vertexs[2], m_vertexs[1]);
	m_vector20 = Vector3::Subtract(m_vertexs[0], m_vertexs[2]);
	//넓이는 삼각형 변 두개의 벡터 외적 값 / 2
	m_duobleAreaSize = Vector3::Length(Vector3::CrossProduct(m_vector01, m_vector12, false));
}

std::pair<bool, float> NavMesh::TriangleNavMesh::IsOnTriangleMesh(const DirectX::XMFLOAT3& position) const
{
	//밑에 연산은 삼각형 두개를 붙인 넓이 이기때문에, /2연산을 옆항으로 옮기거처럼 계산하기 위해 * 2.0f
	static constexpr float COMPARE_AREA_VALUE = FLT_EPSILON * 2.0f;
	XMFLOAT3 toVertex0 = Vector3::Subtract(XMFLOAT3(position.x, 0, position.z), XMFLOAT3(m_vertexs[0]));
	XMFLOAT3 toVertex1 = Vector3::Subtract(XMFLOAT3(position.x, 0, position.z), XMFLOAT3(m_vertexs[1]));
	XMFLOAT3 toVertex2 = Vector3::Subtract(XMFLOAT3(position.x, 0, position.z), XMFLOAT3(m_vertexs[2]));
	float doubleAreaSize = 0.0f;
	//현재 pos에 대해서, 삼각형 정점으로 가는 벡터를 구하고
	//각 벡터들을 외적 => (정점1로 가는 벡터 X 정점 2로 가는벡터) ... 겹치지 않게.
	//이 외적한 값의 크기를 다 더하면, 삼각형 크기와 같다면, pos는 삼각형 위에 있음.

	doubleAreaSize = Vector3::Length(Vector3::CrossProduct(toVertex0, toVertex1, false));
	doubleAreaSize += Vector3::Length(Vector3::CrossProduct(toVertex1, toVertex2, false));
	doubleAreaSize += Vector3::Length(Vector3::CrossProduct(toVertex0, toVertex0, false));

	float areaDIff = doubleAreaSize - m_duobleAreaSize;
	//결과 값이 비교할 넓이 값보다 작다면 삼각형 위에 있다.
	//doubleAreaSize는 삼각형의 크기보다 작기 힘듦. 작다면, 삼각형 내부위에 있고 오차 범위 때문에 그럴 수도?
	//	=> 삼각형 외부점에 대해서 외적하면, 삼각형 보다 더 큰 도형에 대해서 넓이가 나와버림.
	return { areaDIff < COMPARE_AREA_VALUE, areaDIff };
}

//const bool TriangleNavMesh::IsShareLine(const TriangleNavMesh& other) const
//{
//	std::vector<int> res;
//	std::ranges::set_intersection(m_vertexIdxSet, other.m_vertexIdxSet, std::back_inserter(res));
//	return res.size() > 1;
//}

const float NavMesh::TriangleNavMesh::GetDistance(const XMFLOAT3& position) const
{
	return Vector3::Length(Vector3::Subtract(XMFLOAT3(m_center), XMFLOAT3(position)));
}

const float NavMesh::TriangleNavMesh::GetDistance(const TriangleNavMesh& other) const
{
	return Vector3::Length(Vector3::Subtract(XMFLOAT3(m_center), XMFLOAT3(other.m_center)));
}

const float NavMesh::TriangleNavMesh::GetDistance(const float& x, const float& y, const float& z) const
{
	return Vector3::Length(Vector3::Subtract(XMFLOAT3(m_center), XMFLOAT3(x, y, z)));
}

std::optional<float> NavMesh::TriangleNavMesh::GetRelationMeshDistance(std::shared_ptr<TriangleNavMesh> relationMesh) const
{
	auto relationMeshIter = m_relationTriangleMesh.find(relationMesh);
	if (m_relationTriangleMesh.end() != relationMeshIter) {
		return relationMeshIter->second;
	}
	return std::nullopt;
}

void NavMesh::TriangleNavMesh::InsertRelationTriangleMesh(std::shared_ptr<TriangleNavMesh>& otherMesh)
{
	if (1 != m_relationTriangleMesh.count(otherMesh)) {
		m_relationTriangleMesh.emplace(otherMesh, GetDistance(*otherMesh));
	}
}

std::vector<int> NavMesh::TriangleNavMesh::GetRelationVertexIdx(std::shared_ptr<TriangleNavMesh> other) const
{
	std::vector<int> sharedVertexIdx;
	sharedVertexIdx.reserve(2);
	std::ranges::set_intersection(m_vertexIdxSet, other->m_vertexIdxSet, std::back_inserter(sharedVertexIdx));
	return sharedVertexIdx;
}

const std::vector<std::shared_ptr<NavMesh::TriangleNavMesh>> NavMesh::TriangleNavMesh::GetRelationTriangleMeshes() const
{
	std::vector<std::shared_ptr<NavMesh::TriangleNavMesh>> relationTriangleMesh;
	relationTriangleMesh.reserve(m_relationTriangleMesh.size());
	for (auto& relationMesh : m_relationTriangleMesh)
		relationTriangleMesh.push_back(relationMesh.first);
	return relationTriangleMesh;
}

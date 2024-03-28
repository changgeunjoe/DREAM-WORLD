#include "stdafx.h"
#include "NavMeshQuadTree.h"
#include "../TriangleMesh/TriangleNavMesh.h"

NavMesh::Node::NodeBase::NodeBase(const float& centerX, const float& centerY, const float& halfSize)
	: m_center(std::make_pair(centerX, centerY)), m_halfSize(halfSize)
{
}

const NavMesh::Node::CHILD_IDX NavMesh::Node::NodeBase::GetQuadrant(const float& x, const float& y) const
{

	using namespace NavMesh::Node;
	if (x - m_center.first < FLT_EPSILON) {
		if (y - m_center.second < FLT_EPSILON) {//3사분면
			return CHILD_IDX::LEFT_BOTTOM;
		}
		else {//2사분면
			return CHILD_IDX::LEFT_TOP;
		}
	}
	if (y <= m_center.second) {//4사분면
		return CHILD_IDX::RIGHT_BOTTOM;
	}
	else {//1사분면
		return CHILD_IDX::RIGHT_TOP;
	}
}

NavMesh::Node::InternalNode::InternalNode(const float& centerX, const float& centerY, const float& halfSize, const float& minSize)
	:NodeBase(centerX, centerY, halfSize)
{
	if (m_halfSize / 2.0f > minSize) {//만들 자식 노드가 최소 섹션 크기보다 크다면 Internal생성, 아니라면 Leaf생성.
		m_childNodes[0] = std::make_shared<InternalNode>(centerX + halfSize / 2.0f, centerY + halfSize / 2.0f, halfSize / 2.0f, minSize);
		m_childNodes[1] = std::make_shared<InternalNode>(centerX + halfSize / 2.0f, centerY + halfSize / 2.0f, halfSize / 2.0f, minSize);
		m_childNodes[2] = std::make_shared<InternalNode>(centerX + halfSize / 2.0f, centerY + halfSize / 2.0f, halfSize / 2.0f, minSize);
		m_childNodes[3] = std::make_shared<InternalNode>(centerX + halfSize / 2.0f, centerY + halfSize / 2.0f, halfSize / 2.0f, minSize);
	}
	else {
		m_childNodes[0] = std::make_shared<LeafNode>(centerX + halfSize / 2.0f, centerY + halfSize / 2.0f, halfSize / 2.0f);
		m_childNodes[1] = std::make_shared<LeafNode>(centerX + halfSize / 2.0f, centerY + halfSize / 2.0f, halfSize / 2.0f);
		m_childNodes[2] = std::make_shared<LeafNode>(centerX + halfSize / 2.0f, centerY + halfSize / 2.0f, halfSize / 2.0f);
		m_childNodes[3] = std::make_shared<LeafNode>(centerX + halfSize / 2.0f, centerY + halfSize / 2.0f, halfSize / 2.0f);
	}
}

void NavMesh::Node::InternalNode::InsertTriangleMesh(const XMFLOAT3& vertex, std::shared_ptr<TriangleNavMesh>& triangleNavMesh)
{
	NavMesh::Node::CHILD_IDX idx = GetQuadrant(vertex.x, vertex.z);
	m_childNodes[static_cast<int>(idx)]->InsertTriangleMesh(vertex, triangleNavMesh);
}

void NavMesh::Node::InternalNode::InsertTriangleMesh(std::shared_ptr<TriangleNavMesh>& triangleNavMesh)
{
	auto vertexDatas = triangleNavMesh->GetVertexData();
	for (auto& vertexData : vertexDatas) {
		NavMesh::Node::CHILD_IDX idx = GetQuadrant(vertexData.x, vertexData.z);
		m_childNodes[static_cast<int>(idx)]->InsertTriangleMesh(vertexData, triangleNavMesh);
	}
}

std::shared_ptr<NavMesh::TriangleNavMesh> NavMesh::Node::InternalNode::GetOnPositionTriangleMesh(const XMFLOAT3& position) const
{
	NavMesh::Node::CHILD_IDX idx = GetQuadrant(position.x, position.z);
	return m_childNodes[static_cast<int>(idx)]->GetOnPositionTriangleMesh(position);
}

NavMesh::Node::LeafNode::LeafNode(const float& centerX, const float& centerY, const float& halfSize)
	:NodeBase(centerX, centerY, halfSize)
{
}

void NavMesh::Node::LeafNode::InsertTriangleMesh(const XMFLOAT3& vertex, std::shared_ptr<TriangleNavMesh>& triangleNavMesh)
{
	m_navMeshSet.insert(triangleNavMesh);
}

std::shared_ptr<NavMesh::TriangleNavMesh> NavMesh::Node::LeafNode::GetOnPositionTriangleMesh(const XMFLOAT3& position) const
{
	std::vector<std::shared_ptr<NavMesh::TriangleNavMesh>> relationMesh;
	relationMesh.reserve(m_navMeshSet.size() * 3);//각 메시당 연결된 삼각형은 최대 3개이니, 총 메시 * 3개를 미리 할당
	float minAreaDiff = FLT_MAX;
	std::shared_ptr<NavMesh::TriangleNavMesh> minAreaMesh = nullptr;
	for (auto& navMesh : m_navMeshSet) {
		auto onNavMeshResult = navMesh->IsOnTriangleMesh(position);//isSuccess, areaDiffSize
		if (onNavMeshResult.first) {//성공했으면 바로 반환
			return navMesh;
		}
		//가장 적은 차이를 내는 메시 저장
		if (minAreaDiff > onNavMeshResult.second)
			minAreaMesh = navMesh;

		//실패했다면, 연결된 메시를 저장
		auto currentRelationMesh = navMesh->GetRelationTriangleMeshes();
		std::copy_if(currentRelationMesh.begin(), currentRelationMesh.end(), std::back_inserter(relationMesh), [&](const std::shared_ptr<NavMesh::TriangleNavMesh>& mesh) {
			return !m_navMeshSet.count(mesh);//현재 쿼드 트리 섹션이 가진 메시를 제외
			});//연결된 메시들을 저장
	}
	//연결된 메시를 탐색.
	for (auto& navMesh : relationMesh) {
		auto onNavMeshResult = navMesh->IsOnTriangleMesh(position);//isSuccess, areaDiffSize
		if (onNavMeshResult.first) {//성공했으면 바로 반환
			return navMesh;
		}
		if (minAreaDiff > onNavMeshResult.second)
			minAreaMesh = navMesh;
	}

	return minAreaMesh;
}

std::shared_ptr<NavMesh::TriangleNavMesh> NavMesh::QuadTree::GetOnPositionNavMesh(const XMFLOAT3& position)
{
	return m_root->GetOnPositionTriangleMesh(position);
}

void NavMesh::QuadTree::InsertTriangleNavMesh(std::shared_ptr<TriangleNavMesh>& navMesh)
{
	m_root->InsertTriangleMesh(navMesh);
}

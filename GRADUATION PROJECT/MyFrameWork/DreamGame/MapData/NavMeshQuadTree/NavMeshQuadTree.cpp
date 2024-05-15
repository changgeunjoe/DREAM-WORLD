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
		if (y - m_center.second < FLT_EPSILON) {//3��и�
			return CHILD_IDX::LEFT_BOTTOM;
		}
		else {//2��и�
			return CHILD_IDX::LEFT_TOP;
		}
	}
	if (y <= m_center.second) {//4��и�
		return CHILD_IDX::RIGHT_BOTTOM;
	}
	else {//1��и�
		return CHILD_IDX::RIGHT_TOP;
	}
}

NavMesh::Node::InternalNode::InternalNode(const float& centerX, const float& centerY, const float& halfSize, const float& minSize)
	:NodeBase(centerX, centerY, halfSize)
{
	if (m_halfSize / 2.0f > minSize) {//���� �ڽ� ��尡 �ּ� ���� ũ�⺸�� ũ�ٸ� Internal����, �ƴ϶�� Leaf����.
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
	relationMesh.reserve(m_navMeshSet.size() * 3);//�� �޽ô� ����� �ﰢ���� �ִ� 3���̴�, �� �޽� * 3���� �̸� �Ҵ�
	float minAreaDiff = FLT_MAX;
	std::shared_ptr<NavMesh::TriangleNavMesh> minAreaMesh = nullptr;
	for (auto& navMesh : m_navMeshSet) {
		auto onNavMeshResult = navMesh->IsOnTriangleMesh(position);//isSuccess, areaDiffSize
		if (onNavMeshResult.first) {//���������� �ٷ� ��ȯ
			return navMesh;
		}
		//���� ���� ���̸� ���� �޽� ����
		if (minAreaDiff > onNavMeshResult.second)
			minAreaMesh = navMesh;

		//�����ߴٸ�, ����� �޽ø� ����
		auto currentRelationMesh = navMesh->GetRelationTriangleMeshes();
		std::copy_if(currentRelationMesh.begin(), currentRelationMesh.end(), std::back_inserter(relationMesh), [&](const std::shared_ptr<NavMesh::TriangleNavMesh>& mesh) {
			return !m_navMeshSet.count(mesh);//���� ���� Ʈ�� ������ ���� �޽ø� ����
			});//����� �޽õ��� ����
	}
	//����� �޽ø� Ž��.
	for (auto& navMesh : relationMesh) {
		auto onNavMeshResult = navMesh->IsOnTriangleMesh(position);//isSuccess, areaDiffSize
		if (onNavMeshResult.first) {//���������� �ٷ� ��ȯ
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

#include "stdafx.h"
#include "AstarNode.h"
#include "../TriangleMesh/TriangleNavMesh.h"

NavMesh::AstarNode::AstarNode(std::shared_ptr<TriangleNavMesh> parentMesh, const float& parentDistance, const float& destinationDistance)
	: m_parentMesh(parentMesh), m_parentDistance(parentDistance), m_destinationDistance(destinationDistance)
{
}

void NavMesh::AstarNode::RefreshData(std::shared_ptr<TriangleNavMesh> parentMesh, const float& parentDistance)
{
	m_parentMesh = parentMesh;
	m_parentDistance = parentDistance;
}

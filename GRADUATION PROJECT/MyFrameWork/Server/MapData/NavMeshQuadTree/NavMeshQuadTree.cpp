#include "stdafx.h"
//#include "NavMeshQuadTree.h"
//
//NavMeshQuadTree::NavMeshQuadTree() {
//
//}
//
//NavMeshQuadTree::~NavMeshQuadTree() {
//	// ���� Ʈ�� �޸� ����
//	delete root;
//}
//
//void NavMeshQuadTree::Insert(TrinangleMesh& navMesh) {
//	Insert(root, navMesh);
//}
//
//void NavMeshQuadTree::Insert(QuadTreeNode* node, TrinangleMesh& navMesh) {
//
//	int quadrantIdx = node->GetQuadrant(navMesh.GetVertex1().x, navMesh.GetVertex1().z);
//	if (node->GetChildTree(quadrantIdx) != nullptr) {
//		Insert(node->GetChildTree(quadrantIdx), navMesh);
//	}
//	else {
//		node->RegistNode(navMesh);
//	}
//	quadrantIdx = node->GetQuadrant(navMesh.GetVertex2().x, navMesh.GetVertex2().z);
//	if (node->GetChildTree(quadrantIdx) != nullptr) {
//		Insert(node->GetChildTree(quadrantIdx), navMesh);
//	}
//	else {
//		node->RegistNode(navMesh);
//	}
//	quadrantIdx = node->GetQuadrant(navMesh.GetVertex3().x, navMesh.GetVertex3().z);
//	if (node->GetChildTree(quadrantIdx) != nullptr) {
//		Insert(node->GetChildTree(quadrantIdx), navMesh);
//	}
//	else {
//		node->RegistNode(navMesh);
//	}
//	quadrantIdx = node->GetQuadrant(navMesh.GetCenter().x, navMesh.GetCenter().z);
//	if (node->GetChildTree(quadrantIdx) != nullptr) {
//		Insert(node->GetChildTree(quadrantIdx), navMesh);
//	}
//	else {
//		node->RegistNode(navMesh);
//	}
//	//�� �ﰢ�޽��� �ڱ� �ڽ��� ������ ���� Ʈ���� ����Ͽ� �����Ѵ�.
//	// �׺� �޽ø� ���� Ʈ���� �����ϴ� ���� ����
//	// �� �κ��� ���� Ʈ���� ���� ��Ģ�� ���� �׺� �޽ø� ������ ��忡 �߰��ϴ� �۾��� �����ؾ� �մϴ�.
//}
//
//std::set<int> NavMeshQuadTree::GetNearbyNavMeshes(double x, double y) {
//	std::set<int> result;
//	GetNearbyNavMeshes(root, x, y, result);
//	return result;
//}
//
//void NavMeshQuadTree::GetNearbyNavMeshes(QuadTreeNode* node, double x, double y, std::set<int>& result) {
//	int qudrant = node->GetQuadrant(x, y);
//	if (node->GetChildTree(qudrant) != nullptr) {
//		GetNearbyNavMeshes(node->GetChildTree(qudrant), x, y, result);
//	}
//	else {
//		result = node->GetNavMeshIdx();
//		return;
//	}
//	// (x, y) ��ǥ ��ó�� �׺� �޽ø� ã�� ���� ����
//	// �� �κ��� ���� Ʈ���� Ž�� �˰����� Ȱ���Ͽ� ��ó�� �׺� �޽ø� ã�� result ���Ϳ� �߰��ϴ� �۾��� �����ؾ� �մϴ�.
//}
//
////int main() {
////    // ���� Ʈ�� �ʱ�ȭ
////    QuadTree quadTree(0.0, 0.0, 100.0); // ���÷� 2D ������ (-100, -100) ~ (100, 100)�� �簢������ ǥ��
////
////    // �׺� �޽� �߰�
////    NavMesh navMesh1 = { 1, 50.0, 50.0 };
////    NavMesh navMesh2 = { 2, -30.0, 70.0 };
////    NavMesh navMesh3 = { 3, 80.0, -20.0 };
////    quadTree.Insert(navMesh1);
////    quadTree.Insert(navMesh2);
////    quadTree.Insert(navMesh3);
////
////    // (x, y) ��ǥ�� �Է��Ͽ� ��ó�� �׺� �޽� ã��
////    double x = 10.0;
////    double y = 30.0;
////    std::vector<NavMesh> nearbyNavMeshes = quadTree.GetNearbyNavMeshes(x, y);
////
////    // ��� ���
////    std::cout << "Nearby NavMeshes at (" << x << ", " << y << "):" << std::endl;
////    for (const auto& navMesh : nearbyNavMeshes) {
////        std::cout << "NavMesh ID: " << navMesh.id << " (" << navMesh.x << ", " << navMesh.y << ")" << std::endl;
////    }
////
////    return 0;
////}
//
//QuadTreeNode::QuadTreeNode(float centerX, float centerY, float halfSize, float minSize) :m_centerX(centerX), m_centerY(centerY), m_halfSize(halfSize), m_minSize(minSize)
//{
//	if (m_halfSize > minSize) {
//		m_children[0] = new QuadTreeNode(m_centerX + halfSize / 2.0f, m_centerY + halfSize / 2.0f, halfSize / 2.0f, minSize);
//		m_children[1] = new QuadTreeNode(m_centerX - halfSize / 2.0f, m_centerY + halfSize / 2.0f, halfSize / 2.0f, minSize);
//		m_children[2] = new QuadTreeNode(m_centerX - halfSize / 2.0f, m_centerY - halfSize / 2.0f, halfSize / 2.0f, minSize);
//		m_children[3] = new QuadTreeNode(m_centerX + halfSize / 2.0f, m_centerY - halfSize / 2.0f, halfSize / 2.0f, minSize);
//		return;
//	}
//	for (int i = 0; i < 4; ++i) {
//		m_children[i] = nullptr;
//	}
//}
//
//QuadTreeNode::~QuadTreeNode()
//{
//	for (int i = 0; i < 4; ++i)
//		if (m_children[i] != nullptr)
//			delete m_children[i];
//}
//
//QuadTreeNode* QuadTreeNode::GetChildTree(int idx)
//{
//	return m_children[idx];
//}
//
//int QuadTreeNode::GetQuadrant(float x, float y)
//{
//	if (x <= m_centerX) {
//		if (y <= m_centerY) {//3��и�
//			return 2;
//		}
//		else {//2��и�
//			return 1;
//		}
//	}
//	else {
//		if (y <= m_centerY) {//4��и�
//			return 3;
//		}
//		else {//1��и�
//			return 0;
//		}
//	}
//}
//
//std::set<int> QuadTreeNode::GetNavMeshIdx()
//{
//	return m_navMeshIdxSet;
//}

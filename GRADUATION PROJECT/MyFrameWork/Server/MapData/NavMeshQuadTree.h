#pragma once
#include "../PCH/stdafx.h"

// 쿼드 트리 노드 구조체
class QuadTreeNode {
private:
	float m_centerX, m_centerY; // 노드가 나타내는 사각형 영역의 중심 좌표
	float m_halfSize; // 사각형 영역의 절반 크기
	QuadTreeNode* m_children[4]; // 하위 노드들 사분면으로 계산하자
	std::set<int> m_navMeshIdxSet; // 노드에 속한 네비 메시들
	float m_minSize = 10.0f;
public:
	QuadTreeNode(float centerX, float centerY, float halfSize, float minSize);
	~QuadTreeNode();
public:
	void RegistNode(TrinangleMesh& navMesh)
	{
		m_navMeshIdxSet.insert(navMesh.GetIdx());
	}
	QuadTreeNode* GetChildTree(int idx);
	int GetQuadrant(float x, float y);
	std::set<int> GetNavMeshIdx();
};

// 쿼드 트리 클래스
class NavMeshQuadTree {
public:
	//TrinangleMesh;
	NavMeshQuadTree();
	~NavMeshQuadTree();

	void Insert(TrinangleMesh& navMesh);
	std::set<int> GetNearbyNavMeshes(double x, double y);
public:
	void CreateQuadTreeNode(float minSize)
	{
		root = new QuadTreeNode(0.0f, 0.0f, 400.0f, minSize);
	}	
private:
	QuadTreeNode* root;
	void Insert(QuadTreeNode* node, TrinangleMesh& navMesh);
	void GetNearbyNavMeshes(QuadTreeNode* node, double x, double y, std::set<int>& result);
};

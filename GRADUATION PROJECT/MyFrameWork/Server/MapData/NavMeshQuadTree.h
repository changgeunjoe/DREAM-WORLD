#pragma once
#include "../PCH/stdafx.h"

// ���� Ʈ�� ��� ����ü
class QuadTreeNode {
private:
	float m_centerX, m_centerY; // ��尡 ��Ÿ���� �簢�� ������ �߽� ��ǥ
	float m_halfSize; // �簢�� ������ ���� ũ��
	QuadTreeNode* m_children[4]; // ���� ���� ��и����� �������
	std::set<int> m_navMeshIdxSet; // ��忡 ���� �׺� �޽õ�
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

// ���� Ʈ�� Ŭ����
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

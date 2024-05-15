#pragma once
#include "../PCH/stdafx.h"

// ���� Ʈ�� ��� ����ü
namespace NavMesh
{
	class TriangleNavMesh;
	namespace Node
	{
		enum class CHILD_IDX : int
		{
			RIGHT_TOP = 0,
			LEFT_TOP,
			LEFT_BOTTOM,
			RIGHT_BOTTOM
		};
		class NodeBase : std::enable_shared_from_this<NodeBase>
		{
			//friend NavMeshQuadTree;
		public:
			NodeBase(const float& centerX, const float& centerY, const float& halfSize);
			virtual void InsertTriangleMesh(const XMFLOAT3& vertex, std::shared_ptr<TriangleNavMesh>& triangleNavMesh) = 0;
			virtual std::shared_ptr<TriangleNavMesh> GetOnPositionTriangleMesh(const XMFLOAT3& position) const = 0;
		protected:
			const CHILD_IDX GetQuadrant(const float& x, const float& y) const;

			std::pair<float, float> m_center;// ��尡 ��Ÿ���� �簢�� ������ �߽� ��ǥ
			float m_halfSize; // �簢�� ������ ���� ũ��

			// ���� ���� ��и����� �������
		};

		class InternalNode : public NodeBase
		{
		public:
			InternalNode(const float& centerX, const float& centerY, const float& halfSize, const float& minSize);
			virtual void InsertTriangleMesh(const XMFLOAT3& vertex, std::shared_ptr<TriangleNavMesh>& triangleNavMesh) override;
			void InsertTriangleMesh(std::shared_ptr<TriangleNavMesh>& triangleNavMesh);
			virtual std::shared_ptr<TriangleNavMesh> GetOnPositionTriangleMesh(const XMFLOAT3& position) const override;
		private:
			std::array<std::shared_ptr<NodeBase>, 4> m_childNodes;
		};

		class LeafNode : public NodeBase
		{
		public:
			LeafNode(const float& centerX, const float& centerY, const float& halfSize);
			virtual void InsertTriangleMesh(const XMFLOAT3& vertex, std::shared_ptr<TriangleNavMesh>& triangleNavMesh) override;
			virtual std::shared_ptr<TriangleNavMesh> GetOnPositionTriangleMesh(const XMFLOAT3& position) const override;
		private:
			std::unordered_set<std::shared_ptr<TriangleNavMesh>> m_navMeshSet; // ��忡 ���� �׺� �޽õ�
		};
	}

	// ���� Ʈ�� Ŭ����
	class QuadTree
	{
	public:
		//TrinangleMesh;
		std::shared_ptr<TriangleNavMesh> GetOnPositionNavMesh(const XMFLOAT3& position);
		void CreateQuadTreeNode(float minSize = 10.0f)
		{
			m_root = std::make_shared<Node::InternalNode>(0.0f, 0.0f, 400.0f, minSize);
		}
		void InsertTriangleNavMesh(std::shared_ptr<TriangleNavMesh>& navMesh);
	private:
		std::shared_ptr<Node::InternalNode> m_root;
	};
}

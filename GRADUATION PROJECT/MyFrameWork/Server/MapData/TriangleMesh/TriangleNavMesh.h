#pragma once
#include "../PCH/stdafx.h"

namespace NavMesh {
	class TriangleNavMesh : public std::enable_shared_from_this<TriangleNavMesh>
	{
	public:
		TriangleNavMesh(const int& triangleId, const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3, const int& idx1, const int& idx2, const int& idx3);
		std::pair<bool, float> IsOnTriangleMesh(const DirectX::XMFLOAT3& position) const;

		//�ٸ� �ﰢ������ �������� 2�����
		//const bool IsShareLine(const TriangleNavMesh& other) const;

		const float GetDistance(const XMFLOAT3& position) const;
		const float GetDistance(const TriangleNavMesh& other) const;
		const float GetDistance(const float& x, const  float& y, const float& z) const;
		std::optional<float> GetRelationMeshDistance(std::shared_ptr<TriangleNavMesh> relationMesh) const;

		const XMFLOAT3 GetCenter() const { return m_center; }
		const float GetDoubleAreaSize() const { return m_duobleAreaSize; }

		void InsertRelationTriangleMesh(std::shared_ptr<TriangleNavMesh>& otherMesh);
		const std::array<XMFLOAT3, 3>& GetVertexData() const
		{
			return m_vertexs;
		}

		std::vector<int> GetRelationVertexIdx(std::shared_ptr<TriangleNavMesh> other) const;
		const std::vector<std::shared_ptr<TriangleNavMesh>> GetRelationTriangleMeshes() const;

		void ResetRelationData()
		{
			m_relationTriangleMesh.clear();
		}

		const int& GetId() const
		{
			return m_id;
		}

	private:
		int m_id;
		//���� ����
		std::array<XMFLOAT3, 3> m_vertexs;
		//���� �ִ� ������ �ε���
		std::unordered_set<int> m_vertexIdxSet;

		//���� �߽� ��ǥ
		XMFLOAT3 m_center;
		//���� -> �ﰢ�� ���� �� ���� �ִ��� �Ǵ��Ϸ���, �� ������ ���� �� �ﰢ�� �������� ���͸� ���ϰ�, �� ���Ϳ� �ﰢ�� ������ ���� ���͸� ���� / 2���� ��, �ﰢ���� ���̿� ����.
		float m_duobleAreaSize;//����ϱ� ���ϰ�, �ﰢ�� ������ 2�踦 ����

	public:
		//ptr, distance
		std::unordered_map<std::shared_ptr<TriangleNavMesh>, float> m_relationTriangleMesh;
	};
}

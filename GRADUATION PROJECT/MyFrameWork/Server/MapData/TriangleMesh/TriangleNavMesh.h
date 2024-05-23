#pragma once
#include "../PCH/stdafx.h"

namespace NavMesh {
	class TriangleNavMesh : public std::enable_shared_from_this<TriangleNavMesh>
	{
	public:
		TriangleNavMesh(const int& triangleId, const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3, const int& idx1, const int& idx2, const int& idx3);
		std::pair<bool, float> IsOnTriangleMesh(const DirectX::XMFLOAT3& position) const;

		//다른 삼각형과의 공유점이 2개라면
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
		//정점 정보
		std::array<XMFLOAT3, 3> m_vertexs;
		//갖고 있는 정점의 인덱스
		std::unordered_set<int> m_vertexIdxSet;

		//무게 중심 좌표
		XMFLOAT3 m_center;
		//넓이 -> 삼각형 위에 한 점이 있는지 판단하려면, 한 점으로 부터 각 삼각형 점까지의 벡터를 구하고, 그 벡터와 삼각형 정점이 갖는 벡터를 외적 / 2했을 때, 삼각형의 넓이와 같음.
		float m_duobleAreaSize;//계산하기 편하게, 삼각형 널이의 2배를 저장

	public:
		//ptr, distance
		std::unordered_map<std::shared_ptr<TriangleNavMesh>, float> m_relationTriangleMesh;
	};
}

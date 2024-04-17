#pragma once
#include "../PCH/stdafx.h"

namespace NavMesh
{
	class TriangleNavMesh;
	class AstarNode
	{
	public:
		//std::shared_ptr<TriangleNavMesh> ownerMesh
		AstarNode(std::shared_ptr<TriangleNavMesh> parentMesh, const float& parentDistance, const float& destinationDistance);

		void RefreshData(std::shared_ptr<TriangleNavMesh> parentMesh, const float& parentDistance);

		std::shared_ptr<TriangleNavMesh> GetParentMesh() const
		{
			return m_parentMesh;
		}

		const float& GetParentDistance() const
		{
			return m_parentDistance;
		}

		constexpr bool operator<(const AstarNode& other) const
		{
			return (m_parentDistance + m_destinationDistane) < (other.m_parentDistance + other.m_destinationDistane);
		}
		const bool IsStart(std::shared_ptr<TriangleNavMesh> ownerMesh) const
		{
			return ownerMesh == m_parentMesh;
		}
	private:
		////현재 에이스타의 주인 메쉬
		//std::shared_ptr<TriangleNavMesh> m_ownerMesh;
		//현재 노드로 올 수 있게 이어진 메쉬
		std::shared_ptr<TriangleNavMesh> m_parentMesh;

		float m_parentDistance;//부모 메시로부터 거리 값
		float m_destinationDistane;//목적지로 가는 거리 값
	};
}

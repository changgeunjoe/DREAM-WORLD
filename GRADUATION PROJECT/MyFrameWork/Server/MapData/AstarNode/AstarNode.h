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
		////���� ���̽�Ÿ�� ���� �޽�
		//std::shared_ptr<TriangleNavMesh> m_ownerMesh;
		//���� ���� �� �� �ְ� �̾��� �޽�
		std::shared_ptr<TriangleNavMesh> m_parentMesh;

		float m_parentDistance;//�θ� �޽÷κ��� �Ÿ� ��
		float m_destinationDistane;//�������� ���� �Ÿ� ��
	};
}

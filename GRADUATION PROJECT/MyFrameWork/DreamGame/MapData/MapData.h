#pragma once
#include"../stdafx.h"
#include "NavMeshQuadTree/NavMeshQuadTree.h"
using namespace DirectX;

//struct MonsterInitData {
//	XMFLOAT3 position;
//	XMFLOAT3 eulerRotate;
//	MonsterInitData(XMFLOAT3 pos, XMFLOAT3 rot)
//	{
//		position = pos;  eulerRotate = rot;
//	}
//};

class MapCollision;
class MapData
{
public:
	MapData() = delete;
	MapData(const std::string& mapCollisionFile);

	const std::vector<std::shared_ptr<MapCollision>>& GetCollisionData() const;
protected:
	//collision
	std::vector<std::shared_ptr<MapCollision>> m_collisionDatas;
};

namespace NavMesh {
	class TriangleNavMesh;
}
class NavMapData : public MapData
{
public:
	NavMapData() = delete;
	NavMapData(const std::string& mapCollisionFile, const std::string& navMeshFIle);
	~NavMapData();

	const std::shared_ptr<NavMesh::TriangleNavMesh> GetBossStartPosition() const;

	std::shared_ptr<NavMesh::TriangleNavMesh> GetTriangleMesh(const int& idx) const
	{
		return m_triangleMesh[idx];
	}

protected:
	//Nav Data
	//read Data
	std::vector<XMFLOAT3> m_vertex;
	std::vector<int> m_index;
	//GameData
	std::vector<std::shared_ptr<NavMesh::TriangleNavMesh>> m_triangleMesh;
	std::vector<int> m_zeroVertexIdxs;

	NavMesh::QuadTree m_navMeshQuadTree;
	float tiangleMeshGridMinSize = 10.0f;
	std::shared_ptr<NavMesh::TriangleNavMesh> m_bossStartMesh;
};

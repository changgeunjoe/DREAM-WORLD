#pragma once
#include"../PCH/stdafx.h"
#include "MapCollide.h"
#include "NavMeshQuadTree.h"
using namespace DirectX;

struct MonsterInitData {
	XMFLOAT3 position;
	XMFLOAT3 eulerRotate;
	MonsterInitData(XMFLOAT3 pos, XMFLOAT3 rot)
	{
		position = pos;  eulerRotate = rot;
	}
};

class MapData
{
private:
	//fileName
	std::string m_navFileName;
	std::string m_collisionFileName;
	std::string m_monsterDataFileName;
	//Nav Data
	std::vector<XMFLOAT3> m_vertex;
	std::vector<int> m_index;
	std::vector<TrinangleMesh> m_triangleMesh;
	std::vector<int> m_zeroVertexIdxs;
	//collision
	std::vector<MapCollide> m_collisionDatas;
	//monster InitData
	std::vector<MonsterInitData> m_initMonsterDatas;
	bool m_initCoplete = false;
	NavMeshQuadTree m_navMeshQuadTree;
	float tiangleMeshGridMinSize = 10.0f;
	int m_bossStartIdx = -1;
public:
	MapData() {};
	MapData(std::string navFileName, std::string collisionFileName, std::string monsterFileName);
public:
	int GetBossStartIdx() { return m_bossStartIdx; }
	bool GetCompleteState() { return m_initCoplete; }
	void SetFileName(std::string navfileName, std::string collisionName) {
		m_navFileName = navfileName;
		m_collisionFileName = collisionName;
	}
	void GetReadMapData();
	void GetReadCollisionData();
	void GetReadMonsterData();
	std::list<int> AStarLoad(std::atomic_int& myTriangleIdx, float desX, float desZ);
	TrinangleMesh& const GetTriangleMesh(int idx) { return m_triangleMesh[idx]; }
	const std::vector<int> GetZeroIdxs() { return m_zeroVertexIdxs; }
	const int GetFirstIdxs() {
		if (m_zeroVertexIdxs.size() < 1)
			return -1;
		return m_zeroVertexIdxs[0];
	}
	std::vector<MapCollide>& GetCollideData() { return m_collisionDatas; }
	std::vector<MonsterInitData>& GetMonsterData() { return m_initMonsterDatas; }
	std::vector<XMFLOAT3>& GetVertexData() { return m_vertex; }
};

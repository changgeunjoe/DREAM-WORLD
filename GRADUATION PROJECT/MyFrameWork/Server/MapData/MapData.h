#pragma once
#include"../PCH/stdafx.h"
#include "MapCollide.h"
using namespace DirectX;

class MapData
{
private:
	std::string m_navFileName;
	std::string m_collisionFileName;
	std::vector<XMFLOAT3> m_vertex;
	std::vector<int> m_index;
	std::vector<TrinangleMesh> m_triangleMesh;
	std::vector<int> m_zeroVertexIdxs;
	std::vector<MapCollide> m_collisionDatas;
private:
public:
	MapData() {};
	MapData(std::string navFileName, std::string collisionFileName);
public:
	void SetFileName(std::string navfileName, std::string collisionName) {
		m_navFileName = navfileName;
		m_collisionFileName = collisionName;
	}
	void GetReadMapData();
	void GetReadCollisionData();
	std::list<int> AStarLoad(int myTriangleIdx, float desX, float desZ);
	TrinangleMesh& const GetTriangleMesh(int idx) { return m_triangleMesh[idx]; }
	const std::vector<int> GetZeroIdxs() { return m_zeroVertexIdxs; }
	const int GetFirstIdxs() {
		if (m_zeroVertexIdxs.size() < 1)
			return -1;
		return m_zeroVertexIdxs[0];
	}
	std::vector<MapCollide>& GetCollideData() { return m_collisionDatas; }
};

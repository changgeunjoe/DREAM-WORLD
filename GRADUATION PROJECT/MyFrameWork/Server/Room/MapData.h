#pragma once
#include"../PCH/stdafx.h"

using namespace DirectX;

class MapData
{
private:
	std::string m_fileName;
	std::vector<TrinangleMesh> m_triangleMesh;
	std::vector<int> m_zeroVertexIdxs;
private:
public:
	MapData() {};
	MapData(std::string fileName);
public:
	void SetFileName(std::string fileName) {
		m_fileName = fileName;
	}
	void GetReadMapData();
	std::list<int> AStarLoad(int myTriangleIdx, float desX, float desZ);
	TrinangleMesh& const GetTriangleMesh(int idx) { return m_triangleMesh[idx]; }
	const std::vector<int> GetZeroIdxs() { return m_zeroVertexIdxs; }
	const int GetFirstIdxs() {
		if (m_zeroVertexIdxs.size() < 1)
			return -1;
		return m_zeroVertexIdxs[0];
	}
};

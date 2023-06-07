#pragma once
#include"../PCH/stdafx.h"

using namespace DirectX;

class MapData
{
private:
	std::string m_fileName;
	std::vector<TrinangleMesh> m_triangleMesh;
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
	TrinangleMesh& const GetTriangleMesh(int idx) { return m_triangleMesh[idx]; };
};

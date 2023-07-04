#pragma once
#include "../../stdafx.h"
#include"MapCollide.h"
using namespace DirectX;

class MapData
{
private:
	std::string m_navFileName;
	std::string m_collisionFileName;
	std::vector<TrinangleMesh> m_triangleMesh;
	std::vector<MapCollide> m_collisionDatas;

private:
public:
	MapData() {};
	MapData(std::string navFileName, std::string collisionFileName);
public:
	void SetFileName(std::string navFileName) {
		m_navFileName = navFileName;
	}
	void GetReadMapData();
	void GetReadCollisionData();
	std::list<int> AStarLoad(int myTriangleIdx, float desX, float desZ);
	TrinangleMesh& const GetTriangleMesh(int idx) { return m_triangleMesh[idx]; };
	std::vector<TrinangleMesh>& GetTriangleMesh() { return m_triangleMesh; }
	std::vector<MapCollide>& GetCollideData() { return m_collisionDatas; }

};



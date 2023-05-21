#pragma once
#include"../PCH/stdafx.h"

using namespace DirectX;

class TrinangleMesh
{
private:
	XMFLOAT3 m_vertex1;
	XMFLOAT3 m_vertex2;
	XMFLOAT3 m_vertex3;
	XMFLOAT3 m_center;

public:
	TrinangleMesh(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& v3) :m_vertex1(v1), m_vertex2(v2), m_vertex3(v3)
	{
		m_center = Vector3::ScalarProduct(Vector3::Add(m_vertex1, Vector3::Add(m_vertex2, m_vertex3)), 1.0f / 3.0f, false);
	}
	TrinangleMesh& operator=(const TrinangleMesh& rhs) {
		m_vertex1 = rhs.m_vertex1;
		m_vertex2 = rhs.m_vertex2;
		m_vertex3 = rhs.m_vertex3;
		m_center = rhs.m_center;
	}

};

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
};

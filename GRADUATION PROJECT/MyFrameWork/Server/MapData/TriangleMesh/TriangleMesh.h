//#pragma once
//class TriangleMesh
//{
//private:
//	int m_id = -1;
//private:
//	XMFLOAT3 m_vertex1;
//	XMFLOAT3 m_vertex2;
//	XMFLOAT3 m_vertex3;
//private:
//	XMFLOAT3 m_center;
//	float m_areaSize;
//private:
//	XMFLOAT3 vec12;
//	XMFLOAT3 vec13;
//	XMFLOAT3 vec23;
//private:
//	std::set<int> m_vertexIdxSet;
//public:
//	std::map<int, float> m_relationMesh;
//
//public:
//	TrinangleMesh(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& v3, int idx1, int idx2, int idx3, int myIdx) :m_vertex1(v1), m_vertex2(v2), m_vertex3(v3)
//	{
//		m_id = myIdx;
//		m_vertexIdxSet.insert(idx1);
//		m_vertexIdxSet.insert(idx2);
//		m_vertexIdxSet.insert(idx3);
//		m_center = Vector3::ScalarProduct(Vector3::Add(m_vertex1, Vector3::Add(m_vertex2, m_vertex3)), 1.0f / 3.0f, false);
//		vec12 = Vector3::Subtract(m_vertex2, m_vertex1);
//		vec13 = Vector3::Subtract(m_vertex3, m_vertex1);
//		vec23 = Vector3::Subtract(m_vertex3, m_vertex2);
//		m_areaSize = Vector3::Length(Vector3::CrossProduct(vec12, vec13, false)) / 2.0f;
//	}
//	TrinangleMesh& operator=(const TrinangleMesh& other) {
//		m_vertex1 = other.m_vertex1;
//		m_vertex2 = other.m_vertex2;
//		m_vertex3 = other.m_vertex3;
//		m_center = other.m_center;
//		vec12 = other.vec12;
//		vec13 = other.vec13;
//		vec23 = other.vec23;
//		m_areaSize = other.m_areaSize;
//	}
//	float GetDistance(TrinangleMesh& other)
//	{
//		return Vector3::Length(Vector3::Subtract(m_center, other.m_center));
//	}
//	float GetDistance(float x, float y, float z)
//	{
//		return Vector3::Length(Vector3::Subtract(m_center, XMFLOAT3(x, y, z)));
//	}
//	bool IsOnTriangleMesh(DirectX::XMFLOAT3& pos) {
//		XMFLOAT3 triVec1 = Vector3::Subtract(XMFLOAT3(pos.x, 0, pos.z), m_vertex1);
//		XMFLOAT3 triVec2 = Vector3::Subtract(XMFLOAT3(pos.x, 0, pos.z), m_vertex2);
//		XMFLOAT3 triVec3 = Vector3::Subtract(XMFLOAT3(pos.x, 0, pos.z), m_vertex3);
//		float res = 0.0f;
//		//1 - 2
//		res = Vector3::Length(Vector3::CrossProduct(triVec1, triVec2, false));
//		//3 - 1
//		res += Vector3::Length(Vector3::CrossProduct(triVec3, triVec1, false));
//		//2 - 3
//		res += Vector3::Length(Vector3::CrossProduct(triVec2, triVec3, false));
//		res /= 2.0f;
//		float retVal = res - m_areaSize;
//		//#ifdef _DEBUG
//		//		std::cout << "TriangleMesh::retval: " << retVal << std::endl;
//		//#endif
//		return abs(m_areaSize - res) < 0.1f;
//	}
//	bool IsOnTriangleMesh(float x, float y, float z)
//	{
//		XMFLOAT3 triVec1 = Vector3::Subtract(XMFLOAT3(x, 0, z), m_vertex1);
//		XMFLOAT3 triVec2 = Vector3::Subtract(XMFLOAT3(x, 0, z), m_vertex2);
//		XMFLOAT3 triVec3 = Vector3::Subtract(XMFLOAT3(x, 0, z), m_vertex3);
//
//		float res = 0.0f;
//		//1 - 2
//		res = Vector3::Length(Vector3::CrossProduct(triVec1, triVec2, false));
//		//3 - 1
//		res += Vector3::Length(Vector3::CrossProduct(triVec3, triVec1, false));
//		//2 - 3
//		res += Vector3::Length(Vector3::CrossProduct(triVec2, triVec3, false));
//		res /= 2.0f;
//		float retVal = res - m_areaSize;
//		//#ifdef _DEBUG
//		//		std::cout << "TriangleMesh::retval: " << retVal << std::endl;
//		//#endif
//		return abs(m_areaSize - res) < 0.1f;
//	}
//	XMFLOAT3 GetCenter() { return m_center; }
//	float GetAreaSize() { return m_areaSize; }
//	std::set<int>& GetVertexIdxs() { return m_vertexIdxSet; };
//	const XMFLOAT3 GetVertex1() { return m_vertex1; }
//	const XMFLOAT3 GetVertex2() { return m_vertex2; }
//	const XMFLOAT3 GetVertex3() { return m_vertex3; }
//	std::vector<int> IsShareLine(std::set<int>& otherVertexIdxs)//다른 삼각형과의 공유점이 2개라면
//	{
//		std::vector<int> res;
//		std::ranges::set_intersection(m_vertexIdxSet, otherVertexIdxs, std::back_inserter(res));
//		return res;
//	}
//	int GetIdx() { return m_id; }
//	float GetDistanceByPoint(XMFLOAT3& point)
//	{
//		return Vector3::Length(Vector3::Subtract(m_center, point));
//	}
//	float GetCircumscribedLength()
//	{
//		return Vector3::Length(Vector3::Subtract(m_vertex1, m_center));
//	}
//};
//

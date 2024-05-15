#include "stdafx.h"
#include "MapData.h"
#include "TriangleMesh/TriangleNavMesh.h"
#include "MapCollision/MapCollision.h"

MapData::MapData(const std::string& mapCollisionFile)
{
	std::ifstream collisionFile(mapCollisionFile);

	std::vector<XMFLOAT3> modelPosition;
	std::vector<XMFLOAT4> quaternion;

	std::vector<XMFLOAT3> modelScale;
	std::vector<XMFLOAT3> modelEulerRotate;

	std::vector<XMFLOAT3> colliderWorldPosition;
	std::vector<XMFLOAT3> colliderWorldBoundSize;
	std::vector<XMFLOAT3> colliderWorldExtentSize;
	std::vector<XMFLOAT3> colliderForwardVec;
	std::vector<XMFLOAT3> colliderRightVec;
	std::vector<float>	 forwardDot;
	std::vector<float>	 rightDot;

	std::string temp;
	std::vector<std::string> name;
	float number[3] = {};
	float qnumber[4] = {};
#pragma region READ_COLLISION
	while (!collisionFile.eof())
	{
		collisionFile >> temp;
		if (temp == "<position>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				collisionFile >> temp;
				number[i] = stof(temp);
			}
			modelPosition.emplace_back(number[0], number[1], number[2]);
		}
		else if (temp == "<quaternion>:")
		{
			for (int i = 0; i < 4; ++i)
			{
				collisionFile >> temp;
				qnumber[i] = stof(temp);
			}
			quaternion.emplace_back(qnumber[0], qnumber[1], qnumber[2], qnumber[3]);
		}
		else if (temp == "<rotation>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				collisionFile >> temp;
				number[i] = stof(temp);
			}
			modelEulerRotate.emplace_back(number[0], number[1], number[2]);
		}
		else if (temp == "<scale>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				collisionFile >> temp;
				number[i] = stof(temp);
			}
			modelScale.emplace_back(number[0], number[1], number[2]);
		}
		else if (temp == "<BoxCollider>")
		{
			for (int j = 0; j < 7; ++j)
			{
				collisionFile >> temp;
				if (temp == "<center>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						collisionFile >> temp;
						number[i] = stof(temp);
					}
					colliderWorldPosition.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<boundSize>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						collisionFile >> temp;
						number[i] = stof(temp);
					}
					colliderWorldBoundSize.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<extent>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						collisionFile >> temp;
						number[i] = stof(temp);
					}
					colliderWorldExtentSize.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<forward>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						collisionFile >> temp;
						number[i] = stof(temp);
					}
					colliderForwardVec.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<right>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						collisionFile >> temp;
						number[i] = stof(temp);
					}
					colliderRightVec.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<forwardDotRes>:")
				{
					collisionFile >> temp;
					number[0] = stof(temp);
					forwardDot.emplace_back(number[0]);
				}
				else if (temp == "<rightDotRes>:")
				{
					collisionFile >> temp;
					number[0] = stof(temp);
					rightDot.emplace_back(number[0]);
				}
			}
		}
		else name.emplace_back(temp);
	}
#pragma endregion

	for (int i = 0; i < rightDot.size(); ++i) {
		auto collision = std::make_shared<MapCollision>(colliderWorldPosition[i], colliderWorldExtentSize[i], quaternion[i], forwardDot[i], colliderForwardVec[i], rightDot[i], colliderRightVec[i]);
		m_collisionDatas.push_back(collision);
	}

	for (int i = 0; i < m_collisionDatas.size(); i++) {
		for (int j = 0; j < m_collisionDatas.size(); j++) {
			if (i != j) {
				bool intersectResult = m_collisionDatas[i]->GetCollision().Intersects(m_collisionDatas[j]->GetCollision());
				if (intersectResult) {//서로 겹쳡다면 연결된 충돌 객체
					m_collisionDatas[i]->AddRelationCollision(m_collisionDatas[j]);
				}
			}
		}
	}
	cout << mapCollisionFile << " - CollisionData Initialize Success\n";
}

const std::vector<std::shared_ptr<MapCollision>>& MapData::GetCollisionData() const
{
	return m_collisionDatas;
}

NavMapData::NavMapData(const std::string& mapCollisionFile, const std::string& navMeshFIle)
	:MapData(mapCollisionFile)
{
	//MapData생성자에서 collision데이터를 셋함.
	//네비 메쉬 데이터만 셋하면 되는 클래스 생성자
	using namespace std;

	std::string line;
	int vertexNum = 0;
	int vertexNomalNum = 0;
	int vertexTextureNum = 0;

	std::ifstream navFile{ navMeshFIle };
	string fileDataString;
	const string verticesStr = "<Vertices>:";
	const string indicesStr = "<Indices>:";
	const string relationStr = "<Relay>:";

	std::string readObject;
	while (!navFile.eof()) {
		navFile >> readObject;
		//정점 데이터
		if (readObject == verticesStr) {
			navFile >> readObject;

			int vertexCnt = std::stoi(readObject);
			m_vertex.reserve(vertexCnt);
			while (vertexCnt)
			{
				XMFLOAT3 v;
				navFile >> v.x >> v.y >> v.z;
				/*v.x *= 30.0f;
				v.z *= 30.0f;*/
				m_vertex.emplace_back(v.x, 0.0f, v.z);
				vertexCnt--;
			}
		}
		//정점에 대한 인덱스 정보
		else if (readObject == indicesStr) {
			navFile >> readObject;
			int indexCnt = std::stoi(readObject);
			m_triangleMesh.reserve(indexCnt / 3);// (인덱스 갯수 / 3) 이 삼각 네비 메쉬 갯수
			while (indexCnt) {
				int i;
				navFile >> i;
				m_index.emplace_back(i);
				indexCnt--;
			}
			int triangleCnt = 0;
			for (auto indexIter = m_index.begin(); indexIter != m_index.end(); indexIter += 3) {
				m_triangleMesh.emplace_back(
					std::make_shared<NavMesh::TriangleNavMesh>(triangleCnt, m_vertex[*indexIter], m_vertex[*(indexIter + 1)], m_vertex[*(indexIter + 2)], *indexIter, *(indexIter + 1), *(indexIter + 2))
				);
				++triangleCnt;
			}

			m_navMeshQuadTree.CreateQuadTreeNode(tiangleMeshGridMinSize);
			for (auto& tMesh : m_triangleMesh) {
				m_navMeshQuadTree.InsertTriangleNavMesh(tMesh);
			}

		}
		else if (readObject == relationStr) {
			navFile >> readObject;
			int relationCnt = std::stoi(readObject);
			for (int i = 0; i < relationCnt; i++) {
				navFile >> readObject;
				int index1, index2;
				navFile >> index1;
				navFile >> index2;
				if (index1 != index2) {
					m_triangleMesh[index1]->InsertRelationTriangleMesh(m_triangleMesh[index2]);
					m_triangleMesh[index2]->InsertRelationTriangleMesh(m_triangleMesh[index1]);
				}
			}
		}


	}
	XMFLOAT3 bossPos = XMFLOAT3(179.4f, 0.0f, -38.1f);
	int resultBossIdx = -1;
	auto bossStartMesh = m_navMeshQuadTree.GetOnPositionNavMesh(bossPos);

	m_bossStartMesh = bossStartMesh;
	cout << navMeshFIle << " - NavMeshData Initialize Success\n";
}

NavMapData::~NavMapData()
{
	for (auto& triangleMesh : m_triangleMesh)//삼각 메쉬간의 연결 관계 초기화하여, 순환참조로인한 메모리 문제 해결
		triangleMesh->ResetRelationData();
	m_bossStartMesh.reset();
}

const std::shared_ptr<NavMesh::TriangleNavMesh> NavMapData::GetBossStartPosition() const
{
	return m_bossStartMesh;
}

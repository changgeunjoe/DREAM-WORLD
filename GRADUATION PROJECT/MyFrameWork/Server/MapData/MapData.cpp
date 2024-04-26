#include "stdafx.h"
#include "MapData.h"
#include "TriangleMesh/TriangleNavMesh.h"
#include "MapCollision/MapCollision.h"
#include "AstarNode/AstarNode.h"

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
				if (intersectResult) {//���� �㫇�ٸ� ����� �浹 ��ü
					m_collisionDatas[i]->AddRelationCollision(m_collisionDatas[j]);
				}
			}
		}
	}
	spdlog::info("{} - CollisionData Initialize Success", mapCollisionFile);
}

const std::vector<std::shared_ptr<MapCollision>>& MapData::GetCollisionData() const
{
	return m_collisionDatas;
}

NavMapData::NavMapData(const std::string& mapCollisionFile, const std::string& navMeshFIle)
	:MapData(mapCollisionFile)
{
	//MapData�����ڿ��� collision�����͸� ����.
	//�׺� �޽� �����͸� ���ϸ� �Ǵ� Ŭ���� ������
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
		//���� ������
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
		//������ ���� �ε��� ����
		else if (readObject == indicesStr) {
			navFile >> readObject;
			int indexCnt = std::stoi(readObject);
			m_triangleMesh.reserve(indexCnt / 3);// (�ε��� ���� / 3) �� �ﰢ �׺� �޽� ����
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
	spdlog::info("{} - NavMeshData Initialize Success", navMeshFIle);
}

NavMapData::~NavMapData()
{
	for (auto& triangleMesh : m_triangleMesh)//�ﰢ �޽����� ���� ���� �ʱ�ȭ�Ͽ�, ��ȯ���������� �޸� ���� �ذ�
		triangleMesh->ResetRelationData();
	m_bossStartMesh.reset();
}

const std::shared_ptr<NavMesh::TriangleNavMesh> NavMapData::GetBossStartPosition() const
{
	return m_bossStartMesh;
}

std::list<XMFLOAT3> NavMapData::GetAstarNode(const XMFLOAT3& startPosition, const XMFLOAT3& destinationPosition)
{

	std::unordered_map<std::shared_ptr<NavMesh::TriangleNavMesh>, NavMesh::AstarNode> openList;
	std::unordered_map<std::shared_ptr<NavMesh::TriangleNavMesh>, NavMesh::AstarNode> closeList;
	std::shared_ptr<NavMesh::TriangleNavMesh> startMesh = m_navMeshQuadTree.GetOnPositionNavMesh(startPosition);
	std::shared_ptr<NavMesh::TriangleNavMesh> destinationMesh = m_navMeshQuadTree.GetOnPositionNavMesh(destinationPosition);

	if (startMesh == destinationMesh) {
		return std::list<XMFLOAT3>{};
	}

	closeList.try_emplace(startMesh, startMesh, 0, 0);

	auto currentFindMesh = startMesh;
	while (true) {
		auto relationMeshes = currentFindMesh->GetRelationTriangleMeshes();//���� �޽ÿ� ����� �޽� ������
		for (auto& relationMesh : relationMeshes) {
			if (closeList.count(relationMesh)) continue;//�̹� closeList�� �ִٸ� �ѱ�� -> �̹� Ȯ���� �޽�
			float destinationDistacne = destinationMesh->GetDistance(relationMesh->GetCenter()); //������������ �Ÿ�
			std::optional<float> parentDistance = currentFindMesh->GetRelationMeshDistance(relationMesh);//�θ� �������� ���� �Ÿ�
			if (!parentDistance.has_value()) {//���� �޽ð� �θ� �޽ÿ� ������ �ƴ�   (�̷����� �Ͼ�� �ʱ���)
				spdlog::critical("NavMapData::GetAstarNode() - non related Mesh");
				continue;
			}
			auto relationMeshIter = openList.find(relationMesh);
			if (openList.end() != relationMeshIter) {//openList�� �� �޽� ������ ����
				//�����Ѵٸ� Astar����� ���� ���Ͽ� �� ���� �� �ɷ� ��ü
				//���� openList�� �����ϴ� �� ����� ���̽�Ÿ
				NavMesh::AstarNode& existedAstarNode = relationMeshIter->second;
				if (existedAstarNode.GetParentDistance() > parentDistance.value()) {//�̹� �����ϴ� ���̽�Ÿ ��忡 ���ؼ� �θ�κ��� �Ÿ��� �� ª�ٸ� ��ä�ؾ� ��.
					existedAstarNode.RefreshData(currentFindMesh, parentDistance.value());
				}
			}
			else {//openList�� �������� ����.
				openList.try_emplace(relationMesh, currentFindMesh, parentDistance.value(), destinationDistacne);
			}
		}

		auto minIter = openList.begin();//���̽�Ÿ�� ���� ���� ������ Ž��
		for (auto iter = openList.begin(); iter != openList.end(); ++iter) {
			if (iter->second < minIter->second) {
				minIter = iter;
			}
		}
		//�ּ� ���� closeList�� ����
		closeList.emplace(minIter->first, minIter->second);
		if (minIter->first == destinationMesh) {//���࿡ ���� ������ ��ü�� ��������� ������ ����
			break;
		}
		//���� �޽ô� ��� closeList�� ���� �ּҰ��� �޽�
		currentFindMesh = minIter->first;
		//closeList�� ���������� openList������ ����
		openList.erase(minIter);
	}
	//front: ���� �޽� , end: ���� �޽�
	std::list<std::shared_ptr<NavMesh::TriangleNavMesh>> confirmedMesh;
	confirmedMesh.emplace_front(destinationMesh);
	auto currentMesh = destinationMesh;
	while (true) {
		auto currentAstarIter = closeList.find(currentMesh);
		if (closeList.end() != currentAstarIter) {
			//�θ� �޽ø� ����
			auto parentMesh = currentAstarIter->second.GetParentMesh();
			confirmedMesh.emplace_front(parentMesh);
			if (parentMesh == startMesh) break;//���� �޽ÿ� �θ� �޽ð� ���ٸ� ����
			currentMesh = parentMesh;
		}
		else {
			spdlog::critical("NavMapData::GetAstarNode() - can not confirnedMesh");
		}
	}
	return OptimizeAStar(startPosition, destinationPosition, confirmedMesh);
}

std::list<XMFLOAT3> NavMapData::OptimizeAStar(const XMFLOAT3& startPosition, const XMFLOAT3& destinationPosition, std::list<std::shared_ptr<NavMesh::TriangleNavMesh>>& confirmedAstarNode)
{
	static XMFLOAT3 UP = XMFLOAT3(0, 1, 0);
	std::vector<XMFLOAT3> leftPortal;
	std::vector<XMFLOAT3> rightPortal;
	CreatePortal(confirmedAstarNode, leftPortal, rightPortal);

	int leftPortalIdx = 0;
	int rightPortalIdx = 0;

	XMFLOAT3 restartPosition = startPosition;

	std::list<XMFLOAT3> optimizedPositions;
	optimizedPositions.emplace_back(startPosition);

	while (true) {
		//������ �� �ִ� �ε����� �ѱ�� ����
		if (leftPortalIdx + 1 >= static_cast<int>(leftPortal.size()) ||
			rightPortalIdx + 1 >= static_cast<int>(rightPortal.size())) break;

		//��Ż ���� ����
		XMFLOAT3 currentLeftPortal = leftPortal[leftPortalIdx];
		XMFLOAT3 nextLeftPortal = leftPortal[leftPortalIdx + 1];
		XMFLOAT3 currentRightPortal = rightPortal[rightPortalIdx];
		XMFLOAT3 nextRightPortal = rightPortal[rightPortalIdx + 1];

		//���� ��Ż ����
		XMFLOAT3 startToCurrentLeft = Vector3::Subtract(currentLeftPortal, restartPosition);
		XMFLOAT3 startToNextLeft = Vector3::Subtract(nextLeftPortal, restartPosition);
		XMFLOAT3 leftCrossProduct = Vector3::CrossProduct(startToNextLeft, startToCurrentLeft, true);
		//������ ���� ���� ��Ż�� ���� ������Ż���� �� �ܰ��� ��ġ��
		//����� �� �ܰ�
		float resultLeftPortalDot = Vector3::DotProduct(UP, leftCrossProduct);
		bool leftValid = resultLeftPortalDot < 0.0f;

		//���� ��Ż ����
		XMFLOAT3 startToCurrentRight = Vector3::Subtract(currentRightPortal, restartPosition);
		XMFLOAT3 startToNextRight = Vector3::Subtract(nextRightPortal, restartPosition);
		//���� ��Ż�� ���������� �ٲ㼭 ������ �� �ܰ����� �� �� �ְ�.
		XMFLOAT3 rightCrossProduct = Vector3::CrossProduct(startToCurrentRight, startToNextRight, true);
		//������ ���� ���� ������ �� �ܰ�
		float resultRightPortalDot = Vector3::DotProduct(UP, rightCrossProduct);
		bool rightValid = resultRightPortalDot < 0.0f;

		//���� ���� ������ �������� �����̶�� UP���Ϳ� dotProduct���� �� ���
		XMFLOAT3 leftCornerCross = Vector3::CrossProduct(startToNextRight, startToCurrentLeft, true);
		//����� �� ���� ���� ���� ���� ���� �������� �� ���� => ��ȸ��
		float leftCornerDot = Vector3::DotProduct(UP, leftCornerCross);
		bool isLeftCorner = leftCornerDot >= 0.0f;

		XMFLOAT3 rightCornerCross = Vector3::CrossProduct(startToCurrentRight, startToNextLeft, true);
		//��� �� �� ���� ���� ������ �������� �� ���� => ��ȸ��
		float rightCornerDot = Vector3::DotProduct(UP, rightCornerCross);
		bool isRightCorner = rightCornerDot >= 0.0f;

		if (!leftValid && !rightValid) {//�� �� ���ٰ� ���� ���� �� �� ������ ���
			int restartIdx = leftPortalIdx < rightPortalIdx ? leftPortalIdx : rightPortalIdx;//�� �� �� ���� �������� �ٽ� ����(���� �ﰢ������)
			XMFLOAT3 leftVertex = leftPortal[restartIdx];
			XMFLOAT3 rightVertex = rightPortal[restartIdx];

			XMFLOAT3 midVertex = Vector3::ScalarProduct(Vector3::Add(leftVertex, rightVertex), 0.5f, false);

			restartPosition = midVertex;
			optimizedPositions.emplace_back(restartPosition);

			//���� �������� ���� �ﰢ������ ���� ��ġ�� Ȯ�� ������, �� �� ���� �ﰢ������ �����ؾ� ��
			leftPortalIdx = restartIdx + 1;
			rightPortalIdx = restartIdx + 1;
			continue;
		}

		if (leftValid) {
			if (isRightCorner) {//������ ���� ���� ���� ������, �������� �������� �Ѿ�� ��� -> ��ȸ��
				//���⼭ �ѹ� üũ ����Ʈ? �������� ���� ��.
				restartPosition = currentRightPortal;//��ȸ���̴� ���� ���� ������ ������
				optimizedPositions.emplace_back(restartPosition);

				++rightPortalIdx;//���� rightIdx�� �������� �ٽ� �����Ұű� ������, ����
				leftPortalIdx = rightPortalIdx;//���� �ﰢ�� �������� �����ϰ� ����
				continue;
			}
			if (abs(resultLeftPortalDot) > FLT_EPSILON) {//������ �������� ���� ��쵵 ������-> �ٸ� ���� �� ����
				++leftPortalIdx;

				//���� ���� �����Ͱ� �ֽ�ȭ������ �ؿ� ���� ���� �Ǵܿ��� ��ȸ������ �Ǵ��� �� �־�� ��.
				currentLeftPortal = leftPortal[leftPortalIdx];
				startToCurrentLeft = Vector3::Subtract(currentLeftPortal, restartPosition);
				leftCornerCross = Vector3::CrossProduct(startToNextRight, startToCurrentLeft, true);
				leftCornerDot = Vector3::DotProduct(UP, leftCornerCross);
				isLeftCorner = leftCornerDot >= 0.0f;
			}
		}

		if (rightValid) {
			if (isLeftCorner) {
				//���� ���� �ڳʿ� ���� ������� �����ϰ�.
				restartPosition = currentLeftPortal;
				optimizedPositions.emplace_back(restartPosition);

				++leftPortalIdx;
				rightPortalIdx = leftPortalIdx;
				continue;
			}
			if (abs(resultRightPortalDot) > FLT_EPSILON)
				++rightPortalIdx;
		}
	}

	optimizedPositions.emplace_back(destinationPosition);
	return optimizedPositions;
}

void NavMapData::CreatePortal(std::list<std::shared_ptr<NavMesh::TriangleNavMesh>>& confirmedAstarNode, std::vector<XMFLOAT3>& leftPortal, std::vector<XMFLOAT3>& rightPortal)
{
	static XMFLOAT3 UP = XMFLOAT3(0, 1, 0);
	leftPortal.reserve(confirmedAstarNode.size());
	rightPortal.reserve(confirmedAstarNode.size());

	auto startIter = confirmedAstarNode.begin();
	auto endIter = confirmedAstarNode.end();

	std::shared_ptr<NavMesh::TriangleNavMesh> currentMesh = *startIter;

	while (true)
	{
		++startIter;
		if (startIter == endIter) return;
		std::shared_ptr<NavMesh::TriangleNavMesh> nextMesh = *startIter;

		XMFLOAT3 currentCenter = currentMesh->GetCenter();
		XMFLOAT3 nextCenter = nextMesh->GetCenter();
		//���� ��� -> ���� ��� ���� ���� ����
		XMFLOAT3 currentToNextVector = Vector3::Subtract(nextCenter, currentCenter);

		auto sharedVertexIdxes = currentMesh->GetRelationVertexIdx(nextMesh);

		for (auto& sharedVertexIdx : sharedVertexIdxes) {
			XMFLOAT3 sharedVertex = m_vertex[sharedVertexIdx];
			//���� ���� -> �����ϴ� ���� ����
			XMFLOAT3 centerToVertexVector = Vector3::Subtract(sharedVertex, currentCenter);
			/*
				���� ������ ���� X ���� �������� ���� �� => �޼� ��ǥ���� �������� �������̶�� 0,1,0���� // �����̶�� 0,-1,0 �� ����
				-> �׷� ���� 0, 1, 0 ���Ϳ� ���� ���� �� ���, ������ �Ǵ��Ͽ� ��(����), ��(���) �� �Ǵ� �� �� ����.
			*/
			XMFLOAT3 crossProductResult = Vector3::CrossProduct(currentToNextVector, centerToVertexVector, true);
			float dotProductResult = Vector3::DotProduct(UP, crossProductResult);
			if (dotProductResult > 0.0f) {//�����ʿ� ��ġ�� ����.
				rightPortal.push_back(sharedVertex);
			}
			else {
				leftPortal.push_back(sharedVertex);
			}
		}

		currentMesh = nextMesh;
	}

}

MonsterMapData::MonsterMapData(const std::string& mapCollisionFile, const std::string& initMonsterFile)
	:MapData(mapCollisionFile)
{
	std::ifstream objectFile(initMonsterFile);

	std::vector<XMFLOAT3> modelPosition;
	std::vector<XMFLOAT3> modelEulerRotate;
	std::string readData;
	std::vector<std::string> name;

	float qnumber[4] = {};
	while (!objectFile.eof())
	{
		objectFile >> readData;
		if (readData == "<position>:")
		{
			float number[3] = {};
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> readData;
				number[i] = stof(readData);
			}
			modelPosition.emplace_back(number[0], number[1], number[2]);
		}
		else if (readData == "<quaternion>:")
		{
			for (int i = 0; i < 4; ++i)
			{
				objectFile >> readData;
			}
		}
		else if (readData == "<rotation>:")
		{
			float number[3] = {};
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> readData;
				number[i] = stof(readData);
			}
			modelEulerRotate.emplace_back(number[0], number[1], number[2]);
		}
		else if (readData == "<scale>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> readData;
			}
		}
		else name.emplace_back(readData);
	}
	m_monsterInitData.reserve(15);
	for (int i = 0; i < 15; i++)
		m_monsterInitData.emplace_back(modelPosition[i], modelEulerRotate[i]);
}

const std::vector<MonsterInitData>& MonsterMapData::GetMonsterInitData() const
{
	return m_monsterInitData;
}

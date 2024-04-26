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
				if (intersectResult) {//서로 겹쳡다면 연결된 충돌 객체
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
	spdlog::info("{} - NavMeshData Initialize Success", navMeshFIle);
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
		auto relationMeshes = currentFindMesh->GetRelationTriangleMeshes();//현재 메시와 연결된 메시 데이터
		for (auto& relationMesh : relationMeshes) {
			if (closeList.count(relationMesh)) continue;//이미 closeList에 있다면 넘기기 -> 이미 확정된 메시
			float destinationDistacne = destinationMesh->GetDistance(relationMesh->GetCenter()); //목적지까지의 거리
			std::optional<float> parentDistance = currentFindMesh->GetRelationMeshDistance(relationMesh);//부모 목적지로 부터 거리
			if (!parentDistance.has_value()) {//현재 메시가 부모 메시와 연결이 아님   (이런일이 일어나진 않긴함)
				spdlog::critical("NavMapData::GetAstarNode() - non related Mesh");
				continue;
			}
			auto relationMeshIter = openList.find(relationMesh);
			if (openList.end() != relationMeshIter) {//openList에 이 메시 정보가 존재
				//존재한다면 Astar노드의 값을 비교하여 더 값이 싼 걸로 교체
				//현재 openList에 존재하는 이 노드의 에이스타
				NavMesh::AstarNode& existedAstarNode = relationMeshIter->second;
				if (existedAstarNode.GetParentDistance() > parentDistance.value()) {//이미 존재하는 에이스타 노드에 대해서 부모로부터 거리가 더 짧다면 교채해야 됨.
					existedAstarNode.RefreshData(currentFindMesh, parentDistance.value());
				}
			}
			else {//openList에 존재하지 않음.
				openList.try_emplace(relationMesh, currentFindMesh, parentDistance.value(), destinationDistacne);
			}
		}

		auto minIter = openList.begin();//에이스타중 제일 값이 낮은걸 탐색
		for (auto iter = openList.begin(); iter != openList.end(); ++iter) {
			if (iter->second < minIter->second) {
				minIter = iter;
			}
		}
		//최소 값을 closeList에 삽입
		closeList.emplace(minIter->first, minIter->second);
		if (minIter->first == destinationMesh) {//만약에 지금 삽입한 객체가 목적지라면 루프문 종료
			break;
		}
		//현재 메시는 방금 closeList에 들어온 최소값인 메시
		currentFindMesh = minIter->first;
		//closeList에 삽입했으니 openList에서는 삭제
		openList.erase(minIter);
	}
	//front: 시작 메시 , end: 도착 메시
	std::list<std::shared_ptr<NavMesh::TriangleNavMesh>> confirmedMesh;
	confirmedMesh.emplace_front(destinationMesh);
	auto currentMesh = destinationMesh;
	while (true) {
		auto currentAstarIter = closeList.find(currentMesh);
		if (closeList.end() != currentAstarIter) {
			//부모 메시를 삽입
			auto parentMesh = currentAstarIter->second.GetParentMesh();
			confirmedMesh.emplace_front(parentMesh);
			if (parentMesh == startMesh) break;//시작 메시와 부모 메시가 같다면 종료
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
		//조사할 수 있는 인덱스를 넘기면 종료
		if (leftPortalIdx + 1 >= static_cast<int>(leftPortal.size()) ||
			rightPortalIdx + 1 >= static_cast<int>(rightPortal.size())) break;

		//포탈 정보 세팅
		XMFLOAT3 currentLeftPortal = leftPortal[leftPortalIdx];
		XMFLOAT3 nextLeftPortal = leftPortal[leftPortalIdx + 1];
		XMFLOAT3 currentRightPortal = rightPortal[rightPortalIdx];
		XMFLOAT3 nextRightPortal = rightPortal[rightPortalIdx + 1];

		//왼쪽 포탈 정보
		XMFLOAT3 startToCurrentLeft = Vector3::Subtract(currentLeftPortal, restartPosition);
		XMFLOAT3 startToNextLeft = Vector3::Subtract(nextLeftPortal, restartPosition);
		XMFLOAT3 leftCrossProduct = Vector3::CrossProduct(startToNextLeft, startToCurrentLeft, true);
		//양수라면 현재 왼쪽 포탈이 다음 왼쪽포탈보다 더 외곽에 위치함
		//양수면 더 외곽
		float resultLeftPortalDot = Vector3::DotProduct(UP, leftCrossProduct);
		bool leftValid = resultLeftPortalDot < 0.0f;

		//우측 포탈 정보
		XMFLOAT3 startToCurrentRight = Vector3::Subtract(currentRightPortal, restartPosition);
		XMFLOAT3 startToNextRight = Vector3::Subtract(nextRightPortal, restartPosition);
		//우측 포탈은 외적순서를 바꿔서 양수라면 더 외곽임을 알 수 있게.
		XMFLOAT3 rightCrossProduct = Vector3::CrossProduct(startToCurrentRight, startToNextRight, true);
		//양수라면 다음 우측 정점이 더 외곽
		float resultRightPortalDot = Vector3::DotProduct(UP, rightCrossProduct);
		bool rightValid = resultRightPortalDot < 0.0f;

		//다음 우측 정점이 좌측보다 왼쪽이라면 UP벡터와 dotProduct했을 때 양수
		XMFLOAT3 leftCornerCross = Vector3::CrossProduct(startToNextRight, startToCurrentLeft, true);
		//양수일 때 좌측 정점 보다 다음 우측 정점보다 더 좌측 => 좌회전
		float leftCornerDot = Vector3::DotProduct(UP, leftCornerCross);
		bool isLeftCorner = leftCornerDot >= 0.0f;

		XMFLOAT3 rightCornerCross = Vector3::CrossProduct(startToCurrentRight, startToNextLeft, true);
		//양수 일 때 다음 좌측 정점이 우측보다 더 우측 => 우회전
		float rightCornerDot = Vector3::DotProduct(UP, rightCornerCross);
		bool isRightCorner = rightCornerDot >= 0.0f;

		if (!leftValid && !rightValid) {//길 잘 가다가 다음 점이 둘 다 벌어진 경우
			int restartIdx = leftPortalIdx < rightPortalIdx ? leftPortalIdx : rightPortalIdx;//둘 중 더 이전 정점으로 다시 세팅(같은 삼각형으로)
			XMFLOAT3 leftVertex = leftPortal[restartIdx];
			XMFLOAT3 rightVertex = rightPortal[restartIdx];

			XMFLOAT3 midVertex = Vector3::ScalarProduct(Vector3::Add(leftVertex, rightVertex), 0.5f, false);

			restartPosition = midVertex;
			optimizedPositions.emplace_back(restartPosition);

			//위에 중점으로 현재 삼각형으로 가는 위치가 확정 됐으니, 둘 다 다음 삼각형으로 전진해야 됨
			leftPortalIdx = restartIdx + 1;
			rightPortalIdx = restartIdx + 1;
			continue;
		}

		if (leftValid) {
			if (isRightCorner) {//좌측만 봤을 때는 문제 없지만, 좌측점이 우측으로 넘어온 경우 -> 우회전
				//여기서 한번 체크 포인트? 느낌으로 찍어야 됨.
				restartPosition = currentRightPortal;//우회전이니 현재 우측 정점을 지나게
				optimizedPositions.emplace_back(restartPosition);

				++rightPortalIdx;//현재 rightIdx를 기준으로 다시 시작할거기 때문에, 전진
				leftPortalIdx = rightPortalIdx;//같은 삼각형 기준으로 시작하게 세팅
				continue;
			}
			if (abs(resultLeftPortalDot) > FLT_EPSILON) {//지금점 다음점이 같은 경우도 존재함-> 다른 점일 때 전진
				++leftPortalIdx;

				//왼쪽 정점 데이터가 최신화됐으니 밑에 우측 정점 판단에서 좌회전인지 판단할 수 있어야 됨.
				currentLeftPortal = leftPortal[leftPortalIdx];
				startToCurrentLeft = Vector3::Subtract(currentLeftPortal, restartPosition);
				leftCornerCross = Vector3::CrossProduct(startToNextRight, startToCurrentLeft, true);
				leftCornerDot = Vector3::DotProduct(UP, leftCornerCross);
				isLeftCorner = leftCornerDot >= 0.0f;
			}
		}

		if (rightValid) {
			if (isLeftCorner) {
				//위에 우측 코너와 같은 방식으로 동작하게.
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
		//현재 노드 -> 다음 노드 중점 연결 벡터
		XMFLOAT3 currentToNextVector = Vector3::Subtract(nextCenter, currentCenter);

		auto sharedVertexIdxes = currentMesh->GetRelationVertexIdx(nextMesh);

		for (auto& sharedVertexIdx : sharedVertexIdxes) {
			XMFLOAT3 sharedVertex = m_vertex[sharedVertexIdx];
			//현재 중점 -> 공유하는 정점 벡터
			XMFLOAT3 centerToVertexVector = Vector3::Subtract(sharedVertex, currentCenter);
			/*
				다음 노드까지 벡터 X 현재 중점에서 공유 점 => 왼손 좌표계라면 공유점이 오른쪽이라면 0,1,0벡터 // 왼쪽이라면 0,-1,0 이 나옴
				-> 그런 다음 0, 1, 0 벡터와 내적 했을 때 양수, 음수를 판단하여 좌(음수), 우(양수) 를 판단 할 수 있음.
			*/
			XMFLOAT3 crossProductResult = Vector3::CrossProduct(currentToNextVector, centerToVertexVector, true);
			float dotProductResult = Vector3::DotProduct(UP, crossProductResult);
			if (dotProductResult > 0.0f) {//오른쪽에 위치한 정점.
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

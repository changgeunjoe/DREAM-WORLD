#include "stdafx.h"
//#include "MapData.h"
//#include <fstream>
//#include <filesystem>
//
//MapData::MapData(std::string navFileName, std::string collisionFileName, std::string monsterFileName) : m_navFileName(navFileName), m_collisionFileName(collisionFileName), m_monsterDataFileName(monsterFileName)
//{
//	if (navFileName != "NONE")
//		GetReadMapData();
//	if (collisionFileName != "NONE")
//		GetReadCollisionData();
//	if (monsterFileName != "NONE")
//		GetReadMonsterData();
//	m_initCoplete = true;
//}
//
//
//void MapData::GetReadMapData()
//{
//	using namespace std;
//
//	std::string line;
//	int vertexNum = 0;
//	int vertexNomalNum = 0;
//	int vertexTextureNum = 0;
//	//m_fileName = "\\\MapData\\\BossRoom.txt";
//	//cout << std::filesystem::current_path().string() << endl;
//	std::ifstream inFile{ m_navFileName };
//	//cout << std::filesystem::current_path().filename() << endl;
//	////cout << filesystem::current_path() << endl;
//	//std::filesystem::directory_iterator itr(std::filesystem::current_path());
//	//for (auto a = itr; !a._At_end(); a++) {
//	//	cout << a->path() << endl;
//	//}
//	string fileDataString;
//	/*inFile >> std::noskipws;
//	copy(istream_iterator<char>(inFile), {}, back_inserter(fileDataString));*/
//	const string verticesStr = "<Vertices>:";
//	const string indicesStr = "<Indices>:";
//	const string relationStr = "<Relay>:";
//
//	std::string readObject;
//	while (!inFile.eof()) {
//		inFile >> readObject;
//		if (readObject == verticesStr) {
//			inFile >> readObject;
//
//			int vertexCnt = std::stoi(readObject);
//			m_vertex.reserve(vertexCnt);
//			while (vertexCnt)
//			{
//				XMFLOAT3 v;
//				inFile >> v.x >> v.y >> v.z;
//				/*v.x *= 30.0f;
//				v.z *= 30.0f;*/
//				m_vertex.emplace_back(v.x, 0.0f, v.z);
//				vertexCnt--;
//			}
//		}
//		else if (readObject == indicesStr) {
//			inFile >> readObject;
//			int indexCnt = std::stoi(readObject);
//			m_triangleMesh.reserve(indexCnt / 3);
//			while (indexCnt)
//			{
//				int i;
//				inFile >> i;
//				m_index.emplace_back(i);
//				indexCnt--;
//			}
//			int cnt = 0;
//			tiangleMeshGridMinSize = 0.0f;
//			for (auto indexIter = m_index.begin(); indexIter != m_index.end(); indexIter += 3) {
//				m_triangleMesh.emplace_back(m_vertex[*indexIter], m_vertex[*(indexIter + 1)], m_vertex[*(indexIter + 2)], *indexIter, *(indexIter + 1), *(indexIter + 2), cnt);
//				float circumscribedL = m_triangleMesh[cnt].GetCircumscribedLength();
//				if (tiangleMeshGridMinSize < circumscribedL)
//					tiangleMeshGridMinSize = circumscribedL;
//				cnt++;
//			}
//			tiangleMeshGridMinSize += 1.0f;
//			m_navMeshQuadTree.CreateQuadTreeNode(tiangleMeshGridMinSize);
//			for (auto& tMesh : m_triangleMesh) {
//				m_navMeshQuadTree.Insert(tMesh);
//			}
//
//		}
//		else if (readObject == relationStr) {
//			inFile >> readObject;
//			int relationCnt = std::stoi(readObject);
//			for (int i = 0; i < relationCnt; i++) {
//				inFile >> readObject;
//				int index1, index2;
//				inFile >> index1;
//				inFile >> index2;
//				if (index1 != index2) {
//					if (m_triangleMesh[index1].m_relationMesh.count(index2) == 0)
//						m_triangleMesh[index1].m_relationMesh.try_emplace(index2, m_triangleMesh[index1].GetDistance(m_triangleMesh[index2]));//center 거리까지 emplac_back해야됨
//					if (m_triangleMesh[index2].m_relationMesh.count(index1) == 0)
//						m_triangleMesh[index2].m_relationMesh.try_emplace(index1, m_triangleMesh[index2].GetDistance(m_triangleMesh[index1]));//center 거리까지 emplac_back해야됨
//				}
//			}
//		}
//	}
//
//	
//	XMFLOAT3 bossPos = XMFLOAT3(179.4, 0, -38.1);
//	int resultBossIdx = -1;
//	std::set<int> bossIdxs = m_navMeshQuadTree.GetNearbyNavMeshes(179.4, -38.1);
//	bool isOnResult = false;
//	float minDis = FLT_MAX;
//	for (int node : bossIdxs) {
//		isOnResult = m_triangleMesh[node].IsOnTriangleMesh(bossPos);
//		if (isOnResult) {
//			resultBossIdx = node;
//			break;
//		}
//		float dis = m_triangleMesh[node].GetDistanceByPoint(bossPos);
//		if (minDis > dis) {
//			minDis = dis;
//			resultBossIdx = node;
//		}
//	}
//
//	int reFindIdx = -1;
//	int bossNodeIdx = -1;
//	if (isOnResult) {
//		bossNodeIdx = resultBossIdx;
//	}
//	else if (!isOnResult) {
//		for (auto relationList : m_triangleMesh[resultBossIdx].m_relationMesh) {
//			isOnResult = m_triangleMesh[relationList.first].IsOnTriangleMesh(bossPos);
//			reFindIdx = relationList.first;
//			if (isOnResult) break;
//		}
//	}
//	if (reFindIdx == -1)
//		bossNodeIdx = resultBossIdx;
//	else bossNodeIdx = reFindIdx;
//	m_bossStartIdx = bossNodeIdx;
//
//
//	std::cout << "map load end" << std::endl;
//}
//
//void MapData::GetReadCollisionData()
//{
//	std::ifstream objectFile(m_collisionFileName);
//
//	std::vector<XMFLOAT3> modelPosition;
//	std::vector<XMFLOAT4> quaternion;
//
//	std::vector<XMFLOAT3> modelScale;
//	std::vector<XMFLOAT3> modelEulerRotate;
//
//	std::vector<XMFLOAT3> colliderWorldPosition;
//	std::vector<XMFLOAT3> colliderWorldBoundSize;
//	std::vector<XMFLOAT3> colliderWorldExtentSize;
//	std::vector<XMFLOAT3> colliderForwardVec;
//	std::vector<XMFLOAT3> colliderRightVec;
//	std::vector<float>	 forwardDot;
//	std::vector<float>	 rightDot;
//
//	std::string temp;
//	std::vector<std::string> name;
//	float number[3] = {};
//	float qnumber[4] = {};
//	while (!objectFile.eof())
//	{
//		objectFile >> temp;
//		if (temp == "<position>:")
//		{
//			for (int i = 0; i < 3; ++i)
//			{
//				objectFile >> temp;
//				number[i] = stof(temp);
//			}
//			modelPosition.emplace_back(number[0], number[1], number[2]);
//		}
//		else if (temp == "<quaternion>:")
//		{
//			for (int i = 0; i < 4; ++i)
//			{
//				objectFile >> temp;
//				qnumber[i] = stof(temp);
//			}
//			quaternion.emplace_back(qnumber[0], qnumber[1], qnumber[2], qnumber[3]);
//		}
//		else if (temp == "<rotation>:")
//		{
//			for (int i = 0; i < 3; ++i)
//			{
//				objectFile >> temp;
//				number[i] = stof(temp);
//			}
//			modelEulerRotate.emplace_back(number[0], number[1], number[2]);
//		}
//		else if (temp == "<scale>:")
//		{
//			for (int i = 0; i < 3; ++i)
//			{
//				objectFile >> temp;
//				number[i] = stof(temp);
//			}
//			modelScale.emplace_back(number[0], number[1], number[2]);
//		}
//		else if (temp == "<BoxCollider>")
//		{
//			for (int j = 0; j < 7; ++j)
//			{
//				objectFile >> temp;
//				if (temp == "<center>:")
//				{
//					for (int i = 0; i < 3; ++i)
//					{
//						objectFile >> temp;
//						number[i] = stof(temp);
//					}
//					colliderWorldPosition.emplace_back(number[0], number[1], number[2]);
//				}
//				else if (temp == "<boundSize>:")
//				{
//					for (int i = 0; i < 3; ++i)
//					{
//						objectFile >> temp;
//						number[i] = stof(temp);
//					}
//					colliderWorldBoundSize.emplace_back(number[0], number[1], number[2]);
//				}
//				else if (temp == "<extent>:")
//				{
//					for (int i = 0; i < 3; ++i)
//					{
//						objectFile >> temp;
//						number[i] = stof(temp);
//					}
//					colliderWorldExtentSize.emplace_back(number[0], number[1], number[2]);
//				}
//				else if (temp == "<forward>:")
//				{
//					for (int i = 0; i < 3; ++i)
//					{
//						objectFile >> temp;
//						number[i] = stof(temp);
//					}
//					colliderForwardVec.emplace_back(number[0], number[1], number[2]);
//				}
//				else if (temp == "<right>:")
//				{
//					for (int i = 0; i < 3; ++i)
//					{
//						objectFile >> temp;
//						number[i] = stof(temp);
//					}
//					colliderRightVec.emplace_back(number[0], number[1], number[2]);
//				}
//				else if (temp == "<forwardDotRes>:")
//				{
//					objectFile >> temp;
//					number[0] = stof(temp);
//					forwardDot.emplace_back(number[0]);
//				}
//				else if (temp == "<rightDotRes>:")
//				{
//					objectFile >> temp;
//					number[0] = stof(temp);
//					rightDot.emplace_back(number[0]);
//				}
//			}
//		}
//		else name.emplace_back(temp);
//	}
//
//	for (int i = 0; i < rightDot.size(); ++i)
//		m_collisionDatas.emplace_back(colliderWorldPosition[i], colliderWorldExtentSize[i], quaternion[i], forwardDot[i], colliderForwardVec[i], rightDot[i], colliderRightVec[i]);
//
//	for (int i = 0; i < m_collisionDatas.size(); i++)
//	{
//		for (int j = 0; j < m_collisionDatas.size(); j++) {
//			if (i != j) {
//				bool intersectResult = m_collisionDatas[i].GetObb().Intersects(m_collisionDatas[j].GetObb());
//				if (intersectResult) {
//					m_collisionDatas[i].SetRelationIdx(j);
//					name[i];
//					name[j];
//				}
//			}
//		}
//	}
//	return;
//}
//
//std::list<int> MapData::AStarLoad(std::atomic_int& myTriangleIdx, float desX, float desZ)
//{
//	
//	std::map<int, AstarNode> openList;
//	std::map<int, AstarNode> closeList;
//
//	openList.clear();
//	closeList.clear();
//	//nodeIdx cost dist res parent
//	closeList.try_emplace(myTriangleIdx, myTriangleIdx, 0, 0, 0, -1);
//	int currentTriangleIdx = myTriangleIdx;
//	std::set<int> candidateNodes = m_navMeshQuadTree.GetNearbyNavMeshes(desX, desZ);
//	float minDis = FLT_MAX;
//	int idx = -1;
//	bool isOnResult = false;
//	XMFLOAT3 desPos = XMFLOAT3(desX, 0, desZ);
//	for (int node : candidateNodes) {
//		isOnResult = m_triangleMesh[node].IsOnTriangleMesh(desPos);
//		if (isOnResult) {
//			idx = node;
//			break;
//		}
//		float dis = m_triangleMesh[node].GetDistanceByPoint(desPos);
//		if (minDis > dis) {
//			minDis = dis;
//			idx = node;
//		}
//	}
//
//	int reFindIdx = -1;
//	if (!isOnResult) {
//		for (auto relationList : m_triangleMesh[idx].m_relationMesh) {
//			isOnResult = m_triangleMesh[relationList.first].IsOnTriangleMesh(desPos);
//			reFindIdx = relationList.first;
//			if (isOnResult) break;
//		}
//	}
//
//	int desNodeIdx = -1;
//	if (idx == -1)
//		return std::list<int>{myTriangleIdx};
//	else {
//		if (reFindIdx == -1)
//			desNodeIdx = idx;
//		else desNodeIdx = reFindIdx;
//	}
//
//	if (myTriangleIdx == desNodeIdx)
//		return std::list<int>{myTriangleIdx};
//
//	while (true) {
//		auto relationTriangleIdx = m_triangleMesh[currentTriangleIdx].m_relationMesh;
//		//openList update
//		for (auto& triangle : relationTriangleIdx) {
//			if (closeList.count(triangle.first) == 1) continue;
//			float dis = m_triangleMesh[triangle.first].GetDistance(m_triangleMesh[desNodeIdx]);
//			if (openList.count(triangle.first) == 0) {
//				openList.try_emplace(triangle.first, triangle.first, triangle.second, dis, triangle.second + dis, currentTriangleIdx);
//			}
//			else {
//				if (openList[triangle.first].GetResValue() > dis + triangle.second) {
//					openList[triangle.first].RefreshNodeData(triangle.first, triangle.second, dis, triangle.second + dis, currentTriangleIdx);
//				}
//			}
//		}
//
//		AstarNode minNode = AstarNode(-1, FLT_MAX, FLT_MAX, FLT_MAX, -1);
//		for (auto& openNode : openList) {
//			if (openNode.second < minNode)
//				minNode = openNode.second;
//		}
//		if (minNode.GetIdx() == -1) {
//			return std::list<int>{myTriangleIdx};
//			std::cout << "last Index: ";
//			for (auto& triangle : relationTriangleIdx) {
//				std::cout << triangle.first << " ";
//			}
//		}
//		//std::cout << std::endl;
//
//		currentTriangleIdx = minNode.GetIdx();
//	
//		closeList.try_emplace(currentTriangleIdx, minNode);
//		openList.erase(currentTriangleIdx);
//		//#ifdef _DEBUG
//		//		std::cout << "ID: " << minNode.GetIdx() << " Parent: " << minNode.GetParentIdx() << " Res: " << minNode.GetResValue() << "Dis: " << minNode.GetDistance() << std::endl;
//		//#endif
//				//if (m_triangleMesh[currentTriangleIdx].GetDistance(desX, 0.0f, desZ) < 80.0f) {
//
//				//종료 조건 - 현재 인덱스와 목적지 인덱스가 같을때
//		if (currentTriangleIdx == desNodeIdx) {
//			std::list<int> resList;
//			int currentIdx = currentTriangleIdx;
//			while (true) {
//				AstarNode& currentNode = closeList[currentIdx];
//				resList.emplace_front(currentIdx);
//				if (currentIdx == myTriangleIdx) {
//					return resList;
//				}
//				currentIdx = currentNode.GetParentIdx();
//			}
//		}
//
//		//		if (closeList[currentTriangleIdx].GetDistance() < 150.0f) {
//		//#ifdef _DEBUG
//		//			std::cout << "currentIdx: " << currentTriangleIdx;
//		//#endif
//		//			if (m_triangleMesh[currentTriangleIdx].IsOnTriangleMesh(desX, 0.0f, desZ)) {
//		//				std::list<int> resList;				
//		//				int currentIdx = currentTriangleIdx;
//		//				while (true) {
//		//					AstarNode currentNode = closeList[currentIdx];				
//		//					if (currentIdx == myTriangleIdx) {
//		//#ifdef _DEBUG
//		//						std::cout << std::endl << std::endl;
//		//#endif
//		//						//findPath(resList);
//		//						//findPath(resStack);
//		//						return resList;
//		//					}
//		//					resList.emplace_front(currentIdx);
//		//					currentIdx = currentNode.GetParentIdx();
//		//				}
//		//			}
//		//		}
//	}
//}
//
////std::list<int> MapData::AStarLoad(int myTriangleIdx, float desX, float desZ)
////{
////	if (m_triangleMesh[myTriangleIdx].IsOnTriangleMesh(desX, 0.0f, desZ)) {
////		return std::list<int>{myTriangleIdx};
////	}
////	std::map<int, AstarNode> openList;
////	std::map<int, AstarNode> closeList;
////
////	openList.clear();
////	closeList.clear();
////	//nodeIdx cost dist res parent
////	closeList.try_emplace(myTriangleIdx, myTriangleIdx, 0, 0, 0, -1);
////	int currentTriangleIdx = myTriangleIdx;
////
////	while (true) {
////		auto relationTriangleIdx = m_triangleMesh[currentTriangleIdx].m_relationMesh;
////		//openList update
////		for (auto& triangle : relationTriangleIdx) {
////			if (closeList.count(triangle.first) == 1) continue;
////			float dis = m_triangleMesh[triangle.first].GetDistance(desX, 0.0f, desZ);
////			if (openList.count(triangle.first) == 0) {
////				openList.try_emplace(triangle.first, triangle.first, triangle.second, dis, triangle.second + dis, currentTriangleIdx);
////			}
////			else {
////				if (openList[triangle.first].GetResValue() > dis + triangle.second) {
////					openList[triangle.first].RefreshNodeData(triangle.first, triangle.second, dis, triangle.second + dis, currentTriangleIdx);
////				}
////			}
////		}
////
////		AstarNode minNode = AstarNode(-1, FLT_MAX, FLT_MAX, FLT_MAX, -1);
////		for (auto& openNode : openList) {
////			if (openNode.second < minNode)
////				minNode = openNode.second;
////		}
////		if (minNode.GetIdx() == -1) {
////			std::cout << "last Index: ";
////			for (auto& triangle : relationTriangleIdx) {
////				std::cout << triangle.first << " ";
////			}
////		}
////		std::cout << std::endl;
////		currentTriangleIdx = minNode.GetIdx();
////		closeList.try_emplace(currentTriangleIdx, minNode);
////		openList.erase(currentTriangleIdx);
////#ifdef _DEBUG
////		std::cout << "ID: " << minNode.GetIdx() << " Parent: " << minNode.GetParentIdx() << " Res: " << minNode.GetResValue() << "Dis: " << minNode.GetDistance() << std::endl;
////#endif
////		//if (m_triangleMesh[currentTriangleIdx].GetDistance(desX, 0.0f, desZ) < 80.0f) {
////		if (closeList[currentTriangleIdx].GetDistance() < 150.0f) {
////#ifdef _DEBUG
////			std::cout << "currentIdx: " << currentTriangleIdx;
////#endif
////			if (m_triangleMesh[currentTriangleIdx].IsOnTriangleMesh(desX, 0.0f, desZ)) {
////				std::list<int> resList;
////				std::stack<int> resStack;
////				int currentIdx = currentTriangleIdx;
////				resStack.push(currentIdx);
////				while (true) {
////					AstarNode currentNode = closeList[currentIdx];
////					resStack.push(currentIdx);
////					if (currentIdx == myTriangleIdx) {
////#ifdef _DEBUG
////						std::cout << std::endl << std::endl;
////#endif
////						//findPath(resList);
////						//findPath(resStack);
////						return resList;
////					}
////					resList.emplace_front(currentIdx);
////					currentIdx = currentNode.GetParentIdx();
////				}
////			}
////		}
////	}
////}
//
//void MapData::GetReadMonsterData()
//{
//	std::ifstream objectFile(m_monsterDataFileName);
//
//	std::vector<XMFLOAT3> modelPosition;
//	std::vector<XMFLOAT3> modelEulerRotate;
//	std::string readData;
//	std::vector<std::string> name;
//
//	float qnumber[4] = {};
//	while (!objectFile.eof())
//	{
//		objectFile >> readData;
//		if (readData == "<position>:")
//		{
//			float number[3] = {};
//			for (int i = 0; i < 3; ++i)
//			{
//				objectFile >> readData;
//				number[i] = stof(readData);
//			}
//			modelPosition.emplace_back(number[0], number[1], number[2]);
//		}
//		else if (readData == "<quaternion>:")
//		{
//			for (int i = 0; i < 4; ++i)
//			{
//				objectFile >> readData;
//			}
//		}
//		else if (readData == "<rotation>:")
//		{
//			float number[3] = {};
//			for (int i = 0; i < 3; ++i)
//			{
//				objectFile >> readData;
//				number[i] = stof(readData);
//			}
//			modelEulerRotate.emplace_back(number[0], number[1], number[2]);
//		}
//		else if (readData == "<scale>:")
//		{
//			for (int i = 0; i < 3; ++i)
//			{
//				objectFile >> readData;
//			}
//		}
//		else name.emplace_back(readData);
//	}
//	for (int i = 0; i < modelPosition.size(); i++)
//		m_initMonsterDatas.emplace_back(modelPosition[i], modelEulerRotate[i]);
//}

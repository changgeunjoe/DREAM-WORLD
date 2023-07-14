#include "stdafx.h"
#include "MapData.h"


void MapData::GetReadMapData()
{
	using namespace std;
	std::vector<XMFLOAT3> vertex;
	std::vector<int> index;
	std::string line;
	int vertexNum = 0;
	int vertexNomalNum = 0;
	int vertexTextureNum = 0;
	//m_fileName = "\\\MapData\\\BossRoom.txt";
	//cout << std::filesystem::current_path().string() << endl;
	std::ifstream inFile{ m_navFileName };
	//cout << std::filesystem::current_path().filename() << endl;
	////cout << filesystem::current_path() << endl;
	//std::filesystem::directory_iterator itr(std::filesystem::current_path());
	//for (auto a = itr; !a._At_end(); a++) {
	//	cout << a->path() << endl;
	//}
	string fileDataString;
	inFile >> std::noskipws;
	copy(istream_iterator<char>(inFile), {}, back_inserter(fileDataString));
	const string verticesStr = "<Vertices>:";
	const string indicesStr = "<Indices>:";
	const string relationStr = "<Relay>:";
	auto vertexPos = fileDataString.find(verticesStr);
	auto indexPos = fileDataString.find(indicesStr);
	auto relationxPos = fileDataString.find(relationStr);
	inFile.clear();

	//vertex data Read
	inFile.seekg(vertexPos + verticesStr.size() + 1, std::ios::beg);
	inFile >> std::skipws;
	int vertexCnt = -1;
	inFile >> vertexCnt;
	vertex.reserve(vertexCnt);
	while (vertexCnt)
	{
		XMFLOAT3 v;
		inFile >> v.x >> v.y >> v.z;
		vertex.emplace_back(v.x, 0.0f, v.z);
		vertexCnt--;
	}

	//index data Read
	inFile.clear();
	inFile >> std::noskipws;
	inFile.seekg(indexPos + indicesStr.size() + 2, std::ios::beg);
	cout << "index pos: " << inFile.tellg() << endl;
	inFile >> std::skipws;

	int indexCnt = -1;
	inFile >> indexCnt;
	index.reserve(indexCnt);
	m_triangleMesh.reserve(indexCnt / 3);
	while (indexCnt)
	{
		int i;
		inFile >> i;
		index.emplace_back(i);
		indexCnt--;
	}

	//Relay Data Read
	inFile.clear();
	inFile >> std::noskipws;
	inFile.seekg(relationxPos + relationStr.size() + 4, std::ios::beg);
	cout << "relation pos: " << inFile.tellg() << endl;
	inFile >> std::skipws;

	for (auto indexIter = index.begin(); indexIter != index.end(); indexIter += 3) {
		m_triangleMesh.emplace_back(vertex[*indexIter], vertex[*(indexIter + 1)], vertex[*(indexIter + 2)], *indexIter, *(indexIter + 1), *(indexIter + 2));
	}

	while (true)
	{
		int relationvertex = -1;
		inFile >> relationvertex;

		int relationCnt = -1;
		inFile >> relationCnt;
		vector<int> siblingNodeIndex;
		if (relationCnt < 0)break;
		siblingNodeIndex.reserve(relationCnt);//형제 노드들 인덱스 저장
		for (int relationIdx = 0; relationIdx < relationCnt; relationIdx++) {
			int idx = -1;
			inFile >> idx;
			siblingNodeIndex.emplace_back(idx);
		}
		for (auto& currentIdx : siblingNodeIndex) {
			for (auto& sibIdx : siblingNodeIndex) {
				if (sibIdx != currentIdx) {
					if (m_triangleMesh[currentIdx].m_relationMesh.count(sibIdx) == 0)
						m_triangleMesh[currentIdx].m_relationMesh.try_emplace(sibIdx, m_triangleMesh[currentIdx].GetDistance(m_triangleMesh[sibIdx]));//center 거리까지 emplac_back해야됨
				}
			}
		}
		if (inFile.eof())break;
	}

	std::cout << "map load end" << std::endl;
}

std::list<int> MapData::AStarLoad(int myTriangleIdx, float desX, float desZ)
{
	std::map<int, AstarNode> openList;
	std::map<int, AstarNode> closeList;

	openList.clear();
	closeList.clear();
	//nodeIdx cos dis res parent
	closeList.try_emplace(myTriangleIdx, myTriangleIdx, 0, 0, 0, -1);
	int currentTriangleIdx = myTriangleIdx;
	while (true) {
		auto relationTriangleIdx = m_triangleMesh[myTriangleIdx].m_relationMesh;
		//openList update
		for (auto& tri : relationTriangleIdx) {
			if (openList.count(tri.first) == 0) {
				float dis = m_triangleMesh[tri.first].GetDistance(desX, 0.0f, desZ);
				openList.try_emplace(tri.first, tri.first, tri.second, dis, tri.second + dis, currentTriangleIdx);
			}
			else {
				float dis = m_triangleMesh[tri.first].GetDistance(desX, 0.0f, desZ);
				if (openList[tri.first].GetResValue() > dis + tri.second) {
					openList[tri.first].RefreshNodeData(tri.first, tri.second, dis, tri.second + dis, currentTriangleIdx);
				}
			}
		}

		AstarNode minNode = AstarNode(-1, -1, -1, FLT_MAX, -1);
		for (auto& openNode : openList) {
			if (minNode < openNode.second)
				minNode = openNode.second;
		}

		currentTriangleIdx = minNode.GetIdx();
		closeList.try_emplace(currentTriangleIdx, minNode);
		openList.erase(currentTriangleIdx);
		if (m_triangleMesh[currentTriangleIdx].GetDistance(desX, 0.0f, desZ) < 45.0f) {
			std::list<int> resList;
			int currentIdx = currentTriangleIdx;
			while (true) {
				AstarNode currentNode = closeList[currentIdx];
				if (currentIdx == myTriangleIdx) return resList;
				resList.emplace_front(currentIdx);
				currentIdx = currentNode.GetParentIdx();
			}
		}
	}
}

MapData::MapData(std::string fileName, std::string collisionFileName) : m_navFileName(fileName), m_collisionFileName(collisionFileName)
{
	GetReadMapData();
	GetReadCollisionData();
}


void MapData::GetReadCollisionData()
{
	std::vector<XMFLOAT4> quaternion;
	std::vector<XMFLOAT3> tempPos;
	std::vector<XMFLOAT3> tempScale;
	std::vector<XMFLOAT3> tempCenterPos;
	std::vector<XMFLOAT3> tempLocalCenterPos;
	std::vector<XMFLOAT3> tempExtentScale;
	std::vector<float> forwardDotRes;
	std::vector<float> rightDotRes;
	std::vector<XMFLOAT3> forwardNormalVector;
	std::vector<XMFLOAT3> rightNormalVector;
	std::string readObj;
	float number[3] = {};
	float qnumber[4] = {};
	int objCount = -1;

	std::string FileName = "CollisionData.txt";
	std::ifstream objectFile(m_collisionFileName);

	while (!objectFile.eof())
	{
		objectFile >> readObj;
		if (readObj == "<position>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> readObj;
				number[i] = std::stof(readObj);
			}
			tempPos.emplace_back(number[0], number[1], number[2]);
		}
		else if (readObj == "<quaternion>:")
		{
			for (int i = 0; i < 4; ++i)
			{
				objectFile >> readObj;
				qnumber[i] = std::stof(readObj);
			}
			quaternion.emplace_back(qnumber[0], qnumber[1], qnumber[2], qnumber[3]);
		}
		else if (readObj == "<rotation>:")
		{
			for (int i = 0; i < 3; ++i)
				objectFile >> readObj;
		}
		else if (readObj == "<scale>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> readObj;
				number[i] = std::stof(readObj);
			}
			tempScale.emplace_back(number[0], number[1], number[2]);
		}
		else if (readObj == "<BoxCollider>")
		{
			for (int j = 0; j < 8; ++j)
			{
				objectFile >> readObj;
				if (readObj == "<center>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> readObj;
						number[i] = stof(readObj);
					}
					tempCenterPos.emplace_back(number[0], number[1], number[2]);
				}
				else if (readObj == "<size>:")
				{
					for (int i = 0; i < 3; ++i)
						objectFile >> readObj;
				}
				else if (readObj == "<localCenter>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> readObj;
						number[i] = stof(readObj);
					}
					tempLocalCenterPos.emplace_back(number[0], number[1], number[2]);
				}
				else if (readObj == "<localSize>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> readObj;
						number[i] = stof(readObj);
					}
					tempExtentScale.emplace_back(number[0], number[1], number[2]);
				}
				else if (readObj == "<forward>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> readObj;
						number[i] = stof(readObj);
					}
					forwardNormalVector.emplace_back(number[0], number[1], number[2]);
				}
				else if (readObj == "<right>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> readObj;
						number[i] = stof(readObj);
					}
					rightNormalVector.emplace_back(number[0], number[1], number[2]);
				}
				else if (readObj == "<forwardDotRes>:")
				{
					objectFile >> readObj;
					float num = stof(readObj);
					forwardDotRes.emplace_back(num);
				}
				else if (readObj == "<rightDotRes>:")
				{
					objectFile >> readObj;
					float num = stof(readObj);
					rightDotRes.emplace_back(num);
				}
			}
		}
		else if (readObj == "<Seq>:") { objectFile >> readObj; }
		else
		{
			objCount++;
		}
	}

	for (int i = 0; i < objCount; ++i)
	{
		XMFLOAT3 extentPos = XMFLOAT3(tempScale[i].x * tempExtentScale[i].x,
			tempScale[i].y * tempExtentScale[i].y, tempScale[i].z * tempExtentScale[i].z);
		//XMFLOAT3 centerPos = tempCenterPos[i];// Vector3::Add(tempCenterPos[i], tempLocalCenterPos[i]);
		extentPos = XMFLOAT3(extentPos.x * 0.5f, extentPos.y * 0.5f, extentPos.z * 0.5f);
		m_collisionDatas.emplace_back(tempCenterPos[i], extentPos, quaternion[i], forwardDotRes[i], forwardNormalVector[i], rightDotRes[i], rightNormalVector[i]);
		if (i == 0) {
			m_collisionDatas[i].SetRelationIdx(objCount - 1);
			m_collisionDatas[i].SetRelationIdx(i + 1);
		}
		else if (i == objCount - 1) {
			m_collisionDatas[i].SetRelationIdx(i - 1);
			m_collisionDatas[i].SetRelationIdx(0);
		}
	}
	return;
}
#include "stdafx.h"
#include "MapData.h"
#include <fstream>
#include <filesystem>

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
	std::ifstream inFile{ m_fileName };
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
		/*v.x *= 30.0f;
		v.z *= 30.0f;*/
		vertex.emplace_back(v.x, 0.0f, v.z);
		vertexCnt--;
	}

	//index data Read
	inFile.clear();
	inFile >> std::noskipws;
	inFile.seekg(indexPos + indicesStr.size() + 1, std::ios::beg);
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
	inFile.seekg(relationxPos + relationStr.size() + 2, std::ios::beg);
	cout << "relation pos: " << inFile.tellg() << endl;
	inFile >> std::skipws;

	for (auto indexIter = index.begin(); indexIter != index.end(); indexIter += 3) {
		m_triangleMesh.emplace_back(vertex[*indexIter], vertex[*(indexIter + 1)], vertex[*(indexIter + 2)]);
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
	float fltMx = FLT_MAX;
	int idxM = -1;
	int inIdx = -1;
	int zeroIdx = -1;
	vector<int> zeroIdxs;
	for (int i = 0; i < index.size() / 3; i++) {
		float dis = m_triangleMesh[i].GetDistance(300.0f, 0.0f, 100.0f);
		if (fltMx > dis) {
			fltMx = dis;
			idxM = i;
		}
		if (m_triangleMesh[i].IsOnTriangleMesh(300.0f, 0.0f, 100.0f))
			inIdx = i;
		if (m_triangleMesh[i].IsOnTriangleMesh(0.0f, 0.0f, 0.0f))
			zeroIdxs.emplace_back(i);
		//zeroIdx = i;
	}
	std::cout << "dis Min: " << idxM << "dis: " << fltMx << std::endl;
	std::cout << "On idx: " << inIdx << std::endl;
	//std::cout << "On ZeroIdx: " << zeroIdx << std::endl;
	std::cout << "ZeroIdxs: ";
	for (auto& i : zeroIdxs)
		std::cout << i << " ";
	std::cout << std::endl;
	std::cout << "map load end" << std::endl;
}

std::list<int> MapData::AStarLoad(int myTriangleIdx, float desX, float desZ)
{
	std::map<int, AstarNode> openList;
	std::map<int, AstarNode> closeList;

	openList.clear();
	closeList.clear();
	//nodeIdx cost dist res parent
	closeList.try_emplace(myTriangleIdx, myTriangleIdx, 0, 0, 0, -1);
	int currentTriangleIdx = myTriangleIdx;
	while (true) {
		auto relationTriangleIdx = m_triangleMesh[currentTriangleIdx].m_relationMesh;
		//openList update
		for (auto& triangle : relationTriangleIdx) {
			if (closeList.count(triangle.first) == 1) continue;
			float dis = m_triangleMesh[triangle.first].GetDistance(desX, 0.0f, desZ);
			if (openList.count(triangle.first) == 0) {
				openList.try_emplace(triangle.first, triangle.first, triangle.second, dis, triangle.second + dis, currentTriangleIdx);
			}
			else {
				if (openList[triangle.first].GetResValue() > dis + triangle.second) {
					openList[triangle.first].RefreshNodeData(triangle.first, triangle.second, dis, triangle.second + dis, currentTriangleIdx);
				}
			}
		}

		AstarNode minNode = AstarNode(-1, FLT_MAX, FLT_MAX, FLT_MAX, -1);
		for (auto& openNode : openList) {
			if (openNode.second < minNode)
				minNode = openNode.second;
		}
		if (minNode.GetIdx() == -1) {
			std::cout << "last Index: ";
			for (auto& triangle : relationTriangleIdx) {
				std::cout << triangle.first << " ";
			}
		}
		std::cout << std::endl;
		currentTriangleIdx = minNode.GetIdx();
		closeList.try_emplace(currentTriangleIdx, minNode);
		openList.erase(currentTriangleIdx);
#ifdef _DEBUG
		std::cout << "ID: " << minNode.GetIdx() << " Parent: " << minNode.GetParentIdx() << " Res: " << minNode.GetResValue() << "Dis: " << minNode.GetDistance() << std::endl;
#endif
		//if (m_triangleMesh[currentTriangleIdx].GetDistance(desX, 0.0f, desZ) < 80.0f) {
		if (closeList[currentTriangleIdx].GetDistance() < 150.0f) {
#ifdef _DEBUG
			std::cout << "currentIdx: " << currentTriangleIdx;
#endif
			if (m_triangleMesh[currentTriangleIdx].IsOnTriangleMesh(desX, 0.0f, desZ)) {
				std::list<int> resList;
				int currentIdx = currentTriangleIdx;
				while (true) {
					AstarNode currentNode = closeList[currentIdx];
					if (currentIdx == myTriangleIdx) {
#ifdef _DEBUG
						std::cout << std::endl << std::endl;
#endif
						return resList;
					}
					resList.emplace_front(currentIdx);
					currentIdx = currentNode.GetParentIdx();
				}
			}
		}
	}
}

MapData::MapData(std::string fileName) : m_fileName(fileName)
{
	GetReadMapData();
}

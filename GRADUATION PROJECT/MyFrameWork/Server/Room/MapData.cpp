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
	auto vertexPos = fileDataString.find(verticesStr);
	auto indezxPos = fileDataString.find(indicesStr);
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
		vertex.emplace_back(v.x, v.y, v.z);
		vertexCnt--;
	}

	//index data Read
	inFile.clear();
	inFile >> std::noskipws;
	inFile.seekg(indezxPos + indicesStr.size() + 1 , std::ios::beg);
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

	

	for (auto indexIter = index.begin(); indexIter != index.end(); indexIter += 3) {
		m_triangleMesh.emplace_back(vertex[*indexIter], vertex[*(indexIter + 1)], vertex[*(indexIter + 2)]);

	}



	std::unordered_map<int, std::vector<int>> triangleNeighbors;

	// 인덱스를 이용하여 삼각형들을 연결
	for (int i = 0; i < index.size(); i += 3) {
		int triangleIndex = i / 3;
		int vertexA = index[i];
		int vertexB = index[i + 1];
		int vertexC = index[i + 2];

		// 삼각형별 이웃 정보 추가
		triangleNeighbors[triangleIndex].push_back(vertexA);
		triangleNeighbors[triangleIndex].push_back(vertexB);
		triangleNeighbors[triangleIndex].push_back(vertexC);
	}

	// 이웃 관계 출력
	for (const auto& entry : triangleNeighbors) {
		int triangleIndex = entry.first;
		const std::vector<int>& vertices = entry.second;

		std::cout << "Triangle " << triangleIndex << "의 이웃: ";
		for (int vertex : vertices) {
			std::cout << vertex << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "map load end" << std::endl;
}

MapData::MapData(std::string fileName) : m_fileName(fileName)
{
	GetReadMapData();
}

#pragma once

class Room
{
public:
	Room();
	Room(std::string roomId);
	~Room();
private:
	std::wstring m_roomName;
	std::string m_roomId;
	int m_roomOwnerId = -1;// 룸 생성한 자의 ID
private:
	//현재 존재하는 플레이어들을 어떻게 담을까
	//set<pair> // 그러기엔... PlayerObj에 ROle을 넣었다. // Set<int> : Player ID(server상)으로 하면 될까

};


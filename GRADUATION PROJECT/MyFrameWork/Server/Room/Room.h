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
	int m_roomOwnerId = -1;// �� ������ ���� ID
private:
	//���� �����ϴ� �÷��̾���� ��� ������
	//set<pair> // �׷��⿣... PlayerObj�� ROle�� �־���. // Set<int> : Player ID(server��)���� �ϸ� �ɱ�

};


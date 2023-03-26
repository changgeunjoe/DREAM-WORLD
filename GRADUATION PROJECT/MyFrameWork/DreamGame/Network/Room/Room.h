#pragma once

class Room
{
public:
	Room() {}
	~Room() {}
	Room(wstring roomName, string roomId, char* role, string* playerName);
private:
	wstring m_roomName;
	string m_roomId;
	//lock?
	map<ROLE, string> m_inGamePlayers;
public:
	const wstring GetRoomName();
	const string GetRoomId();
	const map<ROLE, string> GetIngamePlayersInfo();
};

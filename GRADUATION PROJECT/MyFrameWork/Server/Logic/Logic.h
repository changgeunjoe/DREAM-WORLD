#pragma once
#include "../PCH/stdafx.h"

class Session;
class Logic
{
private:

public:
	Logic();
	~Logic();
private:
	volatile bool m_isRunningThread = false;
	std::thread m_PlayerMoveThread;
	std::thread m_MatchingThread;
private:
	Concurrency::concurrent_queue<int> warriorPlayerIdQueue;
	Concurrency::concurrent_queue<int> archerPlayerIdQueue;
	Concurrency::concurrent_queue<int> priestPlayerIdQueue;
	Concurrency::concurrent_queue<int> tankerPlayerIdQueue;

	Concurrency::concurrent_queue<int> randPlayerIdQueue;
private:
	//Concurrency::concurrent_unordered_set<int> m_matchPlayerSet;
	/*std::mutex m_matchPlayerLock;
	std::map<ROLE, int> m_matchPlayer;
	std::atomic_char m_MatchRole = 0x00;*/
private:
	std::mutex m_inGameUserLock;
	std::map<std::wstring, int> m_inGameUser;//현재 게임에 접속한 유저에 대해서 key: id, value: server arr idx
public:
	void AcceptPlayer(Session* session, int userId, SOCKET& sock);
	void ProcessPacket(int userId, char* p);

	//send
	void BroadCastPacket(void* p);
	void MultiCastOtherPlayer(int userId, void* p);
	void MultiCastOtherPlayerInRoom(int userId, void* p);
	void BroadCastInRoomByPlayer(int userId, void* p);
	//Boss Broad Cast
	void BroadCastInRoom(int roomId, void* p);

public:
	void AutoMoveServer();

	//matching
private:
	void MatchMaking();
	void InsertMatchQueue(ROLE r, int userId);
private:
	std::string MakeRoomId();
public://save InGamePlayerMap for db
	void InsertInGameUserMap(std::wstring& id, int userId);
	void DeleteInGameUserMap(std::wstring& id);
};

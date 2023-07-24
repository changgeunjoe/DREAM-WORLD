#pragma once
#include "../PCH/stdafx.h"

class UserSession;
class ExpOver;
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
	std::set<std::wstring> m_inGameUser;//현재 게임에 접속한 유저에 대해서 key: id, value: server arr idx
public:
	void AcceptPlayer(UserSession* session, int userId, SOCKET& sock);
	void ProcessPacket(int userId, char* p);

	//send
	void BroadCastPacket(void* p);
	void MultiCastOtherPlayer(int userId, void* p);
	void MultiCastOtherPlayerInRoom(int userId, void* p);
	void MultiCastOtherPlayerInRoom_R(int roomId, ROLE role, void* p);
	void OnlySendPlayerInRoom_R(int roomId, ROLE role, void* p);
	void BroadCastInRoomByPlayer(int userId, void* p);
	//Boss Broad Cast
	void BroadCastInRoom(int roomId, void* p);
	void BroadCastInRoom_Ex(int roomId, ExpOver* expover);

public:
	void AutoMoveServer();

	//matching
private:
	void MatchMaking();
	void InsertMatchQueue(ROLE r, int userId);
private:
	std::string MakeRoomId();
public://save InGamePlayerMap for db
	void InsertInGameUserSet(std::wstring& id);
	void DeleteInGameUserSet(std::wstring& id);
};

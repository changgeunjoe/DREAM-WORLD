//#pragma once
//#include "../../Server/PCH/stdafx.h"
//#include "Room.h"
//
//class RoomManager
//{
//public:
//	RoomManager();
//	~RoomManager();
//private:
//	std::array<Room, MAX_USER / 4> m_roomArr;
//	std::mutex m_currentLastRoomIdLock;
//	int m_currentLastRoomId = 0;
//	Concurrency::concurrent_queue<int> m_restRoomId;
//private:
//	std::mutex m_runningRoomSetLock;
//	std::set<int> m_runningRoomIdSet;
//public:
//	int GetNewRoomId();
//	Room& GetRunningRoomRef(int id);
//	void RoomDestroy(int roomId);
//
//	//Room Func
//public:
//	void BossFindPlayer(int roomId);
//	void ChangeBossState(int roomId);
//	void UpdateGameStateForPlayer(int roomId);
//	void UpdateSmallMonster(int roomId);
//	void BossAttackExecute(int roomId);
//public://character Skill
//	void HealPlayer(int roomId);
//	void SetBarrier(int roomId); 
//	void SkyArrowAttack(int roomId);
//};
//

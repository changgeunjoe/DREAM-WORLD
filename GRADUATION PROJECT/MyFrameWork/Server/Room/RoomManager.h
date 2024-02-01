#pragma once
#include "../../Server/PCH/stdafx.h"
#include "../SingletonBase.h"
#include "Room.h"

class RoomManager : public SingletonBase<RoomManager>
{
	friend SingletonBase;
private:
	RoomManager();
	~RoomManager();

	tbb::concurrent_unordered_map<unsigned int, Room> m_rooms;
	tbb::concurrent_queue<unsigned int> m_restRoomId;
	std::atomic_uint m_currentMaxRoomId;


	//Room Func
public:
	void BossFindPlayer(int roomId);
	void ChangeBossState(int roomId);
	void UpdateGameStateForPlayer(int roomId);
	void UpdateSmallMonster(int roomId);
	void BossAttackExecute(int roomId);
public://character Skill
	void HealPlayer(int roomId);
	void SetBarrier(int roomId);
	void SkyArrowAttack(int roomId);
};


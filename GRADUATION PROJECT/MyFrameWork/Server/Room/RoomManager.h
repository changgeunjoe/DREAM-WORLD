#pragma once
#include "../../Server/PCH/stdafx.h"
#include "../SingletonBase.h"

class MonsterMapData;
class NavMapData;
class Room;
class UserSession;
namespace IOCP {
	class Iocp;
}

class RoomManager : public SingletonBase<RoomManager>
{
	friend SingletonBase;
private:
	RoomManager() = default;
	~RoomManager() = default;

public:
	void Initialize();
	void EraseRoom(std::shared_ptr<Room>& roomRef);
	std::shared_ptr<Room> MakeRunningRoom(std::vector<std::shared_ptr<UserSession>>& userRefVec);
	std::shared_ptr<Room> MakeRunningRoomAloneMode(std::shared_ptr<UserSession>& userRef);
private:
	std::mutex m_runningRoomLock;
	std::unordered_set<std::shared_ptr<Room>> m_runningRooms;
	std::shared_ptr<MonsterMapData> m_stageMap;
	std::shared_ptr<NavMapData> m_bossmap;
};

#include "stdafx.h"
#include "IOCPNetwork/IOCP/IOCPNetwork.h"
#include "Logic/Logic.h"
#include "DB/DBObject.h"
#include "Timer/Timer.h"
#include "Room/RoomManager.h"
#include "MapData/MapData.h"

#include<filesystem>

IOCPNetwork g_iocpNetwork;
MapData		g_bossMapData{ std::filesystem::current_path().string().append("\\\MapData\\\BossRoom.txt"),std::filesystem::current_path().string().append("\\\MapData\\\CollisionData.txt") };
DBObject	g_DBObj;
Timer		g_Timer;
RoomManager	g_RoomManager;
Logic		g_logic;

int main()
{	
	//g_bossMapData.SetFileName(std::filesystem::current_path().string().append("\\\MapData\\\BossRoom.txt"));
	//g_bossMapData.GetReadMapData();
	std::wcout.imbue(std::locale("KOREAN"));
	g_iocpNetwork.Start();
	g_iocpNetwork.Destroy();
}
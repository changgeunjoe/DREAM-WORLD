#include "stdafx.h"
#include "IOCPNetwork/IOCP/IOCPNetwork.h"
#include "Logic/Logic.h"
#include "DB/DBObject.h"
#include "Timer/Timer.h"
#include "Room/RoomManager.h"
#include "MapData/MapData.h"

IOCPNetwork g_iocpNetwork;
//astart collision monster
MapData		g_bossMapData{ std::filesystem::current_path().string().append("\\\MapData\\\BossRoom.txt"),std::filesystem::current_path().string().append("\\\MapData\\\BossCollisionData.txt"),std::filesystem::current_path().string().append("\\\MapData\\\MonsterBoss.txt") };
MapData		g_stage1MapData{ "NONE",std::filesystem::current_path().string().append("\\\MapData\\\Stage1CollisionData.txt"),std::filesystem::current_path().string().append("\\\MapData\\\MonsterStage1.txt") };
//MapData		g_bossPhase1MapData{ std::filesystem::current_path().string().append("\\\MapData\\\BossRoom.txt"),std::filesystem::current_path().string().append("\\\MapData\\\BossCollisionData.txt"),std::filesystem::current_path().string().append("as") };

DBObject	g_DBObj;
Timer		g_Timer;
RoomManager	g_RoomManager;
Logic		g_logic;

int main()
{
	std::wcout.imbue(std::locale("KOREAN"));
	g_iocpNetwork.Start();
	g_iocpNetwork.Destroy();
	TIMER_EVENT timeSnycEvent{ std::chrono::system_clock::now() + std::chrono::seconds(1), 0 , EV_SYNC_TIME };
	g_Timer.InsertTimerQueue(timeSnycEvent);
}

#include "stdafx.h"
#include "Network/IOCP/IOCP.h"
//#include "MapData/MapData.h"
//#include "Network/UserSession/UserSession.h"
//#include "Room/RoomManager.h"
#include "Timer/Timer.h"
#include "Network/UserSession/UserManager.h"
//astart collision monster
//MapData		g_bossMapData{ std::filesystem::current_path().string().append("\\\MapData\\\BossRoom.txt"),std::filesystem::current_path().string().append("\\\MapData\\\BossCollisionData.txt"),std::filesystem::current_path().string().append("\\\MapData\\\MonsterBoss.txt") };
//MapData		g_stage1MapData{ "NONE",std::filesystem::current_path().string().append("\\\MapData\\\Stage1CollisionData.txt"),std::filesystem::current_path().string().append("\\\MapData\\\MonsterStage1.txt") };

int main()
{
	std::wcout.imbue(std::locale("KOREAN"));
	//Trace – Debug – Info – Warning – Error – Critical
	StartLogger();
	Timer::GetInstance().StartTimer();
	UserManager::GetInstance().Initialize();
	IOCP iocp;
	iocp.Start();
	iocp.Destroy();
}

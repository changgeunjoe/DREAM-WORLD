#include "stdafx.h"
#include "Network/IOCP/Iocp.h"
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

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "wsaStartUp Error" << std::endl;
		WSACleanup();
		return -1;
	}

	Iocp iocp;
	//각 클래스에 iocp 객체 등록
	Timer::GetInstance().RegisterIocp(&iocp);
	UserManager::GetInstance().RegisterIocp(&iocp);


	UserManager::GetInstance().Initialize();
	Timer::GetInstance().StartTimer();

	iocp.StartWorkerThread();
}

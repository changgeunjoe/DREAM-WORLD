#include "stdafx.h"
#include "Network/IOCP/Iocp.h"
//#include "MapData/MapData.h"
//#include "Network/UserSession/UserSession.h"
//#include "Room/RoomManager.h"
#include "Timer/Timer.h"
#include "Network/UserSession/UserManager.h"
#include "DB/DB.h"
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
		spdlog::critical("wsaStartUp Error");
		WSACleanup();
		return -1;
	}

	//각 클래스에 iocp 객체 등록
	std::shared_ptr<IOCP::Iocp> iocpRef = std::make_shared<IOCP::Iocp>();
	TIMER::Timer::GetInstance().RegisterIocp(iocpRef->GetSharedPtr());
	UserManager::GetInstance().RegisterIocp(iocpRef->GetSharedPtr());
	DB::DBConnector::GetInstance().RegistIocp(iocpRef->GetSharedPtr());

	//각 클래스 시작
	UserManager::GetInstance().Initialize();
	TIMER::Timer::GetInstance().StartTimer();
	DB::DBConnector::GetInstance().Connect();

	iocpRef->Start();
	iocpRef->WorkerThreadJoin();
}

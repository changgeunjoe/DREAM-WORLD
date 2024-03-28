#include "stdafx.h"
#include "../ThreadManager/ThreadManager.h"
#include "Network/IOCP/Iocp.h"
#include "Timer/Timer.h"
#include "Network/UserSession/UserManager.h"
#include "Network/IocpEvent/IocpEventManager.h"
#include "DB/DB.h"
#include "Match/Matching.h"
#include "Room/RoomManager.h"
//astart collision monster

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
	IocpEventManager::GetInstance().Initailize();
	RoomManager::GetInstance().Initialize();

	//각 클래스 시작
	UserManager::GetInstance().Initialize();
	TIMER::Timer::GetInstance().StartTimer();
	DB::DBConnector::GetInstance().Connect();
	Matching::GetInstance().StartMatching();

	iocpRef->Start();

	ThreadManager::GetInstance().Join();
}

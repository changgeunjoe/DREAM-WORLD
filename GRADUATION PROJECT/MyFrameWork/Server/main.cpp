#include "stdafx.h"
#include "IOCPNetwork/IOCP/IOCPNetwork.h"
#include "Logic/Logic.h"
#include "DB/DBObject.h"
#include "Timer/Timer.h"
#include "Room/RoomManager.h"

IOCPNetwork g_iocpNetwork;
Logic		g_logic;
DBObject	g_DBObj;
Timer		g_Timer;
RoomManager	g_RoomManager;

int main()
{
	std::wcout.imbue(std::locale("KOREAN"));
	g_iocpNetwork.Start();
	g_iocpNetwork.Destroy();
}
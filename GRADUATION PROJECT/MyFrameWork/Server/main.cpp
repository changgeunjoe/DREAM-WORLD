#include "stdafx.h"
#include "IOCPNetwork/IOCP/IOCPNetwork.h"
#include "Logic/Logic.h"
#include "DB/DBObject.h"

IOCPNetwork g_iocpNetwork;
Logic		g_logic;
DBObject	g_DBObj;
int main()
{
	g_iocpNetwork.Start();
	g_iocpNetwork.Destroy();
}
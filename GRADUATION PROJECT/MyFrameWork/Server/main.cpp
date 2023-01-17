#include "stdafx.h"
#include "IOCPNetwork/IOCP/IOCPNetwork.h"
#include "Logic/Logic.h"

IOCPNetwork g_iocpNetwork;
Logic		g_logic;
int main()
{
	g_iocpNetwork.Start();
}
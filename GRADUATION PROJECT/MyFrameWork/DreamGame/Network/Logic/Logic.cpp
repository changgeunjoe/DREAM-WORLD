#include "Logic.h"
#include "../../../Server/IOCPNetwork/protocol/protocol.h"
void clientNet::Logic::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case CS_TEST_CHAT:
	{

	}
	break;

	default:
		break;
	}
}

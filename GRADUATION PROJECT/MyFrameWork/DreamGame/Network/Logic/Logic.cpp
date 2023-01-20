#include "Logic.h"
#include "../../../Server/IOCPNetwork/protocol/protocol.h"

void clientNet::Logic::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case SERVER_PACKET::MOVE:
	{
		SERVER_PACKET::MovePacket* recvPacket = reinterpret_cast<SERVER_PACKET::MovePacket*>(p);
		recvPacket->direction;
		recvPacket->size;
		recvPacket->type;
		recvPacket->userId;
		//TODO - Other Player Move
	}
	break;
	case SERVER_PACKET::ROTATE:
	{
		SERVER_PACKET::RotatePacket* recvPacket = reinterpret_cast<SERVER_PACKET::RotatePacket*>(p);
		recvPacket->axis;
		recvPacket->size;
		recvPacket->type;
		recvPacket->userId;
		//TODO - Other player Rotate
	}
	break;
	case SERVER_PACKET::STOP:
	{
		SERVER_PACKET::StopPacket* recvPacket = reinterpret_cast<SERVER_PACKET::StopPacket*>(p);
		recvPacket->size;
		recvPacket->type;
		recvPacket->userId;
		//TODO - Other Player Stop Move
	}
	break;
	default:
		break;
	}
}

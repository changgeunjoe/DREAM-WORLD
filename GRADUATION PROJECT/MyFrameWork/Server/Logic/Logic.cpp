#include "stdafx.h"
#include "Logic.h"
#include "../Session/Session.h"
#include "../Session/SessionObject/PlayerSessionObject.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../IOCPNetwork/protocol/protocol.h"

extern IOCPNetwork g_iocpNetwork;

Logic::Logic()
{
}

Logic::~Logic()
{
}

void Logic::AcceptPlayer(Session* session, int userId, SOCKET& sock)
{
	session->RegistPlayer(userId, sock);
}

void Logic::ProcessPacket(int userId, char* p)
{
	switch (p[1])
	{
	case CLIENT_PACKET::MOVE:
	{
		CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(p);

		SERVER_PACKET::MovePacket sendPacket;
		sendPacket.direction = recvPacket->direction;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::MOVE;
		sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
		MultiCastOtherPlayer(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::ROTATE:
	{
		CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::RotatePacket*>(p);

		SERVER_PACKET::RotatePacket sendPacket;
		sendPacket.angle = recvPacket->angle;
		sendPacket.axis = recvPacket->axis;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::ROTATE;
		sendPacket.size = sizeof(SERVER_PACKET::RotatePacket);
		MultiCastOtherPlayer(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::STOP:
	{
		CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::StopPacket*>(p);

		SERVER_PACKET::StopPacket sendPacket;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::ROTATE;
		sendPacket.size = sizeof(SERVER_PACKET::StopPacket);
		MultiCastOtherPlayer(userId, &sendPacket);
	}
	break;
	default:
		break;
	}
}

void Logic::BroadCastPacket(void* p)
{
	for (auto& cli : g_iocpNetwork.m_session) {
		if (cli.m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::MultiCastOtherPlayer(int userId, void* p)
{
	for (auto& cli : g_iocpNetwork.m_session) {
		if (cli.GetId() == userId) continue;//자기 자신을 제외한 플레이어들에게 전송
		if (cli.m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}


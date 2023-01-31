#include "stdafx.h"
#include "Logic.h"
#include "../Session/Session.h"
#include "../Session/SessionObject/PlayerSessionObject.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../IOCPNetwork/protocol/protocol.h"

extern IOCPNetwork g_iocpNetwork;

Logic::Logic()
{
	m_isRunningThread = true;
	m_PlayerMoveThread = std::thread{ [this]() {AutoMoveServer(); } };
}

Logic::~Logic()
{
	m_isRunningThread = false;
	if (m_PlayerMoveThread.joinable())
		m_PlayerMoveThread.join();}

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
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->StartMove(); // 움직임 start
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
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->StopMove();

		SERVER_PACKET::StopPacket sendPacket;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::ROTATE;
		sendPacket.size = sizeof(SERVER_PACKET::StopPacket);
		sendPacket.position = recvPacket->position;
		sendPacket.rotate = recvPacket->rotate;

		//bool adjustRes = pSessionObj->AdjustPlayerInfo(recvPacket->position, recvPacket->rotate);
		//if (!adjustRes) {
		//	sendPacket.position = pSessionObj->GetPosition();
		//	BroadCastPacket(&sendPacket);
		//}
		//else
		//	MultiCastOtherPlayer(userId, &sendPacket);
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
		if (cli.GetId() > MAX_USER) break;
		if (cli.m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::AutoMoveServer()
{
	while (m_isRunningThread)
	{
		if (g_iocpNetwork.GetCurrentId() == 0) continue;
		for (auto& cli : g_iocpNetwork.m_session) {
			if (cli.GetId() > MAX_USER) break;
			if (cli.GetPlayerState() == PLAYER_STATE::FREE) continue;
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
			if (cli.m_sessionObject->m_isMove) {
				pSessionObj->AutoMove();
			}
		}
	}
}

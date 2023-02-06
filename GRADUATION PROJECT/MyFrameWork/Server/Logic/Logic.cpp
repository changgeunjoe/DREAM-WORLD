#include "stdafx.h"
#include <chrono>
#include "Logic.h"
#include "../Session/Session.h"
#include "../Session/SessionObject/PlayerSessionObject.h"
#include "../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../DB/DBObject.h"

extern IOCPNetwork	g_iocpNetwork;
extern DBObject		g_DBObj;
Logic::Logic()
{
	m_isRunningThread = true;
	m_PlayerMoveThread = std::thread{ [this]() {AutoMoveServer(); } };
}

Logic::~Logic()
{
	m_isRunningThread = false;
	if (m_PlayerMoveThread.joinable())
		m_PlayerMoveThread.join();
}

void Logic::AcceptPlayer(Session* session, int userId, SOCKET& sock)
{
	session->RegistPlayer(userId, sock);
}

void Logic::ProcessPacket(int userId, char* p)
{
	switch (p[1])
	{
	case CLIENT_PACKET::MOVE_KEY_DOWN:
	{
		CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(p);

		SERVER_PACKET::MovePacket sendPacket;
		sendPacket.direction = recvPacket->direction;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::MOVE_KEY_DOWN;
		sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->StartMove(sendPacket.direction); // ������ start
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
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->Rotate(recvPacket->axis, recvPacket->angle);
		MultiCastOtherPlayer(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::MOVE_KEY_UP:
	{
		CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MovePacket*>(p);

		SERVER_PACKET::MovePacket sendPacket;
		sendPacket.direction = recvPacket->direction;
		sendPacket.userId = userId;
		sendPacket.type = SERVER_PACKET::MOVE_KEY_UP;
		sendPacket.size = sizeof(SERVER_PACKET::MovePacket);
		PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(g_iocpNetwork.m_session[userId].m_sessionObject);
		pSessionObj->ChangeDirection(sendPacket.direction); // ������ start
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

		bool adjustRes = pSessionObj->AdjustPlayerInfo(recvPacket->position, recvPacket->rotate);
		if (!adjustRes) {
			sendPacket.position = pSessionObj->GetPosition();
			BroadCastPacket(&sendPacket);
		}
		else
			MultiCastOtherPlayer(userId, &sendPacket);
	}
	break;
	case CLIENT_PACKET::LOGIN:
	{
		CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::LoginPacket*>(p);
		recvPacket->id;
		recvPacket->pw;

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
		if (cli.GetId() == userId) continue;//�ڱ� �ڽ��� ������ �÷��̾�鿡�� ����
		if (cli.GetId() > MAX_USER) break;
		if (cli.m_sessionCategory == PLAYER) {
			PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
			pSessionObj->Send(p);
		}
	}
}

void Logic::AutoMoveServer()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	while (m_isRunningThread)
	{
		if (g_iocpNetwork.GetCurrentId() == 0) continue;
		currentTime = std::chrono::high_resolution_clock::now();
		for (auto& cli : g_iocpNetwork.m_session) {
			if (cli.GetId() > MAX_USER) break;
			if (cli.GetPlayerState() == PLAYER_STATE::FREE) continue;
			if (cli.m_sessionObject->m_inputDirection != DIRECTION::IDLE) {
				PlayerSessionObject* pSessionObj = dynamic_cast<PlayerSessionObject*>(cli.m_sessionObject);
				pSessionObj->AutoMove();
			}
		}
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count() < 1000.0f / 60.0f) {
		}
	}
}

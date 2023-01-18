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
	/*case CS_TEST_CHAT:
	{
		CS_TEST_CHAT_PACKET* recvPacket = reinterpret_cast<CS_TEST_CHAT_PACKET*>(p);
		SC_TEST_CHAT_PACKET sendPacket;
		sendPacket.size = sizeof(SC_TEST_CHAT_PACKET);
		sendPacket.id = userId;
		sendPacket.type = SC_TEST_CHAT;
		memcpy(sendPacket.message, recvPacket->message, CHAT_SIZE);
		BroadCastPacket(&sendPacket);
	}
	break;*/

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


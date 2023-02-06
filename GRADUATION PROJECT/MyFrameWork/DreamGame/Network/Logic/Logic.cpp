#include "stdafx.h"
#include "Logic.h"
#include "../../../Server/IOCPNetwork/protocol/protocol.h"

clientNet::Logic::Logic()
{
	m_KeyInput = new CKeyInput();
}

void clientNet::Logic::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case SERVER_PACKET::MOVE_KEY_DOWN:
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
	case SERVER_PACKET::LOGIN_OK:
	{
		SERVER_PACKET::LoginPacket* recvPacket = reinterpret_cast<SERVER_PACKET::LoginPacket*>(p);
		wstring wst_name = recvPacket->name;
		m_inGamePlayerSession[0].m_id = myId = recvPacket->userID;
		m_inGamePlayerSession[0].SetName(wst_name);
		//m_inGamePlayerSession[0].m_currentPlayGameObject = 
		std::wcout << "user Name: " << wst_name << std::endl;
	}
	break;
	case SERVER_PACKET::ADD_PLAYER:
	{
		SERVER_PACKET::AddPlayerPacket* recvPacket = reinterpret_cast<SERVER_PACKET::AddPlayerPacket*>(p);
		for (auto& pSession : m_inGamePlayerSession) {
			if (-1 == pSession.m_id) {
				pSession.m_id = recvPacket->userId;
				pSession.SetName(recvPacket->name);
			}
		}
	}
	break;
	default:
		break;
	}
}

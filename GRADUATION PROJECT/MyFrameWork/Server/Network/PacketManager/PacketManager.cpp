#include "stdafx.h"
#include "PacketManager.h"
#include "../protocol/protocol.h"
#include "../../Room/RoomManager.h"

//User <=> Room

int PacketManager::ProccessPacket(const int& useId, const int& roomId, const unsigned long& ioByte, int remainSize, char* buffer)
{
	//switch (buffer[2])
	//{
	////case CLIENT_PACKET::MELEE_ATTACK:
	////{
	////	//CLIENT_PACKET::MeleeAttackPacket* recvPacket = reinterpret_cast<CLIENT_PACKET::MeleeAttackPacket*>(p);
	////	//³»ºÎ·Î »©¾ßµÊ
	////	//Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
	////	//bool attacking = roomRef.GetLeftAttackPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
	////	//DirectX::XMFLOAT3 pos = roomRef.GetPositionPlayCharacter(g_iocpNetwork.m_session[userId].GetRole());
	////	//if (roomRef.MeleeAttack(recvPacket->dir, pos))//ÀÌ°Å´Â 
	////	//	roomRef.m_bossDamagedQueue.push(g_iocpNetwork.m_session[userId].GetAttackDamagePlayCharacter());
	////}
	////break;
	//
	//
	//
	////case CLIENT_PACKET::TRIGGER_BOX_ON:
	////{
	////	int roomId = g_iocpNetwork.m_session[userId].GetRoomId();
	////	if (roomId != -1) {
	////		Room& roomRef = g_RoomManager.GetRunningRoomRef(roomId);
	////	}
	////	Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
	////	//roomRef.SetTriggerCntIncrease();
	////}
	////break;
	////case CLIENT_PACKET::TRIGGER_BOX_OUT:
	////{
	////	Room& roomRef = g_RoomManager.GetRunningRoomRef(g_iocpNetwork.m_session[userId].GetRoomId());
	////	//roomRef.SetTriggerCntDecrease();
	////}
	////break;
	//
	//default:
	//	//PrintCurrentTime();
	//	//std::cout << "unknown Packet" << std::endl;
	//	//std::cout << g_iocpNetwork.m_session[userId].GetUserAddrIn() << std::endl;
	//	//std::cout << p << std::endl;
	//	//g_iocpNetwork.DisconnectClient(userId);
	//	//break;
	//	break;
	//}
	return 0;
}


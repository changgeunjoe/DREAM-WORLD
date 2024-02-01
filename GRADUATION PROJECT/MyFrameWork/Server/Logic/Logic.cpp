#include "stdafx.h"
#include "Logic.h"
#include "../Network/UserSession/UserSession.h"
#include "../Room/RoomManager.h"
#include "../Network/PacketManager/PacketManager.h"


void Logic::CharacterAddDirection(int userId, const DIRECTION& applyDirection)
{
	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;

	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;
	//
	//ROLE role = g_userSession[userId].GetRole();
	//g_roomManager.GetRunningRoomRef(roomId).ChracterAddDirection(userId, role, applyDirection);
	//multicast

}

void Logic::CharacterRemoveDirection(int userId, const DIRECTION& applyDirection)
{
	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;

	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;

	//ROLE role = g_userSession[userId].GetRole();
	//g_roomManager.GetRunningRoomRef(roomId).ChracterRemoveDirection(userId, role, applyDirection);
}

void Logic::CharacterStop(int userId)
{
	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;

	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;

	//ROLE role = g_userSession[userId].GetRole();
	//g_roomManager.GetRunningRoomRef(roomId).ChracterStop(std::move(role));
	/*SERVER_PACKET::StopPacket sendPacket;
	sendPacket.role = g_userSession[userId].GetRole();
	sendPacket.type = SERVER_PACKET::STOP;
	sendPacket.size = sizeof(SERVER_PACKET::StopPacket);*/
	//broadcast
}

void Logic::CharacterRotate(int userId, const ROTATE_AXIS& axis, const float& angle)
{
	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;
	//
	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;
	//
	//ROLE role = g_userSession[userId].GetRole();
	//g_roomManager.GetRunningRoomRef(roomId).CharacterRotate(role, axis, angle);


	/*SERVER_PACKET::RotatePacket sendPacket;
	sendPacket.angle = angle;
	sendPacket.axis = axis;
	sendPacket.role = g_userSession[userId].GetRole();
	sendPacket.type = SERVER_PACKET::ROTATE;
	sendPacket.size = sizeof(SERVER_PACKET::RotatePacket);*/

	//multicast
}

void Logic::CharacterInput(int userId, bool leftClick, bool rightClick)
{
	/*SERVER_PACKET::MouseInputPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::MouseInputPacket);
	sendPacket.type = SERVER_PACKET::MOUSE_INPUT;
	sendPacket.LClickedButton = leftClick;
	sendPacket.RClickedButton = rightClick;*/
	//sendPacket.role = g_iocpNetwork.m_session[userId].GetRole();

	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;
	//
	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;
	//
	//ROLE role = g_userSession[userId].GetRole();
	//g_roomManager.GetRunningRoomRef(roomId).CharacterMouseInput(role, leftClick, rightClick);
	//multicast

}

void Logic::CharacterInputSkiil_Q(int userId, const XMFLOAT3& float3)
{
	//애니메이션 재생할거니 다른 클라에 broadCast하라임
}

void Logic::CharacterInputSkiil_E(int userId, const XMFLOAT3& float3)
{
	//애니메이션 재생할거니 다른 클라에 broadCast하라임
}

void Logic::CharacterSkillExecute_Q(int userId)
{
}

void Logic::CharacterSkillExecute_E(int userId)
{
}

void Logic::CharacterCommonAttack(int userId)
{
	//그냥 애니메이션 재생한다 전송
}

void Logic::CharacterCommonAttackExecute(int userId, const XMFLOAT3& direction, int power)
{
	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;
	//
	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;
	//
	//ROLE role = g_userSession[userId].GetRole();
	//g_roomManager.GetRunningRoomRef(roomId).CharacterCommonAttackExecute(role, direction, power);
}

void Logic::ExitPlayerInRoom(int userId)
{
}

void Logic::SkipNPC_Communication(int userId)
{
	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;
	//
	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;
	//
	//ROLE role = g_userSession[userId].GetRole();
	//g_roomManager.GetRunningRoomRef(roomId).SkipNPC_Communication();
}

void Logic::ForceGameEnd(int userId)
{
	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;
	//
	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;
	//g_roomManager.GetRunningRoomRef(roomId).GameEnd();
}

void Logic::ForceChangeToBossState(int userId)
{
	//if (g_userSession[userId].GetPlayerState() != PLAYER_STATE::IN_GAME) return;
	//
	//int roomId = g_userSession[userId].GetRoomId();
	//if (roomId == -1)return;
	//g_roomManager.GetRunningRoomRef(roomId).ChangeToBossStage();
}

void Logic::TimeSyncRequest(int userId)
{
}


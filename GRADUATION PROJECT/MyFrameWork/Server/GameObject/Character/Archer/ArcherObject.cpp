#include "stdafx.h"
#include "ArcherObject.h"

ArcherObject::ArcherObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:RangedCharacterObject(maxHp, moveSpeed, boundingSize, 80.0f, roomRef, ROLE::ARCHER)
{
	m_skillCtrl->InsertCoolDownEventData(SKILL_Q, EventController::MS(10'000));
	m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(15'000));
	/*
	m_maxHp = m_hp = 400;
	m_attackDamage = 120;
	*/
}

void ArcherObject::SetStagePosition(const ROOM_STATE& roomState)
{
	if (ROOM_STATE::ROOM_COMMON == roomState)
		SetPosition(XMFLOAT3(-1340.84f, 0, -1520.93f));
	else SetPosition(XMFLOAT3(123, 0, -293));
	//SetPosition(XMFLOAT3(-193, 0, -52));
}

void ArcherObject::RecvSkill(const SKILL_TYPE&)
{
}

void ArcherObject::RecvSkill(const SKILL_TYPE&, const XMFLOAT3& vector3)
{
}

void ArcherObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	/*
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	//offset ShootArrow 내부에서 적용함

	if (power == 0) {
		roomRef.ShootArrow(attackDir, m_position, 100.0f, 80.0f);
	}
	else if (power == 1) {
		roomRef.ShootArrow(attackDir, m_position, 140.0f, 115.0f);
	}
	else if (power == 2) {
		roomRef.ShootArrow(attackDir, m_position, 200.0f, 140.0f);
	}

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_ARROW;
	sendPacket.dir = attackDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
	*/
}

void ArcherObject::ExecuteTripleArrow(const XMFLOAT3& direction)
{
}

void ArcherObject::ExecuteRainArrow(const XMFLOAT3& position)
{
}

void ArcherObject::ExecuteCommonAttack(const XMFLOAT3& direction, const int& power)
{
}

/*
void ArcherObject::RecvSkill_1(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_Q);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//탱커 쉴드랑 같은 방식으로 동작. 타이머에서 n초있다가 적용만 아님.
	}
	//3갈래 발사
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteThreeArrow(posOrDir, m_position);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_ARROW;
	sendPacket.dir = posOrDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
}

void ArcherObject::RecvSkill_2(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_E);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//탱커 쉴드랑 같은 방식으로 동작. 타이머에서 n초있다가 적용만 아님.
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.StartSkyArrow(posOrDir);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_ARROW;
	sendPacket.dir = posOrDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::ARCHER, &sendPacket);
}
*/

void ArcherSKill::TripleArrow::Execute()
{
}

void ArcherSKill::SkyArrow::Execute()
{
}

void ArcherSKill::CommonAttack::Execute()
{
}

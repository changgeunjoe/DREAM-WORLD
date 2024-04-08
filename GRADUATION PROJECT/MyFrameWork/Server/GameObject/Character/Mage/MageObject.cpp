#include "stdafx.h"
#include "MageObject.h"

MageObject::MageObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:RangedCharacterObject(maxHp, moveSpeed, boundingSize, 80.0f, roomRef, ROLE::MAGE)
{
	m_skillCtrl->InsertDurationEventData(SKILL_Q, EventController::MS(15'000), EventController::MS(10'000));
	m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(10'000));
	/*
	m_maxHp = m_hp = 500;
	m_attackDamage = 50;
	*/
}

void MageObject::SetStagePosition(const ROOM_STATE& roomState)
{
	if (ROOM_STATE::ROOM_COMMON == roomState)
		SetPosition(XMFLOAT3(-1370.45f, 0, -1450.89f));
	else SetPosition(XMFLOAT3(20, 0, -285));
	//SetPosition(XMFLOAT3(-77.7, 0, -164.7));
}

void MageObject::RecvSkill(const SKILL_TYPE&)
{
}

void MageObject::RecvSkill(const SKILL_TYPE&, const XMFLOAT3& vector3)
{
}

void MageObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	//room에 있는 볼 날라기인데, 이거 바꿔야 할거같은데...
	/*
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	//offset 적용 안됨
	roomRef.ShootBall(attackDir, m_position);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::SHOOTING_BALL;
	sendPacket.dir = attackDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::PRIEST, &sendPacket);
	*/
}

void MageObject::ExecuteThunderSkill1(const XMFLOAT3& position)
{
}

void MageObject::ExecuteHeal()
{
}

void MageObject::ExecuteCommonAttack(const XMFLOAT3& attackDir)
{
}

/*
void MageObject::RecvSkill_1(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_Q);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//탱커 쉴드랑 같은 방식으로 동작. 타이머에서 n초있다가 적용만 아님.
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	g_RoomManager.GetRunningRoomRef(m_roomId).StartHealPlayerCharacter();
}

void MageObject::RecvSkill_2(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_E);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//탱커 쉴드랑 같은 방식으로 동작. 타이머에서 n초있다가 적용만 아님.
	}
	//번개 나오는거.
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteMageThunder(posOrDir);

	SERVER_PACKET::ShootingObject sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::ShootingObject);
	sendPacket.type = SERVER_PACKET::EXECUTE_LIGHTNING;
	sendPacket.dir = posOrDir;
	g_logic.MultiCastOtherPlayerInRoom_R(m_roomId, ROLE::PRIEST, &sendPacket);
}
*/

void MageSkill::ThunderSkill::Execute()
{
}

void MageSkill::HealSkill::Execute()
{
}

void MageSkill::CommonAttack::Execute()
{
}

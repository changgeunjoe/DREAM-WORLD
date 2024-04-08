#include "stdafx.h"
#include "TankerObject.h"
#include "../Room/Room.h"
#include "../Room/RoomEvent.h"

TankerObject::TankerObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:MeleeCharacterObject(maxHp, moveSpeed, boundingSize, 50.0f, roomRef, ROLE::TANKER)
{
	m_skillCtrl->InsertDurationEventData(SKILL_Q, EventController::MS(15'000), EventController::MS(7'000));
	m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(10'000));
	/*
	m_maxHp = m_hp = 780;
	m_attackDamage = 80;
	*/
}

void TankerObject::SetStagePosition(const ROOM_STATE& roomState)
{
	if (ROOM_STATE::ROOM_COMMON == roomState)
		SetPosition(XMFLOAT3(-1260.3f, 0, -1510.7f));
	else SetPosition(XMFLOAT3(82, 0, -223.0f));
	//	SetPosition(XMFLOAT3(106.8, 0, -157));
}

void TankerObject::RecvSkill(const SKILL_TYPE& type)
{
	if (SKILL_TYPE::SKILL_TYPE_Q) {
		//std::make_shared<TankerSkill::ShieldSkill>();
	}
	else spdlog::critical("TankerObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_E");
}

void TankerObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3)
{
	if (SKILL_TYPE::SKILL_TYPE_E) {

	}
	else spdlog::critical("TankerObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_Q");
}

void TankerObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power)
{
	auto tankerCommonAttackSkill = std::make_shared<TankerSkill::CommonAttack>(std::static_pointer_cast<TankerObject>(shared_from_this()), attackDir);
	//m_roomRef->InsertPrevUpdateEvent(std::make_shared<PlayerSkillEvent>(tankerCommonAttackSkill));
}

void TankerObject::ExecuteHammerSkill(const XMFLOAT3& direction)
{
}

void TankerObject::ExecuteShield()
{
}

void TankerObject::ExecuteCommonAttack(const XMFLOAT3& attackDir)
{
}

/*

void TankerObject::RecvSkill_1(const XMFLOAT3& vec3)
{
	//쉴드
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_Q);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//notiPacket보내서 탱커 쉴드 애니메이션 수행.
		//근데 지속 시간 어떻게 넘기지?
		//Timer에 2.4초 뒤에 쉴드 적용하고 브로드 캐스트해야 됨.
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[0]);
	if (m_skillCoolTime[0] > durationTime)	return;
	m_prevSkillInputTime[0] = currentTime;
	TIMER_EVENT gameStateEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(2400), m_roomId ,EV_TANKER_SHIELD_START };	//애니메이션 진행 시간 1.4초 + 공 날아가는 시간 1초
	g_Timer.InsertTimerQueue(gameStateEvent);
	SERVER_PACKET::NotifyPacket sendPacket;
	sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
	sendPacket.type = SERVER_PACKET::SHIELD_START;
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);
}

void TankerObject::RecvSkill_2(const XMFLOAT3& vec3)
{
	auto skillCoolData = m_skillCtrl->GetEventData(SKILL_E);
	const bool isAble = skillCoolData->IsAbleExecute();
	if (isAble) {
		//executeHammerAttack인데. 뭐 알아서 바꾸자.
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_prevSkillInputTime[1]);
	if (m_skillCoolTime[1] > durationTime)	return;
	m_prevSkillInputTime[1] = currentTime;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	roomRef.ExecuteHammerAttack(posOrDir, m_position);
}
*/

void TankerSkill::ThunderHammerSkill::Execute()
{
}

void TankerSkill::ShieldSkill::Execute()
{
}

void TankerSkill::CommonAttack::Execute()
{
}

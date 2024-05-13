#include "stdafx.h"
#include "CharacterObject.h"
#include "../CoolTimeEventCtrl/EventController.h"
#include "../CoolTimeEventCtrl/CoolDownEventBase.h"
#include "../../../../Server/Network/protocol/protocol.h"

WarriorObject::WarriorObject()
{
	m_coolTimeCtrl = std::make_shared<EventController>();
	m_coolTimeCtrl->InsertCoolDownEventData(SKILL_Q, EventController::MS(7'100));
	m_coolTimeCtrl->InsertCoolDownEventData(ATTACK, EventController::MS(2'100));
}

std::shared_ptr<PacketHeader> WarriorObject::GetAttackPacket()
{
	std::shared_ptr<PacketHeader> sendPacket = nullptr;
	auto attackCoolTime = m_coolTimeCtrl->GetEventData(ATTACK);
	auto skillCoolTime = m_coolTimeCtrl->GetEventData(SKILL_Q);

	if (attackCoolTime->IsAbleExecute()) {
		//attack: look, power => PlayerPowerAttackPacket
		sendPacket = std::make_shared<CLIENT_PACKET::PlayerPowerAttackPacket>(XMFLOAT3(0, 0, 1), 0);
	}
	else if (skillCoolTime->IsAbleExecute()) {
		//skill: look => float Data Skill Packet
		sendPacket = std::make_shared<CLIENT_PACKET::FloatDataSkillPacket>(static_cast<char>(CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_Q), XMFLOAT3(0, 0, 1));
	}
	return sendPacket;
}

TankerObject::TankerObject()
{
	m_coolTimeCtrl = std::make_shared<EventController>();
	m_coolTimeCtrl->InsertDurationEventData(SKILL_Q, EventController::MS(15'100), EventController::MS(7'100));
	m_coolTimeCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(10'100));
	m_coolTimeCtrl->InsertCoolDownEventData(ATTACK, EventController::MS(2'100));
}

std::shared_ptr<PacketHeader> TankerObject::GetAttackPacket()
{
	std::shared_ptr<PacketHeader> sendPacket = nullptr;
	auto attackCoolTime = m_coolTimeCtrl->GetEventData(ATTACK);
	auto skillCoolTime_Q = m_coolTimeCtrl->GetEventData(SKILL_Q);
	auto skillCoolTime_E = m_coolTimeCtrl->GetEventData(SKILL_E);

	if (attackCoolTime->IsAbleExecute()) {
		//attack look => PlayerCommonAttackPacket
		sendPacket = std::make_shared<CLIENT_PACKET::PlayerCommonAttackPacket>(XMFLOAT3(0, 0, 1));
	}
	else if (skillCoolTime_Q->IsAbleExecute()) {
		//skillQ: notify -> CommonSkillPacket
		sendPacket = std::make_shared<CLIENT_PACKET::CommonSkillPacket>(static_cast<char>(CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q));
	}
	else if (skillCoolTime_Q->IsAbleExecute()) {
		//Skill E: look -> FloatDataSkillPacket
		sendPacket = std::make_shared<CLIENT_PACKET::FloatDataSkillPacket>(static_cast<char>(CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_E), XMFLOAT3(0, 0, 1));
	}
	return sendPacket;
}

MageObject::MageObject()
{
	m_coolTimeCtrl = std::make_shared<EventController>();
	m_coolTimeCtrl->InsertDurationEventData(SKILL_Q, EventController::MS(15'100), EventController::MS(10'100));
	m_coolTimeCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(10'100));
	m_coolTimeCtrl->InsertCoolDownEventData(ATTACK, EventController::MS(2'100));
}

std::shared_ptr<PacketHeader> MageObject::GetAttackPacket()
{	
	std::shared_ptr<PacketHeader> sendPacket = nullptr;
	auto attackCoolTime = m_coolTimeCtrl->GetEventData(ATTACK);
	auto skillCoolTime_Q = m_coolTimeCtrl->GetEventData(SKILL_Q);
	auto skillCoolTime_E = m_coolTimeCtrl->GetEventData(SKILL_E);

	if (attackCoolTime->IsAbleExecute()) {
		//attack look => PlayerCommonAttackPacket
		sendPacket = std::make_shared<CLIENT_PACKET::PlayerCommonAttackPacket>(XMFLOAT3(0, 0, 1));
	}
	else if (skillCoolTime_Q->IsAbleExecute()) {
		//skill Q: notify => CommonSkillPacket
		sendPacket = std::make_shared<CLIENT_PACKET::CommonSkillPacket>(static_cast<char>(CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q));
	}
	else if (skillCoolTime_Q->IsAbleExecute()) {
		//skill e: position => FloatDataSkillPacket
		sendPacket = std::make_shared<CLIENT_PACKET::FloatDataSkillPacket>(static_cast<char>(CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_E), XMFLOAT3(0, 0, 1));
	}
	return sendPacket;
}

ArcherObject::ArcherObject()
{
	m_coolTimeCtrl = std::make_shared<EventController>();
	m_coolTimeCtrl->InsertCoolDownEventData(SKILL_Q, EventController::MS(10'100));
	m_coolTimeCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(15'100));
	m_coolTimeCtrl->InsertCoolDownEventData(ATTACK, EventController::MS(2'100));
}

std::shared_ptr<PacketHeader> ArcherObject::GetAttackPacket()
{
	std::shared_ptr<PacketHeader> sendPacket = nullptr;
	auto attackCoolTime = m_coolTimeCtrl->GetEventData(ATTACK);
	auto skillCoolTime_Q = m_coolTimeCtrl->GetEventData(SKILL_Q);
	auto skillCoolTime_E = m_coolTimeCtrl->GetEventData(SKILL_E);

	if (attackCoolTime->IsAbleExecute()) {
		//attack: SendPowerAttackExecute
		sendPacket = std::make_shared<CLIENT_PACKET::PlayerPowerAttackPacket>(XMFLOAT3(0, 0, 1), 0);
	}
	else if (skillCoolTime_Q->IsAbleExecute()) {
		//skillQ: look -> FloatDataSkillPacket
		sendPacket = std::make_shared<CLIENT_PACKET::FloatDataSkillPacket>(static_cast<char>(CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_Q), XMFLOAT3(0, 0, 1));
	}
	else if (skillCoolTime_Q->IsAbleExecute()) {
		//Skill E: position -> FloatDataSkillPacket
		sendPacket = std::make_shared<CLIENT_PACKET::FloatDataSkillPacket>(static_cast<char>(CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_E), XMFLOAT3(0, 0, 1));
	}
	return sendPacket;
}

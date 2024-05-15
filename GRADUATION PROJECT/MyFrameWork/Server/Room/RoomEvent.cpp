#include "stdafx.h"
#include "RoomEvent.h"
#include "../Network/protocol/protocol.h"
#include "../Room/Room.h"
#include "../GameObject/Character/PlayerSkill.h"

std::shared_ptr<PacketHeader> PlayerDamagedEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::PlayerDamagedPacket>(role, hp, shield);
}

std::shared_ptr<PacketHeader> PlayerDieEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::PlayerDiePacket>(role);
}

std::shared_ptr<PacketHeader> SmallMonsterDamagedEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::SmallMonsterDamagedPacket>(m_id, hp);
}

std::shared_ptr<PacketHeader> SmallMonsterMoveEvent::GetPacketHeader()
{
	if (EventType::SMALL_MONSTER_STOP == eventType) {
		return std::make_shared<SERVER_PACKET::SmallMonsterPacket>(m_id, static_cast<char>(SERVER_PACKET::TYPE::SMALL_MONSTER_STOP));
	}
	//if (EventType::SMALL_MONSTER_MOVE == eventType) {
	return std::make_shared<SERVER_PACKET::SmallMonsterPacket>(m_id, static_cast<char>(SERVER_PACKET::TYPE::SMALL_MONSTER_MOVE));
	//}
}

std::shared_ptr<PacketHeader> SmallMonsterAttakEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::SmallMonsterAttackPacket>(m_id);
}

std::shared_ptr<PacketHeader> SmallMonsterDestinationEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::SmallMonsterDestinationPacket>(m_id, m_destinationPosition);
}

std::shared_ptr<PacketHeader> SmallMonsterDieEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::SmallMonsterPacket>(m_id, static_cast<char>(SERVER_PACKET::TYPE::SMALL_MONSTER_DIE));
}

PlayerSkillEvent::PlayerSkillEvent(std::shared_ptr<PlayerSkillBase> playerSkillRef)
	: PrevUpdateEvent(), playerSkillRef(playerSkillRef)
{
}

void PlayerSkillEvent::ProcessEvent()
{
	playerSkillRef->Execute();
}

std::shared_ptr<PacketHeader> ShootingArrowEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::ShootingObject>(static_cast<char>(SERVER_PACKET::TYPE::SHOOTING_ARROW), direction);
}

std::shared_ptr<PacketHeader> ShootingIceLaneEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::ShootingObject>(static_cast<char>(SERVER_PACKET::TYPE::SHOOTING_ICE_LANCE), direction);
}

std::shared_ptr<PacketHeader> ThunderEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::ShootingObject>(static_cast<char>(SERVER_PACKET::TYPE::EXECUTE_LIGHTNING), position);
}

std::shared_ptr<PacketHeader> BossMoveDestinationEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::BossMoveDestnationPacket>(destinationPosition);
}

std::shared_ptr<PacketHeader> BossSameNodeEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::BossOnSameNodePacket>(destinationPosition);
}

//std::shared_ptr<PacketHeader> BossStopEvent::GetPacketHeader()
//{
//	return std::make_shared<SERVER_PACKET::BossStopPacket>();
//}
//
//std::shared_ptr<PacketHeader> BossRotateEvent::GetPacketHeader()
//{
//	return std::make_shared<SERVER_PACKET::BossRotatePacket>(angle);
//}

std::shared_ptr<PacketHeader> BossMeteorEvent::GetPacketHeader()
{
	return nullptr;
	// return std::make_shared<SERVER_PACKET::BossAttackMeteorPacket>(BOSS_ATTACK::ATTACK_METEO);
}

std::shared_ptr<PacketHeader> BossFireEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::BossAttackPacket>(SERVER_PACKET::BOSS_ATTACK::ATTACK_FIRE);
}

std::shared_ptr<PacketHeader> BossSpinEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::BossAttackPacket>(SERVER_PACKET::BOSS_ATTACK::ATTACK_SPIN);
}

std::shared_ptr<PacketHeader> BossKickEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::BossDirectionAttackPacket>(SERVER_PACKET::BOSS_ATTACK::ATTACK_KICK, directionVector);
}

std::shared_ptr<PacketHeader> BossPunchEvent::GetPacketHeader()
{
	return std::make_shared<SERVER_PACKET::BossDirectionAttackPacket>(SERVER_PACKET::BOSS_ATTACK::ATTACK_PUNCH, directionVector);
}

void BossRoadSetEvent::ProcessEvent()
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;
	roomRef->SetBossRoad(m_road);
}

void BossAggroSetEvent::ProcessEvent()
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;
	roomRef->SetBossAggro(m_road, m_aggroCharacter);
}

void ChangeBossStageEvent::ProcessEvent()
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;
	roomRef->SetBossStage();
}

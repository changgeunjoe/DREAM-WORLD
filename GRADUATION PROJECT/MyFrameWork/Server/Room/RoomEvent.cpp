#include "stdafx.h"
#include "RoomEvent.h"
#include "../Network/protocol/protocol.h"
#include "../Room/Room.h"
#include "../GameObject/Character/PlayerSkill.h"

std::shared_ptr<PacketHeader> PlayerDamagedEvent::GetPacketHeader()
{
	return std::shared_ptr<PacketHeader>();
}

std::shared_ptr<PacketHeader> PlayerDieEvent::GetPacketHeader()
{
	return std::shared_ptr<PacketHeader>();
}

std::shared_ptr<PacketHeader> SmallMonsterDamagedEvent::GetPacketHeader()
{
	return std::shared_ptr<PacketHeader>();
}

std::shared_ptr<PacketHeader> SmallMonsterMoveEvent::GetPacketHeader()
{
	return std::shared_ptr<PacketHeader>();
}

std::shared_ptr<PacketHeader> SmallMonsterAttakEvent::GetPacketHeader()
{
	return std::shared_ptr<PacketHeader>();
}

std::shared_ptr<PacketHeader> SmallMonsterDestinationEvent::GetPacketHeader()
{
	return std::shared_ptr<PacketHeader>();
}

std::shared_ptr<PacketHeader> SmallMonsterDieEvent::GetPacketHeader()
{
	return std::shared_ptr<PacketHeader>();
}

PlayerSkillEvent::PlayerSkillEvent(std::shared_ptr<PlayerSkillBase> playerSkillRef)
	: PrevUpdateEvent(), playerSkillRef(playerSkillRef)
{
}

void PlayerSkillEvent::ProcessEvent()
{
	playerSkillRef->Execute();
}

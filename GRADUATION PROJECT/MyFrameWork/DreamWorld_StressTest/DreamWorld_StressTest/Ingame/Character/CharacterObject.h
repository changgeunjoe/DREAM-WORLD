#pragma once
#include "../../PCH/stdafx.h"

class EventController;
struct PacketHeader;

class CharacterObject : std::enable_shared_from_this<CharacterObject>
{
protected:
	static constexpr std::string_view ATTACK = "ATTACK";
	static constexpr std::string_view SKILL_Q = "SKILL_Q";
	static constexpr std::string_view SKILL_E = "SKILL_E";

public:
	virtual std::shared_ptr<PacketHeader> GetAttackPacket() = 0;

protected:
	std::shared_ptr<EventController> m_coolTimeCtrl;
};

class WarriorObject : public CharacterObject
{
public:
	WarriorObject();

	virtual std::shared_ptr<PacketHeader> GetAttackPacket() override;
};

class TankerObject : public CharacterObject
{
public:
	TankerObject();
	virtual std::shared_ptr<PacketHeader> GetAttackPacket() override;
};

class MageObject : public CharacterObject
{
public:
	MageObject();
	virtual std::shared_ptr<PacketHeader> GetAttackPacket() override;
};

class ArcherObject : public CharacterObject
{
public:
	ArcherObject();
	virtual std::shared_ptr<PacketHeader> GetAttackPacket() override;
};

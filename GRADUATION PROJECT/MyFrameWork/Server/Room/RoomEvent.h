#pragma once
#include "../PCH/stdafx.h"

class Room;
class CharacterObject;
class PlayerSkillBase;
struct PacketHeader;

/*
	NPC가 Update()이후에 전송할 데이터의 인터페이스 클래스
	//플레이어 피 상태 변화
	//몬스터 피 상태 변화
	//행동 변화
*/
class RoomSendEvent : public std::enable_shared_from_this<RoomSendEvent>
{
public:
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() = 0;
};

class PlayerSendEvent : public RoomSendEvent
{
public:
	PlayerSendEvent() = delete;
	PlayerSendEvent(const ROLE& role) :RoomSendEvent(), role(role) {}
protected:
	ROLE role;
};

//몬스터가 공격하여 Player hp 깍일 때.
class PlayerDamagedEvent : public PlayerSendEvent
{
public:
	PlayerDamagedEvent() = delete;
	PlayerDamagedEvent(const ROLE& role, const float& hp, const float& shield) : PlayerSendEvent(role), hp(hp), shield(shield) {}

	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
private:
	float hp;
	float shield;
};

class PlayerDieEvent : public PlayerSendEvent
{
public:
	PlayerDieEvent(const ROLE& role) : PlayerSendEvent(role) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
};

/*
	SmallMonster에 대한 sendEvent, id만 가짐.
*/
class SmallMonsterEvent : public RoomSendEvent
{
public:
	SmallMonsterEvent(const int& id)
		: RoomSendEvent(), m_id(id) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() = 0;
protected:
	int m_id;
};

class SmallMonsterDamagedEvent : public SmallMonsterEvent
{
public:
	SmallMonsterDamagedEvent(const int& id, const float& hp) : SmallMonsterEvent(id), hp(hp) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
private:
	float hp;
};

class SmallMonsterDieEvent : public SmallMonsterEvent
{
public:
	SmallMonsterDieEvent(const int& id) : SmallMonsterEvent(id) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
};

class SmallMonsterMoveEvent : public SmallMonsterEvent
{
public:
	enum class EventType : char
	{
		SMALL_MONSTER_STOP,
		SMALL_MONSTER_MOVE,
	};
	SmallMonsterMoveEvent(const EventType& eventType, const int& id) : SmallMonsterEvent(id), eventType(eventType) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
private:
	EventType eventType;
};

class SmallMonsterAttakEvent : public SmallMonsterEvent
{
public:
	SmallMonsterAttakEvent(const int& id) : SmallMonsterEvent(id) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
};

class SmallMonsterDestinationEvent : public SmallMonsterEvent
{
public:
	SmallMonsterDestinationEvent(const int& id, const XMFLOAT3& desPosition = XMFLOAT3(0, 0, 0))
		: SmallMonsterEvent(id), m_destinationPosition(desPosition) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
protected:
	XMFLOAT3 m_destinationPosition;
};

class ShootingArrowEvent : public RoomSendEvent
{
public:
	ShootingArrowEvent(const XMFLOAT3& direction) : direction(direction) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
private:
	XMFLOAT3 direction;
};

class ShootingIceLaneEvent : public RoomSendEvent
{
public:
	ShootingIceLaneEvent(const XMFLOAT3& direction) : direction(direction) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
private:
	XMFLOAT3 direction;
};

class ThunderEvent : public RoomSendEvent
{
public:
	ThunderEvent(const XMFLOAT3& position) : position(position) {}
	virtual std::shared_ptr<PacketHeader> GetPacketHeader() override;
private:
	XMFLOAT3 position;
};

/*
	Update에서 위치 업데이트 이전에 처리할 데이터
	플레이어의 공격 패킷을 받았을 때, 저장할 클래스
*/

class PrevUpdateEvent : std::enable_shared_from_this<PrevUpdateEvent>
{
public:
	virtual void ProcessEvent() = 0;
};

class PlayerSkillEvent : public PrevUpdateEvent
{
public:
	PlayerSkillEvent(std::shared_ptr<PlayerSkillBase> playerSkillRef);
	virtual void ProcessEvent() override;
protected:
	std::shared_ptr<PlayerSkillBase> playerSkillRef;
};

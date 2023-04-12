#pragma once
#include "SessionObject.h"
#include "../../Room/Room.h"
#include "../../PCH/stdafx.h"

static std::random_device rd;
static std::default_random_engine dre(rd());
static std::uniform_int_distribution<> randDir(0, 4);

class MonsterSessionObject : public SessionObject
{
public:
	MonsterSessionObject(Session* session);
	MonsterSessionObject(Session* session, std::string& roomId);
	~MonsterSessionObject();

private:
	std::string m_roomId;
	DirectX::XMFLOAT3 m_DestinationPos = { 0,0,0 };
	DirectX::XMFLOAT3 m_RestRotateAngle = { 0,0,0 };
public:
	bool isMove = false;
public:
	virtual void Recv() override;
public:
	virtual void AutoMove() override;
	virtual void StartMove(DIRECTION d) override;
	virtual void StopMove() override;
	virtual void ChangeDirection(DIRECTION d) override;
public:
	virtual const DirectX::XMFLOAT3 GetPosition() override;
	virtual const DirectX::XMFLOAT3 GetRotation() override;
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle) override;
public:
	void SetDirection(DIRECTION d);
	void Move(float fDistance);
public:
	const std::string& GetRoomId() { return m_roomId; }
public:
	void SetDestinationPos(DirectX::XMFLOAT3 des) {
		isMove = true;
		m_DestinationPos = des;
	}
};

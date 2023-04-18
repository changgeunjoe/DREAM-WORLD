#pragma once
#include "SessionObject.h"
#include "../../PCH/stdafx.h"

class Session;
class ShootingSessionObject : public SessionObject
{
public:
	ShootingSessionObject(Session* session);
	ShootingSessionObject(Session* session, std::string& roomId);
	~ShootingSessionObject();
private:
	std::string m_roomId;
	float m_speed;
public:
	virtual void AutoMove();
	virtual void StartMove(DIRECTION d);
	virtual void StopMove();
	virtual void ChangeDirection(DIRECTION d);
	virtual void Rotate(ROTATE_AXIS axis, float angle);
public:
	virtual const DirectX::XMFLOAT3 GetPosition();
	virtual const DirectX::XMFLOAT3 GetRotation();
	virtual void Recv();
	void SetSpeed(float speed);
	void SetStart(XMFLOAT3& dir, XMFLOAT3& srcPos, float speed);
	void Move(float distance);
};


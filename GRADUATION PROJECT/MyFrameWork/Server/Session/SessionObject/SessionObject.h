#pragma once

class Session;
class SessionObject
{
protected:
	Session* m_session;
protected:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotateAngle;
	DirectX::XMFLOAT3 m_directionVector = DirectX::XMFLOAT3{ 0,0,1 };

	std::chrono::high_resolution_clock::time_point m_lastMoveTime;
public:
	bool m_isMove = false;
public:
	SessionObject(Session* session);
	virtual ~SessionObject();
public:
	virtual void AutoMove();
	virtual void StartMove();
	virtual void StopMove();
public:
	virtual const DirectX::XMFLOAT3 GetPosition();
	virtual const DirectX::XMFLOAT3 GetRotation();
};

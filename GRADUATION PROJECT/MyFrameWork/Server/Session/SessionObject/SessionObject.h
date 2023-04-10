#pragma once
//

class Session;
class SessionObject
{
protected:
	Session* m_session;
protected:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotateAngle = { 0,0,0 };
	DirectX::XMFLOAT3 m_directionVector = DirectX::XMFLOAT3{ 0,0,1 };
	DirectX::XMFLOAT3 m_rightVector = DirectX::XMFLOAT3{ 1,0,0 };

	DirectX::XMFLOAT4X4 m_worldMatrix = Matrix4x4::Identity();

	std::chrono::high_resolution_clock::time_point m_lastMoveTime;
public:
	DIRECTION m_inputDirection = DIRECTION::IDLE;
	DIRECTION m_prevDirection = DIRECTION::IDLE;
public:
public:
	SessionObject(Session* session);
	virtual ~SessionObject();
public:
	virtual void AutoMove();
	virtual void StartMove(DIRECTION d);
	virtual void StopMove();
	virtual void ChangeDirection(DIRECTION d);
public:
	virtual const DirectX::XMFLOAT3 GetPosition();
	virtual const DirectX::XMFLOAT3 GetRotation();
public:
	virtual void Rotate(ROTATE_AXIS axis, float angle);
protected:
	void CalcRightVector()
	{
		m_rightVector = Vector3::CrossProduct(DirectX::XMFLOAT3(0, 1, 0), m_directionVector);
	}
};

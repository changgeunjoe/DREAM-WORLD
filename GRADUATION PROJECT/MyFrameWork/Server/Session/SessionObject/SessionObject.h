#pragma once
#include <DirectXMath.h>

class Session;
class SessionObject
{
protected:
	Session* m_session;
protected:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotateAngle;
public:
	SessionObject(Session* session);
	virtual ~SessionObject();
};

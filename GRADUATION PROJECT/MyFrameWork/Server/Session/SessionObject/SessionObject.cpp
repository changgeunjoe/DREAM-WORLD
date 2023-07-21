#include "stdafx.h"
#include "SessionObject.h"
#include "../UserSession.h"
SessionObject::SessionObject()
{
	m_position = XMFLOAT3(0, 0, 0);
	m_rotateAngle = XMFLOAT3(0, 0, 0);
}

SessionObject::SessionObject(float boundingSize) :m_position(XMFLOAT3(0, 0, 0)), m_rotateAngle(XMFLOAT3(0, 0, 0)), m_SPBB(BoundingSphere(XMFLOAT3(0.0f, boundingSize, 0.0f), boundingSize)), m_fBoundingSize(boundingSize)
{

}

SessionObject::~SessionObject()
{

}

void SessionObject::AutoMove()
{
	CalcRightVector();
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;
	Move(((float)durationTime / 1000.0f));
	m_lastMoveTime = currentTime;
}
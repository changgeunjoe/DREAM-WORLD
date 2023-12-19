#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject()
{
	m_position = XMFLOAT3(0, 0, 0);
	m_rotateAngle = XMFLOAT3(0, 0, 0);
}

GameObject::GameObject(float boundingSize) :m_position(XMFLOAT3(0, 0, 0)), m_rotateAngle(XMFLOAT3(0, 0, 0)), m_SPBB(BoundingSphere(XMFLOAT3(0.0f, boundingSize, 0.0f), boundingSize)), m_fBoundingSize(boundingSize)
{

}

GameObject::~GameObject()
{

}

void GameObject::AutoMove()
{
	CalcRightVector();
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;
	Move(((float)durationTime / 1000.0f));
	m_lastMoveTime = currentTime;
}
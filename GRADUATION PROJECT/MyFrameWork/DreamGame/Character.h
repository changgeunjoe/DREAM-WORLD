#pragma once
#include "GameObject.h"

class Character : public GameObject
{
public:
	Character();
	virtual ~Character();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	//virtual void Move(DIRECTION direction, float fDistance);
};

class Warrior : public Character
{
public:
	Warrior();
	virtual ~Warrior();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void Move(DIRECTION direction, float fDsitance) override;
	virtual void Animate(float fTimeElapsed);
};

class Archer : public Character
{
public:
	Archer();
	virtual ~Archer();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void Move(DIRECTION direction, float fDsitance) override;
	virtual void Animate(float fTimeElapsed);
};

class Tanker : public Character
{
public:
	Tanker();
	virtual ~Tanker();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void Move(DIRECTION direction, float fDsitance) override;
	virtual void Animate(float fTimeElapsed);
};

class Priest : public Character
{
public:
	Priest();
	virtual ~Priest();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis);
	virtual void Move(DIRECTION direction, float fDsitance) override;
	virtual void Animate(float fTimeElapsed);
};

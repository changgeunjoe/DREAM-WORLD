#pragma once
#include "GameObject.h"

class Character : public GameObject
{
public:
	Character();
	virtual ~Character();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void RbuttonUp();

protected:
	float m_fHp;
	float m_fSpeed;
	float m_fDamage;

};

class Warrior : public Character
{
public:
	Warrior();
	virtual ~Warrior();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void SetLookAt();
	virtual void Move(DIRECTION direction, float fDsitance);
};

class Archer : public Character
{
public:
	Archer();
	virtual ~Archer();
	virtual void RbuttonClicked(float fTimeElapsed);
	//virtual void SetLookAt();
	virtual void RbuttonUp();
	virtual void Move(DIRECTION direction, float fDsitance);
};

class Tanker : public Character
{
public:
	Tanker();
	virtual ~Tanker();
	virtual void RbuttonClicked(float fTimeElapsed);
	//virtual void SetLookAt();
	virtual void RbuttonUp();
	virtual void Move(DIRECTION direction, float fDsitance);
};

class Priest : public Character
{
public:
	Priest();
	virtual ~Priest();
	virtual void RbuttonClicked(float fTimeElapsed);
	virtual void SetLookAt();
	virtual void Move(DIRECTION direction, float fDsitance);
};

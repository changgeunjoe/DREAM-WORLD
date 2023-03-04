#include "stdafx.h"
#include "Character.h"

Character::Character() : GameObject(UNDEF_ENTITY)
{
	m_bRButtonClicked = false;
}

Character::~Character()
{
}

void Character::RbuttonClicked()
{
}

Warrior::Warrior() : Character()
{
	m_fHp = 200.0f;
	m_fSpeed = 100.0f;
	m_fDamage = 100.0f;
}

Warrior::~Warrior()
{
}

void Warrior::RbuttonClicked()
{
	// Animate Block
}

void Warrior::SetLookAt()
{
}

void Warrior::Move(DIRECTION direction, float fDistance)
{
	MoveForward(fDistance);
}

Archer::Archer() : Character()
{
	m_fHp = 150.0f;
	m_fSpeed = 150.0f;
	m_fDamage = 80.0f;
}

Archer::~Archer()
{
}

void Archer::RbuttonClicked()
{
	m_bRButtonClicked = true;
	// SetZoom
}


void Archer::Move(DIRECTION direction, float fDistance)
{
	fDistance *= m_fSpeed;
	if (!m_bRButtonClicked)
	{
		MoveForward(fDistance);
	}
	else
	{
		fDistance /= 10;
		switch (direction)
		{
		case DIRECTION::IDLE: break;
		case DIRECTION::FRONT: MoveForward(fDistance); break;
		case DIRECTION::FRONT | DIRECTION::RIGHT: MoveDiagonal(1, 1, fDistance); break;
		case DIRECTION::RIGHT: MoveStrafe(fDistance); break;
		case DIRECTION::BACK | DIRECTION::RIGHT: MoveDiagonal(-1, 1, fDistance);  break;
		case DIRECTION::BACK: MoveForward(-fDistance); break;
		case DIRECTION::BACK | DIRECTION::LEFT: MoveDiagonal(-1, -1, fDistance); break;
		case DIRECTION::LEFT: MoveStrafe(-fDistance); break;
		case DIRECTION::FRONT | DIRECTION::LEFT: MoveDiagonal(1, -1, fDistance); break;
		}
	}
}

Tanker::Tanker() : Character()
{
	m_fHp = 300.0f;
	m_fSpeed = 75.0f;
	m_fDamage = 50.0f;
}

Tanker::~Tanker()
{
}

void Tanker::RbuttonClicked()
{
	// Animate Block
}

void Tanker::SetLookAt()
{
}

void Tanker::Move(DIRECTION direction, float fDistance)
{
	MoveForward(fDistance);
}

Priest::Priest() : Character()
{
	m_fHp = 150.0f;
	m_fSpeed = 150.0f;
	m_fDamage = 80.0f;
}

Priest::~Priest()
{
}

void Priest::RbuttonClicked()
{
}

void Priest::SetLookAt()
{
}

void Priest::Move(DIRECTION direction, float fDistance)
{
	MoveForward(fDistance);
}

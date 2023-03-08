#include "stdafx.h"
#include "Character.h"
#include "Animation.h"

Character::Character() : GameObject(UNDEF_ENTITY)
{
}

Character::~Character()
{
}

void Character::RbuttonClicked(float fTimeElapsed)
{
}

void Character::RbuttonUp()
{
	m_pCamera->ReInitCamrea();
	SetCamera(m_pCamera);
	m_iRButtionCount = 0;
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

void Warrior::RbuttonClicked(float fTimeElapsed)
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

void Archer::RbuttonClicked(float fTimeElapsed)
{
	if (m_iRButtionCount < 50)
	{
		if (m_pCamera)
		{
			XMFLOAT3 LookVector = XMFLOAT3(m_pCamera->GetLookVector().x, 0.0f, m_pCamera->GetLookVector().z);
			XMFLOAT3 CameraOffset = m_pCamera->GetOffset();
			LookVector = Vector3::ScalarProduct(LookVector, fTimeElapsed * 10.0f, false);
			CameraOffset = Vector3::Add(CameraOffset, LookVector);
			m_pCamera->SetOffset(CameraOffset);
			m_iRButtionCount++;
			// 애니메이션 변경 (차징으로)
		}
	}
}


void Archer::RbuttonUp()
{
	Character::RbuttonUp();
	// 화살 발사 모먼트
}

void Archer::Move(DIRECTION direction, float fDistance)
{
	//fDistance *= m_fSpeed;
	if (!m_bRButtonClicked)
	{
		MoveForward(fDistance);
	}
	else
	{
		//fDistance /= 3;
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

void Tanker::RbuttonClicked(float fTimeElapsed)
{
	if (m_pCamera && !m_iRButtionCount)
	{
		if (m_pSkinnedAnimationController->GetMove())
		{
			m_pSkinnedAnimationController->SetAnimationBlending(true);
			m_pSkinnedAnimationController->SetAction(true);
			m_pSkinnedAnimationController->m_nUpperBodyAnimation = 6;
		}
		else
		{
			m_pSkinnedAnimationController->SetAction(true);
			m_pSkinnedAnimationController->SetAllTrackdisable();
			m_pSkinnedAnimationController->SetTrackEnable(6, true);	//Defence로 수정 필요 & Defence 애니메이션 정지 상태로 고정 시켜놔야한다.
		}
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 90.0f);
		m_iRButtionCount++;
	}
}

//void Tanker::SetLookAt()
//{
//}

void Tanker::RbuttonUp()
{
	Character::RbuttonUp();
	m_pSkinnedAnimationController->SetAnimationBlending(false);
	m_pSkinnedAnimationController->SetAction(false);
}

void Tanker::Move(DIRECTION direction, float fDistance)
{
	//fDistance *= m_fSpeed;
	if (!m_bRButtonClicked)
	{
		MoveForward(fDistance);
	}
	else
	{
		//fDistance /= 3;
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


Priest::Priest() : Character()
{
	m_fHp = 150.0f;
	m_fSpeed = 150.0f;
	m_fDamage = 80.0f;
}

Priest::~Priest()
{
}

void Priest::RbuttonClicked(float fTimeElapsed)
{
}

void Priest::SetLookAt()
{
}

void Priest::Move(DIRECTION direction, float fDistance)
{
	MoveForward(fDistance);
}

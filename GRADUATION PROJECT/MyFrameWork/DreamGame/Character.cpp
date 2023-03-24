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

void Character::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	m_pCamera->ReInitCamrea();
	SetCamera(m_pCamera);
	m_pCamera->Rotate(CameraAxis.x, CameraAxis.y, CameraAxis.z);
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

}

void Warrior::Move(DIRECTION direction, float fDistance)
{
	//fDistance *= m_fSpeed;
	DIRECTION tespDIR = direction;
	if (((tespDIR & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tespDIR & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::LEFT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::RIGHT);
	}
	if (((tespDIR & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tespDIR & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::FRONT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tespDIR)
		{
		case DIRECTION::FRONT:
		case DIRECTION::FRONT | DIRECTION::RIGHT:
		case DIRECTION::RIGHT:
		case DIRECTION::BACK | DIRECTION::RIGHT:
		case DIRECTION::BACK:
		case DIRECTION::BACK | DIRECTION::LEFT:
		case DIRECTION::LEFT:
		case DIRECTION::FRONT | DIRECTION::LEFT:
			MoveForward(fDistance);
		default: break;
		}
	}
	else
	{
		//fDistance /= 3;
		switch (tespDIR)
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
		default: break;
		}
	}
}

void Warrior::Animate(float fTimeElapsed)
{
	CharacterAnimation AfterAnimation = static_cast<CharacterAnimation>(m_pSkinnedAnimationController->m_CurrentAnimation);
	pair<CharacterAnimation, CharacterAnimation> blendingAnimation;

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation = CharacterAnimation::CA_BLENDING;
			blendingAnimation.first = CharacterAnimation::CA_ATTACK;
			blendingAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			AfterAnimation = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation = CharacterAnimation::CA_ATTACK;
		}
		else						// IDLE
		{
			AfterAnimation = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != static_cast<CharacterAnimation>(m_pSkinnedAnimationController->m_CurrentAnimation))
	{
		m_pSkinnedAnimationController->SetAllTrackdisable();
		m_pSkinnedAnimationController->m_CurrentAnimation = AfterAnimation;

		if (AfterAnimation == CharacterAnimation::CA_BLENDING)
		{
			m_pSkinnedAnimationController->SetAnimationBlending(true);
			m_pSkinnedAnimationController->SetTrackBlending(blendingAnimation.first, blendingAnimation.second);
		}
		else
		{
			m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation, true);
		}
	}

	GameObject::Animate(fTimeElapsed);
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

void Archer::Attack(GameObject* pGameObject)
{
	m_pArrow = static_cast<Arrow*>(pGameObject);
	m_pArrow->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(5.0f, 7.5f, 0.0f)));
	m_pArrow->SetLook(GetObjectLook());
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
		}
	}
}

void Archer::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	Character::RbuttonUp(CameraAxis);
	// 화살 발사 모먼트
}

void Archer::Move(DIRECTION direction, float fDistance)
{
	//fDistance *= m_fSpeed;
	DIRECTION tespDIR = direction;
	if (((tespDIR & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tespDIR & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::LEFT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::RIGHT);
	}
	if (((tespDIR & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tespDIR & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::FRONT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tespDIR)
		{
		case DIRECTION::FRONT:
		case DIRECTION::FRONT | DIRECTION::RIGHT:
		case DIRECTION::RIGHT:
		case DIRECTION::BACK | DIRECTION::RIGHT:
		case DIRECTION::BACK:
		case DIRECTION::BACK | DIRECTION::LEFT:
		case DIRECTION::LEFT:
		case DIRECTION::FRONT | DIRECTION::LEFT:
			MoveForward(fDistance);
		default: break;
		}
	}
	else
	{
		//fDistance /= 3;
		switch (tespDIR)
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
		default: break;
		}
	}
}

void Archer::Animate(float fTimeElapsed)
{
	CharacterAnimation AfterAnimation = static_cast<CharacterAnimation>(m_pSkinnedAnimationController->m_CurrentAnimation);
	pair<CharacterAnimation, CharacterAnimation> blendingAnimation;
	bool RButtonAnimation = false;
	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked)
		{
			AfterAnimation = CharacterAnimation::CA_BLENDING;
			blendingAnimation.first = CharacterAnimation::CA_ATTACK;
			blendingAnimation.second = CharacterAnimation::CA_MOVE;
			RButtonAnimation = true;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation = CharacterAnimation::CA_BLENDING;
			blendingAnimation.first = CharacterAnimation::CA_ATTACK;
			blendingAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			AfterAnimation = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bRButtonClicked)
		{
			AfterAnimation = CharacterAnimation::CA_ATTACK;
			RButtonAnimation = true;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation = CharacterAnimation::CA_ATTACK;
		}
		else						// IDLE
		{
			AfterAnimation = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != static_cast<CharacterAnimation>(m_pSkinnedAnimationController->m_CurrentAnimation))
	{
		m_pSkinnedAnimationController->SetAllTrackdisable();
		m_pSkinnedAnimationController->m_CurrentAnimation = AfterAnimation;

		if (RButtonAnimation)
		{
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_HALF;
			if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition >= m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fLength * 0.7f)
				m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition = -1.0f;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 0.3f;
		}
		else
		{
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_LOOP;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 1.0f;
		}

		if (AfterAnimation == CharacterAnimation::CA_BLENDING)
		{
			m_pSkinnedAnimationController->SetAnimationBlending(true);
			m_pSkinnedAnimationController->SetTrackBlending(blendingAnimation.first, blendingAnimation.second);
		}
		else
		{
			m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation, true);
		}

	}

	if (m_pArrow) m_pArrow->Animate(fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Archer::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	if (m_pArrow) m_pArrow->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
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
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 90.0f);
		m_iRButtionCount++;
	}
}

void Tanker::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	Character::RbuttonUp(CameraAxis);
}

void Tanker::Move(DIRECTION direction, float fDistance)
{
	//fDistance *= m_fSpeed;
	DIRECTION tespDIR = direction;
	if (((tespDIR & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tespDIR & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::LEFT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::RIGHT);
	}
	if (((tespDIR & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tespDIR & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::FRONT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tespDIR)
		{
		case DIRECTION::FRONT:
		case DIRECTION::FRONT | DIRECTION::RIGHT:
		case DIRECTION::RIGHT:
		case DIRECTION::BACK | DIRECTION::RIGHT:
		case DIRECTION::BACK:
		case DIRECTION::BACK | DIRECTION::LEFT:
		case DIRECTION::LEFT:
		case DIRECTION::FRONT | DIRECTION::LEFT:
			MoveForward(fDistance);
		default: break;
		}
	}
	else
	{
		//fDistance /= 3;
		switch (tespDIR)
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
		default: break;
		}
	}
}

void Tanker::Animate(float fTimeElapsed)
{
	CharacterAnimation AfterAnimation = static_cast<CharacterAnimation>(m_pSkinnedAnimationController->m_CurrentAnimation);
	pair<CharacterAnimation, CharacterAnimation> blendingAnimation;

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked)
		{
			AfterAnimation = CharacterAnimation::CA_BLENDING;
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DEFENCE]->m_fPosition = -1.0f;
			blendingAnimation.first = CharacterAnimation::CA_DEFENCE;
			blendingAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation = CharacterAnimation::CA_BLENDING;
			blendingAnimation.first = CharacterAnimation::CA_ATTACK;
			blendingAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			AfterAnimation = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bRButtonClicked)
		{
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DEFENCE]->m_fPosition = -1.0f;
			AfterAnimation = CharacterAnimation::CA_DEFENCE;
		}

		else if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation = CharacterAnimation::CA_ATTACK;
		}

		else						// IDLE
		{
			AfterAnimation = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != static_cast<CharacterAnimation>(m_pSkinnedAnimationController->m_CurrentAnimation))
	{
		m_pSkinnedAnimationController->SetAllTrackdisable();
		m_pSkinnedAnimationController->m_CurrentAnimation = AfterAnimation;

		if (AfterAnimation == CharacterAnimation::CA_BLENDING)
		{
			m_pSkinnedAnimationController->SetAnimationBlending(true);
			m_pSkinnedAnimationController->SetTrackBlending(blendingAnimation.first, blendingAnimation.second);
		}
		else
		{
			m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation, true);
		}
	}

	GameObject::Animate(fTimeElapsed);
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

void Priest::Move(DIRECTION direction, float fDistance)
{
	//fDistance *= m_fSpeed;
	DIRECTION tespDIR = direction;
	if (((tespDIR & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tespDIR & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::LEFT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::RIGHT);
	}
	if (((tespDIR & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tespDIR & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::FRONT);
		tespDIR = (DIRECTION)(tespDIR ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tespDIR)
		{
		case DIRECTION::FRONT:
		case DIRECTION::FRONT | DIRECTION::RIGHT:
		case DIRECTION::RIGHT:
		case DIRECTION::BACK | DIRECTION::RIGHT:
		case DIRECTION::BACK:
		case DIRECTION::BACK | DIRECTION::LEFT:
		case DIRECTION::LEFT:
		case DIRECTION::FRONT | DIRECTION::LEFT:
			MoveForward(fDistance);
		default: break;
		}
	}
	else
	{
		//fDistance /= 3;
		switch (tespDIR)
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
		default: break;
		}
	}
}

void Priest::Animate(float fTimeElapsed)
{
	CharacterAnimation AfterAnimation = static_cast<CharacterAnimation>(m_pSkinnedAnimationController->m_CurrentAnimation);
	pair<CharacterAnimation, CharacterAnimation> blendingAnimation;

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation = CharacterAnimation::CA_BLENDING;
			blendingAnimation.first = CharacterAnimation::CA_ATTACK;
			blendingAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			AfterAnimation = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation = CharacterAnimation::CA_ATTACK;
		}
		else						// IDLE
		{
			AfterAnimation = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != static_cast<CharacterAnimation>(m_pSkinnedAnimationController->m_CurrentAnimation))
	{
		m_pSkinnedAnimationController->SetAllTrackdisable();
		m_pSkinnedAnimationController->m_CurrentAnimation = AfterAnimation;

		if (AfterAnimation == CharacterAnimation::CA_BLENDING)
		{
			m_pSkinnedAnimationController->SetAnimationBlending(true);
			m_pSkinnedAnimationController->SetTrackBlending(blendingAnimation.first, blendingAnimation.second);
		}
		else
		{
			m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation, true);
		}
	}

	GameObject::Animate(fTimeElapsed);
}

void Priest::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	Character::RbuttonUp(CameraAxis);
	// 화살 발사 모먼트
}

Arrow::Arrow() : GameObject(UNDEF_ENTITY)
{
}

Arrow::~Arrow()
{
}

void Arrow::Animate(float fTimeElapsed)
{
	MoveForward(fTimeElapsed * 100);
}

void Arrow::ShootArrow(const XMFLOAT3& xmf3StartPos, const XMFLOAT3& xmf3direction)
{
}

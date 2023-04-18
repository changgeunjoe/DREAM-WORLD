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
	if (m_pCamera)
	{
		m_pCamera->ReInitCamrea();
		SetCamera(m_pCamera);
		m_pCamera->Rotate(CameraAxis.x, CameraAxis.y, CameraAxis.z);
	}
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
	DIRECTION tempDir = direction;
	if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
	}
	if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
	}

	switch (tempDir)
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

void Warrior::Animate(float fTimeElapsed)
{
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimation;
	bool RButtonAnimation = false;
	bool UpperLock = false;
	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_ATTACK:
	{
		if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == false)
		{
			UpperLock = true;
		}
		break;
	}
	}
	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else 
	{
		if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (!UpperLock)					// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimation)
	{
		m_pSkinnedAnimationController->m_CurrentAnimation = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}
	if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == true)
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
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
	for (auto& i : m_pProjectiles)
	{
		if(i != nullptr)
			delete i;
	}
}

void Archer::Attack(float fSpeed)
{
	m_nProjectiles = (m_nProjectiles < 10) ? m_nProjectiles : m_nProjectiles % 10;
	if (m_pProjectiles[m_nProjectiles]->m_fSpeed > 0)
	{
		if (!m_pProjectiles[m_nProjectiles]->m_RAttack)
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = GetObjectLook();
		m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = GetPosition();
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 5.0f, 0.0f)));
		m_pProjectiles[m_nProjectiles]->m_fSpeed = fSpeed;
		m_pProjectiles[m_nProjectiles]->m_bActive = true;
		m_nProjectiles++;
	}
}

void Archer::SetArrow(Projectile* pArrow)
{
	if (m_nProjectiles < 10)
	{
		m_pProjectiles[m_nProjectiles] = new Arrow();
		m_pProjectiles[m_nProjectiles] = static_cast<Projectile*>(pArrow);
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 7.5f, 0.0f)));
		m_pProjectiles[m_nProjectiles]->SetLook(GetObjectLook());
		m_pProjectiles[m_nProjectiles]->m_bActive = false;
		m_nProjectiles++;
	}
}

void Archer::RbuttonClicked(float fTimeElapsed)
{
	if (!(m_pSkinnedAnimationController->m_CurrentAnimation.first == CharacterAnimation::CA_ATTACK &&
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == false))
	{
		if (m_iRButtionCount == 0)
		{
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_HALF;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 0.3f;
		}
		if (m_iRButtionCount < 50)
		{
			if (m_pCamera)
			{
				XMFLOAT3 LookVector = XMFLOAT3(m_pCamera->GetLookVector().x, 0.0f, m_pCamera->GetLookVector().z);
				XMFLOAT3 CameraOffset = m_pCamera->GetOffset();
				LookVector = Vector3::ScalarProduct(LookVector, fTimeElapsed * 10.0f, false);
				CameraOffset = Vector3::Add(CameraOffset, LookVector);
				m_pCamera->SetOffset(CameraOffset);
			}
			m_iRButtionCount++;
		}
	}
}

void Archer::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	if (m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType == ANIMATION_TYPE_HALF)
	{
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_LOOP;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 1.0f;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = true;
	}
	if (m_iRButtionCount != 0)
	{
		float chargingTime = m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition;
		float fullTime = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fLength * 0.7f;
		float arrowSpeed = pow((chargingTime / fullTime), 2);

		if (m_pCamera)
			m_pProjectiles[m_nProjectiles % 10]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, m_pCamera->GetLookVector().y, GetObjectLook().z);
		else
			m_pProjectiles[m_nProjectiles % 10]->m_xmf3direction = GetObjectLook();

		m_pProjectiles[m_nProjectiles % 10]->m_fSpeed = (chargingTime / fullTime > 0.5f) ? arrowSpeed * 400.0f : 0.0f;
		m_pProjectiles[m_nProjectiles % 10]->m_RAttack = true;
		Character::RbuttonUp(CameraAxis);
	}
}

void Archer::Move(DIRECTION direction, float fDistance)
{
	//fDistance *= m_fSpeed;
	DIRECTION tempDir = direction;
	if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
	}
	if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tempDir)
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
		switch (tempDir)
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
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimation;
	bool UpperLock = false;
	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_ATTACK:
	{
		if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == false)
			UpperLock = true;
		break;
	}
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else 
	{
		if (m_bRButtonClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
			
		}
		else if (!UpperLock)					// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimation)
	{
		m_pSkinnedAnimationController->m_CurrentAnimation = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == true)
	{
		Attack();
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
	}

	for (int i = 0; i < m_pProjectiles.size(); ++i)
		if (m_pProjectiles[i]->m_bActive)
			m_pProjectiles[i]->Animate(fTimeElapsed);

	GameObject::Animate(fTimeElapsed);
}

void Archer::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	for (int i = 0; i < m_pProjectiles.size(); ++i)
		if (m_pProjectiles[i]) m_pProjectiles[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
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
	DIRECTION tempDir = direction;
	if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
	}
	if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tempDir)
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
		switch (tempDir)
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
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimation;
	bool RButtonAnimation = false;
	bool UpperLock = false;

	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_ATTACK:
	{
		if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == false)
		{
			UpperLock = true;
		}
		break;
	}
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_DEFENCE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else if(!UpperLock)
	{
		if (m_bRButtonClicked)
		{
			AfterAnimation.first = CharacterAnimation::CA_DEFENCE;
			AfterAnimation.second = CharacterAnimation::CA_DEFENCE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else						// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}


	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimation)
	{
		m_pSkinnedAnimationController->m_CurrentAnimation = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}
	if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == true)
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;

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
	DIRECTION tempDir = direction;
	if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
	}
	if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tempDir)
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
		switch (tempDir)
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
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimation;
	bool UpperLock = false;
	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_ATTACK:
	{
		if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == false)
			UpperLock = true;
		break;
	}
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bRButtonClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;

		}
		else if (!UpperLock)					// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimation)
	{
		m_pSkinnedAnimationController->m_CurrentAnimation = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	if (m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd == true)
	{
		Attack();
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
	}

	for (int i = 0; i < m_pProjectiles.size(); ++i)
		if (m_pProjectiles[i]->m_bActive)
			m_pProjectiles[i]->Animate(fTimeElapsed);

	GameObject::Animate(fTimeElapsed);
}

void Priest::RbuttonUp(const XMFLOAT3& CameraAxis)
{
}

void Priest::Attack(float fSpeed)
{
	m_nProjectiles = (m_nProjectiles < 10) ? m_nProjectiles : m_nProjectiles % 10;
	m_pProjectiles[m_nProjectiles]->m_xmf3direction = GetObjectLook();
	m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = GetPosition();
	m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 5.0f, 0.0f)));
	m_pProjectiles[m_nProjectiles]->m_fSpeed = fSpeed;
	m_pProjectiles[m_nProjectiles]->m_bActive = true;
	m_nProjectiles++;
}

void Priest::SetEnergyBall(Projectile* pEnergyBall)
{
	if (m_nProjectiles < 10)
	{
		m_pProjectiles[m_nProjectiles] = new EnergyBall();
		m_pProjectiles[m_nProjectiles] = static_cast<Projectile*>(pEnergyBall);
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 7.5f, 100.0f)));
		m_pProjectiles[m_nProjectiles]->SetLook(GetObjectLook());
		m_pProjectiles[m_nProjectiles]->m_bActive = false;
		m_nProjectiles++;
	}
}

void Priest::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	for (int i = 0; i < m_pProjectiles.size(); ++i)
		if (m_pProjectiles[i]) m_pProjectiles[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}
Monster::Monster() : Character()
{
}

Monster::~Monster()
{
}

void Monster::Animate(float fTimeElapsed)
{
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 des = Vector3::Subtract(m_xmf3Destination, GetPosition());
	bool OnRight = (Vector3::DotProduct(GetRight(), Vector3::Normalize(des)) > 0) ? true : false;
	float ChangingAngle = Vector3::Angle(Vector3::Normalize(des), GetLook());

	if (Vector3::Length(des) < DBL_EPSILON)
		SetPosition(m_xmf3Destination);
	else
	{
		if (ChangingAngle > 1.0f)
		{
			if (OnRight)
				Rotate(&up, 45.0f * fTimeElapsed);
			else if (!OnRight)
				Rotate(&up, -45.0f * fTimeElapsed);
		}
		else
			SetLook(Vector3::Normalize(des));

		MoveForward(50 * fTimeElapsed);
	}

	Character::Animate(fTimeElapsed);
}

Projectile::Projectile(entity_id eid) : GameObject(eid)
{
	m_xmf3startPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	m_xmf3direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fSpeed = 0.0f;
	m_bActive = false;
	m_RAttack = false;
	m_Angle = 0.0f;
}

Projectile::~Projectile()
{
}

Arrow::Arrow() : Projectile()
{
	m_fSpeed = 150.0f;
}

Arrow::~Arrow()
{
}

void Arrow::Animate(float fTimeElapsed)
{
	SetLook(m_xmf3direction);
	MoveForward(fTimeElapsed * m_fSpeed);
	XMFLOAT3 xmf3CurrentPos = GetPosition();
	if (Vector3::Length(Vector3::Subtract(xmf3CurrentPos, m_xmf3startPosition)) > 200.0f)
	{
		m_bActive = false;
		m_RAttack = false;
	}
}

void Arrow::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	if (m_bActive)
	{
		GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
	}
}

EnergyBall::EnergyBall() : Projectile(SQUARE_ENTITY)
{
	m_fSpeed = 75.0f;
}

EnergyBall::~EnergyBall()
{
}

void EnergyBall::Animate(float fTimeElapsed)
{
	XMFLOAT3 rev = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Angle += 1280.0f * fTimeElapsed * (3.14 / 180.0f);
	rev.x = m_xmf3direction.x * cos(m_Angle) - m_xmf3direction.z * sin(m_Angle);
	rev.z = m_xmf3direction.x * sin(m_Angle) + m_xmf3direction.z * cos(m_Angle);
	rev = Vector3::Normalize(rev);
	SetLook(rev);
	Move(m_xmf3direction, fTimeElapsed * m_fSpeed);
	XMFLOAT3 xmf3CurrentPos = GetPosition();
	if (Vector3::Length(Vector3::Subtract(xmf3CurrentPos, m_xmf3startPosition)) > 200.0f)
	{
		m_bActive = false;
	}
}

void EnergyBall::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	if (m_bActive)
	{
		GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
	}
}

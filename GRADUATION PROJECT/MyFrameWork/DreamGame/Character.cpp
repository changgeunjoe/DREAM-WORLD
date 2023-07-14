#include "stdafx.h"
#include "Character.h"
#include "Animation.h"
#include "GameFramework.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"
#include "Network/MapData/MapData.h"

extern Logic g_Logic;
extern NetworkHelper g_NetworkHelper;
extern bool GameEnd;
extern MapData g_bossMapData;
extern CGameFramework gGameFramework;

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

void Character::Reset()
{
	m_fHp = m_fMaxHp;
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pSkinnedAnimationController->ResetTrack();
	m_bMoveState = false;
	SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_bLButtonClicked = false;
	m_bRButtonClicked = false;
	m_projectilesLookY = 0;
	for (int i = 0; i < m_pProjectiles.size(); ++i)
		if (m_pProjectiles[i]) m_pProjectiles[i]->m_bActive = false;

	if (m_pSkinnedAnimationController->m_CurrentAnimation != CharacterAnimation::CA_IDLE)
	{
		m_pSkinnedAnimationController->m_CurrentAnimation = CharacterAnimation::CA_IDLE;
		m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, 2);
	}
	if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_IDLE)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_IDLE;
		m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_IDLE;
		m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
	}
	if (m_pCamera)
	{
		m_pCamera = nullptr;
	}
}

bool Character::CheckAnimationEnd(int nAnimation)
{
	return m_pSkinnedAnimationController->m_pAnimationTracks[nAnimation].m_bAnimationEnd;
}

Warrior::Warrior() : Character()
{
	m_fHp = 400.0f;
	m_fMaxHp = 400.0f;
	m_fSpeed = 100.0f;
	m_fDamage = 100.0f;
}

Warrior::~Warrior()
{
}

void Warrior::Attack(float fSpeed)
{
	if (m_bQSkillClicked)
	{
		m_bQSkillClicked = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_bAnimationEnd = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fSpeed = 1.0f;
	}
	else if (m_bLButtonClicked)
	{
		if (m_pCamera)
			g_NetworkHelper.SendMeleeAttackPacket(GetLook());
	}
}

void Warrior::RbuttonClicked(float fTimeElapsed)
{

}

void Warrior::Move(DIRECTION direction, float fDistance)
{
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

void Warrior::Animate(float fTimeElapsed)
{
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	bool RButtonAnimation = false;
	bool UpperLock = false;
	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_FIRSTSKILL:
	{
		if (CheckAnimationEnd(CA_FIRSTSKILL) == false)
		{
			UpperLock = true;
		}
		break;
	}
	case CharacterAnimation::CA_ATTACK:
	{
		if (CheckAnimationEnd(CA_ATTACK) == false)
		{
			UpperLock = true;
		}
		break;
	}
	}
	if (CheckAnimationEnd(CA_ATTACK) || CheckAnimationEnd(CA_FIRSTSKILL))
	{
		Attack();
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bQSkillClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
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
		if (m_bQSkillClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			AfterAnimation.second = CharacterAnimation::CA_FIRSTSKILL;
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

	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	GameObject::Animate(fTimeElapsed);
}

Archer::Archer() : Character()
{
	m_fHp = 250.0f;
	m_fMaxHp = 250.0f;
	m_fSpeed = 150.0f;
	m_fDamage = 80.0f;
	m_CameraLook = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Archer::~Archer()
{
	for (auto& i : m_pProjectiles)
	{
		if (i != nullptr)
			delete i;
	}
}

void Archer::Attack(float fSpeed)
{
}

void Archer::SetArrow(Projectile* pArrow)
{
	if (m_nProjectiles < MAX_ARROW)
	{
		m_pProjectiles[m_nProjectiles] = static_cast<Projectile*>(pArrow);
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 7.5f, 0.0f)));
		m_pProjectiles[m_nProjectiles]->SetLook(GetObjectLook());
		m_pProjectiles[m_nProjectiles]->m_bActive = false;
		m_nProjectiles++;
	}
}

void Archer::RbuttonClicked(float fTimeElapsed)
{
}

void Archer::RbuttonUp(const XMFLOAT3& CameraAxis)
{
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

	//if (!m_bRButtonClicked)
	//{
	//	switch (tempDir)
	//	{
	//	case DIRECTION::FRONT:
	//	case DIRECTION::FRONT | DIRECTION::RIGHT:
	//	case DIRECTION::RIGHT:
	//	case DIRECTION::BACK | DIRECTION::RIGHT:
	//	case DIRECTION::BACK:
	//	case DIRECTION::BACK | DIRECTION::LEFT:
	//	case DIRECTION::LEFT:
	//	case DIRECTION::FRONT | DIRECTION::LEFT:
	//		MoveForward(fDistance);
	//	default: break;
	//	}
	//}
	//else
	//{
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
	//}
}

void Archer::Animate(float fTimeElapsed)
{
	m_fTimeElapsed = fTimeElapsed;
	if (m_bESkillClicked)
		SecondSkillDown();

	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}

	if (m_pSkinnedAnimationController->m_CurrentAnimations.first == CharacterAnimation::CA_ATTACK)
	{
		if (CheckAnimationEnd(CA_ATTACK))
		{
			ShootArrow();
			m_bOnAttack = false;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
		}
		else
		{
			m_bOnAttack = true;
		}
	}

	// 현재 재생될 애니메이션 선택
	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked || m_bLButtonClicked || m_bQSkillClicked || m_bESkillClicked)
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else
		{// 그냥 움직이기
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bRButtonClicked || m_bLButtonClicked || m_bQSkillClicked || m_bESkillClicked)
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (!m_bOnAttack)
		{// IDLE
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}

	// 재생될 애니메이션이 기존 애니메이션과 다르면 변경
	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	GameObject::Animate(fTimeElapsed);
}

void Archer::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Archer::MoveObject(DIRECTION& currentDirection, const XMFLOAT3& CameraAxis)
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(CameraAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);

	XMFLOAT3 xmf3Rev = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fRotateAngle = -1.0f;

	DIRECTION tempDir = currentDirection;

	SetLook(xmf3Look);
}

void Archer::FirstSkillDown()
{
	m_bQSkillClicked = true;
	// 실제 화살 발사는 애니메이션이 끝난 후 ShootArrow에서 설정
}

void Archer::FirstSkillUp()
{
}

void Archer::SecondSkillDown()
{
	if (!(m_pSkinnedAnimationController->m_CurrentAnimations.first == CharacterAnimation::CA_ATTACK &&
		CheckAnimationEnd(CA_ATTACK) == false)
		|| m_bESkillClicked == true)
	{
		m_bESkillClicked = true;

		if (m_iRButtionCount == 0)
		{
			// 줌인을 위해서 애니메이션 타입 및 속도 변경
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_HALF;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 0.3f;
		}
		if (m_iRButtionCount < 50)
		{
			// 카메라 줌인 효과
			if (m_pCamera)
			{
				XMFLOAT3 LookVector = XMFLOAT3(m_pCamera->GetLookVector().x, 0.0f, m_pCamera->GetLookVector().z);
				XMFLOAT3 CameraOffset = m_pCamera->GetOffset();
				LookVector = Vector3::ScalarProduct(LookVector, m_fTimeElapsed * 10.0f, false);
				CameraOffset = Vector3::Add(CameraOffset, LookVector);
				m_pCamera->SetOffset(CameraOffset);
			}
			m_iRButtionCount++;
		}
	}
}

void Archer::SecondSkillUp(const XMFLOAT3& CameraAxis)
{
	if (m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType == ANIMATION_TYPE_HALF)
	{
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_LOOP;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 1.0f;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = true;
		m_CameraLook = CameraAxis;
	}
}

void Archer::ShootArrow()
{
	m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
	if (m_bQSkillClicked == true)
	{// 3개의 화살을 발사하는 스킬 실행
		for (int i = 0; i < 3; ++i)
		{
			m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
			XMFLOAT3 tempLook = GetObjectLook();
			XMFLOAT3 Position = GetPosition();
			XMFLOAT3 targetPos = Vector3::Add(Position, tempLook, 150.0f);
			// 임시 목표 설정
			if (m_pCamera)
			{
				m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(tempLook.x, m_pCamera->GetLookVector().y, tempLook.z);
				m_pProjectiles[m_nProjectiles]->m_xmf3direction.x = tempLook.x * cos((1 - i) * (3.14 / 6)) - tempLook.z * sin((1 - i) * (3.14 / 6));
				m_pProjectiles[m_nProjectiles]->m_xmf3direction.z = tempLook.x * sin((1 - i) * (3.14 / 6)) + tempLook.z * cos((1 - i) * (3.14 / 6));
				m_pProjectiles[m_nProjectiles]->m_xmf3direction.y = m_pCamera->GetLookVector().y + 0.5f;
			}
			else
			{
				m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, -sin(m_projectilesLookY * 3.141592 / 180.0f), GetObjectLook().z);
				m_pProjectiles[m_nProjectiles]->m_xmf3direction.x = tempLook.x * cos((1 - i) * (3.14 / 6)) - tempLook.z * sin((1 - i) * (3.14 / 6));
				m_pProjectiles[m_nProjectiles]->m_xmf3direction.z = tempLook.x * sin((1 - i) * (3.14 / 6)) + tempLook.z * cos((1 - i) * (3.14 / 6));
				m_pProjectiles[m_nProjectiles]->m_xmf3direction.y = -sin(m_projectilesLookY * 3.141592 / 180.0f) + 0.5f;
			}

			m_pProjectiles[m_nProjectiles]->m_xmf3direction = Vector3::Normalize(m_pProjectiles[m_nProjectiles]->m_xmf3direction);
			m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = Vector3::Add(Position, XMFLOAT3(0.0f, 5.0f, 0.0f));
			m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(Position, XMFLOAT3(0.0f, 5.0f, 0.0f)));
			m_pProjectiles[m_nProjectiles]->m_fSpeed = 250.0f;
			m_pProjectiles[m_nProjectiles]->m_bActive = true;
			static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_xmf3TargetPos = targetPos;
			static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_ArrowType = 1;
			static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_ArrowPos = 0.0f;
			m_nProjectiles++;
		}
		m_bQSkillClicked = false;
	}
	else if (m_bESkillClicked == true)
	{
		// 1개의 화살을 발사 && 데미지 및 스피드 수정
		// 스피드에 비례하여 데미지 설정	// 서버로는 스피드만 전송
		float chargingTime = m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition;
		float fullTime = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fLength * 0.7f;
		float arrowSpeed = pow((chargingTime / fullTime), 2);

		if (m_pCamera)
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, m_pCamera->GetLookVector().y, GetObjectLook().z);
		else
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, -sin(m_projectilesLookY * 3.141592 / 180.0f), GetObjectLook().z);

		m_pProjectiles[m_nProjectiles]->m_xmf3direction = Vector3::Normalize(m_pProjectiles[m_nProjectiles]->m_xmf3direction);
		m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = GetPosition();
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 5.0f, 0.0f)));
		m_pProjectiles[m_nProjectiles]->m_fSpeed = (chargingTime / fullTime > 0.5f) ? arrowSpeed * 400.0f : -1.0f;
		static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_xmf3TargetPos = XMFLOAT3(0.0f, -1.0f, 0.0f);
		static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_ArrowType = 0;
		// 최대 화살 속도 400

		// 추후 화살 데미지 설정 추가
		if (m_pProjectiles[m_nProjectiles]->m_fSpeed > 10)
		{
			m_pProjectiles[m_nProjectiles]->m_bActive = true;

			if (m_pCamera)
				g_NetworkHelper.SendArrowAttackPacket(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition, m_pProjectiles[m_nProjectiles]->m_xmf3direction, m_pProjectiles[m_nProjectiles]->m_fSpeed);

			m_nProjectiles++;
		}

		m_bESkillClicked = false;

		Character::RbuttonUp(m_CameraLook);
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
	else if (m_bOnAttack == true)
	{// 1개의 화살을 발사하는 기본 공격 실행
		if (m_pCamera)
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, m_pCamera->GetLookVector().y, GetObjectLook().z);
		else
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, -sin(m_projectilesLookY * 3.141592 / 180.0f), GetObjectLook().z);

		m_pProjectiles[m_nProjectiles]->m_xmf3direction = Vector3::Normalize(m_pProjectiles[m_nProjectiles]->m_xmf3direction);
		m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = GetPosition();
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 5.0f, 0.0f)));
		m_pProjectiles[m_nProjectiles]->m_fSpeed = 150.0f;
		m_pProjectiles[m_nProjectiles]->m_bActive = true;
		static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_xmf3TargetPos = XMFLOAT3(0.0f, -1.0f, 0.0f);
		static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_ArrowType = 0;
		if (m_pCamera)
			g_NetworkHelper.SendArrowAttackPacket(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition, m_pProjectiles[m_nProjectiles]->m_xmf3direction, m_pProjectiles[m_nProjectiles]->m_fSpeed);
		m_nProjectiles++;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
}

//void Archer::ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent)
//{
//	GameObject::ShadowRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender, pShaderComponent);
//}

Tanker::Tanker() : Character()
{
	m_fHp = 600.0f;
	m_fMaxHp = 600.0f;
	m_fSpeed = 75.0f;
	m_fDamage = 50.0f;
}

Tanker::~Tanker()
{

}

void Tanker::Attack(float fSpeed)
{
	if (m_pCamera)
		g_NetworkHelper.SendMeleeAttackPacket(GetLook());
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
	if (m_bRButtonClicked)
		RbuttonClicked(fTimeElapsed);

	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	bool RButtonAnimation = false;
	bool UpperLock = false;

	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_ATTACK:
	{
		if (CheckAnimationEnd(CA_ATTACK) == false)
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
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bQSkillClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_SECONDSKILL;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else if (!UpperLock)
	{
		if (m_bRButtonClicked)
		{
			AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			AfterAnimation.second = CharacterAnimation::CA_FIRSTSKILL;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (m_bQSkillClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_SECONDSKILL;
			AfterAnimation.second = CharacterAnimation::CA_SECONDSKILL;
		}
		else						// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}


	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	if (CheckAnimationEnd(CA_ATTACK) == true)
	{
		Attack();
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
	}
	if (CheckAnimationEnd(CA_SECONDSKILL) == true)
	{
		m_bQSkillClicked = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_SECONDSKILL].m_bAnimationEnd = false;
	}

	GameObject::Animate(fTimeElapsed);
}

Priest::Priest() : Character()
{
	m_fHp = 480.0f;
	m_fMaxHp = 480.0f;
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

	//if (!m_bRButtonClicked)
	//{
	//	switch (tempDir)
	//	{
	//	case DIRECTION::FRONT:
	//	case DIRECTION::FRONT | DIRECTION::RIGHT:
	//	case DIRECTION::RIGHT:
	//	case DIRECTION::BACK | DIRECTION::RIGHT:
	//	case DIRECTION::BACK:
	//	case DIRECTION::BACK | DIRECTION::LEFT:
	//	case DIRECTION::LEFT:
	//	case DIRECTION::FRONT | DIRECTION::LEFT:
	//		MoveForward(fDistance);
	//	default: break;
	//	}
	//}
	//else
	//{
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
	//}
}

void Priest::Animate(float fTimeElapsed)
{
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}

	if (m_pHealRange->m_bActive)
	{
		m_fHealTime += fTimeElapsed;
		if (m_fHealTime > 10.0f)
		{
			m_fHealTime = 0.0f;
			m_pHealRange->m_bActive = false;
			m_bQSkillClicked = false;
		}
	}

	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	bool UpperLock = false;
	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_ATTACK:
	{
		if (CheckAnimationEnd(CA_ATTACK) == false)
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

	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	if (CheckAnimationEnd(CA_ATTACK) == true)
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

	if (m_pCamera)
		m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, m_pCamera->GetLookVector().y, GetObjectLook().z);
	else
	{
		m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, -sin(m_projectilesLookY * 3.141592 / 180.0f), GetObjectLook().z);
	}

	m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = GetPosition();
	m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 5.0f, 0.0f)));
	m_pProjectiles[m_nProjectiles]->m_fSpeed = fSpeed;
	m_pProjectiles[m_nProjectiles]->m_bActive = true;
	if (m_pCamera)
		g_NetworkHelper.SendBallAttackPacket(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition, m_pProjectiles[m_nProjectiles]->m_xmf3direction, fSpeed);
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
		if (m_pProjectiles[i]) 
			m_pProjectiles[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);

	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Priest::MoveObject(DIRECTION& currentDirection, const XMFLOAT3& CameraAxis)
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(CameraAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);

	XMFLOAT3 xmf3Rev = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fRotateAngle = -1.0f;

	DIRECTION tempDir = currentDirection;

	SetLook(xmf3Look);
}

void Priest::FirstSkillDown()
{
	m_bQSkillClicked = true;
	m_pHealRange->m_bActive = true;
}

void Priest::FirstSkillUp()
{
}

//void Priest::ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent)
//{
//	for (int i = 0; i < m_pProjectiles.size(); ++i)
//		if (m_pProjectiles[i])
//			m_pProjectiles[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
//
//	GameObject::ShadowRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender, pShaderComponent);
//}

Monster::Monster() : Character()
{
	m_fHp = 2500;
	m_fMaxHp = 2500;
}

Monster::~Monster()
{
}

void Monster::Animate(float fTimeElapsed)
{
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimation != 10)
		{
			m_pSkinnedAnimationController->m_CurrentAnimation = 10;
			m_pSkinnedAnimationController->SetTrackEnable(10, 2);
		}
		Character::Animate(fTimeElapsed);
		return;
	}
	if (m_bMoveState)	// 움직이는 중
	{
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		//XMFLOAT3 desPlayerPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (m_roleDesPlayer != ROLE::NONE_SELECT) {
			XMFLOAT3 desPlayerPos = g_Logic.GetPostion(m_roleDesPlayer);
			XMFLOAT3 desPlayerVector = Vector3::Subtract(desPlayerPos, GetPosition());
			float playerDistance = Vector3::Length(desPlayerVector);
			desPlayerVector = Vector3::Normalize(desPlayerVector);
			m_lockBossRoute.lock();
			if (playerDistance < 120.0f && m_BossRoute.empty()) {
				m_lockBossRoute.unlock();
				/*m_lockBossRoute.lock();
				if (!m_BossRoute.empty())
					int currentNodeIdx = m_BossRoute.front();
				m_lockBossRoute.unlock();
				bool bossAndPlayerOnSameIdx = g_bossMapData.GetTriangleMesh(currentNodeIdx).IsOnTriangleMesh(desPlayerPos);*/

				//if (bossAndPlayerOnSameIdx) {
				float ChangingAngle = Vector3::Angle(desPlayerVector, GetLook());
				if (ChangingAngle > 1.6f) {
					bool OnRight = (Vector3::DotProduct(GetRight(), desPlayerVector) > 0) ? true : false;
					if (OnRight) {
						Rotate(&up, 90.0f * fTimeElapsed);
						m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
					}
					else {
						Rotate(&up, -90.0f * fTimeElapsed);
						m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
					}
				}
				if (playerDistance >= 42.0f)
					MoveForward(50 * fTimeElapsed);
				Character::Animate(fTimeElapsed);
				return;
				//m_position = Vector3::Add(m_position, Vector3::ScalarProduct(desPlayerVector, fTimeElapsed, false));//틱마다 움직임					

				//Rotate(&up, -90.0f * fTimeElapsed);
				//m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
				//임시로 했음 수정해야됨
			//}
			}
			else {
				//m_lockBossRoute.lock();
				if (!m_BossRoute.empty()) {
					int currentNodeIdx = m_BossRoute.front();
					m_lockBossRoute.unlock();
					XMFLOAT3 destinationNodeCenter = g_bossMapData.GetTriangleMesh(currentNodeIdx).GetCenter();	//노드의 위치
					m_xmf3Destination = destinationNodeCenter;
					XMFLOAT3 desNodeVector = Vector3::Subtract(m_xmf3Destination, GetPosition());
					float desNodeDistance = Vector3::Length(desNodeVector);
					desNodeVector = Vector3::Normalize(desNodeVector);
					bool OnRight = (Vector3::DotProduct(GetRight(), Vector3::Normalize(desNodeVector)) > 0) ? true : false;
					float ChangingAngle = Vector3::Angle(desNodeVector, GetLook());

					bool isOnNode = g_bossMapData.GetTriangleMesh(currentNodeIdx).IsOnTriangleMesh(m_position);

					if (desNodeDistance > 70.0f) { //목적지와 거리가 10이상 이라면
						if (desNodeDistance > 50.0f) {
							if (ChangingAngle > 40.0f) {
								if (OnRight) {
									Rotate(&up, 90.0f * fTimeElapsed);
									m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
								}
								else {
									Rotate(&up, -90.0f * fTimeElapsed);
									m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
								}
							}
							else {
								if (ChangingAngle > 1.6f) {
									if (OnRight) {
										Rotate(&up, 90.0f * fTimeElapsed);
										m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
									}
									else {
										Rotate(&up, -90.0f * fTimeElapsed);
										m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
									}
								}
								MoveForward(50 * fTimeElapsed);
							}
						}
						else {//현재 노드에 가까울때
							if (ChangingAngle > 1.6f)
							{
								if (OnRight) {
									Rotate(&up, 90.0f * fTimeElapsed);
									m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
								}
								else {
									Rotate(&up, -90.0f * fTimeElapsed);
									m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
								}
							}
							MoveForward(50 * fTimeElapsed);
						}
						//std::cout << "BossPos: " << m_position.x << "0, " << m_position.z << std::endl;
					}
					else {
						m_lockBossRoute.lock();
						//m_onIdx = m_BossRoute.front();
						m_BossRoute.pop();
						if (m_BossRoute.size() != 0) {
							DirectX::XMFLOAT3 center = g_bossMapData.GetTriangleMesh(m_BossRoute.front()).GetCenter();
							m_xmf3Destination = center;//목적지 다음 노드의 센터
						}
						m_lockBossRoute.unlock();
					}


				}
				else m_lockBossRoute.unlock();


				//else if (Vector3::Length(des) >= 14.0f) {
				//	//SetPosition(m_xmf3Destination);
				//}
			}
		}
	}
	Character::Animate(fTimeElapsed);
}

Projectile::Projectile(entity_id eid) : GameObject(eid)
{
	m_xmf3startPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	m_xmf3direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_fSpeed = 0.0f;
	m_bActive = false;
	m_RAttack = false;
	m_Angle = 0.0f;
}

Projectile::~Projectile()
{
}

void Projectile::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	m_pd3dGraphicsRootSignature = pd3dGraphicsRootSignature;
	ComponentBase* pComponent = GetComponent(component_id::RENDER_COMPONENT);
	if (pComponent != NULL)
	{
		m_pRenderComponent = static_cast<RenderComponent*>(pComponent);
	}
	ComponentBase* pSphereMeshComponent = GetComponent(component_id::SPHEREMESH_COMPONENT);
	if (pSphereMeshComponent != NULL)
	{
		m_pSphereComponent = static_cast<SphereMeshComponent*>(pSphereMeshComponent);
		m_pSphereComponent->BuildObject(pd3dDevice, pd3dCommandList, m_fBoundingSize, 20, 20);
		m_pMeshComponent = m_pSphereComponent;
	}
	ComponentBase* pSphereShaderComponent = GetComponent(component_id::SPHERE_COMPONENT);
	if (pSphereShaderComponent != NULL)
	{
		m_pShaderComponent = static_cast<SphereShaderComponent*>(pSphereShaderComponent);
		m_pShaderComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
		m_pShaderComponent->CreateCbvSrvDescriptorHeaps(pd3dDevice, 1, 0);
		m_pShaderComponent->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_pShaderComponent->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObjects, ncbElementBytes);
		m_pShaderComponent->SetCbvGPUDescriptorHandlePtr(m_pShaderComponent->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
	}
	ComponentBase* pLoadedmodelComponent = GetComponent(component_id::LOADEDMODEL_COMPONET);
	if (pLoadedmodelComponent != NULL)
	{
		CLoadedModelInfoCompnent* pModel = nullptr;
		//MaterialComponent::PrepareShaders(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pd3dcbGameObjects);
		if (m_pLoadedModelComponent == nullptr)
		{
			pModel = static_cast<CLoadedModelInfoCompnent*>(pLoadedmodelComponent);
			pModel = LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
				pd3dGraphicsRootSignature, pszModelNames, NULL, true);//NULL ->Shader
		}
		else
		{
			pModel = m_pLoadedModelComponent;
		}
		SetChild(pModel->m_pModelRootObject, true);
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

Arrow::Arrow() : Projectile()
{
	m_fSpeed = 150.0f;
	m_xmf3TargetPos = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_ArrowType = 0;
	m_ArrowPos = 0;
	// Y값이 마우스 회전 범위 안쪽이면 일반 화살 아니면 꺾이는 화살
}

Arrow::~Arrow()
{
}

void Arrow::Animate(float fTimeElapsed)
{
	XMFLOAT3 xmf3CurrentPos = GetPosition();
	float moveDist = Vector3::Length(Vector3::Subtract(xmf3CurrentPos, m_xmf3startPosition));
	if (moveDist > 200.0f)
	{
		m_bActive = false;
		m_RAttack = false;
		return;
	}

	if (Vector3::Length(xmf3CurrentPos) >= PLAYER_MAX_RANGE)
	{
		m_bActive = false;
		m_RAttack = false;
	}

	if (m_ArrowType == 1)
	{
		if (m_ArrowPos > 1.0f)
		{
			m_bActive = false;
			return;
		}

		m_xmf3TargetPos = g_Logic.m_MonsterSession.m_currentPlayGameObject->GetPosition();
		XMFLOAT3 controlPoint = Vector3::Add(m_xmf3startPosition, m_xmf3direction, 100.0f);
		XMFLOAT3 firstVec = Vector3::Subtract(controlPoint, m_xmf3startPosition);
		XMFLOAT3 secondVec = Vector3::Subtract(m_xmf3TargetPos, controlPoint);

		XMFLOAT3 tempPoint1 = Vector3::Add(Vector3::ScalarProduct(m_xmf3startPosition, (1 - m_ArrowPos), false), Vector3::ScalarProduct(controlPoint, m_ArrowPos, false));
		XMFLOAT3 tempPoint2 = Vector3::Add(Vector3::ScalarProduct(controlPoint, (1 - m_ArrowPos), false), Vector3::ScalarProduct(m_xmf3TargetPos, m_ArrowPos, false));
		XMFLOAT3 curPos = Vector3::Add(Vector3::ScalarProduct(tempPoint1, (1 - m_ArrowPos), false), Vector3::ScalarProduct(tempPoint2, m_ArrowPos, false));

		SetPosition(curPos);
		SetLook(Vector3::Subtract(tempPoint2, tempPoint1));
		m_ArrowPos += fTimeElapsed;	// 스플라인 곡선에서의 t의 역할
	}
	else
	{
		SetLook(m_xmf3direction);
		MoveForward(fTimeElapsed * m_fSpeed);
	}

	if (m_VisualizeSPBB) m_VisualizeSPBB->SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z));
}

void Arrow::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
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
	if (m_VisualizeSPBB) m_VisualizeSPBB->SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z));
	if (Vector3::Length(xmf3CurrentPos) >= PLAYER_MAX_RANGE) m_bActive = false;
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

TrailObject::TrailObject(entity_id eid) : GameObject(eid)
{

}

TrailObject::~TrailObject()
{
}

void TrailObject::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

NormalMonster::NormalMonster() : Character()
{
}

NormalMonster::~NormalMonster()
{
}

void NormalMonster::Animate(float fTimeElapsed)
{
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_DIE)
		{
			pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_DIE, CharacterAnimation::CA_DIE };
			m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
			m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
		}
		return;
	}
	if (gGameFramework.GetCurrentGameState() == GAME_STATE::GS_FIRST_STAGE)
	{
		if (CheckAnimationEnd(CA_FIRSTSKILL) == true)
		{
			pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
			m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
			m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
		}
#ifdef LOCAL_TASK
		if (!m_bHaveTarget)
		{
			XMFLOAT3 currentPos = GetPosition();
			for (auto& session : g_Logic.m_inGamePlayerSession)
			{
				if (session.m_id == -1) continue;
				XMFLOAT3 targetPos = session.m_currentPlayGameObject->GetPosition();
				XMFLOAT3 toTarget = Vector3::Subtract(targetPos, currentPos);
				float targetLength = Vector3::Length(toTarget);
				if (targetLength < 200.0f)
				{
					m_iTargetID = session.m_id;
					m_bHaveTarget = true;
					break;
				}
			}
		}
#endif
	}
	else
	{
		if (gGameFramework.GetCurrentGameState() == GAME_STATE::GS_SECOND_STAGE_FIRST_PHASE)
		{
			return;
		}
		else
		{
			if (m_bCanActive == false)
			{
				if (CheckAnimationEnd(CA_FIRSTSKILL) == true)
				{
					m_bCanActive = true;
					pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
					m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
					m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
				}
				else
				{
					GameObject::Animate(fTimeElapsed);
					return;
				}
			}
#ifdef LOCAL_TASK
			if (!m_bHaveTarget)
			{
				// 플레이어 4명 포지션과 거리 계산해서 목표 설정
				XMFLOAT3 currentPos = GetPosition();
				float farDistance= FLT_MAX;
				for (auto& session : g_Logic.m_inGamePlayerSession)
				{
					if (session.m_id == -1) continue;
					XMFLOAT3 targetPos = session.m_currentPlayGameObject->GetPosition();
					XMFLOAT3 toTarget = Vector3::Subtract(targetPos, currentPos);
					float targetLength = Vector3::Length(toTarget);
					if (targetLength < farDistance)
					{
						m_bHaveTarget = true;
						m_iTargetID = session.m_id;
						farDistance = targetLength;
					}
				}
			}
#endif
		}
	}

	if (m_bHaveTarget)
	{
		auto findRes = find_if(g_Logic.m_inGamePlayerSession.begin(), g_Logic.m_inGamePlayerSession.end(), [&](auto& fObj) {
			return fObj.m_id == m_iTargetID;
			});
		if (findRes == g_Logic.m_inGamePlayerSession.end())
			return;
		m_xmf3Destination = findRes->m_currentPlayGameObject->GetPosition();

		XMFLOAT3 MyPos = GetPosition();
		XMFLOAT3 des = XMFLOAT3(m_xmf3Destination.x - MyPos.x, 0.0f, m_xmf3Destination.z - MyPos.z);
		float distance = Vector3::Length(des);

		if (distance < 30.0f)
		{
			m_bMoveState = false;
			m_bOnAttack = true;
		}
		else if (distance >= 30.0f)
		{
			m_bOnAttack = false;
			m_bMoveState = true;
			SetLook(Vector3::Normalize(des));
			MoveForward(25 * fTimeElapsed);
		}
	}
	else
	{
		m_bMoveState = false;
		m_bOnAttack = false;
	}

	pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_NOTHING, CharacterAnimation::CA_NOTHING };
	if (m_bMoveState)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_MOVE)
			NextAnimations = { CharacterAnimation::CA_MOVE, CharacterAnimation::CA_MOVE };
	}
	else if (m_bOnAttack)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_ATTACK)
		{
			NextAnimations = { CharacterAnimation::CA_ATTACK, CharacterAnimation::CA_ATTACK };
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		}
	}
	else
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_IDLE)
			NextAnimations = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
	}

	if (NextAnimations.first != CharacterAnimation::CA_NOTHING)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
		m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
	}

	GameObject::Animate(fTimeElapsed);
}

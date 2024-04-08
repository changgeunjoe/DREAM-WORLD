#include "stdafx.h"
#include "Animation.h"
#include "Projectile.h"
#include "EffectObject.h"
#include "GameFramework.h"
#include "TrailComponent.h"
#include "sound/GameSound.h"
#include "RangedCharacter.h"
#include "GameobjectManager.h"
#include "Network/Logic/Logic.h"
#include "Network/NetworkHelper.h"
#include "CharacterEvent.h"

extern bool GameEnd;
extern Logic g_Logic;
extern GameSound g_sound;
extern NetworkHelper g_NetworkHelper;
extern CGameFramework gGameFramework;

constexpr float ATTACK1_ATTACK_POINT = 0.24f;
constexpr float ATTACK4_ATTACK_POINT = 0.60f;

RangedCharacter::RangedCharacter() : Player()
{
}

RangedCharacter::RangedCharacter(ROLE role) : Player(role)
{
}

RangedCharacter::~RangedCharacter()
{
}

void RangedCharacter::Move(float fTimeElapsed)
{
	UpdateInterpolateData();
	if (m_interpolateData->GetInterpolateState() == CharacterEvent::INTERPOLATE_STATE::SET_POSITION) {
		SetPosition(m_interpolateData->GetInterpolatePosition());
		if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
		g_sound.Play("WalkSound", CalculateDistanceSound());
		return;
	}
	DIRECTION tempDir = m_currentDirection;
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
	case DIRECTION::FRONT: MoveForward(1, fTimeElapsed); break;
	case DIRECTION::FRONT | DIRECTION::RIGHT: MoveDiagonal(1, 1, fTimeElapsed); break;
	case DIRECTION::RIGHT: MoveStrafe(1, fTimeElapsed); break;
	case DIRECTION::BACK | DIRECTION::RIGHT: MoveDiagonal(-1, 1, fTimeElapsed);  break;
	case DIRECTION::BACK: MoveForward(-1, fTimeElapsed); break;
	case DIRECTION::BACK | DIRECTION::LEFT: MoveDiagonal(-1, -1, fTimeElapsed); break;
	case DIRECTION::LEFT: MoveStrafe(-1, fTimeElapsed); break;
	case DIRECTION::FRONT | DIRECTION::LEFT: MoveDiagonal(1, -1, fTimeElapsed); break;
	default: break;
	}

	
}

void RangedCharacter::SetLookDirection()
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	SetLook(xmf3Look);
}

Archer::Archer() : RangedCharacter(ROLE::ARCHER)
{
	m_fMaxHp = 400.0f;
	m_fTempHp = 100.0f;
	m_fHp = 100.0f;
	m_fSpeed = 50.0f;
	m_fDamage = 80.0f;
	m_CameraLook = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_skillCoolTime = { std::chrono::seconds(10), std::chrono::seconds(15) };
	m_skillDuration = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0], std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
}

Archer::~Archer()
{
	for (auto& i : m_ppProjectiles)
	{
		if (i != nullptr)
			delete i;
	}
}

void Archer::Reset()
{
	m_CameraLook = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_bZoomInState = false;
	m_xmf3TargetPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Player::Reset();
}

void Archer::Attack()
{
	if (m_pCamera) {
		//줌인에 대한 정보 필요할듯함

		int power = 0;//줌인 정보
		//g_NetworkHelper.SendCommonAttackExecute(GetLook(), power);//두번째 인자 - 줌인 정도
	}
}

void Archer::SetArrow(Projectile** ppArrow)
{
	for (int i = 0; i < MAX_ARROW; ++i)
	{
		m_ppProjectiles[i] = ppArrow[i];
		m_ppProjectiles[i]->m_bActive = false;
	}
}

void Archer::SetAdditionArrowForQSkill(Arrow** ppArrow)
{
	for (int i = 0; i < m_ppArrowForQSkill.size(); ++i)
		m_ppArrowForQSkill[i] = ppArrow[i];
}

void Archer::SetAdditionArrowForESkill(Arrow** ppArrow)
{
	for (int i = 0; i < m_ppArrowForESkill.size(); ++i)
		m_ppArrowForESkill[i] = ppArrow[i];
}

void Archer::ResetArrow()
{
	for (int i = 0; i < MAX_ARROW; ++i)
	{
		if (m_ppProjectiles[i])
		{
			m_ppProjectiles[i]->GetTrailComponent()->SetRenderingTrail(m_ppProjectiles[i]->m_bActive);
			if (!m_ppProjectiles[i]->m_bActive)
				m_ppProjectiles[i]->SetPosition(GetPosition());
		}
	}

	for (int i = 0; i < m_ppArrowForQSkill.size(); ++i)
	{
		if (m_ppArrowForQSkill[i])
		{
			m_ppArrowForQSkill[i]->GetTrailComponent()->SetRenderingTrail(m_ppArrowForQSkill[i]->m_bActive);
			if (!m_ppArrowForQSkill[i]->m_bActive)
				m_ppArrowForQSkill[i]->SetPosition(GetPosition());
		}
	}

	for (int i = 0; i < m_ppArrowForQSkill.size(); ++i)
	{
		if (m_ppArrowForESkill[i])
		{
			m_ppArrowForESkill[i]->GetTrailComponent()->SetRenderingTrail(m_ppArrowForESkill[i]->m_bActive);
			if (!m_ppArrowForESkill[i]->m_bActive)
				m_ppArrowForESkill[i]->SetPosition(GetPosition());
		}
	}
}

void Archer::Animate(float fTimeElapsed)
{
	VisualizeSkillCoolTime();

	m_fTimeElapsed = fTimeElapsed;
	if (m_bLButtonClicked)
		ZoomInCamera();

	VisualizeSkillCoolTime();
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->ResetTrack();
			m_pSkinnedAnimationController->m_CurrentAnimations = { CharacterAnimation::CA_DIE, CharacterAnimation::CA_DIE };
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);

			m_bQSkillClicked = false;
			m_bESkillClicked = false;
			m_bLButtonClicked = false;
			m_bRButtonClicked = false;
			m_bCanAttack = true;
			m_bOnAttack = false;
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations = { CharacterAnimation::CA_VICTORY, CharacterAnimation::CA_VICTORY };
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}

	if (m_pSkinnedAnimationController->m_CurrentAnimations.first == CharacterAnimation::CA_ATTACK)
	{
		if (GetAnimationProgressRate(CA_ATTACK) > ATTACK4_ATTACK_POINT)
		{
			if (m_bCanAttack && m_pCamera && !m_bZoomInState)
			{
				//	g_sound.NoLoopPlay("AcherAttackSound", 1.0f);
				ShootArrow();
				m_bCanAttack = false;
			}
			else if (m_bCanAttack && m_bQSkillClicked && m_bESkillClicked && m_pCamera)
			{
				ShootArrow();
				m_bCanAttack = false;
			}
		}
		if (CheckAnimationEnd(CA_ATTACK))
		{
			m_bOnAttack = false;
			m_bCanAttack = true;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
		}
		else
		{
			m_bOnAttack = true;
		}
	}

	if (m_bMoveState)
	{
		if (m_bLButtonClicked || m_bQSkillClicked || m_bESkillClicked || m_bOnAttack)
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
		}
		else
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_MOVE;
		}

		AfterAnimation.second = CharacterAnimation::CA_MOVE;
	}
	else
	{
		if (m_bLButtonClicked || m_bQSkillClicked || m_bESkillClicked || m_bOnAttack)
		{
			AfterAnimation = { CharacterAnimation::CA_ATTACK, CharacterAnimation::CA_ATTACK };
		}
		else if (!m_bOnAttack)
		{// IDLE
			AfterAnimation = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
		}
	}

	ResetArrow();
	ChangeAnimation(AfterAnimation);
	//SetLookDirection();
	Move(fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Archer::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Archer::ZoomInCamera()
{
	if (!(m_pSkinnedAnimationController->m_CurrentAnimations.first == CharacterAnimation::CA_ATTACK &&
		CheckAnimationEnd(CA_ATTACK) == false)
		|| m_bLButtonClicked == true)
	{
		g_sound.Play("ArrowBow", CalculateDistanceSound());
		float currentTime = gGameFramework.GetScene()->GetObjectManager()->GetTotalProgressTime();
		if (m_iRButtionCount == 0)
		{
			// 줌인을 위해서 애니메이션 타입 및 속도 변경
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_HALF;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 0.3f;
			m_fZoomInStartTime = gGameFramework.GetScene()->GetObjectManager()->GetTotalProgressTime();
		}
		if (currentTime - m_fZoomInStartTime < 1.8f)
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

void Archer::FirstSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::ARCHER)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
		if (m_skillCoolTime[0] > duration) return;
		g_NetworkHelper.Send_SkillInput_Q();
		m_skillInputTime[0] = std::chrono::high_resolution_clock::now();
	}
	m_bQSkillClicked = true;
}

void Archer::SecondSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::ARCHER)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[1]);
		if (m_skillCoolTime[1] > duration)
		{
			gGameFramework.GetScene()->GetObjectManager()->m_bPickingenemy = false;
			return;
		}
		g_NetworkHelper.Send_SkillInput_E();
		m_skillInputTime[1] = std::chrono::high_resolution_clock::now();
	}
	m_bESkillClicked = true;
}

void Archer::SetLButtonClicked(bool bLButtonClicked)
{
	m_bLButtonClicked = bLButtonClicked;
	if (bLButtonClicked == false)
	{
		if (m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType == ANIMATION_TYPE_HALF)
		{
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_LOOP;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 1.0f;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = true;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fProgressRate = 0.0f;

			m_bOnAttack = false;
			m_bCanAttack = true;
			m_CameraLook = m_xmf3RotateAxis;
			if (m_pCamera)
				ShootArrow();
		}
	}
	else
	{
		m_bZoomInState = true;
	}
}

void Archer::ShootArrow()//스킬
{
	m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
	if (!m_bQSkillClicked && !m_bESkillClicked) {
		g_sound.Pause("ArrowBow");
		g_sound.NoLoopPlay("AcherAttackSound", CalculateDistanceSound() - 0.2);

	}
	if (m_bQSkillClicked == true)
	{
		if (g_Logic.GetMyRole() == ROLE::ARCHER)
			g_NetworkHelper.Send_SkillExecute_Q(Vector3::Normalize(GetObjectLook()));
		g_sound.NoLoopPlay("ArcherQSkillSound", CalculateDistanceSound());
		for (int i = 0; i < 3; ++i)//서버에 옮겨야됨
		{
			XMFLOAT3 objectLook = GetObjectLook();
			XMFLOAT3 objectRight = GetRight();
			XMFLOAT3 objPosition = GetPosition();
			objPosition.y = 6.0f + (i % 2) * 4.0f;
			objPosition = Vector3::Add(objPosition, objectRight, (1 - i) * 4.0f);
			objPosition = Vector3::Add(objPosition, objectLook, 1.0f);

			m_ppArrowForQSkill[i]->m_xmf3direction = Vector3::Normalize(objectLook);
			m_ppArrowForQSkill[i]->m_xmf3startPosition = objPosition;
			m_ppArrowForQSkill[i]->SetPosition(objPosition);
			m_ppArrowForQSkill[i]->m_fSpeed = 250.0f;
			m_ppArrowForQSkill[i]->m_bActive = true;
		}
		m_bQSkillClicked = false;
	}
	else if (m_bESkillClicked == true)
	{
		g_sound.NoLoopPlay("ArcherESkillSound", CalculateDistanceSound());
		XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		if (g_Logic.GetMyRole() == ROLE::ARCHER)
			g_NetworkHelper.Send_SkillExecute_E(m_xmf3TargetPos);
		for (int i = 0; i < m_ppArrowForESkill.size(); ++i)
		{
			XMFLOAT3 objPosition = GetPosition();
			XMFLOAT3 objectLook = Vector3::Normalize(Vector3::Subtract(m_xmf3TargetPos, objPosition));
			XMFLOAT3 objectRight = Vector3::Normalize(Vector3::CrossProduct(xmf3Up, objectLook, true));
			objPosition.y = RandomValue(8.0f, 24.0f);
			objPosition = Vector3::Add(objPosition, objectRight, -8.0f + (i % 5) * 4.0f);
			objPosition = Vector3::Add(objPosition, objectLook, 3.0f);
			objectLook.y = 0.85f;

			m_ppArrowForESkill[i]->m_xmf3Offset = Vector3::Subtract(objPosition, GetPosition());
			m_ppArrowForESkill[i]->m_xmf3TargetPos = Vector3::Add(m_xmf3TargetPos, m_ppArrowForESkill[i]->m_xmf3Offset);
			m_ppArrowForESkill[i]->m_fArrowPos = -RandomValue(0.0f, 0.1f);
			m_ppArrowForESkill[i]->m_fSpeed = RandomValue(0.5f, 2.0f);
			m_ppArrowForESkill[i]->m_xmf3direction = Vector3::Normalize(objectLook);
			m_ppArrowForESkill[i]->m_xmf3startPosition = objPosition;
			m_ppArrowForESkill[i]->SetPosition(objPosition);
			m_ppArrowForESkill[i]->m_bActive = true;
			m_ppArrowForESkill[i]->m_iArrowType = 1;
		}
		m_bESkillClicked = false;
	}
	else if (m_bZoomInState == true)
	{
		float chargingTime = m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition;
		float fullTime = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fLength * 0.5f;
		float arrowSpeed = pow((chargingTime / fullTime), 2);
		float fProgress = std::clamp(arrowSpeed, 0.65f, 1.0f);
		arrowSpeed = (chargingTime / fullTime > 0.5f) ? arrowSpeed * 100.0f : -1.0f;
		if (arrowSpeed > 10)
		{
			fProgress = fProgress * 250.0f;
			XMFLOAT3 xmf3ArrowStartPosition = XMFLOAT3(GetPosition().x, GetPosition().y + 8.0f, GetPosition().z);
			XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
			XMFLOAT3 xmf3CameraDirection = Vector3::Normalize(m_pCamera->GetLookVector());
			XMFLOAT3 xmf3Destination = Vector3::Add(xmf3CameraPosition, xmf3CameraDirection, fProgress);
			XMFLOAT3 xmf3ArrowDirection = Vector3::Normalize(Vector3::Subtract(xmf3Destination, xmf3ArrowStartPosition));
			if (xmf3ArrowDirection.y < 0.0f)
				xmf3ArrowDirection.y = 0.0f;
			m_ppProjectiles[m_nProjectiles]->m_xmf3direction = xmf3ArrowDirection;
			m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = xmf3ArrowStartPosition;
			m_ppProjectiles[m_nProjectiles]->SetPosition(m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition);

			m_ppProjectiles[m_nProjectiles]->m_bActive = true;
			m_ppProjectiles[m_nProjectiles]->m_fSpeed = 150.0f;
			int arrowPower = (arrowSpeed / 30) - 1;
			switch (arrowPower)
			{
			case 0:
				m_ppProjectiles[m_nProjectiles]->m_fSpeed = 100.0f;
				break;
			case 1:
				m_ppProjectiles[m_nProjectiles]->m_fSpeed = 140.0f;
				break;
			case 2:
				m_ppProjectiles[m_nProjectiles]->m_fSpeed = 200.0f;
				break;
			}
			if (g_Logic.GetMyRole() == ROLE::ARCHER)
				g_NetworkHelper.SendPowerAttackExecute(m_ppProjectiles[m_nProjectiles]->m_xmf3direction, arrowPower);
			m_nProjectiles++;
		}
		m_bZoomInState = false;
		Player::RbuttonUp(m_CameraLook);
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
}

void Archer::ShootArrow(const XMFLOAT3& xmf3Direction)
{
	// 플레이어 캐릭터가 아닐 때
	m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
	if (!m_bQSkillClicked && !m_bESkillClicked) {
		g_sound.Pause("ArrowBow");
		g_sound.NoLoopPlay("AcherAttackSound", CalculateDistanceSound() - 0.2);

	}
	if (m_bQSkillClicked == true)
	{
		g_sound.NoLoopPlay("ArcherQSkillSound", CalculateDistanceSound());
		for (int i = 0; i < 3; ++i)//서버에 옮겨야됨
		{
			XMFLOAT3 objectLook = GetObjectLook();
			XMFLOAT3 objectRight = GetRight();
			XMFLOAT3 objPosition = GetPosition();
			objPosition.y = 6.0f + (i % 2) * 4.0f;
			objPosition = Vector3::Add(objPosition, objectRight, (1 - i) * 4.0f);
			objPosition = Vector3::Add(objPosition, objectLook, 1.0f);

			m_ppArrowForQSkill[i]->m_xmf3direction = Vector3::Normalize(xmf3Direction);
			m_ppArrowForQSkill[i]->m_xmf3startPosition = objPosition;
			m_ppArrowForQSkill[i]->SetPosition(objPosition);
			m_ppArrowForQSkill[i]->m_fSpeed = 250.0f;
			m_ppArrowForQSkill[i]->m_bActive = true;
		}
		m_bQSkillClicked = false;
	}
	else if (m_bESkillClicked == true)
	{
		g_sound.NoLoopPlay("ArcherESkillSound", CalculateDistanceSound());
		m_xmf3TargetPos = xmf3Direction;
		XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		for (int i = 0; i < m_ppArrowForESkill.size(); ++i)
		{
			XMFLOAT3 objPosition = GetPosition();
			XMFLOAT3 objectLook = Vector3::Normalize(Vector3::Subtract(m_xmf3TargetPos, objPosition));
			XMFLOAT3 objectRight = Vector3::Normalize(Vector3::CrossProduct(xmf3Up, objectLook, true));
			objPosition.y = RandomValue(8.0f, 24.0f);
			objPosition = Vector3::Add(objPosition, objectRight, -8.0f + (i % 5) * 4.0f);
			objPosition = Vector3::Add(objPosition, objectLook, 3.0f);
			objectLook.y = 0.85f;

			m_ppArrowForESkill[i]->m_xmf3Offset = Vector3::Subtract(objPosition, GetPosition());
			m_ppArrowForESkill[i]->m_xmf3TargetPos = Vector3::Add(m_xmf3TargetPos, m_ppArrowForESkill[i]->m_xmf3Offset);
			m_ppArrowForESkill[i]->m_fArrowPos = -RandomValue(0.0f, 0.0f);
			m_ppArrowForESkill[i]->m_fSpeed = RandomValue(0.5f, 2.0f);
			m_ppArrowForESkill[i]->m_xmf3direction = Vector3::Normalize(objectLook);
			m_ppArrowForESkill[i]->m_xmf3startPosition = objPosition;
			m_ppArrowForESkill[i]->SetPosition(objPosition);
			m_ppArrowForESkill[i]->m_bActive = true;
			m_ppArrowForESkill[i]->m_iArrowType = 1;
		}
		m_bESkillClicked = false;
	}
	else if (m_bZoomInState == true)
	{
		XMFLOAT3 objPosition = GetPosition();
		objPosition.y += 8.0f;
		m_ppProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
		m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = objPosition;
		m_ppProjectiles[m_nProjectiles]->SetPosition(objPosition);
		m_ppProjectiles[m_nProjectiles]->m_fSpeed = 150.0f;
		m_ppProjectiles[m_nProjectiles]->m_bActive = true;

		m_nProjectiles++;
		m_bZoomInState = false;
		Player::RbuttonUp();
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
}

void Archer::SetStage1Position()
{
	SetLook(XMFLOAT3(0, 0, 1));
	SetPosition(XMFLOAT3(-1340.84f, 0, -1520.93f));
	m_xmf3RotateAxis = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Archer::SetBossStagePostion()
{
	SetPosition(XMFLOAT3(123, 0, -293));
}

Mage::Mage() : RangedCharacter(ROLE::MAGE)
{
	m_fMaxHp = 500.0f;
	m_fTempHp = 100.0f;
	m_fHp = 100.0f;
	m_fSpeed = 50.0f;
	m_fDamage = 80.0f;

	m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(10) };
	m_skillDuration = { std::chrono::seconds(10), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0], std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
}

Mage::~Mage()
{
}

void Mage::Reset()
{
	m_fHealTime = 0.0f;
	Player::Reset();
}

void Mage::Animate(float fTimeElapsed)
{
	VisualizeSkillCoolTime();

	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->ResetTrack();
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);

			m_bQSkillClicked = false;
			m_bESkillClicked = false;
			m_bLButtonClicked = false;
			m_bRButtonClicked = false;
			m_bCanAttack = true;
			m_bOnAttack = false;
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

	if (GetAnimationProgressRate(CA_ATTACK) > FLT_EPSILON)
	{
		m_bOnAttack = true;
	}
	if (GetAnimationProgressRate(CA_ATTACK) > ATTACK1_ATTACK_POINT)
	{
		if (m_bCanAttack && m_pCamera && !m_bESkillClicked)
		{
			Attack();
			m_bCanAttack = false;
		}
	}
	if (CheckAnimationEnd(CA_ATTACK) == true)
	{
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
		m_bCanAttack = true;
		m_bOnAttack = false;
		if (m_bESkillClicked) m_bESkillClicked = false;
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bLButtonClicked || m_bESkillClicked)
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bLButtonClicked || m_bESkillClicked)
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;

		}
		else if (!m_bOnAttack)					// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}

	ChangeAnimation(AfterAnimation);

	if (m_pHealRange->m_bActive)
	{
		UpdateEffect();
	}

	//SetLookDirection();
	Move(fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Mage::Attack()
{
	g_sound.NoLoopPlay("MageAttackSound", CalculateDistanceSound());
	m_nProjectiles = (m_nProjectiles < 10) ? m_nProjectiles : m_nProjectiles % 10;
	XMFLOAT3 ObjectLookVector = GetLook();
	ObjectLookVector.y = -m_xmf3RotateAxis.x / 90.0f;

	XMFLOAT3 xmf3LanceStartPosition = XMFLOAT3(GetPosition().x, GetPosition().y + 8.0f, GetPosition().z);
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	XMFLOAT3 xmf3CameraDirection = Vector3::Normalize(m_pCamera->GetLookVector());
	XMFLOAT3 xmf3Destination = Vector3::Add(xmf3CameraPosition, xmf3CameraDirection, 200.0f);
	XMFLOAT3 xmf3LanceDirection = Vector3::Normalize(Vector3::Subtract(xmf3Destination, xmf3LanceStartPosition));
	if (xmf3LanceDirection.y < 0.0f) xmf3LanceDirection.y = 0.0f;

	if (g_Logic.GetMyRole() == ROLE::MAGE)
		g_NetworkHelper.SendCommonAttackExecute(xmf3LanceDirection);

	m_ppProjectiles[m_nProjectiles]->m_xmf3direction = xmf3LanceDirection;
	m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = xmf3LanceStartPosition;
	m_ppProjectiles[m_nProjectiles]->SetPosition(m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition);
	m_ppProjectiles[m_nProjectiles]->m_bActive = true;
	m_ppProjectiles[m_nProjectiles]->m_fSpeed = 100.0f;
	m_nProjectiles++;
}

void Mage::Attack(const XMFLOAT3& xmf3Direction)
{
	g_sound.NoLoopPlay("MageAttackSound", CalculateDistanceSound());
	m_nProjectiles = (m_nProjectiles < 10) ? m_nProjectiles : m_nProjectiles % 10;
	XMFLOAT3 objectPosition = GetPosition();
	objectPosition.y += 8.0f;
	m_ppProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
	m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = objectPosition;
	m_ppProjectiles[m_nProjectiles]->SetPosition(objectPosition);
	m_ppProjectiles[m_nProjectiles]->m_fSpeed = 100.0f;
	m_ppProjectiles[m_nProjectiles]->m_bActive = true;
	m_nProjectiles++;
}

void Mage::SetProjectile(Projectile** pEnergyBall)
{
	for (int i = 0; i < 10; ++i)
	{
		m_ppProjectiles[i] = pEnergyBall[i];
		m_ppProjectiles[i]->m_bActive = false;
	}
}

void Mage::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	for (int i = 0; i < m_ppProjectiles.size(); ++i)
		if (m_ppProjectiles[i])
			m_ppProjectiles[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);

	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Mage::FirstSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::MAGE)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
		if (m_skillCoolTime[0] > duration) return;
		g_NetworkHelper.Send_SkillInput_Q();
		g_NetworkHelper.Send_SkillExecute_Q();
		m_skillInputTime[0] = std::chrono::high_resolution_clock::now();
	}
	m_bQSkillClicked = true;

#ifdef LOCAL_TASK
	StartEffect(0);
#endif
}

void Mage::SecondSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::MAGE)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[1]);
		if (m_skillCoolTime[1] > duration)
		{
			gGameFramework.GetScene()->GetObjectManager()->m_bPickingenemy = false;
			return;
		}
		g_NetworkHelper.Send_SkillInput_E();
		m_skillInputTime[1] = std::chrono::high_resolution_clock::now();
	}
	m_bESkillClicked = true;
}

void Mage::StartEffect(int nSkillNum)
{
	m_pHealRange->m_bActive = true;
	g_sound.Play("HealSound", CalculateDistanceSound() - 0.2);
	UpdateEffect();
}

void Mage::EndEffect(int nSkillNum)
{
	g_sound.Pause("HealSound");
	m_pHealRange->m_bActive = false;
	EffectObject** HealingEffects = gGameFramework.GetScene()->GetObjectManager()->GetHealingEffectArr();
	for (int i = 0; i < 4; ++i)
	{
		HealingEffects[i]->SetActive(false);
	}
	m_bQSkillClicked = false;
}

void Mage::UpdateEffect()
{
	array<Character*, 4> pOtherCharacter;
	pOtherCharacter[0] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::WARRIOR);
	pOtherCharacter[1] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::MAGE);
	pOtherCharacter[2] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
	pOtherCharacter[3] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);

	XMFLOAT3 myPos = GetPosition();

	int i = 0;
	for (auto p : pOtherCharacter)
	{
		XMFLOAT3 targetPos = p->GetPosition();
		float distance = Vector3::Length(Vector3::Subtract(targetPos, myPos));
		EffectObject** HealingEffects = gGameFramework.GetScene()->GetObjectManager()->GetHealingEffectArr();
		HealingEffects[i++]->SetActive(static_cast<bool>(distance < 75.0f));
	}
}

void Mage::SetStage1Position()
{
	SetLook(XMFLOAT3(0, 0, 1));
	SetPosition(XMFLOAT3(-1370.45, 0, -1450.89f));
	m_xmf3RotateAxis = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Mage::SetBossStagePostion()
{
	SetPosition(XMFLOAT3(20, 0, -285));
}

#include "stdafx.h"
#include "Animation.h"
#include "Projectile.h"
#include "EffectObject.h"
#include "GameFramework.h"
#include "TrailComponent.h"
#include "MeleeCharacter.h"
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

MeleeCharacter::MeleeCharacter() : Player()
{
}

MeleeCharacter::MeleeCharacter(ROLE r) : Player(r)
{
}

MeleeCharacter::~MeleeCharacter()
{
}

void MeleeCharacter::Move(float fTimeElapsed)
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
	case DIRECTION::FRONT:
	case DIRECTION::FRONT | DIRECTION::RIGHT:
	case DIRECTION::RIGHT:
	case DIRECTION::BACK | DIRECTION::RIGHT:
	case DIRECTION::BACK:
	case DIRECTION::BACK | DIRECTION::LEFT:
	case DIRECTION::LEFT:
	case DIRECTION::FRONT | DIRECTION::LEFT:
		MoveForward(1, fTimeElapsed);
		break;
	default: break;
	}
}

void MeleeCharacter::SetLookDirection()
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);

	XMFLOAT3 xmf3Rev = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fRotateAngle = -1.0f;

	DIRECTION tempDir = m_currentDirection;

	if (tempDir != DIRECTION::IDLE)
	{
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
		case DIRECTION::FRONT:						fRotateAngle = 0.0f;	break;
		case DIRECTION::LEFT | DIRECTION::FRONT:	fRotateAngle = 45.0f;	break;
		case DIRECTION::LEFT:						fRotateAngle = 90.0f;	break;
		case DIRECTION::BACK | DIRECTION::LEFT:		fRotateAngle = 135.0f;	break;
		case DIRECTION::BACK:						fRotateAngle = 180.0f;	break;
		case DIRECTION::RIGHT | DIRECTION::BACK:	fRotateAngle = 225.0f;	break;
		case DIRECTION::RIGHT:						fRotateAngle = 270.0f;	break;
		case DIRECTION::FRONT | DIRECTION::RIGHT:	fRotateAngle = 315.0f;	break;
		default:
			return;
		}

		fRotateAngle = fRotateAngle * (3.14159265359 / 180.0f);
		xmf3Rev.x = xmf3Look.x * cos(fRotateAngle) - xmf3Look.z * sin(fRotateAngle);
		xmf3Rev.z = xmf3Look.x * sin(fRotateAngle) + xmf3Look.z * cos(fRotateAngle);
		xmf3Rev = Vector3::Normalize(xmf3Rev);
	}

	if ((xmf3Rev.x || xmf3Rev.y || xmf3Rev.z))
	{
		SetLook(xmf3Rev);
	}
}


Warrior::Warrior() : MeleeCharacter(ROLE::WARRIOR)
{
	m_fMaxHp = 600.0f;
	m_fTempHp = 100.0f;
	m_fHp = 100.0f;
	m_fSpeed = 50.0f;
	m_fDamage = 100.0f;

	m_skillCoolTime = { std::chrono::seconds(7), std::chrono::seconds(0) };
	m_skillDuration = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0], std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
}

Warrior::~Warrior()
{
}

void Warrior::Reset()
{
	m_iAttackType = 0;
	m_bAnimationLock = false;
	m_bComboAttack = false;
	m_attackAnimation = CharacterAnimation::CA_ATTACK;
	m_nextAnimation = CharacterAnimation::CA_NOTHING;
	Player::Reset();
}

void Warrior::Attack()
{
	if (m_bQSkillClicked)
	{
		m_bQSkillClicked = false;

		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_bAnimationEnd = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fSpeed = 1.0f;
	}
	else if (m_attackAnimation != CA_NOTHING)
	{
		int attackPower = 0;
		g_sound.NoLoopPlay("WarriorAttackSound", CalculateDistanceSound());
		switch (m_attackAnimation)
		{
		case CA_ATTACK: attackPower = 0; break;
		case CA_SECONDSKILL: attackPower = 1; break;
		case CA_ADDITIONALANIM: attackPower = 2; break;
		}
		if (m_pCamera) {
			g_NetworkHelper.SendPowerAttackExecute(GetLook(), attackPower);//2번째 인자가 몇번째 타수 공격인지
		}
	}
}

void Warrior::Animate(float fTimeElapsed)
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

	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	bool bCanMove = true;

	if (GetAnimationProgressRate(CA_ATTACK) + GetAnimationProgressRate(CA_FIRSTSKILL) > FLT_EPSILON)
		m_bOnAttack = true;

	if (m_pSkinnedAnimationController->m_CurrentAnimations.first == m_attackAnimation)
	{
		if (GetAnimationProgressRate(m_attackAnimation) > ATTACK1_ATTACK_POINT)
		{
			if (m_bCanAttack)
			{
				Attack();
				m_bCanAttack = false;
			}
		}
		if (CheckAnimationEnd(m_attackAnimation))
		{
			m_bOnAttack = false;
			m_bCanAttack = true;
			m_pSkinnedAnimationController->m_pAnimationTracks[m_attackAnimation].m_bAnimationEnd = false;
			if (m_attackAnimation != m_nextAnimation && m_nextAnimation != CA_NOTHING)
			{
				m_attackAnimation = m_nextAnimation;
				m_nextAnimation = CA_NOTHING;
				m_bComboAttack = true;
			}
			else if (m_attackAnimation == m_nextAnimation || m_nextAnimation == CA_NOTHING)
			{
				m_attackAnimation = CA_ATTACK;
				m_nextAnimation = CA_NOTHING;
				m_bComboAttack = false;
			}
		}
	}


	if (m_pSkinnedAnimationController->m_CurrentAnimations.first == CA_FIRSTSKILL)
	{
		if (GetAnimationProgressRate(CA_FIRSTSKILL) > 0.58f)
		{
			if (m_bCanAttack)
			{
				if (g_Logic.GetMyRole() == ROLE::WARRIOR)
					g_NetworkHelper.Send_SkillExecute_Q(GetLook());
				m_bCanAttack = false;
			}
		}
		if (CheckAnimationEnd(CA_FIRSTSKILL))
		{
			m_bOnAttack = false;
			m_bQSkillClicked = false;
			m_bCanAttack = true;
			m_pSkinnedAnimationController->m_pAnimationTracks[CA_FIRSTSKILL].m_bAnimationEnd = false;
		}
	}

	if (m_bQSkillClicked)
	{
		g_sound.Play("WarriorQskillSound", CalculateDistanceSound());
		m_currentDirection = DIRECTION::IDLE;
		m_bMoveState = false;
	}

	if (m_bMoveState)
	{
		if (!m_bOnAttack)
		{
			if (m_bQSkillClicked)
				AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			else if (m_bLButtonClicked || m_bComboAttack || m_bOnAttack)
				AfterAnimation.first = m_attackAnimation;
			else
				AfterAnimation.first = CharacterAnimation::CA_MOVE;
		}
		AfterAnimation.second = CharacterAnimation::CA_MOVE;
	}
	else
	{
		if (!m_bOnAttack)
		{
			if (m_bQSkillClicked)
				AfterAnimation = { CharacterAnimation::CA_FIRSTSKILL, CharacterAnimation::CA_FIRSTSKILL };
			else if (m_bLButtonClicked || m_bComboAttack)
				AfterAnimation = { m_attackAnimation, m_attackAnimation };
			else
				AfterAnimation = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
		}
		else
		{
			if (m_bQSkillClicked)
				AfterAnimation = { CharacterAnimation::CA_FIRSTSKILL, CharacterAnimation::CA_FIRSTSKILL };
			else if (m_bLButtonClicked || m_bComboAttack)
				AfterAnimation = { m_attackAnimation, m_attackAnimation };
		}
	}

	ChangeAnimation(AfterAnimation);

	if (m_pTrailComponent)
		m_pTrailComponent->SetRenderingTrail(m_bOnAttack || m_bComboAttack);

	//SetLookDirection();
	if (m_bMoveState)
	{
		Move(fTimeElapsed);
	}
	GameObject::Animate(fTimeElapsed);

	if (m_bQSkillClicked)
	{
		if (m_pLoadedModelComponent->m_pWeapon)
		{
			XMFLOAT3 RightVector = XMFLOAT3(1.0f, 0.0f, 0.0f);
			m_pLoadedModelComponent->m_pWeapon->Rotate(&RightVector, 90.0f);
			m_pLoadedModelComponent->m_pWeapon->SetScale(1.5f);
			UpdateTransform(NULL);
		}
	}
}

void Warrior::SetLButtonClicked(bool bLButtonClicked)
{
	m_bLButtonClicked = bLButtonClicked;
	if (bLButtonClicked == true)
	{
		if (GetAnimationProgressRate(m_attackAnimation) > FLT_EPSILON)
		{
			switch (m_attackAnimation)
			{
			case CA_ATTACK:
				if (m_nextAnimation != CA_SECONDSKILL)
					m_nextAnimation = CA_SECONDSKILL;
				break;
			case CA_SECONDSKILL:
				if (m_nextAnimation != CA_ADDITIONALANIM)
					m_nextAnimation = CA_ADDITIONALANIM;
				break;
			case CA_ADDITIONALANIM:
				if (m_nextAnimation != CA_ATTACK)
					m_nextAnimation = CA_ATTACK;
				break;
			default:
				break;
			}
		}
		else
		{
			m_attackAnimation = CA_ATTACK;
			m_nextAnimation = CA_NOTHING;
		}
	}
}

void Warrior::FirstSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::WARRIOR)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);//진행시간
		if (m_skillCoolTime[0] > duration) return;
		g_NetworkHelper.Send_SkillInput_Q();
		m_skillInputTime[0] = std::chrono::high_resolution_clock::now();
	}
	m_currentDirection = DIRECTION::IDLE;
	m_bMoveState = false;
	m_bQSkillClicked = true;
}

void Warrior::SetStage1Position()
{
	SetLook(XMFLOAT3(0, 0, 1));
	SetPosition(XMFLOAT3(-1290.0f, 0, -1470.0f));
	m_xmf3RotateAxis = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Warrior::SetBossStagePostion()
{
	SetPosition(XMFLOAT3(0, 0, -211.0f));
}

Tanker::Tanker() : MeleeCharacter(ROLE::TANKER)
{
	m_fMaxHp = 780.0f;
	m_fTempHp = 100.0f;
	m_fHp = 100.0f;
	m_fSpeed = 50.0f;
	m_fDamage = 50.0f;

	m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(10) };
	m_skillDuration = { std::chrono::seconds(7), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0], std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
}

Tanker::~Tanker()
{

}

void Tanker::Reset()
{
	m_CanActiveQSkill = false;
	Player::Reset();
}

void Tanker::Attack()
{
	if (m_pCamera) {
		g_sound.NoLoopPlay("TankerAttackSound", CalculateDistanceSound() * 0.7);
		g_NetworkHelper.SendCommonAttackExecute(GetLook());
	}
}

void Tanker::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	Character::RbuttonUp(CameraAxis);
}

void Tanker::FirstSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::TANKER)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
		if (m_skillCoolTime[0] > duration) return;
		g_NetworkHelper.Send_SkillInput_Q();
		g_NetworkHelper.Send_SkillExecute_Q();
		m_skillInputTime[0] = std::chrono::high_resolution_clock::now();
	}
	m_currentDirection = DIRECTION::IDLE;
	m_bMoveState = false;
	m_bQSkillClicked = true;

#ifdef LOCAL_TASK
	StartEffect(0);
#endif
}

void Tanker::SecondSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::TANKER)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[1]);
		if (m_skillCoolTime[1] > duration) return;
		g_NetworkHelper.Send_SkillInput_E();
		g_sound.NoLoopPlay("TankerSwingSound", CalculateDistanceSound());
		m_skillInputTime[1] = std::chrono::high_resolution_clock::now();
	}
	m_currentDirection = DIRECTION::IDLE;
	m_bMoveState = false;
	m_bESkillClicked = true;
}

void Tanker::Animate(float fTimeElapsed)
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
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	bool RButtonAnimation = false;

	if (GetAnimationProgressRate(CA_ATTACK) + GetAnimationProgressRate(CA_SECONDSKILL) > FLT_EPSILON)
	{
		m_bOnAttack = true;
	}
	if (GetAnimationProgressRate(CA_ATTACK) > ATTACK1_ATTACK_POINT)
	{
		if (m_bCanAttack)
		{
			Attack();
			m_bCanAttack = false;
		}
	}
	if (GetAnimationProgressRate(CA_SECONDSKILL) > (1.0 - ATTACK1_ATTACK_POINT))
	{
		if (m_bCanAttack)
		{
			if (g_Logic.GetMyRole() == ROLE::TANKER)
				g_NetworkHelper.Send_SkillExecute_E(GetLook());
			m_bCanAttack = false;
		}
	}
	if (CheckAnimationEnd(CA_ATTACK) == true)
	{
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
		m_bCanAttack = true;
		m_bOnAttack = false;
	}
	if (CheckAnimationEnd(CA_FIRSTSKILL) == true)
	{
		if (m_CanActiveQSkill)
		{
			g_sound.NoLoopPlay("TankerQSkill", CalculateDistanceSound());
			for (int i = 0; i < 4; ++i)
			{
				m_ppProjectiles[i]->m_bActive = true;
				m_ppProjectiles[i]->m_xmf3startPosition = GetPosition();
				m_ppProjectiles[i]->m_xmf3startPosition.y += 16.0f;
			}

			m_CanActiveQSkill = false;
		}
		m_bQSkillClicked = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_bAnimationEnd = false;
	}
	if (CheckAnimationEnd(CA_SECONDSKILL) == true)
	{
		gGameFramework.GetScene()->GetObjectManager()->GetTankerAttackEffect()->SetEarthquekePosition();
		g_sound.NoLoopPlay("TankerBombSound", CalculateDistanceSound());
		m_bESkillClicked = false;
		m_bOnAttack = false;
		m_bCanAttack = true;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_SECONDSKILL].m_bAnimationEnd = false;
	}

	if (m_bQSkillClicked || m_bESkillClicked)
	{
		m_currentDirection = DIRECTION::IDLE;
		m_bMoveState = false;
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (!m_bOnAttack) {
			if (m_bQSkillClicked)
				AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			else if (m_bLButtonClicked)	// 공격
				AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			else if (m_bESkillClicked)
				AfterAnimation.first = CharacterAnimation::CA_SECONDSKILL;
			else
				AfterAnimation.first = CharacterAnimation::CA_MOVE;
		}
		AfterAnimation.second = CharacterAnimation::CA_MOVE;
	}
	else if (!m_bOnAttack)
	{
		if (m_bQSkillClicked)
			AfterAnimation = { CharacterAnimation::CA_FIRSTSKILL, CharacterAnimation::CA_FIRSTSKILL };
		else if (m_bLButtonClicked)
			AfterAnimation = { CharacterAnimation::CA_ATTACK, CharacterAnimation::CA_ATTACK };
		else if (m_bESkillClicked)
			AfterAnimation = { CharacterAnimation::CA_SECONDSKILL, CharacterAnimation::CA_SECONDSKILL };
		else
			AfterAnimation = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
	}

	ChangeAnimation(AfterAnimation);

	for (int i = 0; i < 4; ++i)
	{
		if (m_ppProjectiles[i])
		{
			if (m_ppProjectiles[i]->m_bActive)
			{
				float fProgress = static_cast<EnergyBall*>(m_ppProjectiles[i])->m_fProgress;
				fProgress = std::clamp(fProgress / (1.0f - 0.3f), 0.3f, 1.0f);
				m_ppProjectiles[i]->SetinitScale(fProgress, fProgress, fProgress);
				m_ppProjectiles[i]->Animate(fTimeElapsed);
			}
		}
	}
	if (m_pTrailComponent)
	{
		m_pTrailComponent->SetRenderingTrail(m_bOnAttack);
	}

	//SetLookDirection();
	Move(fTimeElapsed);
	GameObject::Animate(fTimeElapsed);

	if (m_bESkillClicked)
	{
		if (m_pLoadedModelComponent->m_pWeapon)
		{
			m_pLoadedModelComponent->m_pWeapon->SetScale(3.0f);
			UpdateTransform(NULL);
		}
	}

}

void Tanker::SetSkillBall(Projectile* pBall)
{
	if (m_nProjectiles < 4)
	{
		m_ppProjectiles[m_nProjectiles] = pBall;
		m_ppProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 16.0f, 0.0f)));
		m_ppProjectiles[m_nProjectiles]->SetLook(GetObjectLook());
		m_ppProjectiles[m_nProjectiles]->m_bActive = false;
		switch (m_nProjectiles)
		{
		case 0: static_cast<EnergyBall*>(m_ppProjectiles[0])->SetTarget(ROLE::WARRIOR); break;
		case 1: static_cast<EnergyBall*>(m_ppProjectiles[1])->SetTarget(ROLE::ARCHER); break;
		case 2: static_cast<EnergyBall*>(m_ppProjectiles[2])->SetTarget(ROLE::MAGE); break;
		case 3: static_cast<EnergyBall*>(m_ppProjectiles[3])->SetTarget(ROLE::TANKER); break;
		default: break;
		}
		m_nProjectiles++;
	}
}

void Tanker::StartEffect(int nSkillNum)
{
	if (nSkillNum == 0)
		m_CanActiveQSkill = true;
}

void Tanker::EndEffect(int nSkillNum)
{
	// 이펙트 렌더 비활성화
	// 실드(HP바) UI 비활성화
	if (nSkillNum == 0)
	{
		EffectObject** shieldEffects = gGameFramework.GetScene()->GetObjectManager()->GetShieldEffectArr();
		for (int i = 0; i < 4; ++i)
			shieldEffects[i]->SetActive(false);
	}
}

void Tanker::SetStage1Position()
{
	SetLook(XMFLOAT3(0, 0, 1));
	SetPosition(XMFLOAT3(-1260.3f, 0, -1510.7f));
	m_xmf3RotateAxis = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Tanker::SetBossStagePostion()
{
	SetPosition(XMFLOAT3(82, 0, -223.0f));
}

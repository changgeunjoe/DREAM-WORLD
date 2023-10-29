#include "stdafx.h"
#include "Player.h"
#include "Animation.h"

Player::Player() : Character()
{
	m_xmf3RotateAxis = XMFLOAT3(0.0f, -90.0f, 0.0f);
	m_skillDuration = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillCoolTime = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now(), std::chrono::high_resolution_clock::now() };
}

Player::Player(ROLE r) : Character(r)
{
	m_xmf3RotateAxis = XMFLOAT3(0.0f, -90.0f, 0.0f);
	m_skillDuration = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillCoolTime = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now(), std::chrono::high_resolution_clock::now() };
}

Player::~Player()
{
}

void Player::Reset()
{
	m_bCanAttack = true;
	m_bShieldActive = false;
	m_bQSkillClicked = false;
	m_bESkillClicked = false;

	m_currentDirection = DIRECTION::IDLE;
	m_xmf3RotateAxis = XMFLOAT3(0.0f, -90.0f, 0.0f);
	m_skillInputTime[0] = std::chrono::high_resolution_clock::now() - m_skillCoolTime[0];
	m_skillInputTime[1] = std::chrono::high_resolution_clock::now() - m_skillCoolTime[1];

	Character::Reset();
}

void Player::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	if (m_pCamera)
	{
		m_pCamera->ReInitCamrea();
		SetCamera(m_pCamera, CameraAxis);
	}
	m_iRButtionCount = 0;
}

void Player::VisualizeSkillCoolTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	double QSkillDuration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]).count();
	double QSkillCoolTime = std::chrono::duration_cast<std::chrono::seconds>(m_skillCoolTime[0]).count();
	if (QSkillCoolTime > DBL_EPSILON)
		m_pSkillQUI->SetSkillTime(1 - (QSkillDuration / QSkillCoolTime));

	double ESkillDuration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[1]).count();
	double ESkillCoolTime = std::chrono::duration_cast<std::chrono::seconds>(m_skillCoolTime[1]).count();
	if (ESkillCoolTime > DBL_EPSILON)
		m_pSkillEUI->SetSkillTime(1 - (ESkillDuration / ESkillCoolTime));
}

void Player::ChangeAnimation(pair<CharacterAnimation, CharacterAnimation> nextAnimation)
{
	if (nextAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = nextAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(nextAnimation);
	}
}

float Player::GetAnimationProgressRate(CharacterAnimation nAnimation)
{
	return m_pSkinnedAnimationController->m_pAnimationTracks[nAnimation].m_fProgressRate;
}

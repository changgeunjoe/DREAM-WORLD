#include "stdafx.h"
#include "Player.h"
#include "Monster.h"
#include "Animation.h"
#include "GameFramework.h"
#include "sound/GameSound.h"
#include "GameobjectManager.h"
#include "Network/Logic/Logic.h"

extern bool GameEnd;
extern Logic g_Logic;
extern GameSound g_sound;
extern CGameFramework gGameFramework;

Monster::Monster() : Character()
{
	m_fHp = 100.0f;
	m_fMaxHp = 6500.0f;
}

Monster::~Monster()
{
}

void Monster::Animate(float fTimeElapsed)
{
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_DIE, 2);
		}
	}
	else
		Move(fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Monster::Move(float fTimeElapsed)
{
	//Player* desChar = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(m_roleDesPlayer);
	//XMFLOAT3 destinationPlayerPos = XMFLOAT3(0, 0, 0);
	//if (desChar != nullptr) {
	//	desChar->GetPosition();
	//}
	//XMFLOAT3 desPlayerVector = Vector3::Subtract(destinationPlayerPos, m_position);
	//float playerDistance = Vector3::Length(desPlayerVector);
	//static XMFLOAT3 up = XMFLOAT3(0, 1, 0);
	//if (m_bMoveState)	// 움직이는 중
	//{
	//	if (m_bOnSkill)
	//	{
	//	}
	//	else {
	//		XMFLOAT3 myLook = GetLook();
	//		float serverChangingAngle = Vector3::Angle(m_desDirecionVec, myLook);
	//		if (serverChangingAngle > 20.0f) {
	//			bool OnRight = (Vector3::DotProduct(GetRight(), m_desDirecionVec) > 0) ? true : false;
	//			if (OnRight) {
	//				Rotate(&up, 180.0f * fTimeElapsed);
	//				m_xmf3rotateAngle.y += 180.0f * fTimeElapsed;
	//			}
	//			else {
	//				Rotate(&up, -180.0f * fTimeElapsed);
	//				m_xmf3rotateAngle.y -= 180.0f * fTimeElapsed;
	//			}
	//			XMFLOAT3 xmf3Position = GetPosition();
	//			xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * fTimeElapsed);
	//			SetPosition(xmf3Position);
	//		}
	//		else if (serverChangingAngle > 1.7f) {
	//			bool OnRight = (Vector3::DotProduct(GetRight(), m_desDirecionVec) > 0) ? true : false;
	//			if (OnRight) {
	//				Rotate(&up, 90.0f * fTimeElapsed);
	//				m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
	//			}
	//			else {
	//				Rotate(&up, -90.0f * fTimeElapsed);
	//				m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
	//			}
	//		}
	//		if (playerDistance >= 34.0f) {
	//			MoveForward(50 * fTimeElapsed);
	//		}
	//		else {
	//			XMFLOAT3 xmf3Position = GetPosition();
	//			xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * fTimeElapsed);
	//			SetPosition(xmf3Position);
	//		}
	//	}
	//}
	//else {
	//	XMFLOAT3 xmf3Position = GetPosition();
	//	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * fTimeElapsed);
	//	SetPosition(xmf3Position);
	//	XMFLOAT3 myLook = GetLook();
	//	float serverChangingAngle = Vector3::Angle(m_desDirecionVec, myLook);
	//	if (serverChangingAngle > 20.0f) {
	//		bool OnRight = (Vector3::DotProduct(GetRight(), m_desDirecionVec) > 0) ? true : false;
	//		if (OnRight) {
	//			Rotate(&up, 180.0f * fTimeElapsed);
	//			m_xmf3rotateAngle.y += 180.0f * fTimeElapsed;
	//		}
	//		else {
	//			Rotate(&up, -180.0f * fTimeElapsed);
	//			m_xmf3rotateAngle.y -= 180.0f * fTimeElapsed;
	//		}
	//	}
	//	if (serverChangingAngle > 1.7f) {
	//		bool OnRight = (Vector3::DotProduct(GetRight(), m_desDirecionVec) > 0) ? true : false;
	//		if (OnRight) {
	//			Rotate(&up, 90.0f * fTimeElapsed);
	//			m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
	//		}
	//		else {
	//			Rotate(&up, -90.0f * fTimeElapsed);
	//			m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
	//		}
	//	}
	//}
}

void Monster::MoveForward(int forwardDirection, float ftimeElapsed)
{
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Look.y = 0.0f;
	xmf3Look = Vector3::ScalarProduct(xmf3Look, (float)forwardDirection);
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, ftimeElapsed * m_fSpeed);
	//xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
	SetPosition(xmf3Position);
}

//void Monster::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)
//{
//	auto clientUtcTime = std::chrono::utc_clock::now();
//	long long durationTime = std::chrono::duration_cast<std::chrono::microseconds>(clientUtcTime - recvTime).count();
//	XMFLOAT3 xmf3Postion = GetPosition();
//	durationTime += g_Logic.GetDiffTime();//1초 마다 업데이트
//	double dDurationTime = (double)durationTime / 1000.0f;
//	dDurationTime = dDurationTime / 1000.0f;
//
//	XMFLOAT3 diff_S2C_Position = Vector3::Subtract(recvPos, xmf3Postion);
//
//	XMFLOAT3 moveDir = Vector3::ScalarProduct(moveVec, dDurationTime * 50.0f);
//
//	XMFLOAT3 interpolateVec = Vector3::Add(diff_S2C_Position, moveDir);
//	float interpolateSize = Vector3::Length(interpolateVec);
//	interpolateVec = Vector3::Normalize(interpolateVec);
//	if (Vector3::Length(diff_S2C_Position) < 3.0f) {
//		m_interpolationDistance = 0.0f;
//	}
//	else if (interpolateSize > 15.0f) {
//		SetPosition(Vector3::Add(recvPos, moveDir));
//	}
//	else {
//		m_interpolationDistance = interpolateSize;
//		m_interpolationVector = interpolateVec;
//	}
//}

NormalMonster::NormalMonster() : Character()
{
	m_fHp = 100.000f;
	m_fMaxHp = 150.0f;
	m_fSpeed = 30.0f;
}

NormalMonster::~NormalMonster()
{
}

void NormalMonster::Animate(float fTimeElapsed)
{
	if (!m_bIsAlive)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_DIE)
		{
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_DIE].m_fProgressRate = 0.0f;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_DIE].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_DIE].m_bAnimationEnd = false;

			pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_DIE, CharacterAnimation::CA_DIE };
			m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
			m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
		}

		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (CheckAnimationEnd(CharacterAnimation::CA_ATTACK) && m_bOnAttack)
	{
		m_bOnAttack = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
	}
	Move(fTimeElapsed);
	SetAnimation();
	GameObject::Animate(fTimeElapsed);
}

void NormalMonster::Move(float fTimeElapsed)
{
	static XMFLOAT3 up = XMFLOAT3(0, 1, 0);
#ifdef LOCAL_TASK	
	GameObject* player = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(g_Logic.GetMyRole());
	if (player != nullptr)
		m_desPos = player->GetPosition();
#endif		
	XMFLOAT3 myLook = GetLook();
	XMFLOAT3 MyPos = GetPosition();
	XMFLOAT3 desVector = Vector3::Subtract(m_desPos, MyPos);
	float desDis = Vector3::Length(desVector);
	desVector = Vector3::Normalize(desVector);
	float frontDotRes = Vector3::DotProduct(desVector, myLook);
	XMFLOAT3 rightVector = GetRight();

	//if (CheckCollision(GetLook(), fTimeElapsed)) {
	//	MyPos = GetPosition();
	//	desVector = Vector3::Subtract(m_desPos, MyPos);
	//	desVector = Vector3::Normalize(desVector);
	//	if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨 - 방향이 맞지 않음
	//		float rightDotRes = Vector3::DotProduct(desVector, rightVector);
	//		if (rightDotRes >= 0) {
	//			Rotate(&up, 90.0f * fTimeElapsed);
	//			m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
	//		}
	//		else {
	//			Rotate(&up, -90.0f * fTimeElapsed);
	//			m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
	//		}
	//	}
	//	MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
	//	SetPosition(MyPos);
	//	return;
	//}
	//std::cout << "desVector: " << desVector.x << ", " << desVector.y << ", " << desVector.z << endl;

	if (desDis <= 30.0f) {//근접 했을때
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨 - 방향이 맞지 않음
			float rightDotRes = Vector3::DotProduct(desVector, rightVector);
			if (rightDotRes >= 0) {
				Rotate(&up, 90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
			}
			else {
				Rotate(&up, -90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
			}
		}
		//MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
		SetPosition(MyPos);
		return;
	}
	else if (desDis <= 80.0f) {
		m_bMoveState = true;
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨
			float rightDotRes = Vector3::DotProduct(desVector, rightVector);
			if (rightDotRes >= 0) {
				Rotate(&up, 90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
			}
			else {
				Rotate(&up, -90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
			}
		}
		MyPos = Vector3::Add(MyPos, Vector3::ScalarProduct(GetLook(), m_fSpeed * fTimeElapsed, false));//틱마다 움직임
		//MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
		SetPosition(MyPos);
		return;
	}
	else if (desDis <= 120.0f) {
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨 - 방향이 맞지 않음
			float rightDotRes = Vector3::DotProduct(desVector, rightVector);
			if (rightDotRes >= 0) {
				Rotate(&up, 90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
			}
			else {
				Rotate(&up, -90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
			}
		}
		//MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
		SetPosition(MyPos);
		return;
	}
	//MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
	SetPosition(MyPos);
}

void NormalMonster::SetAnimation()
{
	pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_NOTHING, CharacterAnimation::CA_NOTHING };
	if (m_bOnAttack)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_ATTACK)
		{
			g_sound.NoLoopPlay("NormalMonsterAttackSound", CalculateDistanceSound());
			NextAnimations = { CharacterAnimation::CA_ATTACK, CharacterAnimation::CA_ATTACK };
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		}
	}
	else if (m_bMoveState)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_MOVE)
			NextAnimations = { CharacterAnimation::CA_MOVE, CharacterAnimation::CA_MOVE };
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
}

//void NormalMonster::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)
//{
//	auto clientUtcTime = std::chrono::utc_clock::now();
//	long long durationTime = std::chrono::duration_cast<std::chrono::microseconds>(clientUtcTime - recvTime).count();
//	XMFLOAT3 xmf3Postion = GetPosition();
//	durationTime += g_Logic.GetDiffTime();//1초 마다 업데이트
//	double dDurationTime = (double)durationTime / 1000.0f;
//	dDurationTime = dDurationTime / 1000.0f;
//
//	XMFLOAT3 diff_S2C_Position = Vector3::Subtract(recvPos, xmf3Postion);
//
//	XMFLOAT3 moveDir = Vector3::ScalarProduct(moveVec, dDurationTime * 30.0f);
//
//	XMFLOAT3 interpolateVec = Vector3::Add(diff_S2C_Position, moveDir);
//	float interpolateSize = Vector3::Length(interpolateVec);
//	interpolateVec = Vector3::Normalize(interpolateVec);
//	if (Vector3::Length(diff_S2C_Position) < DBL_EPSILON) {
//		m_interpolationDistance = 0.0f;
//	}
//	else if (interpolateSize < 2.0f) {
//		m_interpolationDistance = 0.0f;
//	}
//	else if (interpolateSize > 8.0f) {
//		SetPosition(Vector3::Add(recvPos, moveDir));
//	}
//	else {
//		m_interpolationDistance = interpolateSize;
//		m_interpolationVector = interpolateVec;
//	}
//}

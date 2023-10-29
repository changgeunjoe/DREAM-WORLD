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
	Player* desChar = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(m_roleDesPlayer);
	XMFLOAT3 destinationPlayerPos = XMFLOAT3(0, 0, 0);
	if (desChar != nullptr) {
		desChar->GetPosition();
	}
	XMFLOAT3 desPlayerVector = Vector3::Subtract(destinationPlayerPos, m_position);
	float playerDistance = Vector3::Length(desPlayerVector);
	static XMFLOAT3 up = XMFLOAT3(0, 1, 0);
	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bOnSkill)
		{
		}
		else {
			XMFLOAT3 myLook = GetLook();
			float serverChangingAngle = Vector3::Angle(m_desDirecionVec, myLook);
			if (serverChangingAngle > 20.0f) {
				bool OnRight = (Vector3::DotProduct(GetRight(), m_desDirecionVec) > 0) ? true : false;
				if (OnRight) {
					Rotate(&up, 180.0f * fTimeElapsed);
					m_xmf3rotateAngle.y += 180.0f * fTimeElapsed;
				}
				else {
					Rotate(&up, -180.0f * fTimeElapsed);
					m_xmf3rotateAngle.y -= 180.0f * fTimeElapsed;
				}
				XMFLOAT3 xmf3Position = GetPosition();
				xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * fTimeElapsed);
				SetPosition(xmf3Position);
			}
			else if (serverChangingAngle > 1.7f) {
				bool OnRight = (Vector3::DotProduct(GetRight(), m_desDirecionVec) > 0) ? true : false;
				if (OnRight) {
					Rotate(&up, 90.0f * fTimeElapsed);
					m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
				}
				else {
					Rotate(&up, -90.0f * fTimeElapsed);
					m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
				}
			}
			if (playerDistance >= 34.0f) {
				MoveForward(50 * fTimeElapsed);
			}
			else {
				XMFLOAT3 xmf3Position = GetPosition();
				xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * fTimeElapsed);
				SetPosition(xmf3Position);
			}
		}
	}
	else {
		XMFLOAT3 xmf3Position = GetPosition();
		xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * fTimeElapsed);
		SetPosition(xmf3Position);
		XMFLOAT3 myLook = GetLook();
		float serverChangingAngle = Vector3::Angle(m_desDirecionVec, myLook);
		if (serverChangingAngle > 20.0f) {
			bool OnRight = (Vector3::DotProduct(GetRight(), m_desDirecionVec) > 0) ? true : false;
			if (OnRight) {
				Rotate(&up, 180.0f * fTimeElapsed);
				m_xmf3rotateAngle.y += 180.0f * fTimeElapsed;
			}
			else {
				Rotate(&up, -180.0f * fTimeElapsed);
				m_xmf3rotateAngle.y -= 180.0f * fTimeElapsed;
			}
		}
		if (serverChangingAngle > 1.7f) {
			bool OnRight = (Vector3::DotProduct(GetRight(), m_desDirecionVec) > 0) ? true : false;
			if (OnRight) {
				Rotate(&up, 90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
			}
			else {
				Rotate(&up, -90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
			}
		}
	}
}

void Monster::MoveForward(int forwardDirection, float ftimeElapsed)
{
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Look.y = 0.0f;
	xmf3Look = Vector3::ScalarProduct(xmf3Look, (float)forwardDirection);
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, ftimeElapsed * m_fSpeed);
	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
	SetPosition(xmf3Position);
}

void Monster::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)
{
	auto clientUtcTime = std::chrono::utc_clock::now();
	long long durationTime = std::chrono::duration_cast<std::chrono::microseconds>(clientUtcTime - recvTime).count();
	XMFLOAT3 xmf3Postion = GetPosition();
	durationTime += g_Logic.GetDiffTime();//1초 마다 업데이트
	double dDurationTime = (double)durationTime / 1000.0f;
	dDurationTime = dDurationTime / 1000.0f;

	XMFLOAT3 diff_S2C_Position = Vector3::Subtract(recvPos, xmf3Postion);

	XMFLOAT3 moveDir = Vector3::ScalarProduct(moveVec, dDurationTime * 50.0f);

	XMFLOAT3 interpolateVec = Vector3::Add(diff_S2C_Position, moveDir);
	float interpolateSize = Vector3::Length(interpolateVec);
	interpolateVec = Vector3::Normalize(interpolateVec);
	if (Vector3::Length(diff_S2C_Position) < 3.0f) {
		m_interpolationDistance = 0.0f;
	}
	else if (interpolateSize > 15.0f) {
		SetPosition(Vector3::Add(recvPos, moveDir));
	}
	else {
		m_interpolationDistance = interpolateSize;
		m_interpolationVector = interpolateVec;
	}
}

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

	if (CheckCollision(GetLook(), fTimeElapsed)) {
		MyPos = GetPosition();
		desVector = Vector3::Subtract(m_desPos, MyPos);
		desVector = Vector3::Normalize(desVector);
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
		MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
		SetPosition(MyPos);
		return;
	}
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
		MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
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
		MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
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
		MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
		SetPosition(MyPos);
		return;
	}
	MyPos = Vector3::Add(MyPos, m_interpolationVector, 6.0f * m_interpolationDistance * fTimeElapsed);
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

void NormalMonster::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)
{
	auto clientUtcTime = std::chrono::utc_clock::now();
	long long durationTime = std::chrono::duration_cast<std::chrono::microseconds>(clientUtcTime - recvTime).count();
	XMFLOAT3 xmf3Postion = GetPosition();
	durationTime += g_Logic.GetDiffTime();//1초 마다 업데이트
	double dDurationTime = (double)durationTime / 1000.0f;
	dDurationTime = dDurationTime / 1000.0f;

	XMFLOAT3 diff_S2C_Position = Vector3::Subtract(recvPos, xmf3Postion);

	XMFLOAT3 moveDir = Vector3::ScalarProduct(moveVec, dDurationTime * 30.0f);

	XMFLOAT3 interpolateVec = Vector3::Add(diff_S2C_Position, moveDir);
	float interpolateSize = Vector3::Length(interpolateVec);
	interpolateVec = Vector3::Normalize(interpolateVec);
	if (Vector3::Length(diff_S2C_Position) < DBL_EPSILON) {
		m_interpolationDistance = 0.0f;
	}
	else if (interpolateSize < 2.0f) {
		m_interpolationDistance = 0.0f;
	}
	else if (interpolateSize > 8.0f) {
		SetPosition(Vector3::Add(recvPos, moveDir));
	}
	else {
		m_interpolationDistance = interpolateSize;
		m_interpolationVector = interpolateVec;
	}
}

std::pair<bool, XMFLOAT3> NormalMonster::CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	int collideCnt = 0;
	std::vector<std::pair<XMFLOAT3, XMFLOAT3> >  collideCharacterData;

	Player* obj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);
	auto normalVecRes = GetNormalVectorSphere(obj->GetPosition());
	if (normalVecRes.first <= m_SPBB.Radius + 8.0f) {
		normalVecRes.second;
		collideCnt++;
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		collideCharacterData.emplace_back(normalVec, slidingVec);
	}

	obj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::PRIEST);
	normalVecRes = GetNormalVectorSphere(obj->GetPosition());
	if (normalVecRes.first <= m_SPBB.Radius + 8.0f) {
		normalVecRes.second;
		collideCnt++;
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		collideCharacterData.emplace_back(normalVec, slidingVec);
	}

	obj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
	normalVecRes = GetNormalVectorSphere(obj->GetPosition());
	if (normalVecRes.first <= m_SPBB.Radius + 8.0f) {
		normalVecRes.second;
		collideCnt++;
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		collideCharacterData.emplace_back(normalVec, slidingVec);
	}

	obj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::WARRIOR);
	normalVecRes = GetNormalVectorSphere(obj->GetPosition());
	if (normalVecRes.first <= m_SPBB.Radius + 8.0f) {
		normalVecRes.second;
		collideCnt++;
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		collideCharacterData.emplace_back(normalVec, slidingVec);
	}

	if (collideCnt == 0)
		return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
	if (collideCnt == 3) {
		return std::pair<bool, XMFLOAT3>(true, XMFLOAT3(0, 0, 0));
	}
	XMFLOAT3 resultNormal = XMFLOAT3(0, 0, 0);
	XMFLOAT3 resultSliding = XMFLOAT3(0, 0, 0);
	for (auto& character : collideCharacterData) {
		resultNormal = Vector3::Add(resultNormal, character.first);
		resultSliding = Vector3::Add(resultSliding, character.second);
	}
	resultNormal = Vector3::Normalize(resultNormal);
	resultSliding = Vector3::Normalize(resultSliding);
	resultNormal = Vector3::ScalarProduct(resultNormal, 0.5f, false);

	return std::pair<bool, XMFLOAT3>(true, Vector3::Add(resultNormal, resultSliding));
}

std::pair<bool, XMFLOAT3> NormalMonster::CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	NormalMonster** monsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
	vector < pair<XMFLOAT3, XMFLOAT3> > myCollideData_NPC;
	myCollideData_NPC.reserve(4);
	m_position = GetPosition();

	bool isCollideNPC = false;
	int collideCnt = 0;
	for (int i = 0; i < 15; i++) {
		auto normalVecRes = GetNormalVectorSphere(monsterArr[i]->GetPosition());
		if (i == m_nID) continue;
		if (!monsterArr[i]->GetAliveState()) continue;
		if (normalVecRes.first >= m_SPBB.Radius + 8.0f)continue;
		if (collideCnt == 3) {
			return std::pair<bool, XMFLOAT3>(true, XMFLOAT3(0, 0, 0));
		}
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, GetLook());
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		myCollideData_NPC.emplace(myCollideData_NPC.begin() + collideCnt, normalVec, slidingVec);
		collideCnt++;
		isCollideNPC = true;
	}
	if (collideCnt == 0)	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));

	XMFLOAT3 normalVecResult = XMFLOAT3(0, 0, 0);
	XMFLOAT3 slidingVecResult = XMFLOAT3(0, 0, 0);

	for (auto& collideResult : myCollideData_NPC) {
		normalVecResult = Vector3::Add(normalVecResult, collideResult.first);
		slidingVecResult = Vector3::Add(slidingVecResult, collideResult.second);
	}
	if (collideCnt) {
		normalVecResult = Vector3::Normalize(normalVecResult);
		normalVecResult = Vector3::ScalarProduct(normalVecResult, 0.3f);
		slidingVecResult = Vector3::Normalize(slidingVecResult);
	}
	XMFLOAT3 collideNPCMoveDir = Vector3::Normalize(Vector3::Add(normalVecResult, slidingVecResult));
	return std::pair<bool, XMFLOAT3>(true, collideNPCMoveDir);
}

bool NormalMonster::CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed)
{

	std::pair<bool, XMFLOAT3> mapCollideResult;
	XMFLOAT3 mapCollideVector;
	if (gGameFramework.GetScene()->GetObjectManager()->m_nStageType == 1)
		mapCollideResult = CheckCollisionMap_Stage(mapCollideVector, moveDirection, ftimeElapsed);
	else
		mapCollideResult = CheckCollisionMap_Boss(mapCollideVector, moveDirection, ftimeElapsed);
	auto CharacterCollide = CheckCollisionCharacter(moveDirection, ftimeElapsed);
	if (CharacterCollide.first && std::abs(CharacterCollide.second.x) < DBL_EPSILON && std::abs(CharacterCollide.second.z) < DBL_EPSILON) {//캐릭터 콜리전으로 인해 아예 못움직임
		if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
		return true;
	}
	if (mapCollideResult.first) {//맵에 충돌 됨
		if (CharacterCollide.first) {//캐릭터가 충돌 됨
			float dotRes = Vector3::DotProduct(Vector3::Normalize(mapCollideResult.second), Vector3::Normalize(CharacterCollide.second));
			if (dotRes > 0.2f) {//충돌 벡터가 방향이 비슷함
				auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
				if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//노말 몬스터 콜리전으로 인해 아예 못움직임
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "normalMonster no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
				XMFLOAT3 moveDir = Vector3::Normalize(Vector3::Add(mapCollideResult.second, CharacterCollide.second));
				if (normalMonsterCollide.first) {//노말 몬스터 충돌 됨
					float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), moveDir);
					if (dotRes > 0.2f) {//방향이 맞음
						XMFLOAT3 xmf3Position = GetPosition();
						xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, moveDir)), m_fSpeed * ftimeElapsed));
						SetPosition(xmf3Position);
						if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
						PrintCurrentTime();
						std::cout << "normalMonster &char & map Move" << std::endl;
						std::cout << std::endl;
#endif
						return true;
					}
					else {//움직일 수가 없음
						if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
						PrintCurrentTime();
						std::cout << "normalMonster &char & map Move" << std::endl;
						std::cout << std::endl;
#endif
						return true;
					}
				}
				else {//노말 몬스터와 충돌하지 않음
					XMFLOAT3 xmf3Position = GetPosition();
					xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(moveDir), m_fSpeed * ftimeElapsed));
					SetPosition(xmf3Position);
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "char & map Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
			}
			else {//움직일 수 없음
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & char no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			return true;
		}
		//캐릭터가 충돌하진 않았지만 노말 몬스터 체크
		auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
		if (normalMonsterCollide.first) {//노말 몬스터와 충돌함
			if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//노말 몬스터 콜리전으로 인해 아예 못움직임
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), Vector3::Normalize(mapCollideResult.second));
			if (dotRes > 0.2f) {//맵과 노말 몬스터 충돌 벡터 방향이 비슷함
				XMFLOAT3 xmf3Position = GetPosition();
				xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, mapCollideResult.second)), m_fSpeed * ftimeElapsed));
				SetPosition(xmf3Position);
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & monster Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			else {//아예 다름 -> 움직임 x
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & monster no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
		}
		else {//노말 몬스터와 충돌하지 않음 => 맵만 충돌
			XMFLOAT3 xmf3Position = GetPosition();
			xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(mapCollideResult.second, m_fSpeed * ftimeElapsed, false));
			SetPosition(xmf3Position);
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "map Move" << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
	}
	//맵 충돌 하지 않음
	if (CharacterCollide.first) {//캐릭터 와 충돌
		auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
		if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//몬스터 콜리전으로 인해 아예 못움직임
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "monster no Move" << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
		if (normalMonsterCollide.first) {//노말 몬스터와 충돌
			float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), Vector3::Normalize(CharacterCollide.second));
			if (dotRes > 0.2f) {//캐릭터 노말 몬스터 벡터
				XMFLOAT3 xmf3Position = GetPosition();
				xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, CharacterCollide.second)), m_fSpeed * ftimeElapsed));
				SetPosition(xmf3Position);
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster & character Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			else {//벡터가 달라서 움직이지 못함
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster & character no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
		}
		else {// 노말 몬스터와 충돌하지 않음 -> 캐릭터만 충돌
			XMFLOAT3 xmf3Position = GetPosition();
			xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(CharacterCollide.second, m_fSpeed * ftimeElapsed));
			SetPosition(xmf3Position);
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "character Move" << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
	}
	//캐릭터와 충돌하지 않음
	auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
	if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//몬스터 콜리전으로 인해 아예 못움직임
		if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
		PrintCurrentTime();
		std::cout << "monster no Move" << std::endl;
		std::cout << std::endl;
#endif
		return true;
	}
	if (normalMonsterCollide.first) {//노말 몬스터와 충돌함
		XMFLOAT3 xmf3Position = GetPosition();
		xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(normalMonsterCollide.second, m_fSpeed * ftimeElapsed));
		SetPosition(xmf3Position);
		if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef MONSTER_MOVE_LOG
		PrintCurrentTime();
		std::cout << "monster Move" << std::endl;
		std::cout << std::endl;
#endif
		return true;
	}
	return false;
}

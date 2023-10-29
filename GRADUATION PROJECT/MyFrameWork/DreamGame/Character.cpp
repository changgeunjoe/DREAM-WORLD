#include "stdafx.h"
#include "Player.h"
#include "Monster.h"
#include "Character.h"
#include "Animation.h"
#include "Projectile.h"
#include "EffectObject.h"
#include "GameFramework.h"
#include "TrailComponent.h"
#include "sound/GameSound.h"
#include "GameobjectManager.h"
#include "Network/Logic/Logic.h"
#include "Network/NetworkHelper.h"
#include "Network/MapData/MapData.h"

extern Logic g_Logic;
extern NetworkHelper g_NetworkHelper;
extern bool GameEnd;
extern MapData g_bossMapData;
extern MapData g_stage1MapData;
extern CGameFramework gGameFramework;
extern GameSound g_sound;

//#define CHARCTER_MOVE_LOG 1
//#define MONSTER_MOVE_LOG 1

Character::Character() : GameObject(UNDEF_ENTITY)
{
	m_fHp = 100.0f;
	m_fSpeed = 50.0f;
}

Character::Character(ROLE r) :m_role(r)
{
	m_fHp = 100.0f;
	m_fSpeed = 50.0f;
}

Character::~Character()
{
}

void Character::Reset()
{
	m_bOnAttack = false;
	m_bOnSkill = false;
	m_fShield = false;
	m_fHp = 100.0f;
	m_fTempHp = 100.0f;
	m_bActive = true;
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pSkinnedAnimationController->ResetTrack();
	m_bMoveState = false;
	m_bLButtonClicked = false;
	m_bRButtonClicked = false;

	m_interpolationDistance = 0.0f;
	m_interpolationRotateAngleY = 0.0f;
	m_interpolationVector = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < m_ppProjectiles.size(); ++i)
		if (m_ppProjectiles[i]) m_ppProjectiles[i]->m_bActive = false;

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

void Character::MoveForward(int forwardDirection, float ftimeElapsed)
{
	if (std::isnan(m_xmf4x4ToParent._11)) {
		std::cout << "문제 있음" << std::endl;
	}
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Look.y = 0.0f;
	xmf3Look = Vector3::ScalarProduct(xmf3Look, (float)forwardDirection);
	XMFLOAT3 xmf3Position = GetPosition();
	if (CheckCollision(xmf3Look, ftimeElapsed)) {
		xmf3Position = GetPosition();

		xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
		xmf3Position.y = 0.0f;
		GameObject::SetPosition(xmf3Position);

		return;
	}
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, ftimeElapsed * m_fSpeed);
	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
	g_sound.Play("WalkSound", CalculateDistanceSound());
	xmf3Position.y = 0.0f;
	GameObject::SetPosition(xmf3Position);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	return;
}

void Character::MoveStrafe(int rightDirection, float ftimeElapsed)
{
	if (std::isnan(m_xmf4x4ToParent._11)) {
		std::cout << "문제 있음" << std::endl;
	}
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Right.y = 0.0f;
	xmf3Right = Vector3::ScalarProduct(xmf3Right, (float)rightDirection);
	XMFLOAT3 xmf3Position = GetPosition();
	if (CheckCollision(xmf3Right, ftimeElapsed))
	{
		xmf3Position = GetPosition();

		xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
		xmf3Position.y = 0.0f;
		GameObject::SetPosition(xmf3Position);

		return;
	}
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, ftimeElapsed * m_fSpeed);

	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
	g_sound.Play("WalkSound", CalculateDistanceSound());
	xmf3Position.y = 0.0f;
	GameObject::SetPosition(xmf3Position);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	return;
}

void Character::MoveDiagonal(int fowardDirection, int rightDirection, float ftimeElapsed)
{
	if (std::isnan(m_xmf4x4ToParent._11)) {
		std::cout << "문제 있음" << std::endl;
	}
	XMFLOAT3 xmf3Look = GetLook();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Look = Vector3::ScalarProduct(xmf3Look, fowardDirection, false);
	xmf3Right = Vector3::ScalarProduct(xmf3Right, rightDirection, false);

	XMFLOAT3 resultDirection = Vector3::Add(xmf3Right, xmf3Look);
	resultDirection = Vector3::Normalize(resultDirection);
	XMFLOAT3 xmf3Position = GetPosition();
	if (CheckCollision(resultDirection, ftimeElapsed))
	{
		xmf3Position = GetPosition();

		xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
		xmf3Position.y = 0.0f;
		GameObject::SetPosition(xmf3Position);
		return;
	}
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(resultDirection, ftimeElapsed * m_fSpeed));

	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
	g_sound.Play("WalkSound", CalculateDistanceSound());
	xmf3Position.y = 0.0f;
	SetPosition(xmf3Position);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
}

bool Character::CheckAnimationEnd(int nAnimation)
{
	return m_pSkinnedAnimationController->m_pAnimationTracks[nAnimation].m_bAnimationEnd;
}

std::pair<bool, XMFLOAT3> Character::CheckCollisionMap_Boss(XMFLOAT3& normalVector, XMFLOAT3& moveDirection, float ftimeElapsed)
{
	XMFLOAT3 xmf3Position = GetPosition();
	vector<MapCollide>& Collides = g_bossMapData.GetCollideData();
	for (auto& collide : Collides) {
		if (collide.GetObb().Intersects(m_SPBB)) {

			auto& relationIdxsVector = collide.GetRelationCollisionIdxs();
			int secondCollide = -1;
			for (auto& otherCol : relationIdxsVector) {
				if (Collides[otherCol].GetObb().Intersects(m_SPBB)) {
					secondCollide = otherCol;
					break;
				}
			}
			if (secondCollide == -1) {//m_SPBB								
				auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, xmf3Position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//노말 벡터
				XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//슬라이딩 벡터
				float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
				float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//슬라이딩 벡터와 무브 벡터 내적 값				
				collideSlidingVector = Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut, false);
				collideNormalVector = Vector3::ScalarProduct(collideNormalVector, 0.06f * normalVectorDotProductReslut, false);
				//return std::pair<bool, XMFLOAT3>(true, Vector3::Add(collideSlidingVector, collideNormalVector));
				return std::pair<bool, XMFLOAT3>(true, collideSlidingVector);
			}
			else {
				auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, xmf3Position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//노말 벡터
				//XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//슬라이딩 벡터
				float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);//노말 벡터가 가져야할 크기(충돌 위치 조정을 위한)
				float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//슬라이딩 벡터와 룩 벡터 내적 값				

				auto CollidePolygonNormalVector2 = Collides[secondCollide].CalSlidingVector(m_SPBB, xmf3Position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//노말 벡터
				//XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//슬라이딩 벡터
				float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);//노말 벡터가 가져야할 크기(충돌 위치 조정을 위한)
				float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//슬라이딩 벡터와 룩 벡터 내적 값

				XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, Collides[secondCollide].GetObb().Center));
				resultSlidingVector.y = 0.0f;
				float dotRes = Vector3::DotProduct(resultSlidingVector, moveDirection);
				if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);
				collideNormalVector1 = Vector3::ScalarProduct(collideNormalVector1, 0.3f * normalVectorDotProductResult1, false);
				collideNormalVector2 = Vector3::ScalarProduct(collideNormalVector2, 0.3f * normalVectorDotProductResult2, false);
				return std::pair<bool, XMFLOAT3>(true, Vector3::Add(resultSlidingVector, Vector3::Add(collideNormalVector1, collideNormalVector2)));
			}
		}
	}
	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
}

std::pair<bool, XMFLOAT3> Character::CheckCollisionMap_Stage(XMFLOAT3& normalVector, XMFLOAT3& moveDirection, float ftimeElapsed)
{
	XMFLOAT3 xmf3Position = GetPosition();
	vector<MapCollide>& Collides = g_stage1MapData.GetCollideData();

	for (auto& collide : Collides) {
		if (collide.GetObb().Intersects(m_SPBB)) {

			auto& relationIdxsVector = collide.GetRelationCollisionIdxs();
			int secondCollide = -1;
			for (auto& otherCol : relationIdxsVector) {
				if (Collides[otherCol].GetObb().Intersects(m_SPBB)) {
					secondCollide = otherCol;
					break;
				}
			}
			if (secondCollide == -1) {//m_SPBB								
				auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, xmf3Position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//노말 벡터
				XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//슬라이딩 벡터
				float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
				float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//슬라이딩 벡터와 무브 벡터 내적 값
				collideSlidingVector = Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut, false);
				normalVector = collideNormalVector;
				collideNormalVector = Vector3::ScalarProduct(collideNormalVector, 0.06f * normalVectorDotProductReslut, false);
				return std::pair<bool, XMFLOAT3>(true, collideSlidingVector);
				//return std::pair<bool, XMFLOAT3>(true, Vector3::Add(collideSlidingVector, collideNormalVector));
				//std::cout << "one collide" << std::endl;
			}
			else {
				auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, xmf3Position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//노말 벡터
				//XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//슬라이딩 벡터
				float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);//노말 벡터가 가져야할 크기(충돌 위치 조정을 위한)
				float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//슬라이딩 벡터와 룩 벡터 내적 값				

				auto CollidePolygonNormalVector2 = Collides[secondCollide].CalSlidingVector(m_SPBB, xmf3Position, moveDirection);//노말, 슬라이딩, 노말이 가져야할 크기 를 반환
				XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//노말 벡터
				//XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//슬라이딩 벡터
				float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);//노말 벡터가 가져야할 크기(충돌 위치 조정을 위한)
				float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//슬라이딩 벡터와 룩 벡터 내적 값

				XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, Collides[secondCollide].GetObb().Center));
				resultSlidingVector.y = 0.0f;
				float dotRes = Vector3::DotProduct(resultSlidingVector, moveDirection);
				if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);
				normalVector = Vector3::Normalize(Vector3::Add(collideNormalVector1, collideNormalVector2));
				collideNormalVector1 = Vector3::ScalarProduct(collideNormalVector1, 0.3f * normalVectorDotProductResult1, false);
				collideNormalVector2 = Vector3::ScalarProduct(collideNormalVector2, 0.3f * normalVectorDotProductResult2, false);
				//std::cout << "two collide" << std::endl;
				return std::pair<bool, XMFLOAT3>(true, Vector3::Add(resultSlidingVector, Vector3::Add(collideNormalVector1, collideNormalVector2)));
			}
		}
	}
	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
}

std::pair<bool, XMFLOAT3> Character::CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	int collideCnt = 0;
	std::vector<std::pair<XMFLOAT3, XMFLOAT3> >  collideCharacterData;
	if (m_role != ROLE::ARCHER)
	{
		Character* obj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);
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
	}
	if (m_role != ROLE::PRIEST)
	{
		Character* obj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::PRIEST);
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
	}
	if (m_role != ROLE::TANKER)
	{
		Character* obj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
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
	}
	if (m_role != ROLE::WARRIOR)
	{
		Character* obj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::WARRIOR);
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
	resultNormal = Vector3::ScalarProduct(resultNormal, 0.02f, false);

	return std::pair<bool, XMFLOAT3>(true, resultSliding);
}

std::pair<bool, XMFLOAT3> Character::CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	NormalMonster** monsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
	vector < pair<XMFLOAT3, XMFLOAT3> > myCollideData_NPC;
	myCollideData_NPC.reserve(4);

	int collideCnt = 0;
	for (int i = 0; i < 15; i++) {
		if (!monsterArr[i]->GetAliveState()) continue;
		auto normalVecRes = GetNormalVectorSphere(monsterArr[i]->GetPosition());
		if (normalVecRes.first >= m_SPBB.Radius + 8.0f)continue;
		if (collideCnt == 3) {
			return std::pair<bool, XMFLOAT3>(true, XMFLOAT3(0, 0, 0));
		}
		collideCnt++;
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		myCollideData_NPC.emplace_back(normalVec, slidingVec);
	}
	if (collideCnt == 0)	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));

	XMFLOAT3 normalVecResult = XMFLOAT3(0, 0, 0);
	XMFLOAT3 slidingVecResult = XMFLOAT3(0, 0, 0);

	for (auto& collideResult : myCollideData_NPC) {
		normalVecResult = Vector3::Add(normalVecResult, collideResult.first);
		slidingVecResult = Vector3::Add(slidingVecResult, collideResult.second);
	}
	normalVecResult = Vector3::Normalize(normalVecResult);
	normalVecResult = Vector3::ScalarProduct(normalVecResult, 0.02f);
	slidingVecResult = Vector3::Normalize(slidingVecResult);

	//std::cout << "monster slidingVec: " << slidingVecResult.x << ", " << slidingVecResult.y << ", " << slidingVecResult.z << std::endl;
	XMFLOAT3 collideNPCMoveDir = Vector3::Normalize(Vector3::Add(normalVecResult, slidingVecResult));
	return std::pair<bool, XMFLOAT3>(true, slidingVecResult);
}

std::pair<bool, XMFLOAT3> Character::CheckCollisionBoss(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	Monster* boss = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
	auto normalVecRes = GetNormalVectorSphere(boss->GetPosition());
	if (Vector3::DotProduct(normalVecRes.second, moveDirection) > 0.2f)return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
	if (normalVecRes.first >= m_SPBB.Radius + 25.0f) return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
	else {
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		return std::pair<bool, XMFLOAT3>(true, slidingVec);
	}
	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
}

bool Character::CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	std::pair<bool, XMFLOAT3> mapCollideResult;
	XMFLOAT3 mapCollideVector;
	if (gGameFramework.GetScene()->GetObjectManager()->m_nStageType == 1) {
		mapCollideResult = CheckCollisionMap_Stage(mapCollideVector, moveDirection, ftimeElapsed);
		auto CharacterCollide = CheckCollisionCharacter(moveDirection, ftimeElapsed);
		if (CharacterCollide.first && std::abs(CharacterCollide.second.x) < DBL_EPSILON && std::abs(CharacterCollide.second.z) < DBL_EPSILON) {//캐릭터 콜리전으로 인해 아예 못움직임
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
			std::cout << "character no Move" << std::endl;
#endif
			return true;
		}
		if (mapCollideResult.first) {//맵에 충돌 됨
			if (CharacterCollide.first) {//캐릭터가 충돌 됨
				float dotRes = Vector3::DotProduct(Vector3::Normalize(mapCollideResult.second), Vector3::Normalize(CharacterCollide.second));
				if (dotRes > 0.2f) {//충돌 벡터가 방향이 비슷함
					auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
					if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//노말 몬스터 콜리전으로 인해 아예 못움직임
						if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
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
							xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, moveDir)), 0.9f * m_fSpeed * ftimeElapsed));
							SetPosition(xmf3Position);
							if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
							PrintCurrentTime();
							std::cout << "normalMonster &char & map Move" << std::endl;
							std::cout << std::endl;
#endif
							return true;
						}
						else {//움직일 수가 없음
							if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
							PrintCurrentTime();
							std::cout << "normalMonster &char & map Move" << std::endl;
							std::cout << std::endl;
#endif
							return true;
						}
					}
					else {//노말 몬스터와 충돌하지 않음
						XMFLOAT3 xmf3Position = GetPosition();
						xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(moveDir), 0.9f * m_fSpeed * ftimeElapsed));
						SetPosition(xmf3Position);
						if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
						PrintCurrentTime();
						std::cout << "char & map Move" << std::endl;
						std::cout << std::endl;
#endif
						return true;
					}
				}
				else {//움직일 수 없음
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
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
			normalMonsterCollide.second.y = 0;
			if (normalMonsterCollide.first) {//노말 몬스터와 충돌함
				if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//노말 몬스터 콜리전으로 인해 아예 못움직임
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "monster no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
				float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), Vector3::Normalize(mapCollideResult.second));
				if (dotRes > 0.2f) {//맵과 노말 몬스터 충돌 벡터 방향이 비슷함
					XMFLOAT3 xmf3Position = GetPosition();
					xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, mapCollideResult.second)), 0.9f * m_fSpeed * ftimeElapsed));
					SetPosition(xmf3Position);
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "map & monster Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
				else {//아예 다름 -> 움직임 x
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "map & monster no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
			}
			else {//노말 몬스터와 충돌하지 않음 => 맵만 충돌
				mapCollideVector;
				float dotResult = Vector3::DotProduct(mapCollideVector, moveDirection);
				if (dotResult > 0.121) {
					return false;
				}
				XMFLOAT3 xmf3Position = GetPosition();
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map Move" << std::endl;
				std::cout << "prev collision position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
#endif
				xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(mapCollideResult.second, m_fSpeed * ftimeElapsed, false));
				SetPosition(xmf3Position);
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
				std::cout << "after collision position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
#endif
				return true;
			}
		}
		//맵 충돌 하지 않음
		if (CharacterCollide.first) {//캐릭터 와 충돌
			auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
			if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//캐릭터 콜리전으로 인해 아예 못움직임
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
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
					xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, CharacterCollide.second)), 0.9f * m_fSpeed * ftimeElapsed));
					SetPosition(xmf3Position);
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "monster & character Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
				else {//벡터가 달라서 움직이지 못함
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "monster & character no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
			}
			else {// 노말 몬스터와 충돌하지 않음 -> 캐릭터만 충돌
				XMFLOAT3 xmf3Position = GetPosition();
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "character Move" << std::endl;
				std::cout << "prev collision position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
#endif
				XMFLOAT3 moveVec = Vector3::ScalarProduct(CharacterCollide.second, 0.9f * m_fSpeed * ftimeElapsed);
				xmf3Position = Vector3::Add(xmf3Position, moveVec);
				SetPosition(xmf3Position);
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
				std::cout << "after collision position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
				std::cout << "character MoveDirecion: " << moveDirection.x << ", " << moveDirection.y << ", " << moveDirection.z << std::endl;
				std::cout << "slidingVec: " << moveVec.x << ", " << moveVec.y << ", " << moveVec.z << std::endl;
				std::cout << "slidingVec Size: " << Vector3::Length(moveVec) << std::endl;
#endif
				return true;
			}
		}
		//캐릭터와 충돌하지 않음
		auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
		if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//노말 몬스터 콜리전으로 인해 아예 못움직임
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "monster no Move" << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
		if (normalMonsterCollide.first) {//노말 몬스터와 충돌함
			XMFLOAT3 xmf3Position = GetPosition();
			XMFLOAT3 slidingVec = Vector3::ScalarProduct(normalMonsterCollide.second, 0.9f * m_fSpeed * ftimeElapsed);
#ifdef CHARCTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "monster Move" << std::endl;
			std::cout << "collsion slidingVec: " << normalMonsterCollide.second.x << ", " << normalMonsterCollide.second.y << ", " << normalMonsterCollide.second.z << std::endl;
			std::cout << "collision slidingSize: " << Vector3::Length(slidingVec) << std::endl;
			std::cout << "collision prev position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
#endif
			xmf3Position = Vector3::Add(xmf3Position, slidingVec);
			SetPosition(xmf3Position);
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
			std::cout << "collision after position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
		return false;
	}
	else {
		mapCollideResult = CheckCollisionMap_Boss(mapCollideVector, moveDirection, ftimeElapsed);
		auto CharacterCollide = CheckCollisionCharacter(moveDirection, ftimeElapsed);
		if (CharacterCollide.first && std::abs(CharacterCollide.second.x) < DBL_EPSILON && std::abs(CharacterCollide.second.z) < DBL_EPSILON) {//캐릭터 콜리전으로 인해 아예 못움직임
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
			std::cout << "character no Move" << std::endl;
#endif
			return true;
		}
		if (mapCollideResult.first) {//맵에 충돌 됨
			if (CharacterCollide.first) {//캐릭터가 충돌 됨
				float dotRes = Vector3::DotProduct(Vector3::Normalize(mapCollideResult.second), Vector3::Normalize(CharacterCollide.second));
				if (dotRes > 0.2f) {//충돌 벡터가 방향이 비슷함
					auto bossCollide = CheckCollisionBoss(moveDirection, ftimeElapsed);
					XMFLOAT3 moveDir = Vector3::Normalize(Vector3::Add(mapCollideResult.second, CharacterCollide.second));
					if (bossCollide.first) {//노말 몬스터 충돌 됨
						float dotRes = Vector3::DotProduct(Vector3::Normalize(bossCollide.second), moveDir);
						if (dotRes > 0.2f) {//방향이 맞음
							XMFLOAT3 xmf3Position = GetPosition();
							xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(bossCollide.second, moveDir)), 0.9f * m_fSpeed * ftimeElapsed));
							SetPosition(xmf3Position);
							if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
							PrintCurrentTime();
							std::cout << "normalMonster &char & map Move" << std::endl;
							std::cout << std::endl;
#endif
							return true;
						}
						else {//움직일 수가 없음
							if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
							PrintCurrentTime();
							std::cout << "normalMonster &char & map Move" << std::endl;
							std::cout << std::endl;
#endif
							return true;
						}
					}
					else {//노말 몬스터와 충돌하지 않음
						XMFLOAT3 xmf3Position = GetPosition();
						xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(moveDir), 0.9f * m_fSpeed * ftimeElapsed));
						SetPosition(xmf3Position);
						if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
						PrintCurrentTime();
						std::cout << "char & map Move" << std::endl;
						std::cout << std::endl;
#endif
						return true;
					}
				}
				else {//움직일 수 없음
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "map & char no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
				return true;
			}
			//캐릭터가 충돌하진 않았지만 노말 몬스터 체크
			auto bossCollide = CheckCollisionBoss(moveDirection, ftimeElapsed);
			if (bossCollide.first) {//노말 몬스터와 충돌함
				float dotRes = Vector3::DotProduct(Vector3::Normalize(bossCollide.second), Vector3::Normalize(mapCollideResult.second));
				if (dotRes > 0.2f) {//맵과 노말 몬스터 충돌 벡터 방향이 비슷함
					XMFLOAT3 xmf3Position = GetPosition();
					xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(bossCollide.second, mapCollideResult.second)), 0.9f * m_fSpeed * ftimeElapsed));
					SetPosition(xmf3Position);
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "map & monster Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
				else {//아예 다름 -> 움직임 x
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "map & monster no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
			}
			else {//노말 몬스터와 충돌하지 않음 => 맵만 충돌
				mapCollideVector;
				float dotResult = Vector3::DotProduct(mapCollideVector, moveDirection);
				if (dotResult > 0.121) {
					return false;
				}
				XMFLOAT3 xmf3Position = GetPosition();
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map Move" << std::endl;
				std::cout << "prev collision position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
#endif
				xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(mapCollideResult.second, m_fSpeed * ftimeElapsed, false));
				SetPosition(xmf3Position);
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
				std::cout << "after collision position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
#endif
				return true;
			}
		}
		//맵 충돌 하지 않음
		if (CharacterCollide.first) {//캐릭터 와 충돌
			auto bossCollide = CheckCollisionBoss(moveDirection, ftimeElapsed);
			if (bossCollide.first) {//노말 몬스터와 충돌
				float dotRes = Vector3::DotProduct(Vector3::Normalize(bossCollide.second), Vector3::Normalize(CharacterCollide.second));
				if (dotRes > 0.2f) {//캐릭터 노말 몬스터 벡터
					XMFLOAT3 xmf3Position = GetPosition();
					xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(bossCollide.second, CharacterCollide.second)), 0.9f * m_fSpeed * ftimeElapsed));
					SetPosition(xmf3Position);
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "monster & character Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
				else {//벡터가 달라서 움직이지 못함
					if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "monster & character no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
			}
			else {// 노말 몬스터와 충돌하지 않음 -> 캐릭터만 충돌
				XMFLOAT3 xmf3Position = GetPosition();
#ifdef CHARCTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "character Move" << std::endl;
				std::cout << "prev collision position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
#endif
				XMFLOAT3 moveVec = Vector3::ScalarProduct(CharacterCollide.second, 0.9f * m_fSpeed * ftimeElapsed);
				xmf3Position = Vector3::Add(xmf3Position, moveVec);
				SetPosition(xmf3Position);
				if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
				std::cout << "after collision position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
				std::cout << "character MoveDirecion: " << moveDirection.x << ", " << moveDirection.y << ", " << moveDirection.z << std::endl;
				std::cout << "slidingVec: " << moveVec.x << ", " << moveVec.y << ", " << moveVec.z << std::endl;
				std::cout << "slidingVec Size: " << Vector3::Length(moveVec) << std::endl;
#endif
				//PrintCurrentTime();
				//std::cout << "elapsedTime: " << ftimeElapsed << std::endl;
				//std::cout << "speed: " << m_fSpeed << std::endl;
				//std::cout << "position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
				//std::cout << "moveVec: " << moveVec.x << ", " << moveVec.y << ", " << moveVec.z << std::endl << std::endl;
				return true;
			}
		}
		//캐릭터와 충돌하지 않음
		auto bossCollide = CheckCollisionBoss(moveDirection, ftimeElapsed);
		if (bossCollide.first) {//노말 몬스터와 충돌함
			XMFLOAT3 xmf3Position = GetPosition();
			XMFLOAT3 slidingVec = Vector3::ScalarProduct(bossCollide.second, 0.9f * m_fSpeed * ftimeElapsed);
#ifdef CHARCTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "monster Move" << std::endl;
			std::cout << "collsion slidingVec: " << bossCollide.second.x << ", " << bossCollide.second.y << ", " << bossCollide.second.z << std::endl;
			std::cout << "collision slidingSize: " << Vector3::Length(slidingVec) << std::endl;
			std::cout << "collision prev position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
#endif
			xmf3Position = Vector3::Add(xmf3Position, slidingVec);
			SetPosition(xmf3Position);
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
#ifdef CHARCTER_MOVE_LOG
			std::cout << "collision after position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
		return false;
	}


}

void Character::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)
{
	auto clientUtcTime = std::chrono::utc_clock::now();
	long long durationTime = std::chrono::duration_cast<std::chrono::microseconds>(clientUtcTime - recvTime).count();
	XMFLOAT3 xmf3Postion = GetPosition();
	durationTime += g_Logic.GetDiffTime();
	double dDurationTime = (double)durationTime / 1000.0f;
	dDurationTime = dDurationTime / 1000.0f;

	XMFLOAT3 diff_S2C_Position = Vector3::Subtract(recvPos, xmf3Postion);
	float diff_S2C_Size = Vector3::Length(diff_S2C_Position);
	diff_S2C_Position = Vector3::Normalize(diff_S2C_Position);

	float interpolateSize = diff_S2C_Size - dDurationTime * 50.0f;

	if (interpolateSize > 0) {
		if (interpolateSize < 3.0f) {
			m_interpolationDistance = 0.0f;
			m_interpolationVector = XMFLOAT3(0, 0, 0);
		}
		else if (interpolateSize > 8.0f) {
			SetPosition(Vector3::Add(xmf3Postion, diff_S2C_Position, interpolateSize));
			m_interpolationDistance = 0.0f;
			m_interpolationVector = XMFLOAT3(0, 0, 0);
		}
		else {
			m_interpolationDistance = interpolateSize;
			m_interpolationVector = diff_S2C_Position;
		}
	}
	else {
		m_interpolationDistance = 0.0f;
		m_interpolationVector = XMFLOAT3(0, 0, 0);
	}
}

std::pair<float, XMFLOAT3> Character::GetNormalVectorSphere(const XMFLOAT3& point)
{
	XMFLOAT3 position = GetPosition();
	XMFLOAT3 normalVec = Vector3::Subtract(position, point);
	float normalSize = Vector3::Length(normalVec);
	normalVec = Vector3::Normalize(normalVec);
	return std::pair<float, XMFLOAT3>(normalSize, normalVec);
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
#include "stdafx.h"
#include "Character.h"
#include "Animation.h"
#include "GameFramework.h"
#include "GameobjectManager.h"
#include "EffectObject.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"
#include "Network/MapData/MapData.h"
#include "TrailComponent.h"
#include "sound/GameSound.h"

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
	//m_xmf3RotateAxis = XMFLOAT3(0.0f, -90.0f, 0.0f);
	m_skillDuration = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillCoolTime = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now(), std::chrono::high_resolution_clock::now() };
	m_fSpeed = 50.0f;
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
		SetCamera(m_pCamera, CameraAxis);
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

void Character::SetLookDirection()
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);

	XMFLOAT3 xmf3Rev = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fRotateAngle = -1.0f;

	DIRECTION tempDir = m_currentDirection;

	if (m_bRButtonClicked)
	{
		SetLook(xmf3Look);
		return;
	}

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

bool Character::CheckAnimationEnd(int nAnimation)
{
	return m_pSkinnedAnimationController->m_pAnimationTracks[nAnimation].m_bAnimationEnd;
}

void Character::ChangeAnimation(pair<CharacterAnimation, CharacterAnimation> nextAnimation)
{
	if (nextAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = nextAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(nextAnimation);
	}
}

float Character::GetAnimationProgressRate(CharacterAnimation nAnimation)
{
	return m_pSkinnedAnimationController->m_pAnimationTracks[nAnimation].m_fProgressRate;
}

void Character::MoveForward(int forwardDirection, float ftimeElapsed)
{
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Look.y = 0.0f;
	xmf3Look = Vector3::ScalarProduct(xmf3Look, (float)forwardDirection);
	XMFLOAT3 xmf3Position = GetPosition();
	if (CheckCollision(xmf3Look, ftimeElapsed)) {
		xmf3Position = GetPosition();
#ifdef CHARCTER_MOVE_LOG
		if (m_interpolationDistance > DBL_EPSILON) {

			std::cout << "interpolate prev position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
			std::cout << "interpolate Vec: " << m_interpolationVector.x << ", " << m_interpolationVector.y << ", " << m_interpolationVector.z << std::endl;
			std::cout << "interpolate Size: " << m_interpolationDistance << std::endl;
		}
#endif
		xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
		GameObject::SetPosition(xmf3Position);
#ifdef CHARCTER_MOVE_LOG
		if (m_interpolationDistance > DBL_EPSILON) {
			std::cout << "interpolate after position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl << std::endl;
		}
#endif
		return;
	}
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, ftimeElapsed * m_fSpeed);

	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);

	g_sound.NoLoopPlay("WalkSound", 1.0f);
	GameObject::SetPosition(xmf3Position);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	return;
}

void Character::MoveStrafe(int rightDirection, float ftimeElapsed)
{
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Right.y = 0.0f;
	xmf3Right = Vector3::ScalarProduct(xmf3Right, (float)rightDirection);
	XMFLOAT3 xmf3Position = GetPosition();
	if (CheckCollision(xmf3Right, ftimeElapsed))
	{
		xmf3Position = GetPosition();

		xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);
		GameObject::SetPosition(xmf3Position);

		return;
	}
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, ftimeElapsed * m_fSpeed);

	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);

	GameObject::SetPosition(xmf3Position);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	return;
}

void Character::MoveDiagonal(int fowardDirection, int rightDirection, float ftimeElapsed)
{
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

		GameObject::SetPosition(xmf3Position);
		return;
	}
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(resultDirection, ftimeElapsed * m_fSpeed));

	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, 10.0f * m_interpolationDistance * ftimeElapsed);

	SetPosition(xmf3Position);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
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
	if (g_Logic.GetMyRole() != ROLE::ARCHER)
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
	if (g_Logic.GetMyRole() != ROLE::PRIEST)
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
	if (g_Logic.GetMyRole() != ROLE::TANKER)
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
	if (g_Logic.GetMyRole() != ROLE::WARRIOR)
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

	std::cout << "monster slidingVec: " << slidingVecResult.x << ", " << slidingVecResult.y << ", " << slidingVecResult.z << std::endl;
	XMFLOAT3 collideNPCMoveDir = Vector3::Normalize(Vector3::Add(normalVecResult, slidingVecResult));
	return std::pair<bool, XMFLOAT3>(true, slidingVecResult);
}

bool Character::CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed)
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
			//PrintCurrentTime();
			//std::cout << "elapsedTime: " << ftimeElapsed << std::endl;
			//std::cout << "speed: " << m_fSpeed << std::endl;
			//std::cout << "position: " << xmf3Position.x << ", " << xmf3Position.y << ", " << xmf3Position.z << std::endl;
			//std::cout << "moveVec: " << moveVec.x << ", " << moveVec.y << ", " << moveVec.z << std::endl << std::endl;
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

void Character::ExecuteSkill_Q()
{
}

void Character::ExecuteSkill_E()
{
}

void Character::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)
{
	auto clientUtcTime = std::chrono::utc_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(clientUtcTime - recvTime).count();
	XMFLOAT3 xmf3Postion = GetPosition();
	durationTime -= g_Logic.GetDiffTime();
	durationTime = (double)durationTime / 1000.0f;//microseconds to mill
	durationTime = (double)durationTime / 1000.0f;//milliseconds to sec

	XMFLOAT3 diff_S2C_Position = Vector3::Subtract(recvPos, xmf3Postion);

	XMFLOAT3 moveDir = Vector3::ScalarProduct(moveVec, durationTime * 50.0f);

	XMFLOAT3 interpolateVec = Vector3::Add(diff_S2C_Position, moveDir);
	float interpolateSize = Vector3::Length(interpolateVec);
	interpolateVec = Vector3::Normalize(interpolateVec);
	if (m_currentDirection == DIRECTION::IDLE && Vector3::Length(diff_S2C_Position) < DBL_EPSILON) {
		m_interpolationDistance = 0.0f;
		m_interpolationVector = XMFLOAT3(0, 0, 0);
	}
	else if (interpolateSize < 3.0f) {
		//std::cout << "interpolateSize < 3.0f" << endl;
		m_interpolationDistance = 0.0f;
		m_interpolationVector = XMFLOAT3(0, 0, 0);
	}
	else if (interpolateSize > 8.0f) {
		//std::cout << "interpolateSize > 8.0f - SetPosition()" << endl;		
		//std::cout << "diff_S2C_Position Size: " << Vector3::Length(diff_S2C_Position) << ", diff_S2C_Position: " << diff_S2C_Position.x << ", " << diff_S2C_Position.y << ", " << diff_S2C_Position.z << endl;
		//std::cout << "moveDir Size: " << Vector3::Length(moveDir) << ", diff_S2C_Position: " << moveVec.x << ", " << moveVec.y << ", " << moveVec.z << endl;
		//XMFLOAT3 diff = Vector3::Normalize(diff_S2C_Position);
		//float dotP = Vector3::DotProduct(diff, moveVec);
		//std::cout << "dotP: " << dotP << endl;
		SetPosition(Vector3::Add(xmf3Postion, interpolateVec, interpolateSize));
		m_interpolationDistance = 0.0f;
		m_interpolationVector = XMFLOAT3(0, 0, 0);
	}
	else {
		//std::cout << "interpolateSize: " << interpolateSize << ", interpolateVec" << interpolateVec.x << ", " << interpolateVec.y << ", "
		//	<< interpolateVec.z << endl;
		//std::cout << "diff_S2C_Position Size: " << Vector3::Length(diff_S2C_Position) << ", diff_S2C_Position: " << diff_S2C_Position.x << ", " << diff_S2C_Position.y << ", "
		//	<< diff_S2C_Position.z << endl;
		m_interpolationDistance = interpolateSize;
		m_interpolationVector = interpolateVec;
	}
}

constexpr float ATTACK1_ATTACK_POINT = 0.24f;
constexpr float ATTACK4_ATTACK_POINT = 0.60f;

Warrior::Warrior() : Character()
{
	m_fHp = 400.0f;
	m_fMaxHp = 400.0f;
	m_fSpeed = 50.0f;
	m_fDamage = 100.0f;
}

Warrior::~Warrior()
{
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
		switch (m_attackAnimation)
		{
		case CA_ATTACK: attackPower = 0; break;
		case CA_SECONDSKILL: attackPower = 1; break;
		case CA_ADDITIONALANIM: attackPower = 2; break;
		}
		if (m_pCamera) {
			g_NetworkHelper.SendCommonAttackExecute(GetLook(), attackPower);//2번째 인자가 몇번째 타수 공격인지
		}
	}
}

void Warrior::RbuttonClicked(float fTimeElapsed)
{

}

void Warrior::Move(float fTimeElapsed)
{
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

	if (!m_bRButtonClicked)
	{
		switch (tempDir)
		{
		case DIRECTION::IDLE:
		{
			XMFLOAT3 xmf3Position = GetPosition();
			xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, m_interpolationDistance * fTimeElapsed);
			SetPosition(xmf3Position);
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
		}
		break;
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
	else
	{
		//fDistance /= 3;
		switch (tempDir)
		{
		case DIRECTION::IDLE:
		{
			XMFLOAT3 xmf3Position = GetPosition();
			xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, m_interpolationDistance * fTimeElapsed);
			SetPosition(xmf3Position);
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
		}
		break;
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

	if (m_bMoveState)
	{
		SetLookDirection();
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
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
		if (m_skillCoolTime[0] > duration) return;
		m_skillInputTime[0] = std::chrono::high_resolution_clock::now();
	}
	m_currentDirection = DIRECTION::IDLE;
	m_bMoveState = false;
	m_bQSkillClicked = true;
}

void Warrior::SetStage1Position()
{
	SetPosition(XMFLOAT3(-1290.0f, 0, -1470.0f));
}

void Warrior::SetBossStagePostion()
{
	SetPosition(XMFLOAT3(0, 0, -211.0f));
}

void Warrior::ExecuteSkill_Q()
{
	if (m_bQSkillClicked && m_bESkillClicked) {
		m_bQSkillClicked = true;
		g_NetworkHelper.Send_SkillExecute_Q(GetLook());
	}
}

void Warrior::ExecuteSkill_E()
{//None
}

Archer::Archer() : Character()
{
	m_fHp = 250.0f;
	m_fMaxHp = 250.0f;
	m_fSpeed = 50.0f;
	m_fDamage = 80.0f;
	m_CameraLook = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Archer::~Archer()
{
	for (auto& i : m_ppProjectiles)
	{
		if (i != nullptr)
			delete i;
	}
}

void Archer::Attack()
{
	if (m_pCamera) {
		//줌인에 대한 정보 필요할듯함		
		int power = 0;//줌인 정보
		g_NetworkHelper.SendCommonAttackExecute(GetLook(), power);//두번째 인자 - 줌인 정도
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

void Archer::Move(float fTimeElapsed)
{
	//fDistance *= m_fSpeed;
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
	case DIRECTION::IDLE:
	{
		XMFLOAT3 xmf3Position = GetPosition();
		xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, m_interpolationDistance * fTimeElapsed);
		SetPosition(xmf3Position);
		if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	}
	break;
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

void Archer::Animate(float fTimeElapsed)
{
	m_fTimeElapsed = fTimeElapsed;
	if (m_bLButtonClicked)
		ZoomInCamera();

	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations = { CharacterAnimation::CA_DIE, CharacterAnimation::CA_DIE };
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
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
				ShootArrow();
				m_bCanAttack = false;
			}
			else if (m_bQSkillClicked)
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

	// 현재 재생될 애니메이션 선택
	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bLButtonClicked || m_bQSkillClicked || m_bESkillClicked || m_bOnAttack)
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
		if (m_bLButtonClicked || m_bQSkillClicked || m_bESkillClicked || m_bOnAttack)
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

	ResetArrow();
	ChangeAnimation(AfterAnimation);
	SetLookDirection();
	Move(fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Archer::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Archer::SetLookDirection()
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	SetLook(xmf3Look);
}

void Archer::ZoomInCamera()
{
	if (!(m_pSkinnedAnimationController->m_CurrentAnimations.first == CharacterAnimation::CA_ATTACK &&
		CheckAnimationEnd(CA_ATTACK) == false)
		|| m_bLButtonClicked == true)
	{
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
			m_bOnAttack = false;
			m_bCanAttack = true;
			m_CameraLook = m_xmf3RotateAxis;
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
	if (m_bQSkillClicked == true)
	{
		if (g_Logic.GetMyRole() == ROLE::ARCHER)
			g_NetworkHelper.Send_SkillExecute_Q(Vector3::Normalize(GetObjectLook()));
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
	else if (m_bZoomInState == true)
	{
		float chargingTime = m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition;
		float fullTime = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fLength * 0.5f;
		float arrowSpeed = pow((chargingTime / fullTime), 2);
		float fProgress = std::clamp(arrowSpeed, 0.65f, 1.0f);
		arrowSpeed = (chargingTime / fullTime > 0.3f) ? arrowSpeed * 100.0f : -1.0f;
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
			int arrowPower = arrowSpeed / 30;
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
				g_NetworkHelper.SendCommonAttackExecute(m_ppProjectiles[m_nProjectiles]->m_xmf3direction, arrowPower);
			m_nProjectiles++;
		}
		m_bZoomInState = false;
		Character::RbuttonUp(m_CameraLook);
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
	else if (m_bOnAttack == true)
	{// 1개의 화살을 발사하는 기본 공격 실행
		m_ppProjectiles[m_nProjectiles]->m_xmf3direction = Vector3::Normalize(XMFLOAT3(GetObjectLook().x, -sin(m_xmf3RotateAxis.x * 3.141592 / 180.0f), GetObjectLook().z));
		m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = XMFLOAT3(GetPosition().x, GetPosition().y + 8.0f, GetPosition().z);
		m_ppProjectiles[m_nProjectiles]->SetPosition(m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition);
		m_ppProjectiles[m_nProjectiles]->m_fSpeed = 150.0f;
		m_ppProjectiles[m_nProjectiles]->m_bActive = true;

		if (g_Logic.GetMyRole() == ROLE::ARCHER)
			g_NetworkHelper.SendCommonAttackExecute(m_ppProjectiles[m_nProjectiles]->m_xmf3direction, 0);
		m_nProjectiles++;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
}

void Archer::ShootArrow(const XMFLOAT3& xmf3Direction)
{
	// 플레이어 캐릭터가 아닐 때
	m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
	XMFLOAT3 objPosition = GetPosition();
	objPosition.y += 8.0f;

	if (m_bQSkillClicked == true)
	{
		for (int i = 0; i < 3; ++i)//서버에 옮겨야됨
		{
			m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
			XMFLOAT3 objectLook = GetObjectLook();
			XMFLOAT3 objectRight = GetRight();
			objPosition.y = 6.0f + (i % 2) * 4.0f;
			objPosition = Vector3::Add(objPosition, objectRight, (1 - i) * 4.0f);
			objPosition = Vector3::Add(objPosition, objectLook, 1.0f);

			m_ppProjectiles[m_nProjectiles]->m_xmf3direction = Vector3::Normalize(xmf3Direction);
			m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = objPosition;
			m_ppProjectiles[m_nProjectiles]->SetPosition(objPosition);
			m_ppProjectiles[m_nProjectiles]->m_fSpeed = 250.0f;
			m_ppProjectiles[m_nProjectiles]->m_bActive = true;
			m_nProjectiles++;
		}
		m_bQSkillClicked = false;
	}
	else if (m_bZoomInState == true)
	{
		m_ppProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
		m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = objPosition;
		m_ppProjectiles[m_nProjectiles]->SetPosition(objPosition);
		m_ppProjectiles[m_nProjectiles]->m_fSpeed = 150.0f;
		m_ppProjectiles[m_nProjectiles]->m_bActive = true;

		m_nProjectiles++;
		m_bZoomInState = false;
		Character::RbuttonUp();
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
	else if (m_bOnAttack == true)
	{
		m_ppProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
		m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = objPosition;
		m_ppProjectiles[m_nProjectiles]->SetPosition(objPosition);
		m_ppProjectiles[m_nProjectiles]->m_fSpeed = 150.0f;
		m_ppProjectiles[m_nProjectiles]->m_bActive = true;
		m_nProjectiles++;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
}

void Archer::SetStage1Position()
{
	SetPosition(XMFLOAT3(-1340.84f, 0, -1520.93f));
}

void Archer::SetBossStagePostion()
{
	SetPosition(XMFLOAT3(123, 0, -293));
}

void Archer::ExecuteSkill_Q()
{
	if (m_bQSkillClicked && m_bESkillClicked) {
		m_bQSkillClicked = true;
		g_NetworkHelper.Send_SkillExecute_Q(GetLook());
	}
}

void Archer::ExecuteSkill_E()
{
	if (m_bQSkillClicked && m_bESkillClicked) {
		m_bQSkillClicked = true;
		g_NetworkHelper.Send_SkillExecute_E(GetPosition());//tagetPosition
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
	m_fSpeed = 50.0f;
	m_fDamage = 50.0f;

	m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(0) };
	m_skillDuration = { std::chrono::seconds(5), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0], std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
}

Tanker::~Tanker()
{

}

void Tanker::Attack()
{
	if (m_pCamera) {
		g_NetworkHelper.SendCommonAttackExecute(GetLook(), 0);
	}
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

void Tanker::FirstSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::TANKER)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
		if (m_skillCoolTime[0] > duration) return;
		g_NetworkHelper.Send_SkillExecute_Q(XMFLOAT3(0.0, 0.0, 0.0));
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
		m_currentDirection = DIRECTION::IDLE;
		m_skillInputTime[1] = std::chrono::high_resolution_clock::now();
	}
	m_currentDirection = DIRECTION::IDLE;
	m_bMoveState = false;
	m_bESkillClicked = true;
}

void Tanker::Move(float fTimeElapsed)
{
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

	if (!m_bRButtonClicked)
	{
		switch (tempDir)
		{
		case DIRECTION::IDLE:
		{
			XMFLOAT3 xmf3Position = GetPosition();
			xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, m_interpolationDistance * fTimeElapsed);
			SetPosition(xmf3Position);
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
		}
		break;
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
	else
	{
		//fDistance /= 3;
		switch (tempDir)
		{
		case DIRECTION::IDLE:
		{
			XMFLOAT3 xmf3Position = GetPosition();
			xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, m_interpolationDistance * fTimeElapsed);
			SetPosition(xmf3Position);
			if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
		}
		break;
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
}

void Tanker::Animate(float fTimeElapsed)
{
	//if (m_bRButtonClicked)
	//	RbuttonClicked(fTimeElapsed);
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

	SetLookDirection();
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
		case 2: static_cast<EnergyBall*>(m_ppProjectiles[2])->SetTarget(ROLE::PRIEST); break;
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
	SetPosition(XMFLOAT3(-1260.3f, 0, -1510.7f));
}

void Tanker::SetBossStagePostion()
{
	SetPosition(XMFLOAT3(82, 0, -223.0f));
}

void Tanker::ExecuteSkill_Q()
{
	if (m_bQSkillClicked && m_bESkillClicked) {
		m_bQSkillClicked = true;
		g_NetworkHelper.Send_SkillExecute_Q(GetLook());
	}
}

void Tanker::ExecuteSkill_E()
{
	if (m_bQSkillClicked && m_bESkillClicked) {
		m_bQSkillClicked = true;
		g_NetworkHelper.Send_SkillExecute_E(GetLook());
	}
}

Priest::Priest() : Character()
{
	m_fHp = 480.0f;
	m_fMaxHp = 480.0f;
	m_fSpeed = 50.0f;
	m_fDamage = 80.0f;

	m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(7) };
	m_skillDuration = { std::chrono::seconds(9), std::chrono::seconds(3) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0], std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
}

Priest::~Priest()
{
}

void Priest::RbuttonClicked(float fTimeElapsed)
{
}

void Priest::Move(float fTimeElapsed)
{
	//fDistance *= m_fSpeed;
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
	case DIRECTION::IDLE:
	{
		XMFLOAT3 xmf3Position = GetPosition();
		xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, m_interpolationDistance * fTimeElapsed);
		SetPosition(xmf3Position);
		if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	}
	break;
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

	for (int i = 0; i < m_ppProjectiles.size(); ++i)
		if (m_ppProjectiles[i]) m_ppProjectiles[i]->Animate(fTimeElapsed);

	SetLookDirection();
	Move(fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Priest::RbuttonUp(const XMFLOAT3& CameraAxis)
{
}

void Priest::Attack()
{
	m_nProjectiles = (m_nProjectiles < 10) ? m_nProjectiles : m_nProjectiles % 10;
	XMFLOAT3 ObjectLookVector = GetLook();
	ObjectLookVector.y = -m_xmf3RotateAxis.x / 90.0f;

	XMFLOAT3 xmf3LanceStartPosition = XMFLOAT3(GetPosition().x, GetPosition().y + 8.0f, GetPosition().z);
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	XMFLOAT3 xmf3CameraDirection = Vector3::Normalize(m_pCamera->GetLookVector());
	XMFLOAT3 xmf3Destination = Vector3::Add(xmf3CameraPosition, xmf3CameraDirection, 200.0f);
	XMFLOAT3 xmf3LanceDirection = Vector3::Normalize(Vector3::Subtract(xmf3Destination, xmf3LanceStartPosition));
	if (xmf3LanceDirection.y < 0.0f) xmf3LanceDirection.y = 0.0f;

	m_ppProjectiles[m_nProjectiles]->m_xmf3direction = xmf3LanceDirection;
	m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = xmf3LanceStartPosition;
	m_ppProjectiles[m_nProjectiles]->SetPosition(m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition);
	m_ppProjectiles[m_nProjectiles]->m_bActive = true;

	if (g_Logic.GetMyRole() == ROLE::PRIEST)
		g_NetworkHelper.SendCommonAttackExecute(ObjectLookVector, 0);
	m_nProjectiles++;
}

void Priest::Attack(const XMFLOAT3& xmf3Direction)
{
	m_nProjectiles = (m_nProjectiles < 10) ? m_nProjectiles : m_nProjectiles % 10;
	XMFLOAT3 objectPosition = GetPosition();
	objectPosition.y += 8.0f;

	m_ppProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
	m_ppProjectiles[m_nProjectiles]->m_xmf3startPosition = objectPosition;
	m_ppProjectiles[m_nProjectiles]->SetPosition(objectPosition);
	m_ppProjectiles[m_nProjectiles]->m_fSpeed = 150.0f;
	m_ppProjectiles[m_nProjectiles]->m_bActive = true;
	m_nProjectiles++;
}

void Priest::SetProjectile(Projectile** pEnergyBall)
{
	for (int i = 0; i < 10; ++i)
	{
		m_ppProjectiles[i] = pEnergyBall[i];
		m_ppProjectiles[i]->m_bActive = false;
	}
}

void Priest::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	for (int i = 0; i < m_ppProjectiles.size(); ++i)
		if (m_ppProjectiles[i])
			m_ppProjectiles[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);

	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Priest::SetLookDirection()
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	SetLook(xmf3Look);
}

void Priest::FirstSkillDown()
{
	if (g_Logic.GetMyRole() == ROLE::PRIEST)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
		if (m_skillCoolTime[0] > duration) return;
		g_NetworkHelper.Send_SkillExecute_Q(XMFLOAT3(0.0, 0.0, 0.0));
		m_skillInputTime[0] = std::chrono::high_resolution_clock::now();
	}
	m_bQSkillClicked = true;

#ifdef LOCAL_TASK
	StartEffect(0);
#endif
}

void Priest::SecondSkillDown()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[1]);
	if (m_skillCoolTime[1] > duration)
	{
		gGameFramework.GetScene()->GetObjectManager()->m_bPickingenemy = false;
		return;
	}
	m_bESkillClicked = true;
	m_skillInputTime[1] = std::chrono::high_resolution_clock::now();
}

void Priest::FirstSkillUp()
{
}

void Priest::StartEffect(int nSkillNum)
{
	m_pHealRange->m_bActive = true;
	g_sound.Play("HealSound", 1.0);
	UpdateEffect();
}

void Priest::EndEffect(int nSkillNum)
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

void Priest::UpdateEffect()
{
	array<Character*, 4> pOtherCharacter;
	pOtherCharacter[0] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::WARRIOR);
	pOtherCharacter[1] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::PRIEST);
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

void Priest::SetStage1Position()
{
	SetPosition(XMFLOAT3(-1370.45, 0, -1450.89f));
}

void Priest::SetBossStagePostion()
{
	SetPosition(XMFLOAT3(20, 0, -285));
}

void Priest::ExecuteSkill_Q()
{
	if (m_bQSkillClicked && m_bESkillClicked) {
		m_bQSkillClicked = true;
		g_NetworkHelper.Send_SkillExecute_Q(GetLook());
	}
}

void Priest::ExecuteSkill_E()
{
	if (m_bQSkillClicked && m_bESkillClicked) {
		m_bQSkillClicked = true;
		g_NetworkHelper.Send_SkillExecute_E(GetPosition());//target
	}
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
	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bOnSkill)
		{
			// 움직이는 스킬 (DASH_FORWARD)
			// 이동방식 회의 후 구현 위치
		}
		else
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
				}
			}
		}
	}
}

void Monster::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos, XMFLOAT3& moveVec)
{
	auto clientUtcTime = std::chrono::utc_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(clientUtcTime - recvTime).count();
	XMFLOAT3 xmf3Postion = GetPosition();
	durationTime -= g_Logic.GetDiffTime();
	durationTime = (double)durationTime / 1000.0f;//microseconds to mill
	durationTime = (double)durationTime / 1000.0f;//milliseconds to sec

	XMFLOAT3 diff_S2C_Position = Vector3::Subtract(recvPos, xmf3Postion);

	XMFLOAT3 moveDir = Vector3::ScalarProduct(moveVec, durationTime * 50.0f);

	XMFLOAT3 interpolateVec = Vector3::Add(diff_S2C_Position, moveDir);
	float interpolateSize = Vector3::Length(interpolateVec);
	interpolateVec = Vector3::Normalize(interpolateVec);
	if (Vector3::Length(diff_S2C_Position) < DBL_EPSILON) {
		m_interpolationDistance = 0.0f;
	}
	else if (interpolateSize < 5.0f) {
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

Projectile::Projectile(entity_id eid) : GameObject(eid)
{
	m_xmf3startPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	m_xmf3direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_fSpeed = 0.0f;
	m_bActive = false;
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

void Projectile::Move(XMFLOAT3 dir, float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	float moveDist = Vector3::Length(Vector3::Subtract(xmf3Position, m_xmf3startPosition));
	if (moveDist > 250.0f)
	{
		SetActive(false);
		return;
	}

	xmf3Position = Vector3::Add(xmf3Position, dir, fDistance);
	GameObject::SetPosition(xmf3Position);

}

Arrow::Arrow() : Projectile()
{
	m_fSpeed = 150.0f;
	//m_xmf3TargetPos = XMFLOAT3(0.0f, -1.0f, 0.0f);
	//m_ArrowType = 0;
	//m_ArrowPos = 0;
	// Y값이 마우스 회전 범위 안쪽이면 일반 화살 아니면 꺾이는 화살
}

Arrow::~Arrow()
{
}

void Arrow::Animate(float fTimeElapsed)
{
	if (m_bActive == false) return;
	XMFLOAT3 xmf3CurrentPos = GetPosition();

	//if (m_ArrowType == 1)
	//{
	//	if (m_ArrowPos > 1.0f)
	//	{
	//		m_bActive = false;
	//		return;
	//	}
	//	Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
	//	m_xmf3TargetPos = bossMonster->GetPosition();
	//	XMFLOAT3 controlPoint = Vector3::Add(m_xmf3startPosition, m_xmf3direction, 100.0f);
	//	XMFLOAT3 firstVec = Vector3::Subtract(controlPoint, m_xmf3startPosition);
	//	XMFLOAT3 secondVec = Vector3::Subtract(m_xmf3TargetPos, controlPoint);
	//	XMFLOAT3 tempPoint1 = Vector3::Add(Vector3::ScalarProduct(m_xmf3startPosition, (1 - m_ArrowPos), false), Vector3::ScalarProduct(controlPoint, m_ArrowPos, false));
	//	XMFLOAT3 tempPoint2 = Vector3::Add(Vector3::ScalarProduct(controlPoint, (1 - m_ArrowPos), false), Vector3::ScalarProduct(m_xmf3TargetPos, m_ArrowPos, false));
	//	XMFLOAT3 curPos = Vector3::Add(Vector3::ScalarProduct(tempPoint1, (1 - m_ArrowPos), false), Vector3::ScalarProduct(tempPoint2, m_ArrowPos, false));
	//	SetPosition(curPos);
	//	SetLook(Vector3::Subtract(tempPoint2, tempPoint1));
	//	m_ArrowPos += fTimeElapsed;	// 스플라인 곡선에서의 t의 역할
	//}
	//else
	//{
	SetLook(m_xmf3direction);
	Move(m_xmf3direction, fTimeElapsed * m_fSpeed);
	if (m_VisualizeSPBB) m_VisualizeSPBB->SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z));
	if (GetPosition().y < -1.0f) SetActive(false);
}

void Arrow::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Arrow::SetActive(bool bActive)
{
	m_bActive = bActive;
	Character* archerCharacter = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);
	GameObject::SetPosition(archerCharacter->GetPosition());
}

EnergyBall::EnergyBall() : Projectile(SQUARE_ENTITY)
{
	m_fSpeed = 75.0f;
	m_fProgress = 0.0f;
}

EnergyBall::~EnergyBall()
{
}

void EnergyBall::Animate(float fTimeElapsed)
{
	if (m_bActive == false) return;

	if (m_fProgress > 1.0f)
	{
		m_bActive = false;
		m_fProgress = 0.0f;
		EffectObject** shieldEffects = gGameFramework.GetScene()->GetObjectManager()->GetShieldEffectArr();
		for (int i = 0; i < 4; ++i)
			shieldEffects[i]->SetActive(true);
		return;
	}

	Character* Target = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(m_Target);
	XMFLOAT3 TargetPos = Target->GetPosition();
	TargetPos.y += 8.0f;
	m_xmf3direction = Vector3::Normalize(Vector3::Subtract(TargetPos, m_xmf3startPosition));
	m_xmf3direction.y = 0.6f;

	XMFLOAT3 controlPoint = Vector3::Add(m_xmf3startPosition, m_xmf3direction, 100.0f);
	XMFLOAT3 firstVec = Vector3::Subtract(controlPoint, m_xmf3startPosition);
	XMFLOAT3 secondVec = Vector3::Subtract(TargetPos, controlPoint);

	XMFLOAT3 FirstLinePoint = Vector3::Add(Vector3::ScalarProduct(m_xmf3startPosition, (1 - m_fProgress), false), Vector3::ScalarProduct(controlPoint, m_fProgress, false));
	XMFLOAT3 SecondLinePoint = Vector3::Add(Vector3::ScalarProduct(controlPoint, (1 - m_fProgress), false), Vector3::ScalarProduct(TargetPos, m_fProgress, false));
	XMFLOAT3 curPos = Vector3::Add(Vector3::ScalarProduct(FirstLinePoint, (1 - m_fProgress), false), Vector3::ScalarProduct(SecondLinePoint, m_fProgress, false));

	SetPosition(curPos);
	// SetLook(Vector3::Subtract(SecondLinePoint, FirstLinePoint));
	m_fProgress += fTimeElapsed * 0.8f;
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
			pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_DIE, CharacterAnimation::CA_DIE };
			m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
			m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
		}
		/*	if (CheckAnimationEnd(CA_DIE))
			{
				m_bActive = false;
			}*/
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
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(clientUtcTime - recvTime).count();
	XMFLOAT3 xmf3Postion = GetPosition();
	durationTime -= g_Logic.GetDiffTime();
	durationTime = (double)durationTime / 1000.0f;//microseconds to mill
	durationTime = (double)durationTime / 1000.0f;//milliseconds to sec

	XMFLOAT3 diff_S2C_Position = Vector3::Subtract(recvPos, xmf3Postion);

	XMFLOAT3 moveDir = Vector3::ScalarProduct(moveVec, durationTime * 30.0f);

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

std::pair<float, XMFLOAT3> Character::GetNormalVectorSphere(const XMFLOAT3& point)
{
	XMFLOAT3 normalVec = Vector3::Subtract(GetPosition(), point);
	float normalSize = Vector3::Length(normalVec);
	normalVec = Vector3::Normalize(normalVec);
	return std::pair<float, XMFLOAT3>(normalSize, normalVec);
}

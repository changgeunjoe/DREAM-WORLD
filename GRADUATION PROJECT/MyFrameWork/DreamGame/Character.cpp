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
#include "CharacterEvent.h"

extern Logic g_Logic;
extern NetworkHelper g_NetworkHelper;
extern bool GameEnd;
extern MapData g_bossMapData;
extern MapData g_stage1MapData;
extern CGameFramework gGameFramework;
extern GameSound g_sound;

//#define CHARCTER_MOVE_LOG 1
//#define MONSTER_MOVE_LOG 1

Character::Character() : GameObject(UNDEF_ENTITY), m_interpolateData(std::make_shared<CharacterEvent::InterpolateData>())
{
	m_fHp = 100.0f;
	m_fSpeed = 50.0f;
}

Character::Character(ROLE r) :GameObject(UNDEF_ENTITY), m_role(r), m_interpolateData(std::make_shared<CharacterEvent::InterpolateData>())
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

	XMFLOAT3 forwardVector = GetLook();
	forwardVector.y = 0.0f;
	forwardVector = Vector3::Normalize(forwardVector);
	forwardVector = Vector3::ScalarProduct(forwardVector, (float)forwardDirection);

	XMFLOAT3 currentPosition = GetPosition();
	XMFLOAT3 nextPosition = GetPosition();
	nextPosition = Vector3::Add(nextPosition, forwardVector, ftimeElapsed * m_fSpeed);

	std::optional<std::pair<bool, XMFLOAT3>>mapCollide;
	if (gGameFramework.GetScene()->GetObjectManager()->m_nStageType == STAGE1) {
		mapCollide = CheckCollisionMap_Stage(nextPosition, forwardVector, ftimeElapsed);
	}
	else if (gGameFramework.GetScene()->GetObjectManager()->m_nStageType == STAGE2) {
		mapCollide = CheckCollisionMap_Boss(nextPosition, forwardVector, ftimeElapsed);
	}
	if (!mapCollide.has_value()) {
		nextPosition = currentPosition;
	}
	else {
		auto characterCollideResult = CheckCollisionCharacterObject(nextPosition, forwardVector, mapCollide.value().first, ftimeElapsed);
		if (characterCollideResult.has_value()) {
			nextPosition = characterCollideResult.value().second;
		}
	}

	if (m_interpolateData->GetInterpolateState() == CharacterEvent::INTERPOLATE_STATE::INTERPOALTE) {
		auto interpolateData = m_interpolateData->GetInterpolateData();
		nextPosition = Vector3::Add(nextPosition, interpolateData.second, 10.0f * interpolateData.first * ftimeElapsed);
	}
	GameObject::SetPosition(nextPosition);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	g_sound.Play("WalkSound", CalculateDistanceSound());
}

void Character::MoveStrafe(int rightDirection, float ftimeElapsed)
{
	if (std::isnan(m_xmf4x4ToParent._11)) {
		std::cout << "문제 있음" << std::endl;
	}

	XMFLOAT3 rightVector = GetRight();
	rightVector.y = 0.0f;
	rightVector = Vector3::ScalarProduct(rightVector, (float)rightDirection);


	XMFLOAT3 currentPosition = GetPosition();
	XMFLOAT3 nextPosition = GetPosition();
	nextPosition = Vector3::Add(nextPosition, rightVector, ftimeElapsed * m_fSpeed);

	std::optional<std::pair<bool, XMFLOAT3>>mapCollide;
	if (gGameFramework.GetScene()->GetObjectManager()->m_nStageType == STAGE1) {
		mapCollide = CheckCollisionMap_Stage(nextPosition, rightVector, ftimeElapsed);
	}
	else if (gGameFramework.GetScene()->GetObjectManager()->m_nStageType == STAGE2) {
		mapCollide = CheckCollisionMap_Boss(nextPosition, rightVector, ftimeElapsed);
	}
	if (!mapCollide.has_value()) {
		nextPosition = currentPosition;
	}
	else {
		auto characterCollideResult = CheckCollisionCharacterObject(nextPosition, rightVector, mapCollide.value().first, ftimeElapsed);
		if (characterCollideResult.has_value()) {
			nextPosition = characterCollideResult.value().second;
		}
	}

	if (m_interpolateData->GetInterpolateState() == CharacterEvent::INTERPOLATE_STATE::INTERPOALTE) {
		auto interpolateData = m_interpolateData->GetInterpolateData();
		nextPosition = Vector3::Add(nextPosition, interpolateData.second, 10.0f * interpolateData.first * ftimeElapsed);
	}
	GameObject::SetPosition(nextPosition);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	g_sound.Play("WalkSound", CalculateDistanceSound());
}

void Character::MoveDiagonal(int fowardDirection, int rightDirection, float ftimeElapsed)
{
	if (std::isnan(m_xmf4x4ToParent._11)) {
		std::cout << "문제 있음" << std::endl;
	}

	XMFLOAT3 xmf3Look = GetLook();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Look = Vector3::ScalarProduct(xmf3Look, fowardDirection);
	xmf3Right = Vector3::ScalarProduct(xmf3Right, rightDirection);

	XMFLOAT3 dialogVector = Vector3::Add(xmf3Right, xmf3Look);
	dialogVector.y = 0;
	dialogVector = Vector3::Normalize(dialogVector);

	XMFLOAT3 currentPosition = GetPosition();
	XMFLOAT3 nextPosition = GetPosition();
	nextPosition = Vector3::Add(nextPosition, dialogVector, ftimeElapsed * m_fSpeed);

	std::optional<std::pair<bool, XMFLOAT3>>mapCollide;
	if (gGameFramework.GetScene()->GetObjectManager()->m_nStageType == STAGE1) {
		mapCollide = CheckCollisionMap_Stage(nextPosition, dialogVector, ftimeElapsed);
	}
	else if (gGameFramework.GetScene()->GetObjectManager()->m_nStageType == STAGE2) {
		mapCollide = CheckCollisionMap_Boss(nextPosition, dialogVector, ftimeElapsed);
	}
	if (!mapCollide.has_value()) {
		nextPosition = currentPosition;
	}
	else {
		auto characterCollideResult = CheckCollisionCharacterObject(nextPosition, dialogVector, mapCollide.value().first, ftimeElapsed);
		if (characterCollideResult.has_value()) {
			nextPosition = characterCollideResult.value().second;
		}
	}

	if (m_interpolateData->GetInterpolateState() == CharacterEvent::INTERPOLATE_STATE::INTERPOALTE) {
		auto interpolateData = m_interpolateData->GetInterpolateData();
		nextPosition = Vector3::Add(nextPosition, interpolateData.second, 10.0f * interpolateData.first * ftimeElapsed);
	}
	GameObject::SetPosition(nextPosition);
	if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
	g_sound.Play("WalkSound", CalculateDistanceSound());
}

void Character::SetInterpolateData(const chrono::high_resolution_clock::time_point& recvTime, const XMFLOAT3& recvPos)
{
	m_interpolateData->InsertEvent(recvPos, recvTime);
}

std::optional<std::pair<bool, XMFLOAT3>> Character::CheckCollisionCharacterObject(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, const bool& isApplySlidingVector, float ftimeElapsed)
{
	Character* collideObject = nullptr;
	auto allCharacterObjects = gGameFramework.GetScene()->GetObjectManager()->GetAllCharacterObject();
	for (auto& characterObject : allCharacterObjects) {
		if (!characterObject->GetAliveState()) continue;
		if (characterObject == this) continue;
		float distance = characterObject->GetDistance(nextPosition);
		if (FLT_EPSILON > m_SPBB.Radius + characterObject->m_SPBB.Radius - distance) continue;
		if (isApplySlidingVector || nullptr != collideObject)
			return std::nullopt;
		collideObject = characterObject;
	}
	if (nullptr == collideObject)
		return std::pair<bool, XMFLOAT3>(false, nextPosition);
	XMFLOAT3 toThisVector = collideObject->GetToVector(GetPosition());
	XMFLOAT3 normalVector = Vector3::Normalize(toThisVector);
	XMFLOAT3 slidingVector = XMFLOAT3(-normalVector.z, 0.0f, normalVector.x);//(다른 객체 -> 현재 객체)벡터를 90도 회전 => 즉 좌 또는 우 방향성 벡터
	XMFLOAT3 currentMoveVector = moveVector;

	float slidingDotLookResult = Vector3::DotProduct(slidingVector, currentMoveVector);//현재 룩 방향에 맞는 슬라이딩 벡터 확인하기 위한 내적
	if (slidingDotLookResult < 0)slidingVector = Vector3::ScalarProduct(slidingVector, -1.0f, false);//0보다 작다는건, 반대 방향을 의미
	XMFLOAT3 applySlidingVectorPosition = GetPosition();
	applySlidingVectorPosition = Vector3::Add(applySlidingVectorPosition, slidingVector, m_fSpeed * ftimeElapsed);
	return std::pair<bool, XMFLOAT3>(true, applySlidingVectorPosition);
}

std::optional<std::pair<bool, XMFLOAT3>> Character::CheckCollisionBoss(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, float ftimeElapsed)
{
	Monster* boss = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
	return std::optional<std::pair<bool, XMFLOAT3>>();
}

void Character::UpdateInterpolateData()
{
	m_interpolateData->ProcessEvents(this);
}

bool Character::CheckAnimationEnd(int nAnimation)
{
	return m_pSkinnedAnimationController->m_pAnimationTracks[nAnimation].m_bAnimationEnd;
}

std::optional<std::pair<bool, XMFLOAT3>> Character::CheckCollisionMap_Boss(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, float ftimeElapsed)
{
	return std::pair<bool, XMFLOAT3>(false, nextPosition);
}

std::optional<std::pair<bool, XMFLOAT3>> Character::CheckCollisionMap_Stage(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, float ftimeElapsed)
{
	return std::pair<bool, XMFLOAT3>(false, nextPosition);
}

const float Character::GetDistance(const XMFLOAT3& otherPosition) const
{
	auto vector = GetFromVector(otherPosition);
	return Vector3::Length(vector);
}

const XMFLOAT3 Character::GetToVector(const XMFLOAT3& otherPosition) const
{
	return { otherPosition.x - GetPosition().x, 0.0f , otherPosition.z - GetPosition().z };
}

const XMFLOAT3 Character::GetFromVector(const XMFLOAT3& otherPosition) const
{
	return { GetPosition().x - otherPosition.x, 0.0f , GetPosition().z - otherPosition.z };
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
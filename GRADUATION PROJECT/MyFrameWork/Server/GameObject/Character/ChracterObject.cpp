#include "stdafx.h"
#include "ChracterObject.h"
#include "../Room/Room.h"
#include "../MapData/MapData.h"
#include "../MapData/MapCollision/MapCollision.h"
#include "../EventController/CoolDownEventBase.h"
#include "../EventController/DurationEvent.h"
#include "../Room/RoomEvent.h"
#include "../Network/protocol/protocol.h"

CharacterObject::CharacterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, const float& attackDamage, std::shared_ptr<Room>& roomRef, const ROLE& role)
	: LiveObject(maxHp, moveSpeed, boundingSize, roomRef), m_currentDirection(static_cast<char>(DIRECTION::IDLE)),
	m_leftMouseInput(false), m_rightMouseInput(false),
	m_Shield(0.0f), m_activeShield(false), m_commonAttackDamage(attackDamage), m_role(role)
{
	m_skillCtrl = std::make_unique<EventController>();
}

void CharacterObject::Update()
{
	//회전 각 업데이트
	UpdateRotate();
	//Move
	float elapstedTime = GetElapsedLastUpdateTime();
	auto positionResult = UpdateNextPosition(elapstedTime);
	if (positionResult.has_value()) {
		SetPosition(positionResult.value());
	}
}

void CharacterObject::StopMove()
{
	constexpr static char IDLE_BIT = static_cast<char>(DIRECTION::IDLE);
	m_applyDIrection = IDLE_BIT;
}

void CharacterObject::RecvDirection(const DIRECTION& dir, const bool& apply)
{
	if (apply) m_applyDIrection |= static_cast<char>(dir);
	else m_applyDIrection ^= static_cast<char>(dir);
	//spdlog::warn("RecvDirection - applyDir: {}", (int)m_applyDIrection);
}

void CharacterObject::RecvRotate(const ROTATE_AXIS& axis, const float& angle)
{
	switch (axis)
	{
	case ROTATE_AXIS::X:
		m_angleData.x.applyAngle += angle;
		break;
	case ROTATE_AXIS::Y:
		m_angleData.y.applyAngle += angle;
		break;
	case ROTATE_AXIS::Z:
		m_angleData.z.applyAngle += angle;
		break;
	default:
		break;
	}
}

void CharacterObject::SetShield(const bool& active)
{
	constexpr static float MAX_SHIELD = 200.0f;
	if (active) {
		m_Shield = MAX_SHIELD;
		m_activeShield = true;
		return;
	}
	m_activeShield = false;
}

const float CharacterObject::GetShield() const
{
	return m_Shield;
}

void CharacterObject::Attacked(const float& damage)
{
	float applyDamage = damage;
	if (m_activeShield) {
		applyDamage *= REDUCE_DAMAGE_APPLY_RATIO;
		if (m_Shield - applyDamage < FLT_EPSILON) {
			applyDamage -= m_Shield;
			m_Shield = 0.0f;
		}
		else {
			applyDamage = 0.0f;
			m_Shield -= applyDamage;
		}
	}
	m_hp -= applyDamage;
	auto damagedEvent = std::make_shared<PlayerDamagedEvent>(m_role, m_hp, m_Shield);
	m_roomRef->InsertAftrerUpdateEvent(damagedEvent);

	if (m_hp < FLT_EPSILON) {
		auto dieEvent = std::make_shared<PlayerDieEvent>(m_role);
		m_roomRef->InsertAftrerUpdateEvent(dieEvent);
		m_hp = 0.0;
		m_isAlive = false;
	}
}

void CharacterObject::RecvMouseInput(const bool& LmouseInput, const bool& RmouseInput)
{
	m_leftMouseInput = LmouseInput;
	m_rightMouseInput = RmouseInput;
}

void CharacterObject::RecvSkillInput(const SKILL_TYPE& type)
{
	if (SKILL_TYPE::SKILL_TYPE_Q == type) {
		auto skillData = m_skillCtrl->GetEventData(SKILL_Q);
		const bool isAbleSkill = skillData->IsAbleExecute();
		if (isAbleSkill) {
			SERVER_PACKET::NotifyPlayerAnimationPacket sendPacket(static_cast<char>(SERVER_PACKET::TYPE::START_ANIMATION_Q), GetRole(), skillData->GetLastExeTime());
			m_roomRef->BroadCastPacket(&sendPacket);
		}
	}
	else if (SKILL_TYPE::SKILL_TYPE_E == type) {
		auto skillData = m_skillCtrl->GetEventData(SKILL_E);
		const bool isAbleSkill = skillData->IsAbleExecute();
		if (isAbleSkill) {
			SERVER_PACKET::NotifyPlayerAnimationPacket sendPacket(static_cast<char>(SERVER_PACKET::TYPE::START_ANIMATION_E), GetRole(), skillData->GetLastExeTime());
			m_roomRef->BroadCastPacket(&sendPacket);
		}
	}
}

void CharacterObject::ForceStopMove()
{
	m_currentDirection = IDLE_BIT;
}

void CharacterObject::ResetSkillCoolTime()
{
	auto skillQ = m_skillCtrl->GetEventData(SKILL_Q);
	if (nullptr != skillQ) skillQ->ResetCoolTime();
	auto skillE = m_skillCtrl->GetEventData(SKILL_E);
	if (nullptr != skillE) skillE->ResetCoolTime();
}

void CharacterObject::UpdateDirection()
{
	m_currentDirection = m_applyDIrection;
}

void CharacterObject::UpdateRotate()
{
	//mouse Rotate Update
	static constexpr int Get_X = 0;
	static constexpr int Get_Y = 1;
	static constexpr int Get_Z = 2;
	auto newAngleData = m_angleData.Update();
	GameObject::Rotate(ROTATE_AXIS::Y, std::get<Get_Y>(newAngleData));
	GameObject::Rotate(ROTATE_AXIS::X, std::get<Get_X>(newAngleData));
	GameObject::Rotate(ROTATE_AXIS::Z, std::get<Get_Z>(newAngleData));
}

std::optional<const XMFLOAT3>  CharacterObject::UpdateNextPosition(const float& elapsedTime)
{
	if ((m_currentDirection | IDLE_BIT) == IDLE_BIT) return std::nullopt;

	XMFLOAT3 commonNextPosition = GetCommonNextPosition(elapsedTime);

	//collide함수는 slidingvector가 적용된 위치라면 true를 리턴
	auto collideWallResult = CollideWall(commonNextPosition, elapsedTime, false);
	if (collideWallResult.has_value()) {
		auto collideLiveObjectResult = CollideLiveObject(collideWallResult.value().second, elapsedTime, collideWallResult.value().first);
		if (collideLiveObjectResult.has_value()) {
			return collideLiveObjectResult.value().second;
		}
		else return std::nullopt;
	}
	return std::nullopt;
}

std::optional<std::pair<bool, XMFLOAT3>> CharacterObject::CollideWall(const XMFLOAT3& nextPosition, const float& elapsedTime, const bool& isSlidingPosition)
{
	//characters와 같은 방법으로 수행
	BoundingSphere boudingSphere{ nextPosition,  m_collisionSphere.Radius };
	boudingSphere.Center.y = 0;
	auto mapData = m_roomRef->GetMapData();
	auto& mapCollision = mapData->GetCollisionData();
	std::shared_ptr<MapCollision> collideMap{ nullptr };
	for (auto& collision : mapCollision) {
		if (collision->CollideMap(boudingSphere)) {
			if (nullptr != collideMap) return std::nullopt;
			collideMap = collision;
		}
	}
	if (nullptr == collideMap) return std::pair<bool, XMFLOAT3>(false, nextPosition);
	auto slidingVectorResult = collideMap->GetSlidingVector(shared_from_this(), m_moveVector);//power, vector
	XMFLOAT3 applySlidingPosition = GetPosition();
	applySlidingPosition = Vector3::Add(applySlidingPosition, slidingVectorResult.second, m_moveSpeed * elapsedTime * slidingVectorResult.first);
	return std::pair<bool, XMFLOAT3>(true, applySlidingPosition);
}

MeleeCharacterObject::MeleeCharacterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, const float& attackDamage, std::shared_ptr<Room>& roomRef, const ROLE& role)
	:CharacterObject(maxHp, moveSpeed, boundingSize, attackDamage, roomRef, role)
{
}

const XMFLOAT3 MeleeCharacterObject::GetCommonNextPosition(const float& elapsedTime)
{
	XMFLOAT3 currentPosition = GetPosition();
	return Vector3::Add(currentPosition, m_moveVector, m_moveSpeed * elapsedTime);
}

const XMFLOAT3 MeleeCharacterObject::GetMoveVector() const
{
	//좌, 우 // 앞, 뒤를 동시에 눌렀을 때, 적용안되게.
	char moveDirection = m_currentDirection;

	if ((moveDirection & (LEFT_BIT | RIGHT_BIT)) == (LEFT_BIT | RIGHT_BIT))
	{
		moveDirection = moveDirection ^ LEFT_BIT;
		moveDirection = moveDirection ^ RIGHT_BIT;
	}
	if ((moveDirection & (FRONT_BIT | BACK_BIT)) == (FRONT_BIT | BACK_BIT))
	{
		moveDirection = moveDirection ^ FRONT_BIT;
		moveDirection = moveDirection ^ BACK_BIT;
	}

	if ((moveDirection | IDLE_BIT) == IDLE_BIT)
		return XMFLOAT3{};

	float rotateAngle = 0.0f;
	switch (moveDirection)
	{

	case FRONT_BIT:						rotateAngle = 0.0f;		break;
	case FRONT_BIT | LEFT_BIT:			rotateAngle = 45.0f;	break;
	case LEFT_BIT:						rotateAngle = 90.0f;	break;
	case BACK_BIT | LEFT_BIT:			rotateAngle = 135.0f;	break;
	case BACK_BIT:						rotateAngle = 180.0f;	break;
	case BACK_BIT | RIGHT_BIT:			rotateAngle = 225.0f;	break;
	case RIGHT_BIT:						rotateAngle = 270.0f;	break;
	case FRONT_BIT | RIGHT_BIT:			rotateAngle = 315.0f;	break;
	default:
		return XMFLOAT3{};
	}

	XMFLOAT3 lookVector = GetLookVector();
	XMFLOAT3 moveVector{ 0,0,0 };
	moveVector.x = lookVector.x * cos(rotateAngle) - lookVector.z * sin(rotateAngle);
	moveVector.z = lookVector.x * sin(rotateAngle) + lookVector.z * cos(rotateAngle);
	return Vector3::Normalize(moveVector);
}

void MeleeCharacterObject::UpdateRotate()
{
	CharacterObject::UpdateRotate();
	UpdateDirectionRotate();
}

void MeleeCharacterObject::UpdateDirectionRotate()
{
	//m_applyDirection은 recvDirection에서 변화할 수 있으니, 스택 변수로 복사하여 운용
	UpdateDirection();
	//정지 상태로 변화 시 할 필요가 없읍.
	if ((m_currentDirection | IDLE_BIT) == IDLE_BIT)
		return;
	m_moveVector = GetMoveVector();
}

RangedCharacterObject::RangedCharacterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, const float& attackDamage, std::shared_ptr<Room>& roomRef, const ROLE& role)
	:CharacterObject(maxHp, moveSpeed, boundingSize, attackDamage, roomRef, role)
{
}

void RangedCharacterObject::Update()
{
	UpdateDirection();
	CharacterObject::Update();
}

const XMFLOAT3 RangedCharacterObject::GetCommonNextPosition(const float& elapsedTime)
{
	//m_applyDirection은 recvDirection에서 변화할 수 있으니, 스택 변수로 복사하여 운용
	m_currentDirection = m_applyDIrection;
	//정지 상태로 변화 시 할 필요가 없읍.
	if ((m_currentDirection | IDLE_BIT) == IDLE_BIT)
		return GetPosition();
	XMFLOAT3 currentPosition = GetPosition();
	m_moveVector = GetMoveVector();
	XMFLOAT3 returnPosition = Vector3::Add(currentPosition, m_moveVector, m_moveSpeed * elapsedTime);
	//spdlog::critical("position: returnPosition - x: {}, y: {}, z: {}, moveVector - x: {}, y: {}, z: {}", returnPosition.x, returnPosition.y, returnPosition.z
	//		, m_moveVector.x, m_moveVector.y, m_moveVector.z);
	return returnPosition;
}

const XMFLOAT3 RangedCharacterObject::GetMoveVector() const
{
	//좌, 우 // 앞, 뒤를 동시에 눌렀을 때, 적용안되게.
	char moveDirection = m_currentDirection;

	if ((moveDirection & (LEFT_BIT | RIGHT_BIT)) == (LEFT_BIT | RIGHT_BIT))
	{
		moveDirection = moveDirection ^ LEFT_BIT;
		moveDirection = moveDirection ^ RIGHT_BIT;
	}
	if ((moveDirection & (FRONT_BIT | BACK_BIT)) == (FRONT_BIT | BACK_BIT))
	{
		moveDirection = moveDirection ^ FRONT_BIT;
		moveDirection = moveDirection ^ BACK_BIT;
	}

	if ((moveDirection | IDLE_BIT) == IDLE_BIT)
		return XMFLOAT3{};

	XMFLOAT3 moveVector;
	switch (moveDirection)
	{
	case FRONT_BIT:
		moveVector = GetMoveFowardVector(1);
		break;
	case BACK_BIT:
		moveVector = GetMoveFowardVector(-1);
		break;
	case RIGHT_BIT:
		moveVector = GetMoveRightVector(1);
		break;
	case LEFT_BIT:
		moveVector = GetMoveRightVector(-1);
		break;
	default:
		moveVector = GetMoveDiagonalVector(moveDirection);
		break;
	}
	return moveVector;
}

XMFLOAT3 RangedCharacterObject::GetMoveFowardVector(const char& type) const
{
	XMFLOAT3 fowardVector = GetLookVector();
	fowardVector.y = 0;
	if (type == -1)
		fowardVector = Vector3::ScalarProduct(fowardVector, -1.0, true);
	return Vector3::Normalize(fowardVector);
}

XMFLOAT3 RangedCharacterObject::GetMoveRightVector(const char& type) const
{
	XMFLOAT3 rightVector = GetRightVector();
	rightVector.y = 0;
	if (type == -1)
		rightVector = Vector3::ScalarProduct(rightVector, -1.0, true);
	return Vector3::Normalize(rightVector);
}

XMFLOAT3 RangedCharacterObject::GetMoveDiagonalVector(const char& type) const
{
	XMFLOAT3 fowardVector = GetLookVector();
	XMFLOAT3 rightVector = GetRightVector();
	fowardVector.y = 0;
	rightVector.y = 0;
	if (type & BACK_BIT)
		fowardVector = Vector3::ScalarProduct(fowardVector, -1.0, true);
	if (type & LEFT_BIT)
		rightVector = Vector3::ScalarProduct(rightVector, -1.0, true);
	return Vector3::Normalize(Vector3::Add(fowardVector, rightVector));
}

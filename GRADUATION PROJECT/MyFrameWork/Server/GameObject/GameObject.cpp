#include "stdafx.h"
#include "GameObject.h"
#include "../Room/Room.h"

GameObject::GameObject(const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:m_boundingSize(boundingSize), m_roomRef(roomRef),
	m_worldMatrix(Matrix4x4::Identity())
{
}

void GameObject::Rotate(const ROTATE_AXIS& axis, const float& angle)
{
	if (FLT_EPSILON >= abs(angle)) return;
	static constexpr DirectX::XMFLOAT3 axisX{ 1, 0, 0 };
	static constexpr DirectX::XMFLOAT3 axisY{ 0, 1, 0 };
	static constexpr DirectX::XMFLOAT3 axisZ{ 0, 0, 1 };
	switch (axis)
	{
	case ROTATE_AXIS::X:
	{
		XMMATRIX rotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&axisX), XMConvertToRadians(angle));
		m_worldMatrix = Matrix4x4::Multiply(rotateMatrix, m_worldMatrix);
	}
	break;
	case ROTATE_AXIS::Y:
	{
		XMMATRIX rotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&axisY), XMConvertToRadians(angle));
		m_worldMatrix = Matrix4x4::Multiply(rotateMatrix, m_worldMatrix);

	}
	break;
	case ROTATE_AXIS::Z:
	{
		XMMATRIX rotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&axisZ), XMConvertToRadians(angle));
		m_worldMatrix = Matrix4x4::Multiply(rotateMatrix, m_worldMatrix);
	}
	break;
	default:
		break;
	}
}

bool GameObject::IsCollide(const BoundingSphere& otherCollision)
{
	return m_collisionSphere.Intersects(otherCollision);
}

bool GameObject::IsCollide(const BoundingOrientedBox& otherCollision)
{
	return m_collisionSphere.Intersects(otherCollision);
}

void GameObject::SetPosition(const DirectX::XMFLOAT3& newPosition)
{
	m_worldMatrix._41 = newPosition.x;
	m_worldMatrix._42 = newPosition.y;
	m_worldMatrix._43 = newPosition.z;
	UpdateCollision();
}

const DirectX::XMFLOAT3 GameObject::GetPosition() const
{
	return DirectX::XMFLOAT3(m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43);
}

const DirectX::XMFLOAT3 GameObject::GetLookVector() const
{
	return DirectX::XMFLOAT3(m_worldMatrix._31, m_worldMatrix._32, m_worldMatrix._33);
}

const DirectX::XMFLOAT3 GameObject::GetRightVector() const
{
	return DirectX::XMFLOAT3(m_worldMatrix._11, m_worldMatrix._12, m_worldMatrix._13);
}

const BoundingSphere& GameObject::GetCollision() const
{
	return m_collisionSphere;
}

const XMFLOAT3 GameObject::GetFromVector(const XMFLOAT3& from) const
{
	return { GetPosition().x - from.x, 0.0f , GetPosition().z - from.z };
}

const XMFLOAT3 GameObject::GetToVector(const XMFLOAT3& to) const
{
	return { to.x - GetPosition().x, 0.0f , to.z - GetPosition().z };
}

const float GameObject::GetDistance(const XMFLOAT3& other) const
{
	XMFLOAT3 distioanceVector = GetFromVector(other);
	return Vector3::Length(distioanceVector);
}

const float GameObject::GetDistance(std::shared_ptr<GameObject>& other) const
{
	XMFLOAT3 distanceVector = GetFromVector(other->GetPosition());
	return Vector3::Length(distanceVector);
}

void GameObject::UpdateCollision()
{
	m_collisionSphere.Center = GetPosition();
	m_collisionSphere.Center.y = 0;
}

void GameObject::SetLook(const XMFLOAT3& lookVector)
{
	static XMFLOAT3 upVector = XMFLOAT3(0, 1, 0);
	XMFLOAT3 newLookVector = lookVector;
	XMFLOAT3 newRightVector = Vector3::CrossProduct(upVector, newLookVector, false);
	m_worldMatrix._11 = newRightVector.x; m_worldMatrix._12 = newRightVector.y; m_worldMatrix._13 = newRightVector.z;
	m_worldMatrix._21 = newLookVector.x; m_worldMatrix._22 = newLookVector.y; m_worldMatrix._23 = newLookVector.z;
	m_worldMatrix._31 = upVector.x; m_worldMatrix._32 = upVector.y; m_worldMatrix._33 = upVector.z;
}

LiveObject::LiveObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:m_maxHp(maxHp), m_hp(maxHp), m_moveSpeed(moveSpeed), GameObject(moveSpeed, boundingSize, roomRef), m_moveVector(0, 0, 1), m_isAlive(true)
{
}

void LiveObject::Attacked(const float& damage)
{
	m_hp -= damage;
	if (m_hp <= 0) {
		m_hp = 0;
		m_isAlive = false;
	}
}

const float LiveObject::GetHp() const
{
	return m_hp;
}

const float LiveObject::GetMaxHp() const
{
	return m_maxHp;
}

std::optional<std::pair<bool, XMFLOAT3>> LiveObject::CollideLiveObject(const XMFLOAT3& nextPosition, const float& elapsedTime, const bool& isSlidingPosition)
{
	std::shared_ptr<LiveObject> collideCharacter;
	BoundingSphere collideSphere{ nextPosition, m_boundingSize };
	std::vector<std::shared_ptr<LiveObject>>& liveObjects = m_roomRef->GetLiveObjects();
	for (auto& liveObject : liveObjects) {
		//자기 자신이라면 넘어감
		if (liveObject == shared_from_this()) continue;
		if (liveObject->GetCollision().Radius + m_boundingSize - liveObject->GetDistance(nextPosition) < FLT_EPSILON) continue;
		if (true == isSlidingPosition || nullptr != collideCharacter) {
			//이미 충돌해서 또 충돌한거면 이동x
			//또는 상위에서 이미 슬라이딩 벡터 구해서 충돌이 하나라도 있으면 이동x
			return std::nullopt;
		}
		collideCharacter = liveObject;

		//if (liveObject->IsCollide(collideSphere)) {
		//}
	}
	if (nullptr == collideCharacter) return std::make_pair(false, nextPosition);

	//calculate Sliding vector
	XMFLOAT3 toThisVector = collideCharacter->GetToVector(GetPosition());
	XMFLOAT3 normalVector = Vector3::Normalize(toThisVector);
	XMFLOAT3 slidingVector = XMFLOAT3(-normalVector.z, 0.0f, normalVector.x);

	float slidingDotLookResult = Vector3::DotProduct(slidingVector, m_moveVector);//현재 룩 방향에 맞는 슬라이딩 벡터 확인하기 위한 내적
	if (slidingDotLookResult < 0)slidingVector = Vector3::ScalarProduct(slidingVector, -1.0f, false);//0보다 작다는건, 반대 방향을 의미

	XMFLOAT3 applySlidingVectorPosition = GetPosition();
	applySlidingVectorPosition = Vector3::Add(applySlidingVectorPosition, slidingVector, m_moveSpeed * elapsedTime);
	//spdlog::debug("Collide Living Object");
	return std::make_pair(true, applySlidingVectorPosition);
}

const std::chrono::high_resolution_clock::time_point LiveObject::GetLastUpdateTime() const
{
	return m_lastUpdateTime;
}


float LiveObject::GetElapsedLastUpdateTime()
{
	using namespace std::chrono;
	auto nowTime = high_resolution_clock::now();
	auto durationTime = duration_cast<microseconds>(nowTime - m_lastUpdateTime).count();
	m_lastUpdateTime = nowTime;
	return (float)durationTime / 1000'000.0f;
}

void LiveObject::UpdateLastUpdateTime()
{
	m_lastUpdateTime = std::chrono::high_resolution_clock::now();
}
#pragma once
#include "../PCH/stdafx.h"

class Room;
class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	GameObject() = delete;
	GameObject(const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~GameObject() = default;
	virtual void Update() = 0;

	void Rotate(const ROTATE_AXIS& axis, const float& angle);

	bool IsCollide(const BoundingSphere& otherCollision);
	bool IsCollide(const BoundingOrientedBox& otherCollision);

	void SetPosition(const DirectX::XMFLOAT3& newPosition);
	const DirectX::XMFLOAT3 GetPosition() const;

	const DirectX::XMFLOAT3 GetLookVector() const;
	const DirectX::XMFLOAT3 GetRightVector() const;

	const BoundingSphere& GetCollision() const;
	const XMFLOAT3 GetFromVector(const XMFLOAT3& from) const;
	const XMFLOAT3 GetToVector(const XMFLOAT3& to) const;
	const float GetDistance(const XMFLOAT3& other) const;
	const float GetDistance(std::shared_ptr<GameObject>& other) const;
protected:
	void UpdateCollision();
private:
	void SetLook(const XMFLOAT3& lookVector);
protected:
	//11 12 13 r
	//21 22 23 u
	//31 32 33 f
	//41 42 43 pos
	DirectX::XMFLOAT4X4 m_worldMatrix;

	float  m_boundingSize;
	BoundingSphere m_collisionSphere;

	//Room의 멤버 변수여서 먼저 소멸자 호출되니, 문제 없을거로 생각 됨.
	std::shared_ptr<Room> m_roomRef;
public:
	bool debug = false;
};

class LiveObject : public GameObject
{
public:
	LiveObject() = delete;
	LiveObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~LiveObject() = default;

	virtual void Attacked(const float& damage);

	const float GetHp() const;
	const float GetMaxHp() const;
	const bool IsAlive() const
	{
		return m_isAlive;
	}

	const std::chrono::high_resolution_clock::time_point GetLastUpdateTime() const;
protected:
	std::optional<std::pair<bool, XMFLOAT3>> CollideLiveObject(const XMFLOAT3& nextPosition, const float& elapsedTime, const bool& isSlidingPosition);

	virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) = 0;
	float GetElapsedLastUpdateTime();
	void UpdateLastUpdateTime();

protected:
	std::chrono::high_resolution_clock::time_point m_lastUpdateTime;

	float m_moveSpeed;
	XMFLOAT3 m_moveVector;

	std::atomic_bool m_isAlive;
	std::atomic<float>	m_hp;
	float	m_maxHp;
};

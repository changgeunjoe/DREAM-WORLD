#include "../PCH/stdafx.h"
#include "../GameObject.h"

/*
���� ������ ��Ÿ y + 8
���� Q
			objPosition.y = 6.0f + (i % 2) * 4.0f;
			objPosition = Vector3::Add(objPosition, objectRight, (1 - i) * 4.0f);
			objPosition = Vector3::Add(objPosition, derection, 1.0f);

����ü 4
���׿� 10
*/

class ProjectileObject : public MoveObject
{
protected:
	using SEC = std::chrono::seconds;
	using TimePoint = std::chrono::high_resolution_clock::time_point;
public:
	ProjectileObject(const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector, const float& attackDamage, const SEC& lifeTime);
	virtual void Update() override;
	const bool IsDestroy() const;
	virtual void UpdateCollision();
protected:
	virtual bool EnermyObjectsAttackCheck() = 0;
	void Attack(std::shared_ptr<LiveObject> damagedObject);

private:
	bool m_isCollide;
	float m_attackDamage;
	TimePoint m_destroyTime;
};

class PlayerProjectileObject :public ProjectileObject
{
public:
	PlayerProjectileObject(const float& moveSpeed, std::shared_ptr<Room>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector, const float& attackDamage, const SEC& lifeTime);
protected:
	virtual bool EnermyObjectsAttackCheck() override;//���͵�, ������ �浹 üũ
};

class MonsterProjectileObject : public ProjectileObject
{
public:
	MonsterProjectileObject(const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector, const float& attackDamage, const SEC& lifeTime);
protected:
	virtual bool EnermyObjectsAttackCheck() override;//Player�� �浹 üũ
};

class CommonArrowObject : public PlayerProjectileObject
{
public:
	static constexpr float POWER_ARROW_LEVEL1_SPEED = 100.0f;
	static constexpr float POWER_ARROW_LEVEL2_SPEED = 140.0f;
	static constexpr float POWER_ARROW_LEVEL3_SPEED = 200.0f;

	static constexpr float POWER_ARROW_LEVEL1_DAMAGE = 60.0f;
	static constexpr float POWER_ARROW_LEVEL2_DAMAGE = 85.0f;
	static constexpr float POWER_ARROW_LEVEL3_DAMAGE = 110.0f;
private:
	static constexpr SEC ARROW_LIFE_TIME = SEC(10);
	//�����ڿ� int power
public:
	CommonArrowObject(const float& moveSpeed, std::shared_ptr<Room>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector, const float& attackDamage);
};

class TripleArrowObject : public PlayerProjectileObject
{
private:
	static constexpr float TRIPLE_ARROW_SPEED = 250.0f;
	static constexpr float TRIPLE_ARROW_DAMAGE = 95.0f;//ȭ�� �ϳ��� ������ -> 3�� �� ������ 3 * 95������.
	static constexpr SEC TRIPE_ARROW_LIFE_TIME = SEC(10);
public:
	TripleArrowObject(std::shared_ptr<Room>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector);
};

class IceLanceBall : public PlayerProjectileObject
{
private:
	static constexpr float IceLance_DAMAGE = 65.0f;
	static constexpr float IceLance_SPEED = 100.0f;
	static constexpr SEC ICE_LANCE_LIFE_TIME = SEC(7);
public:
	IceLanceBall(std::shared_ptr<Room>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector);
};

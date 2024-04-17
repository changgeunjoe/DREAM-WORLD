#pragma once
#pragma once
#include "../../PCH/stdafx.h"
#include "../GameObject.h"
#include "../EventController/EventController.h"

//User 플레이 캐릭터
class CharacterObject : public LiveObject
{
public:
	enum SKILL_TYPE
	{
		SKILL_TYPE_Q,
		SKILL_TYPE_E
	};
protected:
	static constexpr std::string_view SKILL_Q = "SKILL_Q";
	static constexpr std::string_view SKILL_E = "SKILL_E";
private:
	static constexpr float REDUCE_DAMAGE_APPLY_RATIO = 85.0f / 100.0f;
	struct UodateAngle {
		std::atomic<float> applyAngle;
		float currentAngle;
		UodateAngle() : applyAngle(0), currentAngle(0) {}
		float Update()
		{
			//현재 적용할 각도에 수신 각도를 저장
			float newAngle = applyAngle;
			//현재 적용되어있는 각도와, 적용할 각도의 차이 계산
			float diffAngle = newAngle - currentAngle;
			//회전했으니, 현재 각도 최신화
			currentAngle = newAngle;
			if (currentAngle > 360.0f) currentAngle -= 360.0f;
			if (currentAngle < 0.0f) currentAngle += 360.0f;
			return diffAngle;
		}
	};

	struct UodateAngle3 {
		UodateAngle x;
		UodateAngle y;
		UodateAngle z;
		UodateAngle3() : x(UodateAngle()), y(UodateAngle()), z(UodateAngle()) {}
		std::tuple<float, float, float> Update()
		{
			return { x.Update(), y.Update(), z.Update() };
		}
	};
protected:
	constexpr static char FRONT_BIT = static_cast<char>(DIRECTION::FRONT);
	constexpr static char BACK_BIT = static_cast<char>(DIRECTION::BACK);
	constexpr static char LEFT_BIT = static_cast<char>(DIRECTION::LEFT);
	constexpr static char RIGHT_BIT = static_cast<char>(DIRECTION::RIGHT);
	constexpr static char IDLE_BIT = static_cast<char>(DIRECTION::IDLE);
public:
	CharacterObject() = delete;
	CharacterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, const float& attackDamage, std::shared_ptr<Room>& roomRef, const ROLE& role);
	~CharacterObject() = default;

	virtual void Update() override;
	void StopMove();

	void RecvDirection(const DIRECTION& dir, const bool& apply);
	void RecvRotate(const ROTATE_AXIS& axis, const float& angle);
	void RecvMouseInput(const bool& LmouseInput, const bool& RmouseInput);

	virtual void RecvSkillInput(const SKILL_TYPE&);
	virtual void RecvSkill(const SKILL_TYPE&) = 0;
	virtual void RecvSkill(const SKILL_TYPE&, const XMFLOAT3& vector3) = 0;
	virtual void RecvAttackCommon(const XMFLOAT3& attackDir, const int& power = 0) = 0;

	void SetShield(const bool& active);
	const float GetShield() const;

	void Heal(const float& heal)
	{
		if (!m_isAlive) return;
		m_hp += heal;
	}

	virtual void Attacked(const float& damage) override;
	const float GetAttackDamage() const
	{
		return m_commonAttackDamage;
	}

	virtual void SetStagePosition(const ROOM_STATE& roomState) = 0;
	const ROLE& GetRole() const
	{
		return m_role;
	}

	void ForceStopMove();
	void ResetSkillCoolTime();
protected:
	void UpdateDirection();
	virtual void UpdateRotate();
	std::optional<const XMFLOAT3> UpdateNextPosition(const float& elapsedTime);
	virtual const XMFLOAT3 GetMoveVector() const = 0;
	//Collision By Wall, Boss, Monster
	//움직일 수 없다면 nullopt
	//움직일 수 있다면, 현재 위치 또는, 슬라이딩 벡터 적용한 위치
	//근데, 어쨌든 움직일 수있다 없다면
	//움직이면 슬라이딩 벡터 적용하든, nextPosition리턴하면 되지 않나?
	std::optional<std::pair<bool, XMFLOAT3>> CollideWall(const XMFLOAT3& nextPosition, const float& elapsedTime, const bool& isSlidingPosition);
protected:
	bool m_leftMouseInput;
	bool m_rightMouseInput;

	//적용할 방향
	std::atomic<char> m_applyDIrection;
	//현재 방향
	char m_currentDirection;

	UodateAngle3 m_angleData;

	//쉴드 적용 정보
	std::atomic_bool	m_activeShield;
	float				m_Shield = 0.0f;

	float m_commonAttackDamage = 10.0f;

	ROLE m_role;
	//Skill Contoller
	std::unique_ptr<EventController> m_skillCtrl;
};

class MeleeCharacterObject : public CharacterObject
{
public:
	MeleeCharacterObject(const float& maxHp, const float& moveSpeed, const float& attackDamage, const float& boundingSize, std::shared_ptr<Room>& roomRef, const ROLE& role);
protected:
	virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) override;
	virtual const XMFLOAT3 GetMoveVector() const override;

	void UpdateDirectionRotate();
	virtual void UpdateRotate() override;

	//원거리 캐릭터는 무조건 앞을 보고 전후좌우 움직이지만
	//근거리는 전후좌후로 바라보면서 움직임. => 공격이 옆으로 나갈 수 있어야 함.
};

class RangedCharacterObject : public CharacterObject
{
public:
	RangedCharacterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, const float& attackDamage, std::shared_ptr<Room>& roomRef, const ROLE& role);

	virtual void Update() override;
protected:
	virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) override;
	virtual const XMFLOAT3 GetMoveVector() const override;

private:
	XMFLOAT3 GetMoveFowardVector(const char& type) const;//1 - f // -1 - b
	XMFLOAT3 GetMoveRightVector(const char& type) const; //1 - r // -1 - l
	XMFLOAT3 GetMoveDiagonalVector(const char& type) const;
};

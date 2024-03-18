#pragma once
#pragma once
#include "../../PCH/stdafx.h"
#include "../GameObject.h"
#include "SkillController.h"

//User �÷��� ĳ����
class CharacterObject : public LiveObject
{
private:
	struct UodateAngle {
		std::atomic<float> applyAngle;
		float currentAngle;
		UodateAngle() : applyAngle(0), currentAngle(0) {}
		float Update()
		{
			//���� ������ ������ ���� ������ ����
			float newAngle = applyAngle;
			//���� ����Ǿ��ִ� ������, ������ ������ ���� ���
			float diffAngle = newAngle - currentAngle;
			//ȸ��������, ���� ���� �ֽ�ȭ
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
	CharacterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime);
	~CharacterObject() = default;

	virtual void Update() override;
	void StopMove();

	void RecvDirection(const DIRECTION& dir, const bool& apply);
	void RecvRotate(const ROTATE_AXIS& axis, const float& angle);
	void RecvMouseInput(const bool& LmouseInput, const bool& RmouseInput);

	virtual void RecvSkill_1(const XMFLOAT3& vec3) = 0;
	virtual void RecvSkill_2(const XMFLOAT3& vec3) = 0;
	virtual void RecvAttackCommand(const XMFLOAT3& attackDir, const int& power) = 0;

	void SetShield(const bool& active);
	const float GetShield() const;

	virtual void SetStagePosition(const ROOM_STATE& roomState) = 0;

protected:
	void UpdateDirection();
	virtual void UpdateRotate();
	std::optional<const XMFLOAT3> UpdateNextPosition(const float& elapsedTime);
	virtual const XMFLOAT3 GetMoveVector() const = 0;
	//Collision By Wall, Boss, Monster
	//������ �� ���ٸ� nullopt
	//������ �� �ִٸ�, ���� ��ġ �Ǵ�, �����̵� ���� ������ ��ġ
	//�ٵ�, ��·�� ������ ���ִ� ���ٸ�
	//�����̸� �����̵� ���� �����ϵ�, nextPosition�����ϸ� ���� �ʳ�?
	std::optional<std::pair<bool, XMFLOAT3>> CollideWall(const XMFLOAT3& nextPosition, const float& elapsedTime, const bool& isSlidingPosition);
protected:
	bool m_leftMouseInput;
	bool m_rightMouseInput;

	//������ ����
	std::atomic<char> m_applyDIrection;
	//���� ����
	char m_currentDirection;

	UodateAngle3 m_angleData;

	//���� ���� ����
	std::atomic_bool	m_activeShield;
	float				m_Shield = 0.0f;
	float				m_damageRedutionRate = 0.0f;

	//Skill Contoller
	std::unique_ptr<SkillController> m_skillCtrl;
};

class MeleeCharacterObject : public CharacterObject
{
public:
	MeleeCharacterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime);
protected:
	virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) override;
	virtual const XMFLOAT3 GetMoveVector() const override;

	void UpdateDirectionRotate();
	virtual void UpdateRotate() override;

	//���Ÿ� ĳ���ʹ� ������ ���� ���� �����¿� ����������
	//�ٰŸ��� �������ķ� �ٶ󺸸鼭 ������. => ������ ������ ���� �� �־�� ��.
};

class RangedCharacterObject : public CharacterObject
{
public:
	RangedCharacterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime);

	virtual void Update() override;
protected:
	virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) override;
	virtual const XMFLOAT3 GetMoveVector() const override;

private:
	XMFLOAT3 GetMoveFowardVector(const char& type) const;//1 - f // -1 - b
	XMFLOAT3 GetMoveRightVector(const char& type) const; //1 - r // -1 - l
	XMFLOAT3 GetMoveDiagonalVector(const char& type) const;
};

class WarriorObject : public MeleeCharacterObject
{
public:
	WarriorObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime);
	/*	:CharacterObject(ROLE::WARRIOR)
	{
		m_skillCoolTime = { std::chrono::seconds(7), std::chrono::seconds(0) };
		m_skillDuration = { std::chrono::seconds(0), std::chrono::seconds(0) };
		m_prevSkillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0],
			std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
		m_CommonAttackCoolTime = std::chrono::seconds(1);
		m_prevCommonAttackTime = std::chrono::high_resolution_clock::now() - m_CommonAttackCoolTime;
		SetStage_1Position();
	}*/
	~WarriorObject() = default;
public:
	virtual void SetStagePosition(const ROOM_STATE& roomState)override;

	virtual void RecvSkill_1(const XMFLOAT3& vec3) override;
	virtual void RecvSkill_2(const XMFLOAT3& vec3) override;
	virtual void RecvAttackCommand(const XMFLOAT3& attackDir, const int& power) override;
};

class TankerObject : public MeleeCharacterObject
{
public:
	TankerObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime);
	/*{
		m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(10) };
		m_skillDuration = { std::chrono::seconds(7), std::chrono::seconds(0) };
		m_prevSkillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0],
			std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
		m_CommonAttackCoolTime = std::chrono::seconds(1);
		m_prevCommonAttackTime = std::chrono::high_resolution_clock::now() - m_CommonAttackCoolTime;
		SetStage_1Position();
	}*/
	~TankerObject() = default;
public:
	virtual void SetStagePosition(const ROOM_STATE& roomState)override;

	virtual void RecvSkill_1(const XMFLOAT3& vec3) override;
	virtual void RecvSkill_2(const XMFLOAT3& vec3) override;
	virtual void RecvAttackCommand(const XMFLOAT3& attackDir, const int& power) override;
};

class MageObject : public RangedCharacterObject
{
public:
	MageObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime);
	/*{
		m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(10) };
		m_skillDuration = { std::chrono::seconds(10), std::chrono::seconds(0) };
		m_prevSkillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0],
			std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
		m_CommonAttackCoolTime = std::chrono::seconds(1);
		m_prevCommonAttackTime = std::chrono::high_resolution_clock::now() - m_CommonAttackCoolTime;
		SetStage_1Position();
	}*/
	~MageObject() = default;
public:
	virtual void SetStagePosition(const ROOM_STATE& roomState)override;

	virtual void RecvSkill_1(const XMFLOAT3& vec3) override;
	virtual void RecvSkill_2(const XMFLOAT3& vec3) override;
	virtual void RecvAttackCommand(const XMFLOAT3& attackDir, const int& power) override;
};

class ArcherObject : public RangedCharacterObject
{
public:
	ArcherObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef, const std::vector<std::chrono::seconds>& durationTime, const std::vector<std::chrono::seconds>& coolTime);
	/*{
		m_skillCoolTime = { std::chrono::seconds(10), std::chrono::seconds(15) };
		m_skillDuration = { std::chrono::seconds(0), std::chrono::seconds(0) };
		m_prevSkillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0],
			std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
		m_CommonAttackCoolTime = std::chrono::seconds(1);
		m_prevCommonAttackTime = std::chrono::high_resolution_clock::now() - m_CommonAttackCoolTime;
		SetStage_1Position();
	}*/
	~ArcherObject() = default;
public:
	virtual void SetStagePosition(const ROOM_STATE& roomState)override;

	virtual void RecvSkill_1(const XMFLOAT3& vec3) override;
	virtual void RecvSkill_2(const XMFLOAT3& vec3) override;
	virtual void RecvAttackCommand(const XMFLOAT3& attackDir, const int& power) override;
};

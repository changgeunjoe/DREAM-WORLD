#pragma once
#include "../../PCH/stdafx.h"
#include "MonsterObject.h"

class CharacterObject;
class BossMonsterObject;
/*
	���� �̵� ���� ���� ���Ͽ� ���ؼ� ���� ����
	IDLE, MOVE, ATTACK_SPIN, ATTACK_FIRE, ATTACK_METEO, ATTACK_KICK, ATTACK_PUNCH
*/

namespace BossState
{
	using MS = std::chrono::milliseconds;
	using SEC = std::chrono::seconds;
	using TIME_POINT = std::chrono::high_resolution_clock::time_point;

	enum class STATE
	{
		MOVE,
		MOVE_AGGRO,
		SPIN,
		FIRE,
		METEOR,
		KICK,
		PUNCH
	};

	class StateBase
	{
	public:
		StateBase(std::shared_ptr<BossMonsterObject> bossObject) : bossObject(bossObject), isEntered(false) {}

		void Execute();
		virtual void EnterState()
		{
			isEntered = true;
		}
		virtual void UpdateState() = 0;

		virtual void ExitState()
		{
			isEntered = false;
		}

	protected:
		std::shared_ptr<BossMonsterObject> bossObject;//BossObject�� ���� ��� ������ ������ ���� ������, ��ȯ ���� �߻� ���Ұŷ� ���� ��.
		bool isEntered;
	};

	class AttackState : public StateBase
	{
	public:
		AttackState(std::shared_ptr<BossMonsterObject> bossObject, const MS& endTime) : StateBase(bossObject), m_endTime(endTime) {}
		virtual void EnterState() override;
		virtual void ExitState() override;
	protected:
		TIME_POINT m_endTimePoint;
		MS m_endTime;
	};

	class MoveState : public StateBase
	{
	public:
		MoveState(std::shared_ptr<BossMonsterObject> bossObject) : StateBase(bossObject) {}
	protected:
		virtual void EnterState() override;
		virtual void UpdateState() override;
	};

	class MoveAggroState : public StateBase
	{
	public:
		MoveAggroState(std::shared_ptr<BossMonsterObject> bossObject) : StateBase(bossObject) {}
	protected:
		virtual void EnterState() override;
		virtual void UpdateState() override;
	};

	class AttackFire : public AttackState
	{
		//1s -> 1�� ���� ����
		//	50~70 ���� �ѹ�
		//	100 ����
		//2.5s -> 2�� ���� ����
		//	50 �̳��� �ѹ�
		//	120����

		//3s -> Idle��
		static constexpr SEC FIRST_ATTACK_TIME = SEC(1);
		static constexpr MS SECOND_ATTACK_TIME = std::chrono::duration_cast<MS>(SEC(2)) + MS(500);
		static constexpr MS END_TIME = std::chrono::duration_cast<MS>(SEC(3));

		static constexpr float INNER_RANGE = 50.0f;
		static constexpr float OUTER_RANGE = 70.0f;

		static constexpr float FIRST_DAMAGE = 100.0f;
		static constexpr float SECOND_DAMAGE = 120.0f;

	public:
		AttackFire(std::shared_ptr<BossMonsterObject> bossObject) : AttackState(bossObject, END_TIME), m_attackCnt(0) {}
	protected:
		virtual void EnterState() override;
		virtual void UpdateState() override;
	private:
		TIME_POINT m_firstAttackExecuteTime;
		TIME_POINT m_secondAttackExecuteTime;
		int m_attackCnt;
	};

	class AttackSpin : public AttackState
	{
		//300ms -> 1�� ����
		//600ms -> 2�� ����
		//900ms -> 3�� ����
		//1s -> Idle��
		//45�̸� ����
		//��� 45����

		static constexpr MS FIRST_ATTACK_TIME = MS(300);
		static constexpr MS SECOND_ATTACK_TIME = MS(600);
		static constexpr MS THIRD_ATTACK_TIME = MS(900);
		static constexpr MS END_TIME = std::chrono::duration_cast<MS>(SEC(1));

		static constexpr float DAMAGE = 55.0f;
		static constexpr float RANGE = 45.0f;

	public:
		AttackSpin(std::shared_ptr<BossMonsterObject> bossObject) : AttackState(bossObject, END_TIME), m_attackCnt(0) {}
	protected:
		virtual void EnterState() override;
		virtual void UpdateState() override;
	private:
		TIME_POINT m_firstAttackExecuteTime;
		TIME_POINT m_secondAttackExecuteTime;
		TIME_POINT m_thirdAttackExecuteTime;
		int m_attackCnt;
	};

	class AttackKick : public AttackState
	{
		//332ms -> ���� ����
		//823ms -> �ٽ� Idle��
		static constexpr MS ATTACK_TIME = MS(332);
		static constexpr MS END_TIME = MS(832);

		static constexpr float DAMAGE = 75.0f;
		static constexpr float RANGE = 60.0f;
		static constexpr float VALID_RADIAN = 20.0f * 3.14f / 180.0f;
	public:
		AttackKick(std::shared_ptr<BossMonsterObject> bossObject) : AttackState(bossObject, END_TIME), m_isAttacked(false) {}
	protected:
		virtual void EnterState() override;
		virtual void UpdateState() override;
	private:
		TIME_POINT m_attackExecuteTime;
		bool m_isAttacked;
	};

	class AttackPunch : public AttackState
	{
		//332ms -> ���� ����
		//823ms -> �ٽ� Idle

		static constexpr MS ATTACK_TIME = MS(332);
		static constexpr MS END_TIME = MS(832);

		static constexpr float DAMAGE = 55.0f;
		static constexpr float RANGE = 50.0f;
		static constexpr float VALID_RADIAN = 15.0f * 3.14f / 180.0f;
	public:
		AttackPunch(std::shared_ptr<BossMonsterObject> bossObject) : AttackState(bossObject, END_TIME), m_isAttacked(false) {}
	protected:
		virtual void EnterState() override;
		virtual void UpdateState() override;
	private:
		TIME_POINT m_attackExecuteTime;
		bool m_isAttacked;
	};

	class AttackMeteor : public AttackState
	{
		//1.25s -> Idle
		//������ ���׿� ������ => ����ü�� ���� => ����ü�� ������ �ִµ�, ���⼭ set�ؾ����� �����غ��� ��.
		static constexpr MS END_TIME = std::chrono::duration_cast<MS>(SEC(1)) + MS(250);

	public:
		AttackMeteor(std::shared_ptr<BossMonsterObject> bossObject) : AttackState(bossObject, END_TIME) {}
	protected:
		virtual void EnterState() override;
		virtual void UpdateState() override;
	};
}



class BossMonsterObject : public MonsterObject
{
private:
	static constexpr std::string_view RESEARCH_ROAD = "RESEARCH_ROAD";
	static constexpr std::string_view SEND_AGGRO_POSITION = "SEND_AGGRO_POSITION";

	static constexpr std::string_view ATTACK_SPIN = "ATTACK_SPIN";
	static constexpr std::string_view ATTACK_FIRE = "ATTACK_FIRE";
	static constexpr std::string_view ATTACK_METEOR = "ATTACK_METEOR";
	static constexpr std::string_view ATTACK_KICK = "ATTACK_KICK";
	static constexpr std::string_view ATTACK_PUNCH = "ATTACK_PUNCH";


	static constexpr EventController::MS RESEARCH_ROAD_COOL_TIME = EventController::MS(200);
	static constexpr EventController::MS SEND_AGGRO_POSITION_TIME = EventController::MS(200);
	static constexpr EventController::MS FIND_PLAYER_COOM_TIME = std::chrono::duration_cast<EventController::MS>(EventController::SEC(5));
	static constexpr EventController::MS BETWEEN_ATTACK_COOM_TIME = EventController::MS(500);


	static constexpr EventController::MS METEOR_COOL_TIME = std::chrono::duration_cast<EventController::MS>(EventController::SEC(30));
	static constexpr EventController::MS SPIN_ATTACK_COOL_TIME = std::chrono::duration_cast<EventController::MS>(EventController::SEC(4));
	static constexpr EventController::MS FIRE_ATTACK_COOL_TIME = std::chrono::duration_cast<EventController::MS>(EventController::SEC(7));
	static constexpr EventController::MS KICK_ATTACK_COOL_TIME = std::chrono::duration_cast<EventController::MS>(EventController::SEC(2) + EventController::MS(500));
	static constexpr EventController::MS PUNCH_ATTACK_COOL_TIME = std::chrono::duration_cast<EventController::MS>(EventController::SEC(1) + EventController::MS(500));

	//�ʿ� ȸ���� �� �ִ� �ִ� ���� - ���Ϸ�
	static constexpr float IDLE_ROTATE_ANGLE = 60.0f;

public:
	BossMonsterObject() = delete;
	BossMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);

	virtual void Update() override;

	void Initialize();

	//��׷ο� �� Ž���ϴ� Ÿ�̸ӿ� ���ؼ� ����(����, �̵��� �����ϰ� ��� Update���� Ȯ��)
	void CheckUpdateRoad();
	void UpdateAggro(std::shared_ptr<CharacterObject> aggroCharacter, std::shared_ptr<std::list<XMFLOAT3>> nodeList);
	void UpdateRoad(std::shared_ptr<std::list<XMFLOAT3>> nodeList);

	//state����� ��
	void SendBossState(const BossState::STATE& state);

	void AttackSpin(const float& damage, const float& attackRange);
	void AttackFire(const float& damage, const float& innerRange, const float& outerRange, const int& mode);
	void AttackKick(const float& damage, const float& attackRange, const float& validRadian);
	void AttackPunch(const float& damage, const float& attackRange, const float& validRadian);
	void AttackMeteor();

	void Move();
	void MoveAggro();

	void ChangeBossState(const BossState::STATE& state);

protected:
	virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) override;
private:
	virtual std::shared_ptr<CharacterObject> FindAggroCharacter() override;

	void MoveUpdate();
	void MoveAggroUpdate();

	const bool AbleSpinAttack();
	const bool AbleFireAttack();
	const bool AbleMeteorAttack();
	const bool AbleKickAttack();
	const bool AblePunchAttack();

	void AttackCheck();
private:

	std::shared_ptr<BossState::StateBase> m_currentState;
	std::shared_ptr<std::list<XMFLOAT3>> m_road;
	std::shared_ptr<CharacterObject> m_aggroCharacter;

	std::unordered_map<BossState::STATE, std::shared_ptr<BossState::StateBase>> m_bossStates;

	XMFLOAT3 m_currentAggroPosition;
};


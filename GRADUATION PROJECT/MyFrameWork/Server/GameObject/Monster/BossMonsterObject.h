#pragma once
#include "../../PCH/stdafx.h"
#include "MonsterObject.h"

class CharacterObject;
class BossMonsterObject;
/*
	보스 이동 정지 공격 패턴에 대해서 상태 패턴
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
		std::shared_ptr<BossMonsterObject> bossObject;//BossObject만 내부 멤버 변수로 참조를 갖기 때문에, 순환 참조 발생 안할거로 예상 됨.
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
		//1s -> 1차 공격 판정
		//	50~70 사이 한번
		//	100 뎀지
		//2.5s -> 2차 공격 판정
		//	50 이내에 한번
		//	120뎀지

		//3s -> Idle로
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
		//300ms -> 1차 공격
		//600ms -> 2차 공격
		//900ms -> 3차 공격
		//1s -> Idle로
		//45미만 공격
		//방당 45뎀지

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
		//332ms -> 공격 판정
		//823ms -> 다시 Idle로
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
		//332ms -> 공격 판정
		//823ms -> 다시 Idle

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
		//위에서 메테오 떨어짐 => 투사체로 판정 => 투사체에 데미지 있는데, 여기서 set해야할지 생각해봐야 됨.
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

	//초에 회전할 수 있는 최대 각도 - 오일러
	static constexpr float IDLE_ROTATE_ANGLE = 60.0f;

public:
	BossMonsterObject() = delete;
	BossMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);

	virtual void Update() override;

	void Initialize();

	//어그로와 길 탐색하는 타이머에 대해서 동작(공격, 이동과 무관하게 계속 Update에서 확인)
	void CheckUpdateRoad();
	void UpdateAggro(std::shared_ptr<CharacterObject> aggroCharacter, std::shared_ptr<std::list<XMFLOAT3>> nodeList);
	void UpdateRoad(std::shared_ptr<std::list<XMFLOAT3>> nodeList);

	//state변경될 때
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


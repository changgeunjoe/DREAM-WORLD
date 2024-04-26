#include "stdafx.h"
#include "BossMonsterObject.h"
#include "../Room/Room.h"
#include "../Character/ChracterObject.h"
#include "../Room/RoomEvent.h"
#include "../EventController/CoolDownEventBase.h"
#include "BossEvent.h"


BossMonsterObject::BossMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef)
	:MonsterObject(maxHp, moveSpeed, boundingSize, roomRef), m_currentState(nullptr)
{
	//200ms���� �� ã�� �����
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(RESEARCH_ROAD, RESEARCH_ROAD_COOL_TIME);
	//5�ʸ��� ��׷� ��� ����
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(FIND_PLAYER, FIND_PLAYER_COOM_TIME);
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(SEND_AGGRO_POSITION, SEND_AGGRO_POSITION_TIME);

	//��ü������ ���� ��ų�� ����ϱ����� ��Ÿ�� => ��ų ��Ÿ�Ӱ� ������ ����Ǵ� ��Ÿ��
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(ATTACK_PLAYER, BETWEEN_ATTACK_COOM_TIME);

	m_behaviorTimeEventCtrl->InsertCoolDownEventData(ATTACK_METEOR, METEOR_COOL_TIME);
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(ATTACK_SPIN, SPIN_ATTACK_COOL_TIME);
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(ATTACK_FIRE, FIRE_ATTACK_COOL_TIME);
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(ATTACK_KICK, KICK_ATTACK_COOL_TIME);
	m_behaviorTimeEventCtrl->InsertCoolDownEventData(ATTACK_PUNCH, PUNCH_ATTACK_COOL_TIME);

}

void BossMonsterObject::Update()
{
	CheckUpdateRoad();
	if (nullptr != m_currentState)
		m_currentState->Execute();
}

void BossMonsterObject::Initialize()
{
	//�����ڿ��� shared_from_this()�� ����
	//m_bossStates.try_emplace(BossState::STATE::IDLE, std::make_shared<BossState::IdleState>(std::static_pointer_cast<BossMonsterObject>(shared_from_this())));
	m_bossStates.try_emplace(BossState::STATE::MOVE, std::make_shared<BossState::MoveState>(std::static_pointer_cast<BossMonsterObject>(shared_from_this())));
	m_bossStates.try_emplace(BossState::STATE::MOVE_AGGRO, std::make_shared<BossState::MoveAggroState>(std::static_pointer_cast<BossMonsterObject>(shared_from_this())));
	m_bossStates.try_emplace(BossState::STATE::METEOR, std::make_shared<BossState::AttackMeteor>(std::static_pointer_cast<BossMonsterObject>(shared_from_this())));
	m_bossStates.try_emplace(BossState::STATE::SPIN, std::make_shared<BossState::AttackSpin>(std::static_pointer_cast<BossMonsterObject>(shared_from_this())));
	m_bossStates.try_emplace(BossState::STATE::FIRE, std::make_shared<BossState::AttackFire>(std::static_pointer_cast<BossMonsterObject>(shared_from_this())));
	m_bossStates.try_emplace(BossState::STATE::KICK, std::make_shared<BossState::AttackKick>(std::static_pointer_cast<BossMonsterObject>(shared_from_this())));
	m_bossStates.try_emplace(BossState::STATE::PUNCH, std::make_shared<BossState::AttackPunch>(std::static_pointer_cast<BossMonsterObject>(shared_from_this())));
	SetPosition(XMFLOAT3(0, 0, 0));
}

void BossMonsterObject::CheckUpdateRoad()
{
	auto findPlayerEvent = m_behaviorTimeEventCtrl->GetEventData(FIND_PLAYER);
	const bool isAbleFindPlayer = findPlayerEvent->IsAbleExecute();
	if (isAbleFindPlayer) {
		auto researchRoadEvent = m_behaviorTimeEventCtrl->GetEventData(RESEARCH_ROAD);
		researchRoadEvent->ForceExecute();
		auto characterRef = FindAggroCharacter();
		auto aggroEvent = std::make_shared<TIMER::BossAggroEvent>(m_roomRef, characterRef->GetPosition(), GetPosition(), characterRef);
		m_roomRef->InserTimerEvent(aggroEvent);
		return;
	}
	auto researchRoadEvent = m_behaviorTimeEventCtrl->GetEventData(RESEARCH_ROAD);
	const bool isAbleResearchRoad = researchRoadEvent->IsAbleExecute();
	if (isAbleResearchRoad) {
		auto researchEvent = std::make_shared<TIMER::BossCalculateRoadEvent>(m_roomRef, m_aggroCharacter->GetPosition(), GetPosition());
		m_roomRef->InserTimerEvent(researchEvent);
	}

}

void BossMonsterObject::UpdateAggro(std::shared_ptr<CharacterObject> aggroCharacter, std::shared_ptr<std::list<XMFLOAT3>> nodeList)
{
	m_aggroCharacter = aggroCharacter;
	UpdateRoad(nodeList);
	if (nullptr == m_currentState) {
		ChangeBossState(BossState::STATE::MOVE);
	}
}

void BossMonsterObject::UpdateRoad(std::shared_ptr<std::list<XMFLOAT3>> nodeList)
{
	m_road = nodeList;
	//m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<SendBossRoadSetEvent>(m_road)));
}

void BossMonsterObject::SendBossState(const BossState::STATE& state)
{
	switch (state)
	{
		/*case BossState::STATE::IDLE:
		{
			m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossStopEvent>()));
		}
		break;*/

	case BossState::STATE::MOVE_AGGRO:
	{
		spdlog::info("changeBossState: MOVE_AGGRO");
		UpdateLastUpdateTime();
		auto sendAggroPositionEvent = m_behaviorTimeEventCtrl->GetEventData(SEND_AGGRO_POSITION);
		//��׷θ� �ٷ� ���󰡾� �ϱ� ������, ������
		sendAggroPositionEvent->ForceExecute();
		m_currentAggroPosition = m_aggroCharacter->GetPosition();
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossSameNodeEvent>(m_currentAggroPosition)));
	}
	break;
	case BossState::STATE::MOVE:
	{
		spdlog::info("changeBossState: MOVE");
		UpdateLastUpdateTime();
		XMFLOAT3 destination = m_road->front();//�ƴ϶�� ���� ������ ��ġ��
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossMoveDestinationEvent>(destination)));
	}
	break;
	case BossState::STATE::METEOR:
	{
		spdlog::info("changeBossState: METEOR");
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossMeteorEvent>()));
	}
	break;
	case BossState::STATE::FIRE:
	{
		spdlog::info("changeBossState: FIRE");
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossFireEvent>()));
	}
	break;
	case BossState::STATE::SPIN:
	{
		spdlog::info("changeBossState: SPIN");
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossSpinEvent>()));
	}
	break;
	case BossState::STATE::KICK:
	{
		spdlog::info("changeBossState: KICK");
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossKickEvent>(GetLookVector())));
	}
	break;
	case BossState::STATE::PUNCH:
	{
		spdlog::info("changeBossState: PUNCH");
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossPunchEvent>(GetLookVector())));
	}
	break;
	default:
		break;
	}
}

void BossMonsterObject::AttackSpin(const float& damage, const float& attackRange)
{
	auto characters = m_roomRef->GetCharacters();
	for (auto& character : characters) {
		float  distance = character->GetDistance(shared_from_this());
		if (distance < attackRange) {
			character->Attacked(damage);
		}
	}
}

void BossMonsterObject::AttackFire(const float& damage, const float& inner, const float& outer, const int& mode)
{
	//�ܰ� - ���� ������ ����
	auto characters = m_roomRef->GetCharacters();
	if (mode == 0) {
		for (auto& character : characters) {
			float  distance = character->GetDistance(shared_from_this());
			if (distance < outer && distance > inner) {
				character->Attacked(damage);
			}
		}
	}
	else {
		for (auto& character : characters) {
			float  distance = character->GetDistance(shared_from_this());
			if (distance < inner) {
				character->Attacked(damage);
			}
		}
	}
}

void BossMonsterObject::AttackKick(const float& damage, const float& attackRange, const float& validRadian)
{
	auto characters = m_roomRef->GetCharacters();
	for (auto& character : characters) {
		float  distance = character->GetDistance(shared_from_this());
		if (distance < attackRange) continue;
		float betweenRadian = GetBetweenAngleRadian(character->GetPosition());
		if (betweenRadian > validRadian) {
			character->Attacked(damage);
		}
	}
}

void BossMonsterObject::AttackPunch(const float& damage, const float& attackRange, const float& validRadian)
{
	auto characters = m_roomRef->GetCharacters();
	for (auto& character : characters) {
		float  distance = character->GetDistance(shared_from_this());
		if (distance < attackRange) continue;
		float betweenRadian = GetBetweenAngleRadian(character->GetPosition());
		if (betweenRadian > validRadian) {
			character->Attacked(damage);
		}
	}
}

void BossMonsterObject::AttackMeteor()
{
	//m_roomRef->prev...
}

void BossMonsterObject::Move()
{
	AttackCheck();
	if (m_currentState != m_bossStates[BossState::STATE::MOVE]) return;//���� ���°� MOVE�� �ƴ϶��, �ٸ� ���·� ���� ��.
	if (m_road->empty()) {
		m_currentState->ExitState();
		ChangeBossState(BossState::STATE::MOVE_AGGRO);
		return;
	}
	MoveUpdate();
}

void BossMonsterObject::MoveAggro()
{
	static constexpr float STOP_DISTANCE = 40.0;
	AttackCheck();
	if (m_currentState != m_bossStates[BossState::STATE::MOVE_AGGRO]) return;//���� ���°� MOVE�� �ƴ϶��, �ٸ� ���·� ���� ��.

	if (!m_road->empty()) {//�ٽ� �� ã�Ʊ� ������, Move�� ���� ��ȭ
		XMFLOAT3 currentAggroPosition = m_currentAggroPosition;
		float distance = GetDistance(currentAggroPosition);
		if (distance > STOP_DISTANCE) {
			m_currentState->ExitState();
			ChangeBossState(BossState::STATE::MOVE);
			return;
		}
	}
	//��׷ΰ� �����̱� ������, ���� ��׷� ��ġ �ֽ�ȭ
	auto aggroPositionSendCoolTimeData = m_behaviorTimeEventCtrl->GetEventData(SEND_AGGRO_POSITION);
	const bool isAbleSendAggroPosition = aggroPositionSendCoolTimeData->IsAbleExecute();
	if (isAbleSendAggroPosition) {
		m_currentAggroPosition = m_aggroCharacter->GetPosition();
		m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(std::make_shared<BossSameNodeEvent>(m_aggroCharacter->GetPosition())));
	}
	MoveAggroUpdate();
}

void BossMonsterObject::ChangeBossState(const BossState::STATE& state)
{
	m_currentState = m_bossStates[state];
	m_currentState->Execute();//enterStateȣ�� �� �� �ְ�.
}

const XMFLOAT3 BossMonsterObject::GetCommonNextPosition(const float& elapsedTime)
{
	XMFLOAT3 lookVector = GetLookVector();
	XMFLOAT3 position = GetPosition();
	return Vector3::Add(position, lookVector, elapsedTime * m_moveSpeed);
}

std::shared_ptr<CharacterObject> BossMonsterObject::FindAggroCharacter()
{
	auto characters = m_roomRef->GetCharacters();
	auto ableRole = m_roomRef->GetLiveRoles();//Ŀ���� �Ǿ��ְ�, ����ִ� ĳ���͸�
	std::shared_ptr<CharacterObject> minDistanceObject = nullptr;
	float minDistance = 0.0f;
	for (const auto& role : ableRole) {
		auto character = m_roomRef->GetCharacterObject(role);
		float distance = character->GetDistance(shared_from_this());
		if (nullptr == minDistanceObject || minDistance > distance) {//���� ������ �ּ� �Ÿ� ��ü�� ���ų�, �ּҰŸ����� �� ª�� �Ÿ��� ��
			minDistanceObject = character;
			minDistance = distance;
		}
	}
	if (nullptr == minDistanceObject) {
		spdlog::warn("No Aggro Character");
	}
	return minDistanceObject;
}

void BossMonsterObject::MoveUpdate()
{
	static constexpr float CHANGE_NODE_DISTANCE = 3.0f;

	//�� �̻� �̵��� ��尡 ���� => ���� ��忡 ����.
	XMFLOAT3 currentDestinationPosition = m_road->front();
	XMFLOAT3 currentPosition = GetPosition();
	XMFLOAT3 toDestinationVector = Vector3::Subtract(currentDestinationPosition, currentPosition);

	//���� ������ ���������� �Ÿ�
	float distance = Vector3::Length(toDestinationVector);
	if (distance < CHANGE_NODE_DISTANCE) {//road�� ���� ���ƴٸ�, front�� ������ ��ġ�� ���� ���̱⶧����, �˾Ƽ� send ��
		m_road->pop_front();
		if (m_road->empty()) {//���� ��忡 ����
			m_currentState->ExitState();
			ChangeBossState(BossState::STATE::MOVE_AGGRO);
			return;
		}
	}
	currentDestinationPosition = m_road->front();
	auto changeDestinationEvent = std::make_shared<BossMoveDestinationEvent>(currentDestinationPosition);
	m_roomRef->InsertAftrerUpdateSendEvent(std::static_pointer_cast<RoomSendEvent>(changeDestinationEvent));
	toDestinationVector = Vector3::Subtract(currentDestinationPosition, currentPosition);
	toDestinationVector = Vector3::Normalize(toDestinationVector);
	SetLook(toDestinationVector);

	float elapsedTime = GetElapsedLastUpdateTime();
	XMFLOAT3 nextPosition = GetCommonNextPosition(elapsedTime);
	SetPosition(nextPosition);
}

void BossMonsterObject::MoveAggroUpdate()
{
	static constexpr float STOP_DISTANCE = 40.0;

	XMFLOAT3 currentAggroPosition = m_currentAggroPosition;
	auto betweenEulerAngle = GetBetweenAngleEuler(currentAggroPosition);
	const float elapsedTime = GetElapsedLastUpdateTime();

	float applyRotateAngle = 0.0f;
	if (betweenEulerAngle.second < IDLE_ROTATE_ANGLE * elapsedTime) {// ���� ���� , �����Ϸ��� �������� �۴� => �����Ҷ� �������� => ���̰� ��ŭ ȸ��
		applyRotateAngle = betweenEulerAngle.second;
	}
	else {
		applyRotateAngle = IDLE_ROTATE_ANGLE * elapsedTime;
	}
	if (applyRotateAngle > FLT_EPSILON)
		Rotate(ROTATE_AXIS::Y, betweenEulerAngle.first * applyRotateAngle);

	float enermyDistance = GetDistance(currentAggroPosition);
	if (enermyDistance < STOP_DISTANCE) {
		//Only Rotate
		return;
	}
	XMFLOAT3 nextPosition = GetCommonNextPosition(elapsedTime);
	SetPosition(nextPosition);
}

const bool BossMonsterObject::AbleSpinAttack()
{
	static constexpr float SPIN_ATTACK_RANGE = 45.0f;
	auto characters = m_roomRef->GetCharacters();

	//���� üũ
	int validCnt = 0;
	for (auto& character : characters) {
		float distance = character->GetDistance(shared_from_this());
		if (distance < SPIN_ATTACK_RANGE) {
			++validCnt;
		}
	}
	//2�� �̻��� �� ����
	if (validCnt < 2) return false;

	//��Ÿ�� üũ
	auto spinAttackCoolTimeData = m_behaviorTimeEventCtrl->GetEventData(ATTACK_SPIN);
	const bool able = spinAttackCoolTimeData->IsAbleExecute();
	return able;
}

const bool BossMonsterObject::AbleFireAttack()
{
	static constexpr float FIRE_ATTACK_RANGE = 69.0f;
	auto characters = m_roomRef->GetCharacters();

	//���� üũ
	int validCnt = 0;
	for (auto& character : characters) {
		float distance = character->GetDistance(shared_from_this());
		if (distance < FIRE_ATTACK_RANGE) {
			++validCnt;
		}
	}
	//2�� �̻��� �� ����
	if (validCnt < 2) return false;

	//��Ÿ�� üũ
	auto fireAttackCoolTimeData = m_behaviorTimeEventCtrl->GetEventData(ATTACK_FIRE);
	const bool able = fireAttackCoolTimeData->IsAbleExecute();
	return able;
}

const bool BossMonsterObject::AbleMeteorAttack()
{
	//���׿� ������ ��Ÿ�� �Ǹ� �����ϰ�
	auto meteorAttackCoolTimeData = m_behaviorTimeEventCtrl->GetEventData(ATTACK_METEOR);
	const bool able = meteorAttackCoolTimeData->IsAbleExecute();
	return able;
}

const bool BossMonsterObject::AbleKickAttack()
{
	static constexpr float KICK_RANGE = 60.0f;
	static constexpr float VALID_RADIAN = 20.0f * 3.14f / 180.0f;
	auto characters = m_roomRef->GetCharacters();
	//���� üũ
	int validCnt = 0;
	for (auto& character : characters) {
		float distance = character->GetDistance(shared_from_this());
		if (distance < KICK_RANGE) {
			float betweenRadian = GetBetweenAngleRadian(character->GetPosition());
			if (betweenRadian > VALID_RADIAN)
				++validCnt;
		}
	}
	if (validCnt == 0) return false;
	auto kickAttackCoolTimeData = m_behaviorTimeEventCtrl->GetEventData(ATTACK_KICK);
	const bool able = kickAttackCoolTimeData->IsAbleExecute();
	return able;
}

const bool BossMonsterObject::AblePunchAttack()
{
	static constexpr float PUNCH_RANGE = 50.0f;
	static constexpr float VALID_RADIAN = 15.0f * 3.14f / 180.0f;
	auto characters = m_roomRef->GetCharacters();
	//���� üũ
	int validCnt = 0;
	for (auto& character : characters) {
		float distance = character->GetDistance(shared_from_this());
		if (distance < PUNCH_RANGE) {
			float betweenRadian = GetBetweenAngleRadian(character->GetPosition());
			if (betweenRadian > VALID_RADIAN)
				++validCnt;
		}
	}
	if (validCnt == 0) return false;
	auto punchAttackCoolTimeData = m_behaviorTimeEventCtrl->GetEventData(ATTACK_PUNCH);
	const bool able = punchAttackCoolTimeData->IsAbleExecute();
	return able;
}

void BossMonsterObject::AttackCheck()
{
	/*	if (AbleMeteorAttack()) {
			m_currentState->ExitState();
			ChangeBossState(BossState::STATE::METEOR);
		}
		else */if (AbleSpinAttack()) {
			m_currentState->ExitState();
			ChangeBossState(BossState::STATE::SPIN);
		}
		else if (AbleFireAttack()) {
			m_currentState->ExitState();
			ChangeBossState(BossState::STATE::FIRE);
		}
		else if (AbleKickAttack()) {
			m_currentState->ExitState();
			ChangeBossState(BossState::STATE::KICK);
		}
		else if (AblePunchAttack()) {
			m_currentState->ExitState();
			ChangeBossState(BossState::STATE::PUNCH);
		}
}

#pragma region BOSS_STATE

void BossState::StateBase::Execute()
{
	if (!isEntered)
		EnterState();
	UpdateState();
}

void BossState::MoveState::EnterState()
{
	BossState::StateBase::EnterState();
	bossObject->SendBossState(BossState::STATE::MOVE);
}

void BossState::MoveState::UpdateState()
{
	bossObject->Move();
}

void BossState::MoveAggroState::EnterState()
{
	BossState::StateBase::EnterState();
	bossObject->SendBossState(BossState::STATE::MOVE_AGGRO);
}

void BossState::MoveAggroState::UpdateState()
{
	bossObject->MoveAggro();
}


#pragma region BOSS_ATTACK_STATE

void BossState::AttackState::EnterState()
{
	BossState::StateBase::EnterState();
	m_endTimePoint = std::chrono::high_resolution_clock::now() + m_endTime;
}

void BossState::AttackState::ExitState()
{
	BossState::StateBase::ExitState();
	bossObject->ChangeBossState(BossState::STATE::MOVE_AGGRO);
}

void BossState::AttackFire::EnterState()
{
	BossState::AttackState::EnterState();
	m_firstAttackExecuteTime = std::chrono::high_resolution_clock::now() + FIRST_ATTACK_TIME;
	m_secondAttackExecuteTime = std::chrono::high_resolution_clock::now() + SECOND_ATTACK_TIME;
	m_attackCnt = 0;
	bossObject->SendBossState(BossState::STATE::FIRE);
}

void BossState::AttackFire::UpdateState()
{
	const auto nowTime = std::chrono::high_resolution_clock::now();
	switch (m_attackCnt)
	{
	case 0:
	{
		if (m_firstAttackExecuteTime < nowTime) {//ù��° ���� ����
			bossObject->AttackFire(FIRST_DAMAGE, INNER_RANGE, OUTER_RANGE, m_attackCnt);
			m_attackCnt += 1;
		}
	}
	break;
	case 1:
	{
		if (m_secondAttackExecuteTime < nowTime) {//�ι�° ���� ����
			bossObject->AttackFire(SECOND_DAMAGE, INNER_RANGE, OUTER_RANGE, m_attackCnt);
			m_attackCnt += 1;
		}
	}
	break;
	default:
		break;
	}
	if (m_endTimePoint < nowTime)
		ExitState();
}

void BossState::AttackSpin::EnterState()
{
	BossState::AttackState::EnterState();
	m_firstAttackExecuteTime = std::chrono::high_resolution_clock::now() + FIRST_ATTACK_TIME;
	m_secondAttackExecuteTime = std::chrono::high_resolution_clock::now() + SECOND_ATTACK_TIME;
	m_thirdAttackExecuteTime = std::chrono::high_resolution_clock::now() + THIRD_ATTACK_TIME;
	m_attackCnt = 0;
	bossObject->SendBossState(BossState::STATE::SPIN);
}

void BossState::AttackSpin::UpdateState()
{
	const auto nowTime = std::chrono::high_resolution_clock::now();
	switch (m_attackCnt)
	{
	case 0:
	{
		if (m_firstAttackExecuteTime < nowTime) {//ù��° ���� ����
			bossObject->AttackSpin(DAMAGE, RANGE);
			m_attackCnt += 1;
		}
	}
	break;
	case 1:
	{
		if (m_secondAttackExecuteTime < nowTime) {//�ι�° ���� ����
			bossObject->AttackSpin(DAMAGE, RANGE);
			m_attackCnt += 1;
		}
	}
	break;
	case 2:
		if (m_thirdAttackExecuteTime < nowTime) {//�ι�° ���� ����
			bossObject->AttackSpin(DAMAGE, RANGE);
			m_attackCnt += 1;
		}
		break;
	default:
		break;
	}
	if (m_endTimePoint < nowTime)
		ExitState();
}

void BossState::AttackKick::EnterState()
{
	BossState::AttackState::EnterState();
	m_attackExecuteTime = std::chrono::high_resolution_clock::now() + ATTACK_TIME;
	m_isAttacked = false;
	bossObject->SendBossState(BossState::STATE::KICK);
}

void BossState::AttackKick::UpdateState()
{
	const auto nowTime = std::chrono::high_resolution_clock::now();
	if (m_isAttacked) {
		if (m_endTimePoint < nowTime)
			ExitState();
	}
	else if (m_attackExecuteTime < nowTime) {
		bossObject->AttackKick(DAMAGE, RANGE, VALID_RADIAN);
		m_isAttacked = true;
	}
}

void BossState::AttackPunch::EnterState()
{
	BossState::AttackState::EnterState();
	m_attackExecuteTime = std::chrono::high_resolution_clock::now() + ATTACK_TIME;
	m_isAttacked = false;
	bossObject->SendBossState(BossState::STATE::PUNCH);
}

void BossState::AttackPunch::UpdateState()
{
	const auto nowTime = std::chrono::high_resolution_clock::now();
	if (m_isAttacked) {
		if (m_endTimePoint < nowTime)
			ExitState();
	}
	else if (m_attackExecuteTime < nowTime) {
		bossObject->AttackPunch(DAMAGE, RANGE, VALID_RADIAN);
		m_isAttacked = true;
	}
}

void BossState::AttackMeteor::EnterState()
{
	BossState::AttackState::EnterState();
	bossObject->AttackMeteor();
	bossObject->SendBossState(BossState::STATE::METEOR);
}

void BossState::AttackMeteor::UpdateState()
{
	const auto nowTime = std::chrono::high_resolution_clock::now();
	if (m_endTimePoint < nowTime)
		ExitState();
}
#pragma endregion//BOSS_ATTACK_STATE
#pragma endregion //BOSS_STATE
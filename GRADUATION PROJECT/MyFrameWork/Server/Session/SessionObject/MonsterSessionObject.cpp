#include "stdafx.h"
#include "../Session.h"
#include "MonsterSessionObject.h"
#include "../../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../../Logic/Logic.h"
#include "../../Timer/Timer.h"
#include "../../Room/RoomManager.h"
#include "PlayerSessionObject.h"


extern Logic g_logic;
extern Timer g_Timer;
extern IOCPNetwork g_iocpNetwork;
extern RoomManager g_RoomManager;

MonsterSessionObject::MonsterSessionObject() : SessionObject()
{
	m_maxHp = m_hp = 2500;
}

MonsterSessionObject::MonsterSessionObject(std::string& roomId) : SessionObject()
{
	m_roomId = roomId;
	m_maxHp = m_hp = 2500;
}

MonsterSessionObject::~MonsterSessionObject()
{

}

void MonsterSessionObject::AutoMove()
{
	CalcRightVector();
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;
	Move(((float)durationTime / 1000.0f) * 50.0f, ((float)durationTime / 1000.0f));
	m_lastMoveTime = currentTime;
	//std::cout << "Boss current Position " << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;
	//std::cout << "Boss Dir Vector" << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

void MonsterSessionObject::StartMove(DIRECTION d)
{
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	isMove = true;
}

void MonsterSessionObject::StopMove()
{

}

void MonsterSessionObject::ChangeDirection(DIRECTION d)
{

}

const DirectX::XMFLOAT3 MonsterSessionObject::GetPosition()
{
	return m_position;
}

const DirectX::XMFLOAT3 MonsterSessionObject::GetRotation()
{
	return m_rotateAngle;
}

void MonsterSessionObject::Rotate(ROTATE_AXIS axis, float angle)
{
	switch (axis)
	{
	case X:
	{

	}
	break;
	case Y:
	{
		m_rotateAngle.y += angle;
	}
	break;
	case Z:
	{

	}
	break;
	default:
		break;
	}
	//std::cout << "Rotate angle: " << angle << std::endl;//
	DirectX::XMFLOAT3 xmf3Rev = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	xmf3Rev.x = m_directionVector.x * cos(XMConvertToRadians(-angle)) - m_directionVector.z * sin(XMConvertToRadians(-angle));
	xmf3Rev.z = m_directionVector.x * sin(XMConvertToRadians(-angle)) + m_directionVector.z * cos(XMConvertToRadians(-angle));
	xmf3Rev = Vector3::Normalize(xmf3Rev);
	m_directionVector = xmf3Rev;
	//std::cout << "Boss Dir Vector" << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

void MonsterSessionObject::SetDirection(DIRECTION d)
{

}

void MonsterSessionObject::Move(float fDistance, float elapsedTime)
{
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 des = Vector3::Subtract(m_DestinationPos, m_position);	// �������� ��ġ�� ����	
	CalcRightVector();
	bool OnRight = (Vector3::DotProduct(m_rightVector, Vector3::Normalize(des)) > 0) ? true : false;	// �������� �ô��� ��
	float ChangingAngle = Vector3::Angle(Vector3::Normalize(des), m_directionVector);
	if (Vector3::Length(des) < 10.0f) {
		auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_lastAttackTime);
		if (durationTime > std::chrono::seconds(1) + std::chrono::milliseconds(200)) {
			//TIMER_EVENT bossAttackEvent{ std::chrono::system_clock::now() + std::chrono::milliseconds(1), m_roomId, -1,EV_BOSS_ATTACK_ORDER };
			//g_Timer.InsertTimerQueue(bossAttackEvent);
			ExpOver* ov = new ExpOver();
			ov->m_opCode = OP_BOSS_ATTACK_SELECT;
			memcpy(ov->m_buffer, m_roomId.c_str(), m_roomId.size());
			ov->m_buffer[m_roomId.size()] = 0;
			PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, -1, &ov->m_overlap);
			;
		}
	}
	else {
		if (ChangingAngle > 0.5f)
		{
			if (OnRight)
				Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
			else if (!OnRight)
				Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
		}
		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, fDistance));//ƽ���� ������
		m_SPBB = BoundingSphere(DirectX::XMFLOAT3(m_position.x, m_position.y + 12.5f, m_position.z), 22.5f);
	}
	//std::cout << "BossPos: " << m_position.x << "0, " << m_position.z << std::endl;
}

void MonsterSessionObject::SetAggroPlayerId(int id)
{
	m_aggroPlayerId = id;
}

void MonsterSessionObject::AttackTimer()
{
	switch (currentAttack)
	{
	case ATTACK_KICK:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(563), m_roomId, 0,EV_BOSS_KICK };
		g_Timer.InsertTimerQueue(attackTimer);
	}
	break;
	case ATTACK_PUNCH:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(563), m_roomId, 0,EV_BOSS_PUNCH };
		g_Timer.InsertTimerQueue(attackTimer);
	}
	break;
	case ATTACK_SPIN:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(300), m_roomId, 2,EV_BOSS_SPIN };
		g_Timer.InsertTimerQueue(attackTimer);
		attackTimer.wakeupTime += std::chrono::milliseconds(300);
		attackTimer.playerId = 1;
		g_Timer.InsertTimerQueue(attackTimer);
		attackTimer.wakeupTime += std::chrono::milliseconds(300);
		attackTimer.playerId = 0;
		g_Timer.InsertTimerQueue(attackTimer);
	}
	break;
	default:
		break;
	}
}

void MonsterSessionObject::AttackPlayer(int restCount)
{
	Room& room = g_RoomManager.GetRunningRoom(m_roomId);
	auto& playerMap = room.GetInGamePlayerMap();
	switch (currentAttack)
	{
	case ATTACK_KICK:
	{
		for (auto& playerInfo : playerMap) {
			auto bossToPlayerVector = Vector3::Subtract(g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->GetPos(), m_position);
			float dotProductRes = Vector3::DotProduct(bossToPlayerVector, m_directionVector);
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 30.0f && abs(dotProductRes) < 90.0f) {
				g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->AttackedHp(40);
				//player Hit Kick
			}
		}
	}
	break;
	case ATTACK_PUNCH:
	{
		for (auto& playerInfo : playerMap) {
			auto bossToPlayerVector = Vector3::Subtract(g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->GetPos(), m_position);
			float dotProductRes = Vector3::DotProduct(bossToPlayerVector, m_directionVector);
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 15.0f && abs(dotProductRes) < 90.0f) {
				g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->AttackedHp(20);
				//player Hit Foward
			}
		}
	}
	break;
	case ATTACK_SPIN:
	{
		for (auto& playerInfo : playerMap) {
			auto bossToPlayerVector = Vector3::Subtract(g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->GetPos(), m_position);
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 15.0f) {
				g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->AttackedHp(15);
				//player Hit spin
			}
		}
	}
	break;
	default:
		break;
	}
	m_lastAttackTime = std::chrono::high_resolution_clock::now();
	if (restCount == 0) {
		isAttack = false;
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(200), m_roomId, -1,EV_FIND_PLAYER };
		g_Timer.InsertTimerQueue(attackTimer);
	}
}

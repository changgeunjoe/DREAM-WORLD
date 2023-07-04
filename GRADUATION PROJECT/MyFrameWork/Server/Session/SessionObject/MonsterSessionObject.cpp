#include "stdafx.h"
#include "../Session.h"
#include "MonsterSessionObject.h"
#include "../../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../../Logic/Logic.h"
#include "../../Timer/Timer.h"
#include "../../Room/RoomManager.h"
#include "../../MapData/MapData.h"
#include "PlayerSessionObject.h"
#include "../../IOCPNetwork/protocol/protocol.h"

extern Logic g_logic;
extern Timer g_Timer;
extern IOCPNetwork g_iocpNetwork;
extern RoomManager g_RoomManager;
extern MapData g_bossMapData;

MonsterSessionObject::MonsterSessionObject() : SessionObject()
{
	m_maxHp = m_hp = 2500;
	m_onIdx = g_bossMapData.GetFirstIdxs();
}

MonsterSessionObject::MonsterSessionObject(int& roomId) : SessionObject(roomId)
{
	m_maxHp = m_hp = 2500;
	m_DestinationPos = XMFLOAT3{ 0,0,0 };
	m_onIdx = g_bossMapData.GetFirstIdxs();
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
	//PrintCurrentTime();
	//std::cout << "MonsterSessionObject::StartMove()" << std::endl;
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	isMove = true;
}

void MonsterSessionObject::StopMove()
{

}

void MonsterSessionObject::ChangeDirection(DIRECTION d)
{

}

//const DirectX::XMFLOAT3 MonsterSessionObject::GetPosition()
//{
//	return m_position;
//}

//const DirectX::XMFLOAT3 MonsterSessionObject::GetRotation()
//{
//	return m_rotateAngle;
//}

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
	//PrintCurrentTime();
	//std::cout << std::endl << "Rotate angle: " << m_rotateAngle.y << std::endl;//
	DirectX::XMFLOAT3 xmf3Rev = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 currentVector = m_directionVector;
	xmf3Rev.x = currentVector.x * cos(XMConvertToRadians(-angle)) - currentVector.z * sin(XMConvertToRadians(-angle));
	xmf3Rev.z = currentVector.x * sin(XMConvertToRadians(-angle)) + currentVector.z * cos(XMConvertToRadians(-angle));
	xmf3Rev.y = 0;
	xmf3Rev = Vector3::Normalize(xmf3Rev);
	m_directionVector = xmf3Rev;
	//std::cout << "Boss Dir Vector" << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
}

void MonsterSessionObject::SetDirection(DIRECTION d)
{

}

void MonsterSessionObject::Move(float fDistance, float elapsedTime)
{
	//new - Astar
	XMFLOAT3 destinationPlayerPos = g_iocpNetwork.m_session[m_aggroPlayerId].m_sessionObject->GetPos();//�÷��̾� ��ġ
	XMFLOAT3 desPlayerVector = Vector3::Subtract(destinationPlayerPos, m_position);
	float playerDistance = Vector3::Length(Vector3::Subtract(m_position, desPlayerVector));
	desPlayerVector = Vector3::Normalize(desPlayerVector);
	CalcRightVector();
	if (playerDistance < 40.0f) {//�÷��̾ ������
		float ChangingAngle = Vector3::Angle(desPlayerVector, m_directionVector);
		if (ChangingAngle > 1.6f) {
			bool OnRight = (Vector3::DotProduct(m_rightVector, desPlayerVector) > 0) ? true : false;	// �������� ������ ��
			OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			//������ �ܺ� if���� �˾Ƽ� ���� ������ ��
		}
	}
	else {//�׷��� ���ϴٸ� ���� ã�� �����Ѵ�.
		m_reserveRoadLock.lock();
		if (m_ReserveRoad.size() > 0) {
			int currentNodeIdx = *m_ReserveRoad.begin();
			m_reserveRoadLock.unlock();
			XMFLOAT3 destinationNodeCenter = g_bossMapData.GetTriangleMesh(currentNodeIdx).GetCenter();	//����� ��ġ			
			m_DestinationPos = destinationNodeCenter;
			auto desNodeVector = Vector3::Subtract(m_DestinationPos, m_position);//���� ����
			auto desNodeDistance = Vector3::Length(desNodeVector); // �Ÿ�
			desNodeVector = Vector3::Normalize(desNodeVector);
			bool OnRight = (Vector3::DotProduct(m_rightVector, desNodeVector) > 0) ? true : false;	// �������� ������ ��
			float ChangingAngle = Vector3::Angle(desNodeVector, m_directionVector);
			//�Ÿ��� ���� ���		
			bool isOnNode = g_bossMapData.GetTriangleMesh(currentNodeIdx).IsOnTriangleMesh(m_position);

			if (desNodeDistance > 70.0f) { //�������� �Ÿ��� 10�̻� �̶��
				if (desNodeDistance > 50.0f) {
					if (ChangingAngle > 40.0f) {
						OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
					}
					else {
						if (ChangingAngle > 1.6f) {
							OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
						}
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, fDistance, false));//ƽ���� ������
						m_SPBB = BoundingSphere(DirectX::XMFLOAT3(m_position.x, m_position.y + 30.0f, m_position.z), 30.0f);
					}
				}
				else {//���� ��忡 ����ﶧ
					if (ChangingAngle > 1.6f)
					{
						OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
					}
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, fDistance, false));//ƽ���� ������
					m_SPBB = BoundingSphere(DirectX::XMFLOAT3(m_position.x, m_position.y + 30.0f, m_position.z), 30.0f);
				}
				//std::cout << "BossPos: " << m_position.x << "0, " << m_position.z << std::endl;
			}
			else {
				m_reserveRoadLock.lock();
				m_onIdx = *m_ReserveRoad.begin();
				m_ReserveRoad.erase(m_ReserveRoad.begin());
				if (m_ReserveRoad.size() != 0) {
					DirectX::XMFLOAT3 center = g_bossMapData.GetTriangleMesh(*m_ReserveRoad.begin()).GetCenter();
					m_DestinationPos = center;//������ ���� ����� ����
				}
				m_reserveRoadLock.unlock();
			}
		}
		else 		m_reserveRoadLock.unlock();
	}
	//else {
	//m_reserveRoadLock.unlock();

	//CalcRightVector();
	//bool OnRight = (Vector3::DotProduct(m_rightVector, desVector) > 0) ? true : false;	// �������� ������ ��
	//float ChangingAngle = Vector3::Angle(desVector, m_directionVector);
	//if (ChangingAngle > 15.0f && dis < 40.0f) {
	//	OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
	//}
	//else {
	//	if (ChangingAngle > 0.5f)
	//	{
	//		OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
	//	}
	//}

	//}
}


void MonsterSessionObject::SetDestinationPos(int id)
{
	//��ֹ��� �ֱ⶧���� �������� �ƴ�, Road�� �����Ͽ� ��������
	// ���������� ����� �ؼ� �����̰�, Ŭ�󿡼��� �������� ���� ���� ������ �ڵ����� ��������.
	//m_DestinationPos = des;
	SERVER_PACKET::BossMoveNodePacket sendPacket;
	sendPacket.desPlayerId = GetAggroPlayerId();
	std::cout << "aggro Id: " << sendPacket.desPlayerId << std::endl;
	XMFLOAT3 playerPos = g_iocpNetwork.m_session[sendPacket.desPlayerId].m_sessionObject->GetPos();
	m_reserveRoadLock.lock();
	m_ReserveRoad = g_bossMapData.AStarLoad(m_onIdx, playerPos.x, playerPos.z);
	m_reserveRoadLock.unlock();
	m_reserveRoadLock.lock();
	if (m_ReserveRoad.front() == m_onIdx) {
		m_reserveRoadLock.unlock();
		sendPacket.nodeCnt = -1;
		//sendPacket.desPos = XMFLOAT3(des.x, 0, des.z);
	}
	else {
		sendPacket.nodeCnt = m_ReserveRoad.size() > 40 ? 40 : m_ReserveRoad.size();
		auto iter = m_ReserveRoad.begin();
		for (int i = 0; i < sendPacket.nodeCnt; i++) {
			sendPacket.node[i] = *(iter++);
		}
		m_reserveRoadLock.unlock();
	}
	sendPacket.type = SERVER_PACKET::BOSS_MOVE_NODE;
	sendPacket.size = sizeof(SERVER_PACKET::BossMoveNodePacket);
	sendPacket.bossPos = m_position;
	g_logic.BroadCastInRoom(m_roomId, &sendPacket);
	//�̹� �� ������ ����Ҷ� ���� �� �ִµ� ���� ���⼭ �ؾ��ұ�? �Ǵ� ���� ���� ����� ���� Ŭ�󿡼� ��� �����ٱ�??
}

void MonsterSessionObject::ReserveAggroPlayerId(int id)
{
	m_newAggroPlayerId = id;
}

void MonsterSessionObject::SetAggroPlayerId()
{
	m_aggroPlayerId = m_newAggroPlayerId;
}


void MonsterSessionObject::AttackTimer()
{
	//std::cout << "ReSet lastAttack Time Boss" << std::endl;
	m_lastAttackTime = std::chrono::high_resolution_clock::now();

	switch (currentAttack)
	{
	case ATTACK_KICK:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(332), m_roomId ,EV_BOSS_KICK };
		g_Timer.InsertTimerQueue(attackTimer);
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(823), m_roomId , EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(endAttackTimer);
	}
	break;
	case ATTACK_PUNCH:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(332), m_roomId ,EV_BOSS_PUNCH };
		g_Timer.InsertTimerQueue(attackTimer);
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(824), m_roomId , EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(endAttackTimer);
	}
	break;
	case ATTACK_SPIN:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(300), m_roomId ,EV_BOSS_SPIN };
		g_Timer.InsertTimerQueue(attackTimer);
		attackTimer.wakeupTime += std::chrono::milliseconds(300);
		g_Timer.InsertTimerQueue(attackTimer);
		attackTimer.wakeupTime += std::chrono::milliseconds(300);
		g_Timer.InsertTimerQueue(attackTimer);
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::seconds(1), m_roomId , EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(endAttackTimer);
	}
	break;
	default:
		break;
	}

}

void MonsterSessionObject::AttackPlayer()
{
	if (m_hp <= 0) return;
	Room& room = g_RoomManager.GetRunningRoomRef(m_roomId);
	auto& playerMap = room.GetPlayerMap();
	switch (currentAttack)
	{
	case ATTACK_KICK:
	{
		for (auto& playerInfo : playerMap) {
			if (g_iocpNetwork.m_session[playerInfo.second].m_sessionObject == nullptr)continue;
			auto bossToPlayerVector = Vector3::Subtract(g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->GetPos(), m_position);
			float dotProductRes = Vector3::DotProduct(bossToPlayerVector, m_directionVector);
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 50.0f && abs(dotProductRes) < cosf(3.141592f / 12.0f)) {// 15 + 15 �� �� 30�� ���� �ִٸ�
				g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->AttackedHp(40);
				//player Hit Kick
			}
		}
	}
	break;
	case ATTACK_PUNCH:
	{
		for (auto& playerInfo : playerMap) {
			if (g_iocpNetwork.m_session[playerInfo.second].m_sessionObject == nullptr)continue;
			auto bossToPlayerVector = Vector3::Subtract(g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->GetPos(), m_position);
			float dotProductRes = Vector3::DotProduct(bossToPlayerVector, m_directionVector);
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 45.0f && abs(dotProductRes) < cosf(3.141592f / 12.0f)) { // 15 + 15 �� �� 30�� ���� �ִٸ�
				g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->AttackedHp(20);
				//player Hit Foward
			}
		}
	}
	break;
	case ATTACK_SPIN:
	{
		for (auto& playerInfo : playerMap) {
			if (g_iocpNetwork.m_session[playerInfo.second].m_sessionObject == nullptr)continue;
			auto bossToPlayerVector = Vector3::Subtract(g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->GetPos(), m_position);
			float bossToPlayerDis = Vector3::Length(bossToPlayerVector);
			if (bossToPlayerDis < 45.0f) {
				g_iocpNetwork.m_session[playerInfo.second].m_sessionObject->AttackedHp(15);
				//player Hit spin
			}
		}
	}
	break;
	default:
		break;
	}
	//if (restCount == 0) {
	//	StartMove(DIRECTION::FRONT);
	//	isAttack = false;
	//}

}

bool MonsterSessionObject::StartAttack()
{
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	auto pPos = g_iocpNetwork.m_session[m_aggroPlayerId].m_sessionObject->GetPos();
	XMFLOAT3 des = Vector3::Subtract(pPos, m_position);	// �������� ��ġ�� ����	
	float len = Vector3::Length(des);
	des = Vector3::Normalize(des);
	CalcRightVector();

	float lookDesDotRes = Vector3::DotProduct(m_directionVector, des);
	bool OnRight = (Vector3::DotProduct(m_rightVector, des) > 0) ? true : false;	// �������� �ô��� ��
	if (lookDesDotRes >= cosf(3.141592f / 12.0f)) { // ���� look�� �÷��̾���� ������ 30degree�϶�
		if (len <= 42.0f) {
			auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_lastAttackTime);
			if (durationTime > std::chrono::seconds(1) + std::chrono::milliseconds(500)) {
				//std::cout << "startAttack Return true ElapsedTime: " << durationTime << std::endl;
				return true;
			}
		}
	}
	return false;
}

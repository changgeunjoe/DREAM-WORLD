#include "stdafx.h"
#include "../UserSession.h"
#include "MonsterSessionObject.h"
#include "../../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../../Logic/Logic.h"
#include "../../Timer/Timer.h"
#include "../../Room/RoomManager.h"
#include "../../MapData/MapData.h"
#include "ChracterSessionObject.h"
#include "../../IOCPNetwork/protocol/protocol.h"

extern Logic g_logic;
extern Timer g_Timer;
extern IOCPNetwork g_iocpNetwork;
extern RoomManager g_RoomManager;
extern MapData g_bossMapData;

MonsterSessionObject::MonsterSessionObject() : SessionObject(30.0f)
{
	m_speed = 50.0f;
	m_maxHp = m_hp = 6500.0f;
	m_DestinationPos = XMFLOAT3{ 0,0,0 };
	m_onIdx = g_bossMapData.GetFirstIdxs();
}

MonsterSessionObject::MonsterSessionObject(int roomId) :SessionObject()
{
	m_speed = 50.0f;
	m_maxHp = m_hp = 6500.0f;
	m_DestinationPos = XMFLOAT3{ 0,0,0 };
	m_onIdx = g_bossMapData.GetFirstIdxs();
	m_roomId = roomId;
}

MonsterSessionObject::~MonsterSessionObject()
{

}

void MonsterSessionObject::StartMove()
{
	//PrintCurrentTime();
	//std::cout << "MonsterSessionObject::StartMove()" << std::endl;
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	isMove = true;
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

bool MonsterSessionObject::Move(float elapsedTime)
{
	XMFLOAT3 destinationPlayerPos = g_RoomManager.GetRunningRoomRef(m_roomId).GetPositionPlayCharacter(m_aggroPlayerRole);//플레이어 위치
	XMFLOAT3 desPlayerVector = Vector3::Subtract(destinationPlayerPos, m_position);
	float playerDistance = Vector3::Length(desPlayerVector);
	desPlayerVector = Vector3::Normalize(desPlayerVector);
	CalcRightVector();
	bool bossAndPlayerOnSameIdx = g_bossMapData.GetTriangleMesh(m_onIdx).IsOnTriangleMesh(destinationPlayerPos);
	if (playerDistance < 120.0f && bossAndPlayerOnSameIdx) {//플레이어가 근접함 80으로 변경하고 같은 노드인지 확인 하는 문 추가하자
		float ChangingAngle = Vector3::Angle(desPlayerVector, m_directionVector);
		if (ChangingAngle > 1.6f) {
			bool OnRight = (Vector3::DotProduct(m_rightVector, desPlayerVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
			OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			//공격은 외부 if에서 알아서 조건 맞으면 함
		}
		if (playerDistance >= 42.0f) {
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
			m_SPBB.Center = m_position;
			m_SPBB.Center.y = m_fBoundingSize;
		}
		return true;
	}
	else {//그렇지 못하다면 길을 찾아 가야한다.
		m_reserveRoadLock.lock();
		int secondNodeIdx = -1;
		int thridNodeIdx = -1;
		if (m_ReserveRoad.size() > 0) {
			int currentNodeIdx = *m_ReserveRoad.begin();
			if (m_ReserveRoad.size() > 1)
				secondNodeIdx = *(++m_ReserveRoad.begin());
			if (m_ReserveRoad.size() > 2)
				thridNodeIdx = *m_ReserveRoad.begin();
			m_reserveRoadLock.unlock();

			XMFLOAT3 destinationNodeCenter = g_bossMapData.GetTriangleMesh(currentNodeIdx).GetCenter();	//노드의 위치			
			m_DestinationPos = destinationNodeCenter;
			auto desNodeVector = Vector3::Subtract(m_DestinationPos, m_position);//방향 벡터
			auto desNodeDistance = Vector3::Length(desNodeVector); // 거리
			desNodeVector = Vector3::Normalize(desNodeVector);
			bool OnRight = (Vector3::DotProduct(m_rightVector, desNodeVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
			float ChangingAngle = Vector3::Angle(desNodeVector, m_directionVector);
			//거리와 벡터 계산		
			bool isOnNode = g_bossMapData.GetTriangleMesh(currentNodeIdx).IsOnTriangleMesh(m_position);

			if (desNodeDistance > 70.0f) { //목적지와 거리가 10이상 이라면
				if (desNodeDistance > 50.0f) {
					if (ChangingAngle > 40.0f) {
						OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
					}
					else {
						if (ChangingAngle > 1.6f) {
							OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
						}
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
						//m_SPBB = BoundingSphere(DirectX::XMFLOAT3(m_position.x, m_position.y + 30.0f, m_position.z), 30.0f);
					}
				}
				else {//현재 노드에 가까울때
					if (ChangingAngle > 1.6f)
					{
						OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
					}
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
					m_SPBB.Center = m_position;
					m_SPBB.Center.y += m_fBoundingSize;
					//m_SPBB = BoundingSphere(DirectX::XMFLOAT3(m_position.x, m_position.y + 30.0f, m_position.z), 30.0f);
				}
				//std::cout << "BossPos: " << m_position.x << "0, " << m_position.z << std::endl;
			}
			else {
				m_reserveRoadLock.lock();
				m_onIdx = *m_ReserveRoad.begin();
				m_ReserveRoad.erase(m_ReserveRoad.begin());
				if (m_ReserveRoad.size() != 0) {
					DirectX::XMFLOAT3 center = g_bossMapData.GetTriangleMesh(*m_ReserveRoad.begin()).GetCenter();
					m_DestinationPos = center;//목적지 다음 노드의 센터
				}
				m_reserveRoadLock.unlock();
			}
		}
		else 		m_reserveRoadLock.unlock();
	}
	//else {
	//m_reserveRoadLock.unlock();

	//CalcRightVector();
	//bool OnRight = (Vector3::DotProduct(m_rightVector, desVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
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
	return true;
}


void MonsterSessionObject::SetDestinationPos()
{
	//장애물이 있기때문에 목적지가 아닌, Road를 저장하여 움직이자
	// 서버에서는 계산을 해서 움직이고, 클라에서는 서버에서 계산된 값을 가지고 자동으로 움직이자.
	//m_DestinationPos = des;
	SERVER_PACKET::BossMoveNodePacket sendPacket;
	sendPacket.targetRole = m_aggroPlayerRole;
	XMFLOAT3& pPos = g_RoomManager.GetRunningRoomRef(m_roomId).GetPositionPlayCharacter(sendPacket.targetRole);
	std::cout << "aggro Role: " << sendPacket.targetRole << std::endl;
	//DataRace	
	m_reserveRoadLock.lock();
	m_ReserveRoad = g_bossMapData.AStarLoad(m_onIdx, pPos.x, pPos.z);
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
	//이미 점 데이터 계산할때 가질 수 있는데 굳이 여기서 해야할까? 또는 방향 벡터 계산을 이제 클라에서 어떻게 맞춰줄까??
}

void MonsterSessionObject::ReserveAggroPlayerRole(ROLE r)
{
	m_newAggroPlayerRole = r;
}

void MonsterSessionObject::SetAggroPlayerRole()
{
	m_aggroPlayerRole = m_newAggroPlayerRole;
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

	//if (restCount == 0) {
	//	StartMove(DIRECTION::FRONT);
	//	isAttack = false;
	//}

}

bool MonsterSessionObject::StartAttack()
{
	if (m_aggroPlayerRole == ROLE::NONE_SELECT) return false;
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3& pPos = g_RoomManager.GetRunningRoomRef(m_roomId).GetPositionPlayCharacter(m_aggroPlayerRole);
	XMFLOAT3 des = Vector3::Subtract(pPos, m_position);	// 목적지랑 위치랑 벡터	
	float len = Vector3::Length(des);
	des = Vector3::Normalize(des);
	CalcRightVector();

	float lookDesDotRes = Vector3::DotProduct(m_directionVector, des);
	bool OnRight = (Vector3::DotProduct(m_rightVector, des) > 0) ? true : false;	// 목적지가 올느쪽 왼
	if (lookDesDotRes >= cosf(3.141592f / 12.0f)) { // 보스 look과 플레이어와의 각도가 30degree일때
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

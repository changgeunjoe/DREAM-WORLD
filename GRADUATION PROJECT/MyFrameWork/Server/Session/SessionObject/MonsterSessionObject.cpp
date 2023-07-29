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
	m_fBoundingSize = 40.0f;
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
	if (isMove) {
		XMFLOAT3 destinationPlayerPos = g_RoomManager.GetRunningRoomRef(m_roomId).GetPositionPlayCharacter(m_aggroPlayerRole);//플레이어 위치
		XMFLOAT3 desPlayerVector = Vector3::Subtract(destinationPlayerPos, m_position);
		float playerDistance = Vector3::Length(desPlayerVector);
		desPlayerVector = Vector3::Normalize(desPlayerVector);
		CalcRightVector();

		bool isOnNode = g_bossMapData.GetTriangleMesh(m_onIdx).IsOnTriangleMesh(m_position);
		if (!isOnNode) {
			std::cout << "boss change index" << std::endl;
			m_reserveRoadLock.lock();
			if (m_ReserveRoad.size() > 1) {
				m_ReserveRoad.erase(m_ReserveRoad.begin());
				m_onIdx = *m_ReserveRoad.begin();
			}
			else {
				m_onIdx = *m_ReserveRoad.begin();
			}
			m_reserveRoadLock.unlock();
		}

		m_reserveRoadLock.lock();
		if (m_ReserveRoad.size() > 1) {
			int currentNodeIdx = *m_ReserveRoad.begin();
			int secondNodeIdx = *(++m_ReserveRoad.begin());
			m_reserveRoadLock.unlock();
			TrinangleMesh destinationNodeCenter = g_bossMapData.GetTriangleMesh(currentNodeIdx);
			TrinangleMesh nextMoveNode = g_bossMapData.GetTriangleMesh(secondNodeIdx);
			std::vector<int> sharedPoints = destinationNodeCenter.IsShareLine(g_bossMapData.GetTriangleMesh(secondNodeIdx).GetVertexIdxs());

			std::vector<XMFLOAT3> mapVertexData = g_bossMapData.GetVertexData();
			XMFLOAT3 nodeVec1 = Vector3::Normalize(Vector3::Subtract(mapVertexData[sharedPoints[0]], m_position));
			XMFLOAT3 nodeVec2 = Vector3::Normalize(Vector3::Subtract(mapVertexData[sharedPoints[1]], m_position));
			if (nodeVec1.x < DBL_EPSILON && nodeVec1.z < DBL_EPSILON) {
				nodeVec1 = desPlayerVector;
			}
			if (nodeVec2.x < DBL_EPSILON && nodeVec2.z < DBL_EPSILON) {
				nodeVec2 = desPlayerVector;
			}
			float dotResult1 = Vector3::DotProduct(desPlayerVector, nodeVec1);
			float dotResult2 = Vector3::DotProduct(desPlayerVector, nodeVec2);
			XMFLOAT3 monsterLookTo = m_rightVector;
			if (dotResult1 > 0) {
				if (dotResult2 > 0) {
					monsterLookTo = desPlayerVector;
				}
				else {
					monsterLookTo = nodeVec1;
				}
			}
			else {
				if (dotResult2 > 0) {
					monsterLookTo = nodeVec2;
				}

			}//위에서 나온 벡터 기준 회전하여 이동하자자
			SERVER_PACKET::BossDirectionPacket postData;
			postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
			postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
			postData.directionVec = monsterLookTo;
			ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
			postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
			PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);
			;

			m_desVector = monsterLookTo;
			bool OnRight = (Vector3::DotProduct(m_rightVector, monsterLookTo) > 0) ? true : false;	// 목적지가 오른쪽 왼
			float ChangingAngle = Vector3::Angle(monsterLookTo, m_directionVector);
			//거리와 벡터 계산				
			if (ChangingAngle > 40.0f) {
				std::cout << "Astar Move - Rotate angle >40" << std::endl;
				OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			}
			else if (ChangingAngle > 1.7f) {
				std::cout << "Astar Move - Rotate angle <1.7 && move" << std::endl;
				OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
				m_SPBB.Center = m_position;
				m_SPBB.Center.y += m_fBoundingSize;
			}
			else {
				std::cout << "Astar Move - move forward" << std::endl;
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
				m_SPBB.Center = m_position;
				m_SPBB.Center.y += m_fBoundingSize;
			}
		}
		else {
			m_reserveRoadLock.unlock();

			m_desVector = desPlayerVector;
			float ChangingAngle = Vector3::Angle(desPlayerVector, m_directionVector);
			if (ChangingAngle > 40.0f) {
				std::cout << "EQ IDX - angle > 40" << std::endl;
				bool OnRight = (Vector3::DotProduct(m_rightVector, desPlayerVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
				OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
				return true;
			}
			else if (ChangingAngle > 1.8f) {
				std::cout << "EQ IDX - angle > 1.8 & move" << std::endl;
				bool OnRight = (Vector3::DotProduct(m_rightVector, desPlayerVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
				OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
				//공격은 외부 if에서 알아서 조건 맞으면 함
			}
			if (playerDistance >= 42.0f) {
				std::cout << "EQ IDX - move forward" << std::endl;
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
				m_SPBB.Center = m_position;
				m_SPBB.Center.y = m_fBoundingSize;
			}
			return true;
		}
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
	//std::cout << "aggro Role: " << sendPacket.targetRole << std::endl;
	//DataRace	
	m_reserveRoadLock.lock();
	m_ReserveRoad = g_bossMapData.AStarLoad(m_onIdx, pPos.x, pPos.z);
	m_reserveRoadLock.unlock();
	m_reserveRoadLock.lock();
	if (m_ReserveRoad.size() < 2) {
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
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(332), m_roomId ,EV_BOSS_ATTACK };
		g_Timer.InsertTimerQueue(attackTimer);
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(823), m_roomId , EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(endAttackTimer);
	}
	break;
	case ATTACK_PUNCH:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(332), m_roomId ,EV_BOSS_ATTACK };
		g_Timer.InsertTimerQueue(attackTimer);
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(824), m_roomId , EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(endAttackTimer);
	}
	break;
	case ATTACK_SPIN:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::milliseconds(300), m_roomId ,EV_BOSS_ATTACK };
		g_Timer.InsertTimerQueue(attackTimer);
		attackTimer.wakeupTime += std::chrono::milliseconds(300);
		g_Timer.InsertTimerQueue(attackTimer);
		attackTimer.wakeupTime += std::chrono::milliseconds(300);
		g_Timer.InsertTimerQueue(attackTimer);
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::seconds(1), m_roomId , EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(endAttackTimer);
	}
	break;
	case ATTACK_FLOOR_BOOM:
	{
		TIMER_EVENT attackTimer{ std::chrono::system_clock::now() + std::chrono::seconds(1), m_roomId ,EV_BOSS_ATTACK };
		g_Timer.InsertTimerQueue(attackTimer);
		attackTimer.wakeupTime += std::chrono::seconds(1);
		g_Timer.InsertTimerQueue(attackTimer);
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::seconds(2) + std::chrono::milliseconds(150), m_roomId , EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(endAttackTimer);
	}
	break;
	case ATTACK_METEO:
	{		
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::seconds(1) + std::chrono::milliseconds(150), m_roomId , EV_BOSS_STATE };
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
	if (lookDesDotRes >= BOSS_ABLE_ATTACK_COS_VALUE) { // 보스 look과 플레이어와의 각도가 30degree일때
		if (len <= 42.0f) {
			auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_lastAttackTime);
			if (durationTime > std::chrono::seconds(1) + std::chrono::milliseconds(500)) {
				std::cout << "startAttack Return true ElapsedTime: " << durationTime << std::endl;
				return true;
			}
		}
	}
	return false;
}

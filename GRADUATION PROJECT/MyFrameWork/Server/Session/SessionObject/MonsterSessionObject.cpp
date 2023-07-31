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

MonsterSessionObject::MonsterSessionObject(int roomId) :SessionObject(30.0f)
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
		destinationPlayerPos.y = 0.0;
		XMFLOAT3 desPlayerVector = Vector3::Subtract(destinationPlayerPos, m_position);
		float playerDistance = Vector3::Length(desPlayerVector);
		desPlayerVector = Vector3::Normalize(desPlayerVector);
		CalcRightVector();

		bool isOnNode = g_bossMapData.GetTriangleMesh(m_onIdx).IsOnTriangleMesh(m_position);
		bool isPlayerOnNode = g_bossMapData.GetTriangleMesh(m_onIdx).IsOnTriangleMesh(destinationPlayerPos);

		XMFLOAT3 monsterVector = m_directionVector;
		//if (isOnNode && isPlayerOnNode) {
		//	m_desVector = desPlayerVector;
		//	bool OnRight = (Vector3::DotProduct(m_rightVector, m_desVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
		//	float ChangingAngle = Vector3::Angle(m_desVector, m_directionVector);		
		//	if (ChangingAngle > 20.0f) {
		//		//std::cout << "Astar Move - Rotate angle >40" << std::endl;
		//		OnRight ? Rotate(ROTATE_AXIS::Y, 180.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -180.0f * elapsedTime);
		//	}
		//	else if (ChangingAngle > 1.7f) {
		//		//std::cout << "Astar Move - Rotate angle <1.7 && move" << std::endl;			
		//		OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
		//		if (playerDistance >= 34.0f) {
		//			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
		//			m_SPBB.Center = m_position;
		//			m_SPBB.Center.y += m_fBoundingSize;
		//		}
		//	}
		//	else {
		//		//std::cout << "Astar Move - move forward" << std::endl;
		//		if (playerDistance >= 34.0f) {
		//			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
		//			m_SPBB.Center = m_position;
		//			m_SPBB.Center.y += m_fBoundingSize;
		//		}
		//	}
		//}
		if (m_changeRoad) {
			int firstIdx;
			int secondIdx;
			m_reserveRoadLock.lock();
			if (m_ReserveRoad.size() > 1) {
				firstIdx = *m_ReserveRoad.begin();
				secondIdx = *(++m_ReserveRoad.begin());
				m_reserveRoadLock.unlock();
				TrinangleMesh& firstTriangle = g_bossMapData.GetTriangleMesh(firstIdx);
				TrinangleMesh& secondTriangle = g_bossMapData.GetTriangleMesh(secondIdx);
				std::vector<int> sharedPoint = firstTriangle.IsShareLine(secondTriangle.GetVertexIdxs());
				auto& mapVertexData = g_bossMapData.GetVertexData();

				XMFLOAT3 sharedPointVec1 = Vector3::Subtract(mapVertexData[sharedPoint[0]], m_position);
				XMFLOAT3 normalizeSharedPointVec1 = Vector3::Normalize(sharedPointVec1);

				XMFLOAT3 sharedPointVec2 = Vector3::Subtract(mapVertexData[sharedPoint[1]], m_position);
				XMFLOAT3 normalizeSharedPointVec2 = Vector3::Normalize(sharedPointVec2);

				XMFLOAT3 centerVec = Vector3::Add(normalizeSharedPointVec1, normalizeSharedPointVec2);
				XMFLOAT3 normalizeCenterVec = Vector3::Normalize(centerVec);

				float centerToPDot = Vector3::DotProduct(desPlayerVector, normalizeCenterVec);
				float sharedToPDot1 = Vector3::DotProduct(desPlayerVector, normalizeSharedPointVec1);
				float sharedToPDot2 = Vector3::DotProduct(desPlayerVector, normalizeSharedPointVec2);

				float sharedToCenter1 = Vector3::DotProduct(normalizeCenterVec, normalizeSharedPointVec1);
				float sharedToCenter2 = Vector3::DotProduct(normalizeCenterVec, normalizeSharedPointVec2);

				if (centerToPDot >= sharedToCenter1 && centerToPDot >= sharedToCenter2) {
					monsterVector = desPlayerVector;
				}
				else {
					XMFLOAT3 middleLineVec = Vector3::Add(sharedPointVec1, sharedPointVec2);
					XMFLOAT3 normalizeMiddleLineVec = Vector3::Normalize(middleLineVec);
					if (sharedToPDot1 > sharedToPDot2) {
						monsterVector = Vector3::Normalize(Vector3::Add(middleLineVec, sharedPointVec1, 1.4f));
					}
					else {
						monsterVector = Vector3::Normalize(Vector3::Add(middleLineVec, sharedPointVec2, 1.4f));
					}
				}

				m_desVector = monsterVector;
				bool OnRight = (Vector3::DotProduct(m_rightVector, m_desVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
				float ChangingAngle = Vector3::Angle(m_desVector, m_directionVector);

				if (ChangingAngle > 20.0f) {
					//std::cout << "Astar Move - Rotate angle >40" << std::endl;
					SERVER_PACKET::BossDirectionPacket postData;
					postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
					postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
					postData.directionVec = monsterVector;
					ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
					postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
					PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);
					OnRight ? Rotate(ROTATE_AXIS::Y, 180.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -180.0f * elapsedTime);
				}
				else if (ChangingAngle > 1.7f) {
					//std::cout << "Astar Move - Rotate angle <1.7 && move" << std::endl;
					SERVER_PACKET::BossDirectionPacket postData;
					postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
					postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
					postData.directionVec = monsterVector;
					ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
					postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
					PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);

					OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
					if (playerDistance >= 34.0f) {
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
					}
				}
				else {
					//std::cout << "Astar Move - move forward" << std::endl;
					if (playerDistance >= 34.0f) {
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
					}
				}
			}
			else {
				m_reserveRoadLock.unlock();
				monsterVector = desPlayerVector;
				m_desVector = monsterVector;
				bool OnRight = (Vector3::DotProduct(m_rightVector, m_desVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
				float ChangingAngle = Vector3::Angle(m_desVector, m_directionVector);
				if (ChangingAngle > 20.0f) {
					//std::cout << "Astar Move - Rotate angle >40" << std::endl;
					SERVER_PACKET::BossDirectionPacket postData;
					postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
					postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
					postData.directionVec = m_desVector;
					ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
					postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
					PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);
					OnRight ? Rotate(ROTATE_AXIS::Y, 180.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -180.0f * elapsedTime);
				}
				else if (ChangingAngle > 1.7f) {
					//std::cout << "Astar Move - Rotate angle <1.7 && move" << std::endl;
					SERVER_PACKET::BossDirectionPacket postData;
					postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
					postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
					postData.directionVec = m_desVector;
					ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
					postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
					PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);

					OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
					if (playerDistance >= 34.0f) {
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
					}
				}
				else {
					//std::cout << "Astar Move - move forward" << std::endl;
					if (playerDistance >= 34.0f) {
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
					}
				}
				//플레이어 따라가기

			}
			m_changeRoad = false;
		}
		else if (!isOnNode) {
			m_reserveRoadLock.lock();
			if (!m_ReserveRoad.empty() && m_ReserveRoad.size() > 1) {
				m_onIdx = *m_ReserveRoad.begin();
				m_ReserveRoad.erase(m_ReserveRoad.begin());
				m_reserveRoadLock.unlock();

			}
			else m_reserveRoadLock.unlock();
			int firstIdx;
			int secondIdx;
			m_reserveRoadLock.lock();
			if (m_ReserveRoad.size() > 1) {
				firstIdx = *m_ReserveRoad.begin();
				secondIdx = *(++m_ReserveRoad.begin());
				m_reserveRoadLock.unlock();
				TrinangleMesh& firstTriangle = g_bossMapData.GetTriangleMesh(firstIdx);
				TrinangleMesh& secondTriangle = g_bossMapData.GetTriangleMesh(secondIdx);
				std::vector<int> sharedPoint = firstTriangle.IsShareLine(secondTriangle.GetVertexIdxs());
				auto& mapVertexData = g_bossMapData.GetVertexData();

				XMFLOAT3 sharedPointVec1 = Vector3::Subtract(mapVertexData[sharedPoint[0]], m_position);
				XMFLOAT3 normalizeSharedPointVec1 = Vector3::Normalize(sharedPointVec1);

				XMFLOAT3 sharedPointVec2 = Vector3::Subtract(mapVertexData[sharedPoint[1]], m_position);
				XMFLOAT3 normalizeSharedPointVec2 = Vector3::Normalize(sharedPointVec2);

				XMFLOAT3 centerVec = Vector3::Add(normalizeSharedPointVec1, normalizeSharedPointVec2);
				XMFLOAT3 normalizeCenterVec = Vector3::Normalize(centerVec);

				float centerToPDot = Vector3::DotProduct(desPlayerVector, normalizeCenterVec);
				float sharedToPDot1 = Vector3::DotProduct(desPlayerVector, normalizeSharedPointVec1);
				float sharedToPDot2 = Vector3::DotProduct(desPlayerVector, normalizeSharedPointVec2);

				if (centerToPDot >= sharedToPDot1 && centerToPDot >= sharedToPDot2) {
					monsterVector = desPlayerVector;
				}
				else {
					XMFLOAT3 middleLineVec = Vector3::Add(sharedPointVec1, sharedPointVec2);
					XMFLOAT3 normalizeMiddleLineVec = Vector3::Normalize(middleLineVec);
					if (sharedToPDot1 > sharedToPDot2) {
						monsterVector = Vector3::Normalize(Vector3::Add(middleLineVec, sharedPointVec1, 1.4f));
					}
					else {
						monsterVector = Vector3::Normalize(Vector3::Add(middleLineVec, sharedPointVec2, 1.4f));
					}
				}
				m_desVector = monsterVector;
				bool OnRight = (Vector3::DotProduct(m_rightVector, m_desVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
				float ChangingAngle = Vector3::Angle(m_desVector, m_directionVector);


				if (ChangingAngle > 20.0f) {
					//std::cout << "Astar Move - Rotate angle >40" << std::endl;
					SERVER_PACKET::BossDirectionPacket postData;
					postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
					postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
					postData.directionVec = m_desVector;
					ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
					postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
					PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);
					OnRight ? Rotate(ROTATE_AXIS::Y, 180.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -180.0f * elapsedTime);
				}
				else if (ChangingAngle > 1.7f) {
					//std::cout << "Astar Move - Rotate angle <1.7 && move" << std::endl;
					SERVER_PACKET::BossDirectionPacket postData;
					postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
					postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
					postData.directionVec = m_desVector;
					ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
					postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
					PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);

					OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
					if (playerDistance >= 34.0f) {
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
					}
				}
				else {
					//std::cout << "Astar Move - move forward" << std::endl;
					if (playerDistance >= 34.0f) {
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
					}
				}
			}
			else {
				m_reserveRoadLock.unlock();
				monsterVector = desPlayerVector;
				m_desVector = monsterVector;
				bool OnRight = (Vector3::DotProduct(m_rightVector, m_desVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
				float ChangingAngle = Vector3::Angle(m_desVector, m_directionVector);
				if (ChangingAngle > 20.0f) {
					//std::cout << "Astar Move - Rotate angle >40" << std::endl;
					SERVER_PACKET::BossDirectionPacket postData;
					postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
					postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
					postData.directionVec = m_desVector;
					ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
					postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
					PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);
					OnRight ? Rotate(ROTATE_AXIS::Y, 180.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -180.0f * elapsedTime);
				}
				else if (ChangingAngle > 1.7f) {
					//std::cout << "Astar Move - Rotate angle <1.7 && move" << std::endl;
					SERVER_PACKET::BossDirectionPacket postData;
					postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
					postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
					postData.directionVec = m_desVector;
					ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
					postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
					PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);

					OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
					if (playerDistance >= 34.0f) {
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
					}
				}
				else {
					//std::cout << "Astar Move - move forward" << std::endl;
					if (playerDistance >= 34.0f) {
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
						m_SPBB.Center = m_position;
						m_SPBB.Center.y += m_fBoundingSize;
					}
				}
				//플레이어 따라가기
			}
		}
		else {
			float diffAngle = Vector3::Angle(m_desVector, m_directionVector);
			if (diffAngle > 1.7f) {
				SERVER_PACKET::BossDirectionPacket postData;
				postData.size = sizeof(SERVER_PACKET::BossDirectionPacket);
				postData.type = SERVER_PACKET::BOSS_CHANGE_DIRECION;
				postData.directionVec = m_desVector;
				ExpOver* postQueue = new ExpOver(reinterpret_cast<char*>(&postData));
				postQueue->m_opCode = OP_BOSS_CHANGE_DIRECTION;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postQueue->m_overlap);
			}

			m_desVector = desPlayerVector;
			bool OnRight = (Vector3::DotProduct(m_rightVector, m_desVector) > 0) ? true : false;	// 목적지가 오른쪽 왼
			float ChangingAngle = Vector3::Angle(m_desVector, m_directionVector);
			if (ChangingAngle > 20.0f) {
				//std::cout << "Astar Move - Rotate angle >40" << std::endl;
				OnRight ? Rotate(ROTATE_AXIS::Y, 180.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -180.0f * elapsedTime);
			}
			else if (ChangingAngle > 1.7f) {
				//std::cout << "Astar Move - Rotate angle <1.7 && move" << std::endl;			
				OnRight ? Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime) : Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
				if (playerDistance >= 34.0f) {
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
					m_SPBB.Center = m_position;
					m_SPBB.Center.y += m_fBoundingSize;
				}
			}
			else {
				//std::cout << "Astar Move - move forward" << std::endl;
				if (playerDistance >= 34.0f) {
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//틱마다 움직임
					m_SPBB.Center = m_position;
					m_SPBB.Center.y += m_fBoundingSize;
				}
			}
		}
	}
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
	bool isOnNode = g_bossMapData.GetTriangleMesh(m_onIdx).IsOnTriangleMesh(m_position);
	if (!isOnNode) {
		auto& quadTree = g_bossMapData.GetNavMeshQuadTree();
		auto candidateNodes = quadTree.GetNearbyNavMeshes(m_position.x, m_position.z);
		auto triangleMesh = g_bossMapData.GetTriangleMeshes();
		float minDis = FLT_MAX;
		int idx = -1;
		bool isOnResult = false;
		XMFLOAT3 desPos = XMFLOAT3(m_position.x, 0, m_position.z);
		for (int node : candidateNodes) {
			isOnResult = triangleMesh[node].IsOnTriangleMesh(desPos);
			if (isOnResult) {
				idx = node;
				break;
			}
			float dis = triangleMesh[node].GetDistanceByPoint(desPos);
			if (minDis > dis) {
				minDis = dis;
				idx = node;
			}
		}
		int reFindIdx = -1;
		if (!isOnResult) {
			for (auto relationList : triangleMesh[idx].m_relationMesh) {
				isOnResult = triangleMesh[relationList.first].IsOnTriangleMesh(desPos);
				reFindIdx = relationList.first;
				if (isOnResult) break;
			}
		}
		int desNodeIdx = -1;
		if (reFindIdx == -1)
			desNodeIdx = idx;
		else desNodeIdx = reFindIdx;
		m_onIdx = desNodeIdx;
		m_changeRoad = true;
	}
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
		attackTimer.wakeupTime += std::chrono::seconds(1) + std::chrono::milliseconds(500);
		g_Timer.InsertTimerQueue(attackTimer);
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::seconds(3), m_roomId , EV_BOSS_STATE };
		g_Timer.InsertTimerQueue(endAttackTimer);
	}
	break;
	case ATTACK_METEO:
	{
		TIMER_EVENT endAttackTimer{ std::chrono::system_clock::now() + std::chrono::seconds(1) + std::chrono::milliseconds(250), m_roomId , EV_BOSS_STATE };
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
				//std::cout << "startAttack Return true ElapsedTime: " << durationTime << std::endl;
				return true;
			}
		}
	}
	return false;
}

void MonsterSessionObject::SetBossStagePosition()
{
	m_position = XMFLOAT3(-13.8, 0, -61);
}

#include "stdafx.h"
#include "SmallMonsterSessionObject.h"
#include "../../Logic/Logic.h"
#include "../../Room/RoomManager.h"
#include "../../MapData/MapData.h"
#include "../../IOCPNetwork/protocol/protocol.h"

extern Logic g_logic;
extern RoomManager g_RoomManager;
extern MapData g_bossMapData;
extern MapData g_stage1MapData;

//#define MONSTER_MOVE_LOG 1

SmallMonsterSessionObject::SmallMonsterSessionObject() :SessionObject(8.0f)
{
	m_speed = 30.0f;
	m_maxHp = m_hp = 150.0f;
}

SmallMonsterSessionObject::SmallMonsterSessionObject(int roomId) :SessionObject(8.0f)
{
	m_speed = 30.0f;
	m_maxHp = m_hp = 150.0f;
}

SmallMonsterSessionObject::~SmallMonsterSessionObject()
{

}

void SmallMonsterSessionObject::SetDestinationPos(XMFLOAT3* posArr)
{
	if (!m_isAlive)return;
	std::pair<float, int> minDis = std::make_pair(-1.0f, -1);
	for (int i = 0; i < 4; i++) {
		if (minDis.first < 0.0f) {
			float dis = Vector3::Length(Vector3::Subtract(m_position, posArr[i]));
			minDis.first = dis;
			minDis.second = i;
		}
		else {
			float dis = Vector3::Length(Vector3::Subtract(m_position, posArr[i]));
			if (dis < minDis.first) {
				minDis.first = dis;
				minDis.second = i;
			}
		}
	}
	m_desPos = posArr[minDis.second];
	//m_desDis = minDis.first;
}

bool SmallMonsterSessionObject::StartAttack()
{
	if (!m_isAlive)return true;
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_lastAttackTime);
	if (!isMove) {
		if (durationTime > std::chrono::milliseconds(300)) {//���� �ִϸ��̼� �� �ٽ� �̵��� �ð� �����ؾߵ�
			isMove = true;
			return false;
		}
		return true;
	}

	for (auto& playCharacter : roomRef.GetPlayCharacters()) {
		XMFLOAT3 des = Vector3::Subtract(playCharacter.second->GetPos(), m_position);	// �������� ��ġ�� ����	
		float len = Vector3::Length(des);
		des = Vector3::Normalize(des);
		float lookDesDotRes = Vector3::DotProduct(m_directionVector, des);
		if (lookDesDotRes >= MONSTER_ABLE_ATTACK_COS_VALUE) { // ���� look�� �÷��̾���� ������ 30degree�϶�
			if (len <= 42.0f) {
				if (durationTime > std::chrono::seconds(1) + std::chrono::milliseconds(500)) {
					playCharacter.second->AttackedHp(70.0f);
					SERVER_PACKET::SmallMonsterAttackPlayerPacket sendPacket;
					sendPacket.size = sizeof(SERVER_PACKET::SmallMonsterAttackPlayerPacket);
					sendPacket.type = SERVER_PACKET::SMALL_MONSTER_ATTACK;
					sendPacket.attackedRole = playCharacter.first;
					sendPacket.attackMonsterIdx = m_id;
					g_logic.BroadCastInRoom(m_roomId, &sendPacket);
					isMove = false;
					m_lastAttackTime = std::chrono::high_resolution_clock::now();
					return true;
				}
			}
		}
	}
	return false;
}

void SmallMonsterSessionObject::StartMove()
{
	if (!m_isAlive)return;
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	isMove = true;
}

void SmallMonsterSessionObject::Rotate(ROTATE_AXIS axis, float angle)
{
	if (!m_isAlive)return;
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
}

bool SmallMonsterSessionObject::Move(float elapsedTime)
{
	if (!m_isAlive)return true;
	if (!isMove)return true;
	XMFLOAT3 desVector = Vector3::Subtract(m_desPos, m_position);
	float desDis = Vector3::Length(desVector);
	desVector = Vector3::Normalize(desVector);
	float frontDotRes = Vector3::DotProduct(desVector, m_directionVector);
	//std::cout << "desVector: " << desVector.x << ", " << desVector.y << ", " << desVector.z << endl;

	if (CheckCollision(m_directionVector, elapsedTime)) {
		desVector = Vector3::Subtract(m_desPos, m_position);
		desVector = Vector3::Normalize(desVector);
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//���� �ٲ�ߵ� - ������ ���� ����
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
			}
			else {
				Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			}
		}
		return true;
	}

	if (desDis <= 30.0f) {//���� ������
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//���� �ٲ�ߵ� - ������ ���� ����
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
			}
			else {
				Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			}
		}
		return true;
	}

	else if (desDis <= 80.0f) {
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//���� �ٲ�ߵ�			
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
			}
			else {
				Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			}
		}
		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, m_speed * elapsedTime, false));//ƽ���� ������		
		SetPosition(m_position);
		return true;
	}
	else if (desDis <= 120.0f) {
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//���� �ٲ�ߵ� - ������ ���� ����			
			float rightDotRes = Vector3::DotProduct(desVector, m_rightVector);
			if (rightDotRes >= 0) {
				Rotate(ROTATE_AXIS::Y, 90.0f * elapsedTime);
			}
			else {
				Rotate(ROTATE_AXIS::Y, -90.0f * elapsedTime);
			}
		}
	}
	return true;
}

void SmallMonsterSessionObject::UpdateMonsterState()
{
	if (m_hp < DBL_EPSILON)
		m_isAlive = false;
}

float SmallMonsterSessionObject::GetDistance(XMFLOAT3& point)
{
	return Vector3::Length(Vector3::Subtract(point, m_position));
}

std::pair<float, XMFLOAT3> SmallMonsterSessionObject::GetNormalVectorSphere(XMFLOAT3& point)
{
	XMFLOAT3 normalVec = Vector3::Subtract(m_position, point);
	float normalSize = Vector3::Length(normalVec);
	normalVec = Vector3::Normalize(normalVec);
	return std::pair<float, XMFLOAT3>(normalSize, normalVec);
}

std::pair<bool, XMFLOAT3> SmallMonsterSessionObject::CheckCollisionMap_Boss(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	std::vector<MapCollide>& Collides = g_bossMapData.GetCollideData();
	for (auto& collide : Collides) {
		if (collide.GetObb().Intersects(m_SPBB)) {

			auto& relationIdxsVector = collide.GetRelationCollisionIdxs();
			int secondCollide = -1;
			for (auto& otherCol : relationIdxsVector) {
				if (Collides[otherCol].GetObb().Intersects(m_SPBB)) {
					secondCollide = otherCol;
					break;
				}
			}
			if (secondCollide == -1) {//m_SPBB								
				auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, m_position, moveDirection);//�븻, �����̵�, �븻�� �������� ũ�� �� ��ȯ
				XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//�븻 ����
				XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//�����̵� ����
				float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
				float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//�����̵� ���Ϳ� ���� ���� ���� ��				
				collideSlidingVector = Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut, false);
				collideNormalVector = Vector3::ScalarProduct(collideNormalVector, 0.06f * normalVectorDotProductReslut, false);
				return std::pair<bool, XMFLOAT3>(true, Vector3::Add(collideSlidingVector, collideNormalVector));
			}
			else {
				auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, m_position, moveDirection);//�븻, �����̵�, �븻�� �������� ũ�� �� ��ȯ
				XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//�븻 ����
				//XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//�����̵� ����
				float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);//�븻 ���Ͱ� �������� ũ��(�浹 ��ġ ������ ����)
				float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//�����̵� ���Ϳ� �� ���� ���� ��				

				auto CollidePolygonNormalVector2 = Collides[secondCollide].CalSlidingVector(m_SPBB, m_position, moveDirection);//�븻, �����̵�, �븻�� �������� ũ�� �� ��ȯ
				XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//�븻 ����
				//XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//�����̵� ����
				float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);//�븻 ���Ͱ� �������� ũ��(�浹 ��ġ ������ ����)
				float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//�����̵� ���Ϳ� �� ���� ���� ��

				XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, Collides[secondCollide].GetObb().Center));
				resultSlidingVector.y = 0.0f;
				float dotRes = Vector3::DotProduct(resultSlidingVector, moveDirection);
				if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);
				collideNormalVector1 = Vector3::ScalarProduct(collideNormalVector1, 0.3f * normalVectorDotProductResult1, false);
				collideNormalVector2 = Vector3::ScalarProduct(collideNormalVector2, 0.3f * normalVectorDotProductResult2, false);
				return std::pair<bool, XMFLOAT3>(true, Vector3::Add(resultSlidingVector, Vector3::Add(collideNormalVector1, collideNormalVector2)));
			}
		}
	}
	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
}

std::pair<bool, XMFLOAT3> SmallMonsterSessionObject::CheckCollisionMap_Stage(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);

	std::vector<MapCollide>& Collides = g_stage1MapData.GetCollideData();

	for (auto& collide : Collides) {
		if (collide.GetObb().Intersects(m_SPBB)) {

			auto& relationIdxsVector = collide.GetRelationCollisionIdxs();
			int secondCollide = -1;
			for (auto& otherCol : relationIdxsVector) {
				if (Collides[otherCol].GetObb().Intersects(m_SPBB)) {
					secondCollide = otherCol;
					break;
				}
			}
			if (secondCollide == -1) {//m_SPBB								
				auto CollidePolygonNormalVector = collide.CalSlidingVector(m_SPBB, m_position, moveDirection);//�븻, �����̵�, �븻�� �������� ũ�� �� ��ȯ
				XMFLOAT3 collideNormalVector = std::get<0>(CollidePolygonNormalVector);//�븻 ����
				XMFLOAT3 collideSlidingVector = std::get<1>(CollidePolygonNormalVector);//�����̵� ����
				float normalVectorDotProductReslut = std::get<2>(CollidePolygonNormalVector);
				float slidingVectorDotProductReslut = std::get<3>(CollidePolygonNormalVector);//�����̵� ���Ϳ� ���� ���� ���� ��				
				collideSlidingVector = Vector3::ScalarProduct(collideSlidingVector, slidingVectorDotProductReslut, false);
				collideNormalVector = Vector3::ScalarProduct(collideNormalVector, 0.06f * normalVectorDotProductReslut, false);
				return std::pair<bool, XMFLOAT3>(true, Vector3::Add(collideSlidingVector, collideNormalVector));
			}
			else {
				auto CollidePolygonNormalVector1 = collide.CalSlidingVector(m_SPBB, m_position, moveDirection);//�븻, �����̵�, �븻�� �������� ũ�� �� ��ȯ
				XMFLOAT3 collideNormalVector1 = std::get<0>(CollidePolygonNormalVector1);//�븻 ����
				//XMFLOAT3 collideSlidingVector1 = std::get<1>(CollidePolygonNormalVector1);//�����̵� ����
				float normalVectorDotProductResult1 = std::get<2>(CollidePolygonNormalVector1);//�븻 ���Ͱ� �������� ũ��(�浹 ��ġ ������ ����)
				float slidingVectorDotProductResult1 = std::get<3>(CollidePolygonNormalVector1);//�����̵� ���Ϳ� �� ���� ���� ��				

				auto CollidePolygonNormalVector2 = Collides[secondCollide].CalSlidingVector(m_SPBB, m_position, moveDirection);//�븻, �����̵�, �븻�� �������� ũ�� �� ��ȯ
				XMFLOAT3 collideNormalVector2 = std::get<0>(CollidePolygonNormalVector2);//�븻 ����
				//XMFLOAT3 collideSlidingVector2 = std::get<1>(CollidePolygonNormalVector2);//�����̵� ����
				float normalVectorDotProductResult2 = std::get<2>(CollidePolygonNormalVector2);//�븻 ���Ͱ� �������� ũ��(�浹 ��ġ ������ ����)
				float slidingVectorDotProductResult2 = std::get<3>(CollidePolygonNormalVector2);//�����̵� ���Ϳ� �� ���� ���� ��

				XMFLOAT3 resultSlidingVector = Vector3::Normalize(Vector3::Subtract(collide.GetObb().Center, Collides[secondCollide].GetObb().Center));
				resultSlidingVector.y = 0.0f;
				float dotRes = Vector3::DotProduct(resultSlidingVector, moveDirection);
				if (dotRes < 0)resultSlidingVector = Vector3::ScalarProduct(resultSlidingVector, -1.0f, false);
				collideNormalVector1 = Vector3::ScalarProduct(collideNormalVector1, 0.3f * normalVectorDotProductResult1, false);
				collideNormalVector2 = Vector3::ScalarProduct(collideNormalVector2, 0.3f * normalVectorDotProductResult2, false);
				return std::pair<bool, XMFLOAT3>(true, Vector3::Add(resultSlidingVector, Vector3::Add(collideNormalVector1, collideNormalVector2)));
			}
		}
	}
	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
}

std::pair<bool, XMFLOAT3> SmallMonsterSessionObject::CheckCollisionCharacter(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	int collideCnt = 0;
	std::vector<std::pair<XMFLOAT3, XMFLOAT3> >  collideCharacterData;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	auto playerMap = roomRef.GetPlayerMap();
	for (auto& playCharacter : roomRef.GetPlayCharacters()) {
		if (!playerMap.count(playCharacter.first))continue;
		auto normalVecRes = GetNormalVectorSphere(playCharacter.second->GetPos());
		if (normalVecRes.first <= m_SPBB.Radius + 8.0f) {
			normalVecRes.second;
			collideCnt++;
			XMFLOAT3 normalVec = normalVecRes.second;
			XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
			float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, moveDirection);
			if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
			collideCharacterData.emplace_back(normalVec, slidingVec);
		}
	}
	if (collideCnt == 0)
		return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));
	if (collideCnt == 3) {
		return std::pair<bool, XMFLOAT3>(true, XMFLOAT3(0, 0, 0));
	}
	XMFLOAT3 resultNormal = XMFLOAT3(0, 0, 0);
	XMFLOAT3 resultSliding = XMFLOAT3(0, 0, 0);
	for (auto& character : collideCharacterData) {
		resultNormal = Vector3::Add(resultNormal, character.first);
		resultSliding = Vector3::Add(resultSliding, character.second);
	}
	resultNormal = Vector3::Normalize(resultNormal);
	resultSliding = Vector3::Normalize(resultSliding);
	resultNormal = Vector3::ScalarProduct(resultNormal, 0.5f, false);
	return std::pair<bool, XMFLOAT3>(true, Vector3::Add(resultNormal, resultSliding));
}

std::pair<bool, XMFLOAT3> SmallMonsterSessionObject::CheckCollisionNormalMonster(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	SmallMonsterSessionObject* monsterArr = roomRef.GetStageMonsterArr();
	std::vector < std::pair<XMFLOAT3, XMFLOAT3> > myCollideData_NPC;
	myCollideData_NPC.reserve(4);

	bool isCollideNPC = false;
	int collideCnt = 0;
	for (int i = 0; i < 15; i++) {
		if (!monsterArr[i].m_isAlive)continue;
		auto normalVecRes = GetNormalVectorSphere(monsterArr[i].GetPos());
		if (i == m_id)continue;
		if (normalVecRes.first >= m_SPBB.Radius + 8.0f)continue;
		if (collideCnt == 3) {
			return std::pair<bool, XMFLOAT3>(true, XMFLOAT3(0, 0, 0));
		}
		XMFLOAT3 normalVec = normalVecRes.second;
		XMFLOAT3 slidingVec = XMFLOAT3(-normalVec.z, 0.0f, normalVec.x);
		float directionVectorDotslidingVec = Vector3::DotProduct(slidingVec, GetLook());
		if (directionVectorDotslidingVec < 0)slidingVec = Vector3::ScalarProduct(slidingVec, -1.0f, false);
		myCollideData_NPC.emplace(myCollideData_NPC.begin() + collideCnt, normalVec, slidingVec);
		collideCnt++;
		isCollideNPC = true;
	}
	if (collideCnt == 0)	return std::pair<bool, XMFLOAT3>(false, XMFLOAT3(0, 0, 0));

	XMFLOAT3 normalVecResult = XMFLOAT3(0, 0, 0);
	XMFLOAT3 slidingVecResult = XMFLOAT3(0, 0, 0);

	for (auto& collideResult : myCollideData_NPC) {
		normalVecResult = Vector3::Add(normalVecResult, collideResult.first);
		slidingVecResult = Vector3::Add(slidingVecResult, collideResult.second);
	}
	if (collideCnt) {
		normalVecResult = Vector3::Normalize(normalVecResult);
		normalVecResult = Vector3::ScalarProduct(normalVecResult, 0.3f);
		slidingVecResult = Vector3::Normalize(slidingVecResult);
	}
	XMFLOAT3 collideNPCMoveDir = Vector3::Normalize(Vector3::Add(normalVecResult, slidingVecResult));
	return std::pair<bool, XMFLOAT3>(true, collideNPCMoveDir);
}

bool SmallMonsterSessionObject::CheckCollision(XMFLOAT3& moveDirection, float ftimeElapsed)
{
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	std::pair<bool, XMFLOAT3> mapCollideResult;
	if (roomRef.GetRoomState() == ROOM_STATE::ROOM_STAGE1)//stage1�̶��
	{
		mapCollideResult = CheckCollisionMap_Stage(moveDirection, ftimeElapsed);
	}
	else
		mapCollideResult = CheckCollisionMap_Boss(moveDirection, ftimeElapsed);

	auto CharacterCollide = CheckCollisionCharacter(moveDirection, ftimeElapsed);
	if (CharacterCollide.first && std::abs(CharacterCollide.second.x) < DBL_EPSILON && std::abs(CharacterCollide.second.z) < DBL_EPSILON) {//ĳ���� �ݸ������� ���� �ƿ� ��������
#ifdef MONSTER_MOVE_LOG
		std::cout << "character no Move" << std::endl;
#endif
		return true;
}
	if (mapCollideResult.first) {//�ʿ� �浹 ��
		if (CharacterCollide.first) {//ĳ���Ͱ� �浹 ��
			float dotRes = Vector3::DotProduct(Vector3::Normalize(mapCollideResult.second), Vector3::Normalize(CharacterCollide.second));
			if (dotRes > 0.2f) {//�浹 ���Ͱ� ������ �����
				auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
				if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//�븻 ���� �ݸ������� ���� �ƿ� ��������
#ifdef MONSTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "normalMonster no Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
			}
				XMFLOAT3 moveDir = Vector3::Normalize(Vector3::Add(mapCollideResult.second, CharacterCollide.second));
				if (normalMonsterCollide.first) {//�븻 ���� �浹 ��
					float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), moveDir);
					if (dotRes > 0.2f) {//������ ����
						m_position = Vector3::Add(m_position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, moveDir)), m_speed * ftimeElapsed));
						SetPosition(m_position);
#ifdef MONSTER_MOVE_LOG
						PrintCurrentTime();
						std::cout << "normalMonster &char & map Move" << std::endl;
						std::cout << std::endl;
#endif
						return true;
					}
					else {//������ ���� ����
#ifdef MONSTER_MOVE_LOG
						PrintCurrentTime();
						std::cout << "normalMonster &char & map Move" << std::endl;
						std::cout << std::endl;
#endif
						return true;
					}
				}
				else {//�븻 ���Ϳ� �浹���� ����
					m_position = Vector3::Add(m_position, Vector3::ScalarProduct(Vector3::Normalize(moveDir), m_speed * ftimeElapsed));
					SetPosition(m_position);
#ifdef MONSTER_MOVE_LOG
					PrintCurrentTime();
					std::cout << "char & map Move" << std::endl;
					std::cout << std::endl;
#endif
					return true;
				}
			}
			else {//������ �� ����
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & char no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
		}
			return true;
	}
		//ĳ���Ͱ� �浹���� �ʾ����� �븻 ���� üũ
		auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
		if (normalMonsterCollide.first) {//�븻 ���Ϳ� �浹��
			if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//�븻 ���� �ݸ������� ���� �ƿ� ��������
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
		}
			float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), Vector3::Normalize(mapCollideResult.second));
			if (dotRes > 0.2f) {//�ʰ� �븻 ���� �浹 ���� ������ �����
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, mapCollideResult.second)), m_speed * ftimeElapsed));
				SetPosition(m_position);
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & monster Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			else {//�ƿ� �ٸ� -> ������ x
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "map & monster no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
		}
		else {//�븻 ���Ϳ� �浹���� ���� => �ʸ� �浹
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(mapCollideResult.second, m_speed * ftimeElapsed, false));
			SetPosition(m_position);
#ifdef MONSTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "map Move" << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
	}
	//�� �浹 ���� ����
	if (CharacterCollide.first) {//ĳ���� �� �浹
		auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
		if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//���� �ݸ������� ���� �ƿ� ��������
#ifdef MONSTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "monster no Move" << std::endl;
			std::cout << std::endl;
#endif
			return true;
	}
		if (normalMonsterCollide.first) {//�븻 ���Ϳ� �浹
			float dotRes = Vector3::DotProduct(Vector3::Normalize(normalMonsterCollide.second), Vector3::Normalize(CharacterCollide.second));
			if (dotRes > 0.2f) {//ĳ���� �븻 ���� ����
				m_position = Vector3::Add(m_position, Vector3::ScalarProduct(Vector3::Normalize(Vector3::Add(normalMonsterCollide.second, CharacterCollide.second)), m_speed * ftimeElapsed));
				SetPosition(m_position);
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster & character Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
			else {//���Ͱ� �޶� �������� ����
#ifdef MONSTER_MOVE_LOG
				PrintCurrentTime();
				std::cout << "monster & character no Move" << std::endl;
				std::cout << std::endl;
#endif
				return true;
			}
		}
		else {// �븻 ���Ϳ� �浹���� ���� -> ĳ���͸� �浹
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(CharacterCollide.second, m_speed * ftimeElapsed));
			SetPosition(m_position);
#ifdef MONSTER_MOVE_LOG
			PrintCurrentTime();
			std::cout << "character Move" << std::endl;
			std::cout << std::endl;
#endif
			return true;
		}
	}
	//ĳ���Ϳ� �浹���� ����
	auto normalMonsterCollide = CheckCollisionNormalMonster(moveDirection, ftimeElapsed);
	if (normalMonsterCollide.first && std::abs(normalMonsterCollide.second.x) < DBL_EPSILON && std::abs(normalMonsterCollide.second.z) < DBL_EPSILON) {//���� �ݸ������� ���� �ƿ� ��������
#ifdef MONSTER_MOVE_LOG
		PrintCurrentTime();
		std::cout << "monster no Move" << std::endl;
		std::cout << std::endl;
#endif
		return true;
	}
	if (normalMonsterCollide.first) {//�븻 ���Ϳ� �浹��
		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(normalMonsterCollide.second, m_speed * ftimeElapsed));
		SetPosition(m_position);
#ifdef MONSTER_MOVE_LOG
		PrintCurrentTime();
		std::cout << "monster Move" << std::endl;
		std::cout << std::endl;
#endif
		return true;
	}
	return false;
}
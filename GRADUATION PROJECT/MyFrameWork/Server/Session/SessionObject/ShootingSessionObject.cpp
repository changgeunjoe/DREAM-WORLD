#include "stdafx.h"
#include "ShootingSessionObject.h"
#include "MonsterSessionObject.h"
#include "../UserSession.h"
#include "../../Room/RoomManager.h"
#include "../../Room/MapData.h"
#include "../../IOCPNetwork/protocol/protocol.h"
#include "../../IOCPNetwork/IOCP/IOCPNetwork.h"

extern IOCPNetwork g_iocpNetwork;
extern RoomManager g_RoomManager;
extern MapData g_bossMapData;
extern MapData g_stage1MapData;

ShootingSessionObject::ShootingSessionObject() : SessionObject(4.0f)
{
	m_speed = 100.0f;
}

ShootingSessionObject::~ShootingSessionObject()
{

}

const DirectX::XMFLOAT3 ShootingSessionObject::GetPosition()
{
	return DirectX::XMFLOAT3();
}

const DirectX::XMFLOAT3 ShootingSessionObject::GetRotation()
{
	return DirectX::XMFLOAT3();
}

void ShootingSessionObject::SetSpeed(float speed)
{
	m_speed;
}

int ShootingSessionObject::DetectCollision(MonsterSessionObject* m_bossSession)
{
	//if (m_SPBB.Intersects(m_bossSession->m_SPBB))
	//{
	//	// 보스 체력 -
	//	std::cout << "충돌체크 완료" << std::endl;
	//	m_active = false;	// 추후 생명주기 관리에 사용
	//	return m_id;
	//}
	return -1;
}

void ShootingSessionObject::SetStart(XMFLOAT3& dir, XMFLOAT3& srcPos, float speed)//초당 이동 속도
{
	m_lastMoveTime = std::chrono::high_resolution_clock::now();
	m_active = true;
	m_position = srcPos;
	m_directionVector = dir;
	m_speed = speed;
	m_SPBB.Center = srcPos;
	CalcRightVector();
}

bool ShootingSessionObject::Move(float elapsedTime)
{
	if (!m_active)return true;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	SmallMonsterSessionObject* monsters = roomRef.GetStageMonsterArr();
	m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, elapsedTime * m_speed));
	m_SPBB.Center = m_position;
	m_distance += elapsedTime * m_speed;

	bool isMonsterCollide = false;
	for (int i = 0; i < 15; i++) {
		if (m_SPBB.Intersects(monsters[i].GetSpbb())) {
			monsters[i].AttackedHp(m_damage);
			SERVER_PACKET::ProjectileDamagePacket sendPacket;
			sendPacket.size = sizeof(SERVER_PACKET::ProjectileDamagePacket);

			if (m_OwnerRole == ROLE::PRIEST)
				sendPacket.type = SERVER_PACKET::MONSTER_DAMAGED_BALL;
			else if (m_isSkill)
				sendPacket.type = SERVER_PACKET::MONSTER_DAMAGED_ARROW_SKILL;
			else
				sendPacket.type = SERVER_PACKET::MONSTER_DAMAGED_ARROW;

			sendPacket.projectileId = m_id;
			sendPacket.position = m_position;
			sendPacket.damage = m_damage;
			ExpOver* postOver = new ExpOver(reinterpret_cast<char*>(&sendPacket));
			postOver->m_opCode = OP_PROJECTILE_ATTACK;
			PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postOver->m_overlap);
			isMonsterCollide = true;
			break;
		}
	}
	if (isMonsterCollide || m_distance > 250.0f) {
		m_active = false;
		if (m_OwnerRole == ROLE::PRIEST) {
			roomRef.PushRestBall(m_id);
		}
		else if (!m_isSkill) {
			roomRef.PushRestArrow(m_id);
		}
	}
	return true;
}
;
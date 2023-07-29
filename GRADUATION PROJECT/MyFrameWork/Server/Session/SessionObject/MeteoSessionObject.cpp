#include "stdafx.h"
#include "MeteoSessionObject.h"
#include "../../Room/RoomManager.h"
#include "../../IOCPNetwork/IOCP/IOCPNetwork.h"
#include "../../IOCPNetwork/protocol/protocol.h"

extern RoomManager g_RoomManager;
extern IOCPNetwork g_iocpNetwork;

MeteoSessionObject::MeteoSessionObject() :SessionObject(10.0f)
{
	m_position = XMFLOAT3(0, 150, 0);
	m_directionVector = XMFLOAT3(0, -1, 0);
}

MeteoSessionObject::~MeteoSessionObject()
{
}

void MeteoSessionObject::InitSetDamage()
{
	m_attackDamage = 200.0f;
}

void MeteoSessionObject::SetRoomId(int roomId)
{
	m_roomId = roomId;
}

void MeteoSessionObject::SetId(int id)
{
	m_id = id;
}

bool MeteoSessionObject::Move(float elapsedTime)
{
	if (!m_active)return true;
	Room& roomRef = g_RoomManager.GetRunningRoomRef(m_roomId);
	m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, elapsedTime * m_speed));
	m_SPBB.Center = m_position;

	auto playCharacters = roomRef.GetPlayCharacters();
	auto player = roomRef.GetPlayerMap();
	bool isDestroy = false;
	for (auto& pCharacter : playCharacters) {
		if (player.count(pCharacter.first)) {
			if (m_SPBB.Intersects(pCharacter.second->GetSpbb())) {
				pCharacter.second->AttackedHp(m_attackDamage);
				SERVER_PACKET::BossAttackPlayerPacket sendPacket;
				sendPacket.size = sizeof(SERVER_PACKET::BossAttackPlayerPacket);
				sendPacket.type = SERVER_PACKET::METEO_PLAYER_ATTACK;
				sendPacket.currentHp = pCharacter.second->GetHp();
				ExpOver* postOver = new ExpOver(reinterpret_cast<char*>(&sendPacket));
				postOver->m_opCode = OP_METEO_ATTACK_PLAYER;
				PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postOver->m_overlap);
				isDestroy = true;
			}

		}
	}
	if (isDestroy || m_position.y <= 10.0f) {
		SERVER_PACKET::DestroyedMeteoPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::DestroyedMeteoPacket);
		sendPacket.type = SERVER_PACKET::METEO_DESTROY;
		sendPacket.idx = m_id;
		ExpOver* postOver = new ExpOver(reinterpret_cast<char*>(&sendPacket));
		postOver->m_opCode = OP_DESTROY_METEO_DESTROY;
		PostQueuedCompletionStatus(g_iocpNetwork.GetIocpHandle(), 1, m_roomId, &postOver->m_overlap);
		m_active = false;
	}
}

void MeteoSessionObject::SetStart(float& speed, XMFLOAT3& srcPosition)
{
	m_position = srcPosition;
	m_SPBB.Center = m_position;
	m_speed = speed;
	m_lastMoveTime = std::chrono::high_resolution_clock::now();	
}

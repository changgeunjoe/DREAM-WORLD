#include "stdafx.h"
//#include "RoomManager.h"
//
//RoomManager::RoomManager()
//{
//	for (int i = 0; i < m_roomArr.size(); i++)
//		m_roomArr[i].SetRoomId(i);
//}
//
//RoomManager::~RoomManager()
//{
//}
//
//int RoomManager::GetNewRoomId()
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_currentLastRoomIdLock };
//		if (m_currentLastRoomId < MAX_USER / 4) {
//			std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//			m_runningRoomIdSet.insert(m_currentLastRoomId);
//			return m_currentLastRoomId++;
//		}
//	}
//	int restRoomId = -1;
//	m_restRoomId.try_pop(restRoomId);
//	std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//	m_runningRoomIdSet.insert(restRoomId);
//	return restRoomId;
//}
//
//Room& RoomManager::GetRunningRoomRef(int id)
//{
//	return m_roomArr[id];
//};
//
//void RoomManager::RoomDestroy(int roomId)
//{
//	m_roomArr[roomId].GameEnd();
//	std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//	m_runningRoomIdSet.erase(roomId);
//	m_restRoomId.push(roomId);
//}
//
//void RoomManager::BossFindPlayer(int roomId)
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//		if (!m_runningRoomIdSet.count(roomId)) return;
//	}
//	//m_roomArr[roomId].BossFindPlayer();
//}
//
//void RoomManager::ChangeBossState(int roomId)
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//		if (!m_runningRoomIdSet.count(roomId)) return;
//	}
//	//m_roomArr[roomId].ChangeBossState();
//}
//
//void RoomManager::UpdateGameStateForPlayer(int roomId)
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//		if (!m_runningRoomIdSet.count(roomId)) return;
//	}
//	;
//	/*m_roomArr[roomId].UpdateShieldData();
//	if (m_roomArr[roomId].GetRoomState() == ROOM_STAGE1) {
//		m_roomArr[roomId].UpdateGameStateForPlayer_STAGE1();
//		return;
//	}
//	m_roomArr[roomId].UpdateGameStateForPlayer_BOSS();*/
//}
//
//void RoomManager::UpdateSmallMonster(int roomId)
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//		if (!m_runningRoomIdSet.count(roomId)) return;
//	}
//	//m_roomArr[roomId].GetRoomState();
//	//m_roomArr[roomId].UpdateSmallMonster();
//}
//
//void RoomManager::BossAttackExecute(int roomId)
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//		if (!m_runningRoomIdSet.count(roomId)) return;
//	}
//	//m_roomArr[roomId].BossAttackExecute();
//}
//
//void RoomManager::HealPlayer(int roomId)
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//		if (!m_runningRoomIdSet.count(roomId)) return;
//	}
//	m_roomArr[roomId].HealPlayerCharacter();
//}
//
//void RoomManager::SetBarrier(int roomId)
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//		if (!m_runningRoomIdSet.count(roomId)) return;
//	}
//	m_roomArr[roomId].PutBarrierOnPlayer();
//}
//
//void RoomManager::SkyArrowAttack(int roomId)
//{
//	{
//		std::lock_guard<std::mutex> lg{ m_runningRoomSetLock };
//		if (!m_runningRoomIdSet.count(roomId)) return;
//	}
//	m_roomArr[roomId].ExecuteSkyArrow();
//}
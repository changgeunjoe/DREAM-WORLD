#include "stdafx.h"
#include "BossEvent.h"
#include "../../Network/ExpOver/ExpOver.h"
#include "../Network/IocpEvent/IocpEventManager.h"
#include "../Room/Room.h"
#include "../MapData/MapData.h"
#include "../Room/RoomEvent.h"

void IOCP::BossCalculateRoadEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	const auto& opCode = over->GetOpCode();
	if (opCode == IOCP_OP_CODE::OP_RESEARCH_ROAD) {
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr == roomRef) return;

		std::shared_ptr<NavMapData> navMapData = roomRef->GetBossMapData();
		auto roadRef = std::make_shared<std::list<XMFLOAT3>>(std::move(navMapData->GetAstarNode(m_startPosition, m_destination)));
		auto bossRoadEvent = std::make_shared<BossRoadSetEvent>(roomRef, roadRef);
		roomRef->InsertPrevUpdateEvent(std::static_pointer_cast<PrevUpdateEvent>(bossRoadEvent));
	}
	else {
		spdlog::warn("BossCalculateRoadEvent::Execute() - Invalid Op_Code: {}", static_cast<int>(opCode));
	}
}

void IOCP::BossCalculateRoadEvent::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	spdlog::warn("BossCalculateRoadEvent::Fail()");
}

void IOCP::BossAggroEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	const auto& opCode = over->GetOpCode();
	if (opCode == IOCP_OP_CODE::OP_FIND_PLAYER) {
		auto roomRef = m_roomWeakRef.lock();
		if (nullptr == roomRef) return;

		std::shared_ptr<NavMapData> navMapData = roomRef->GetBossMapData();
		auto roadRef = std::make_shared<std::list<XMFLOAT3>>(std::move(navMapData->GetAstarNode(m_startPosition, m_destination)));
		auto characterRef = m_aggroCharacter.lock();
		auto bossRoadEvent = std::make_shared<BossAggroSetEvent>(roomRef, roadRef, characterRef);
		roomRef->InsertPrevUpdateEvent(std::static_pointer_cast<PrevUpdateEvent>(bossRoadEvent));
	}
	else {
		spdlog::warn("BossCalculateRoadEvent::Execute() - Invalid Op_Code: {}", static_cast<int>(opCode));
	}
}

void IOCP::BossAggroEvent::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	spdlog::warn("BossCalculateRoadEvent::Fail()");
}

void TIMER::BossCalculateRoadEvent::Execute(HANDLE iocpHandle)
{
	auto roomRef = m_roomWeakRef.lock();
	if (nullptr == roomRef) return;
	auto bossEvent = std::make_shared<IOCP::BossCalculateRoadEvent>(roomRef, m_destination, m_startPosition);
	ExpOver* overlapped = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_RESEARCH_ROAD, std::static_pointer_cast<IOCP::EventBase>(bossEvent));
	PostQueuedCompletionStatus(iocpHandle, 1, 999, overlapped);
}

void TIMER::BossAggroEvent::Execute(HANDLE iocpHandle)
{
	auto roomRef = m_roomWeakRef.lock();
	auto characterRef = m_aggroCharacter.lock();
	if (nullptr == roomRef || nullptr == characterRef) return;
	auto bossEvent = std::make_shared<IOCP::BossAggroEvent>(roomRef, m_destination, m_startPosition, characterRef);
	ExpOver* overlapped = IocpEventManager::GetInstance().CreateExpOver(IOCP_OP_CODE::OP_FIND_PLAYER, std::static_pointer_cast<IOCP::EventBase>(bossEvent));
	PostQueuedCompletionStatus(iocpHandle, 1, 999, overlapped);
}

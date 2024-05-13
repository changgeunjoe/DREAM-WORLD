#include "stdafx.h"
#include "UserManager.h"
#include "../UserSession/UserSession.h"
#include "../UserSession/StressUserSession.h"



void Network::UserManager::Initialize()
{
	for (auto& userSession : m_UserSessions) {
		userSession = std::make_shared<DreamWorld::StressUserSession>();
		userSession->Initialize();
	}
	m_currentId = 0;
}

void Network::UserManager::ForceDisconnect(const int& id)
{
	m_UserSessions[id]->Disconnect();
}

void Network::UserManager::InsertDisconnectId(const int& id)
{
	m_restId.push(id);
}

std::shared_ptr<Network::UserSession> Network::UserManager::GetTryConnectUserSession()
{
	int validCientId = -1;
	if (m_currentId < DreamWorld::MAX_CLIENTS)
		validCientId = m_currentId++;
	else {
		bool isSuccess = m_restId.try_pop(validCientId);
		if (!isSuccess) return nullptr;
	}
	return m_UserSessions[validCientId];
}

void Network::UserManager::GetPointCloud(int& stageSize, int& bossSize, float*& stagePoints, float*& bossPoints)
{
	int stageCnt = 0;
	int bossCnt = 0;
	for (auto& userSession : m_UserSessions) {
		auto currentUserSession = std::static_pointer_cast<DreamWorld::StressUserSession>(userSession);
		if (!currentUserSession->IsActive())continue;
		if (ROLE::NONE_SELECT == currentUserSession->GetRole()) continue;
		float  x = currentUserSession->GetX();
		float z = currentUserSession->GetZ();
		if (abs(x) < 400 && abs(z) < 400) {
			bossCnt++;
			m_bossPointCloud[stageCnt * 2] = x;
			m_bossPointCloud[stageCnt * 2 + 1] = z;
		}
		else {
			m_stagePointCloud[stageCnt * 2] = x;
			m_stagePointCloud[stageCnt * 2 + 1] = z;
			stageCnt++;
		}
	}
	stageSize = stageCnt;
	bossSize = bossCnt;
	stagePoints = m_stagePointCloud;
	bossPoints = m_bossPointCloud;
}

void Network::UserManager::RunActiveClient(const int& connections)
{
	for (int i = 0; i < connections; ++i) {
		m_UserSessions[i]->SendPacketByState();
	}
}



#pragma once
#include "../../PCH/stdafx.h"
#include "../../SingletonBase.h"
#include "../NetworkModule/NetworkModule.h"

namespace DreamWorld {
	class StressUserSession;
}
namespace Network
{
	class UserSession;
	class UserManager : public SingletonBase<UserManager>
	{
		friend SingletonBase;

	public:
		void Initialize();
		void ForceDisconnect(const int& id);
		void InsertDisconnectId(const int& id);
		std::shared_ptr<UserSession> GetTryConnectUserSession();
		void GetPointCloud(int& stageSize, int& bossSize, float*& stagePoints, float*& bossPoints);
		void RunActiveClient(const int& connections);
	private:
		int m_currentId;
		tbb::concurrent_queue<int> m_restId;
		std::array < std::shared_ptr<DreamWorld::StressUserSession>, DreamWorld::MAX_CLIENTS> m_UserSessions;
		float m_stagePointCloud[DreamWorld::MAX_TEST * 2];
		float m_bossPointCloud[DreamWorld::MAX_TEST * 2];
	};
}


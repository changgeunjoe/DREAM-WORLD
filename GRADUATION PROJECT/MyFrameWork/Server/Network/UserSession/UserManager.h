#pragma once
#include "../../SingletonBase.h"
#include "UserSession.h"

namespace IOCP
{
	class Iocp;
}

class UserManager : public SingletonBase<UserManager>
{
	friend SingletonBase;
private:
	UserManager();
	~UserManager();

public:
	void Initialize();
	void RegisterIocp(std::shared_ptr<IOCP::Iocp>& iocpPtr);
	void RegistPlayer(SOCKET sock);

private:
	tbb::concurrent_queue<unsigned int> m_restId;
	std::atomic_uint m_currentMaxId;

	std::unordered_set<std::shared_ptr<UserSession>> m_lobbyUser;
	std::mutex m_lobbyUserLock;
	std::shared_ptr<IOCP::Iocp> iocpRef;
};

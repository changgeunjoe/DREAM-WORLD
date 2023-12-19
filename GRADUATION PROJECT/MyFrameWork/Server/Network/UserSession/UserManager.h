#pragma once
#include "../../SingletonBase.h"
#include "UserSession.h"

class UserManager : public SingletonBase<UserManager>
{
	friend SingletonBase;
private:
	UserManager();
	~UserManager();

	concurrency::concurrent_unordered_map<int, UserSession> m_userSession;
	Concurrency::concurrent_queue<int> m_restId;
	std::atomic_uint m_currentMaxId;
public:
	void Initialize();
	void AcceptPlayer(HANDLE& iocpHandle, SOCKET&& sock);
	void RecvPacket(const unsigned int& userId, const DWORD& ioSize);
};


#pragma once
#include "../../SingletonBase.h"
#include "UserSession.h"

class Iocp;
class UserManager : public SingletonBase<UserManager>
{
	friend SingletonBase;
private:
	UserManager();
	~UserManager();

public:
	void Initialize();
	void RegisterIocp(Iocp* iocpPtr);
	void AcceptPlayer(SOCKET&& sock);
	void RecvPacket(const unsigned int& userId, const DWORD& ioSize);

private:
	tbb::concurrent_unordered_map<unsigned int, UserSession> m_userSession;
	tbb::concurrent_queue<unsigned int> m_restId;
	std::atomic_uint m_currentMaxId;
	Iocp* iocp;
};

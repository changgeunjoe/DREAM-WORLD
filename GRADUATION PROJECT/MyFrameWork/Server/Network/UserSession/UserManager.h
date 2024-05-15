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
	void LobbyUserToDisconnectUser(std::shared_ptr<UserSession> lobbyUserSession);
	std::shared_ptr<UserSession> FindDisconnectUser(const std::wstring& name);
	void EraseDiconnectUser(const std::wstring& name);

private:
	tbb::concurrent_queue<unsigned int> m_restId;
	std::atomic_uint m_currentMaxId;

	std::unordered_set<std::shared_ptr<UserSession>> m_lobbyUser;
	std::mutex m_lobbyUserLock;

	std::unordered_map<std::wstring, std::shared_ptr<UserSession>> m_disconnectedUser;
	std::mutex m_diconnectedUserLock;

	std::shared_ptr<IOCP::Iocp> iocpRef;
};

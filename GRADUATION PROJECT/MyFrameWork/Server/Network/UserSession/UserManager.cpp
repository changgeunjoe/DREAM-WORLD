#include "stdafx.h"
#include "UserManager.h"
#include "../IOCP/Iocp.h"

UserManager::UserManager()
{
}

UserManager::~UserManager()
{
	spdlog::info("UserManager::~UserManager()");

}

void UserManager::Initialize()
{
	m_currentMaxId = 0;
	spdlog::info("UserManager::Initialize() - Initialize");
}

void UserManager::RegisterIocp(std::shared_ptr<IOCP::Iocp>& iocp)
{
	iocpRef = iocp;
}

void UserManager::RegistPlayer(SOCKET sock)
{
	while (true) {
		unsigned int playerId = 0;
		bool ableReuseId = m_restId.try_pop(playerId);

		//���� ���̵� ���ٸ�
		if (!ableReuseId) {
			//����� id�� ���� �� ���� �ִ� id �ֽ�ȭ
			//m_currentMaxId => atomic::operator++()�� ���������� atomic�ϰ� �����ϰ� return --_After;�ؼ� ���� ����
			playerId = m_currentMaxId++;
		}
		//���ο� UserSession�߰�
		auto newPlayerRef = std::make_shared<UserSession>(playerId, sock);
		//lobby User �߰�
		m_lobbyUserLock.lock();
		auto inserResult = m_lobbyUser.insert(newPlayerRef);
		m_lobbyUserLock.unlock();
		//�����ߴٸ� socket�� ����ϰ� return, �����ߴٸ� ��õ�(while-loop)
		if (inserResult.second) {
			//socket�� iocp�� ���
			iocpRef->RegistHandle(reinterpret_cast<HANDLE>(sock), playerId);
			newPlayerRef->StartRecv();
			return;
		}
	}
}

void UserManager::LobbyUserToDisconnectUser(std::shared_ptr<UserSession> lobbyUserSession)
{
	bool isErase = false;
	m_lobbyUserLock.lock();
	if (m_lobbyUser.count(lobbyUserSession)) {
		isErase = true;
		m_lobbyUser.erase(lobbyUserSession);
	}
	m_lobbyUserLock.unlock();

	if (!isErase) return;

	//m_diconnectedUserLock.lock();
	//m_disconnectedUser.try_emplace(lobbyUserSession->GetPlayerName(), lobbyUserSession);
	//m_diconnectedUserLock.unlock();
}

std::shared_ptr<UserSession> UserManager::FindDisconnectUser(const std::wstring& name)
{
	std::shared_ptr<UserSession> disconnectedUserSession = nullptr;
	m_diconnectedUserLock.lock();
	auto findIter = m_disconnectedUser.find(name);
	if (findIter != m_disconnectedUser.end()) {
		disconnectedUserSession = findIter->second;
		m_disconnectedUser.erase(findIter);
	}
	m_diconnectedUserLock.unlock();
	return disconnectedUserSession;
}

void UserManager::EraseDiconnectUser(const std::wstring& name)
{
	std::lock_guard<std::mutex> userLockGurad{m_lobbyUserLock};
	auto findIter = m_disconnectedUser.find(name);
	if (findIter != m_disconnectedUser.end()) {
		m_disconnectedUser.erase(findIter);
	}
}

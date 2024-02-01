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
	for (int i = 0; i < 2/*MAX_USER*/; ++i) {
		m_userSession.insert(std::make_pair(i, UserSession(i)));
		m_restId.push(i);
	}
	m_currentMaxId = MAX_USER;
	spdlog::info("UserManager::Initialize() - Initialize");
}

void UserManager::RegisterIocp(Iocp* iocpPtr)
{
	iocp = iocpPtr;
}

void UserManager::AcceptPlayer(SOCKET&& sock)
{
	while (true) {
		unsigned int playerId = 0;
		bool ableReuseId = m_restId.try_pop(playerId);
		bool isSuccess = true;

		//���� ���̵� ���ٸ�
		if (!ableReuseId) {
			//����� id�� ���� �� ���� �ִ� id �ֽ�ȭ
			playerId = m_currentMaxId++;
			//���ο� UserSession�߰�
			auto inserResult = m_userSession.insert(std::make_pair(playerId, UserSession(playerId)));
			//���� ����
			isSuccess = inserResult.second;
#ifdef _DEBUG
			spdlog::debug("UserManager::AcceptPlayer() - reuseId: {0:d}", playerId);
#endif // _DEBUG
		}
#ifdef _DEBUG
		if (ableReuseId) {
			spdlog::debug("UserManager::AcceptPlayer() - newId: {0:d}", playerId);
		}
#endif // _DEBUG

		//�����ߴٸ� socket�� ����ϰ� return, �����ߴٸ� ��õ�(while-loop)
		if (isSuccess) {
			iocp->RegistHandle(reinterpret_cast<HANDLE>(sock), playerId);
			m_userSession[playerId].RegistSocket(std::move(sock));
			return;
		}
	}
}

void UserManager::RecvPacket(const unsigned int& userId, const DWORD& ioSize)
{
	m_userSession[userId].ContructPacket(ioSize);
}

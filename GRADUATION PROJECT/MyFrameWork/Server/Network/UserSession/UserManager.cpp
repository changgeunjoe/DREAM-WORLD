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
		auto inserResult = m_lobbyUser.insert(std::make_pair(playerId, newPlayerRef));
		//�����ߴٸ� socket�� ����ϰ� return, �����ߴٸ� ��õ�(while-loop)
		if (inserResult) {
			//socket�� iocp�� ���
			iocpRef->RegistHandle(reinterpret_cast<HANDLE>(sock), playerId);
			newPlayerRef->StartRecv();
			return;
		}
	}
}

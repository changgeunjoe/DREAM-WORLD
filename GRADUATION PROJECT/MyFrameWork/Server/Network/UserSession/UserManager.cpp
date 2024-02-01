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

		//재사용 아이디가 없다면
		if (!ableReuseId) {
			//등록할 id를 저장 및 현재 최대 id 최신화
			playerId = m_currentMaxId++;
			//새로운 UserSession추가
			auto inserResult = m_userSession.insert(std::make_pair(playerId, UserSession(playerId)));
			//성공 여부
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

		//성공했다면 socket을 등록하고 return, 실패했다면 재시도(while-loop)
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

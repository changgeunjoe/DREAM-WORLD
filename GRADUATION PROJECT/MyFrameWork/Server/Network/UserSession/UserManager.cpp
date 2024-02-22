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

		//재사용 아이디가 없다면
		if (!ableReuseId) {
			//등록할 id를 저장 및 현재 최대 id 최신화
			//m_currentMaxId => atomic::operator++()이 내부적으로 atomic하게 연산하고서 return --_After;해서 문제 없음
			playerId = m_currentMaxId++;
		}
		//새로운 UserSession추가
		auto newPlayerRef = std::make_shared<UserSession>(playerId, sock);
		//lobby User 추가
		auto inserResult = m_lobbyUser.insert(std::make_pair(playerId, newPlayerRef));
		//성공했다면 socket을 등록하고 return, 실패했다면 재시도(while-loop)
		if (inserResult) {
			//socket을 iocp에 등록
			iocpRef->RegistHandle(reinterpret_cast<HANDLE>(sock), playerId);
			newPlayerRef->StartRecv();
			return;
		}
	}
}

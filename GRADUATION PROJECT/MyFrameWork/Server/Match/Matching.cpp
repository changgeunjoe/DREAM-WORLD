#include "stdafx.h"
#include "Matching.h"
#include "../ThreadManager/ThreadManager.h"
#include "../Network/UserSession/UserSession.h"
#include "../Room/RoomManager.h"
#include "../Room/Room.h"

#define ALONE_TEST
//테스트할 때, 한 게임당 들어올 인원 수
#define TEST_MODE_PEOPLE 2

void Matching::InserMatch(std::shared_ptr<UserSession>& userRef, const ROLE& role)
{
#ifdef ALONE_TEST
	spdlog::debug("Matching::InserMatch() - Alone Test Mode");
	userRef->SetIngameRole(role);
	auto roomRef = RoomManager::GetInstance().MakeRunningRoomAloneMode(userRef);
	roomRef->InitializeAllGameObject();
	userRef->SetIngameRef(roomRef, roomRef->GetCharacterObject(role));
	roomRef->Start();
#else
	switch (role)
	{
	case ROLE::WARRIOR:
		m_warriorQueue.InsertMatchUser(userRef);
		break;
	case ROLE::MAGE:
		m_mageQueue.InsertMatchUser(userRef);
		break;
	case ROLE::TANKER:
		m_tankerQueue.InsertMatchUser(userRef);
		break;
	case ROLE::ARCHER:
		m_archerQueue.InsertMatchUser(userRef);
		break;
	default:
		spdlog::warn("Matching::InserMatch() - Undefined Role");
		break;
	}
#endif // ALONE_TEST
}

void Matching::CancelMatch(std::shared_ptr<UserSession>& userRef, const ROLE& role)
{
	switch (role)
	{
	case ROLE::WARRIOR:
		m_warriorQueue.InsertCancelUser(userRef);
		break;
	case ROLE::MAGE:
		m_mageQueue.InsertCancelUser(userRef);
		break;
	case ROLE::TANKER:
		m_tankerQueue.InsertCancelUser(userRef);
		break;
	case ROLE::ARCHER:
		m_archerQueue.InsertCancelUser(userRef);
		break;
	default:
		spdlog::warn("Matching::CacelMatch() - Undefined Role");
		break;
	}
}

void Matching::MatchFunc()
{
	static constexpr BYTE matchSuccessCondition = static_cast<BYTE>(ROLE::WARRIOR) |
		static_cast<BYTE>(ROLE::MAGE) |
		static_cast<BYTE>(ROLE::TANKER) |
		static_cast<BYTE>(ROLE::ARCHER);

	std::vector<std::shared_ptr<UserSession>> userRefVec;
	userRefVec.reserve(4);
	while (true) {
		BYTE matchState = 0;
		int currentMatchedUserCnt = 0;

		auto warriorUserRef = m_lastWarriorUser.lock();
		warriorUserRef = GetCurrentMatchUserSession(m_warriorQueue, warriorUserRef);
		if (nullptr != warriorUserRef) {
			matchState |= static_cast<BYTE>(ROLE::WARRIOR);
			userRefVec.emplace_back(warriorUserRef);
			++currentMatchedUserCnt;
		}

		auto mageUserRef = m_lastMageUser.lock();
		mageUserRef = GetCurrentMatchUserSession(m_mageQueue, mageUserRef);
		if (nullptr != mageUserRef) {
			matchState |= static_cast<BYTE>(ROLE::MAGE);
			userRefVec.emplace_back(mageUserRef);
			++currentMatchedUserCnt;
		}

		auto tankerUserRef = m_lastTankerUser.lock();
		tankerUserRef = GetCurrentMatchUserSession(m_tankerQueue, tankerUserRef);
		if (nullptr != tankerUserRef) {
			matchState |= static_cast<BYTE>(ROLE::TANKER);
			userRefVec.emplace_back(tankerUserRef);
			++currentMatchedUserCnt;
		}

		auto archerUserRef = m_lastArcherUser.lock();
		archerUserRef = GetCurrentMatchUserSession(m_archerQueue, archerUserRef);
		if (nullptr != archerUserRef) {
			matchState |= static_cast<BYTE>(ROLE::ARCHER);
			userRefVec.emplace_back(archerUserRef);
			++currentMatchedUserCnt;
		}

#ifdef TEST_MODE_PEOPLE
		if (TEST_MODE_PEOPLE == currentMatchedUserCnt) {
			if (nullptr != warriorUserRef) {
				warriorUserRef->SetIngameRole(ROLE::WARRIOR);
				m_lastWarriorUser.reset();
			}
			if (nullptr != mageUserRef) {
				mageUserRef->SetIngameRole(ROLE::MAGE);
				m_lastMageUser.reset();
			}
			if (nullptr != tankerUserRef) {
				tankerUserRef->SetIngameRole(ROLE::TANKER);
				m_lastTankerUser.reset();
			}
			if (nullptr != archerUserRef) {
				archerUserRef->SetIngameRole(ROLE::ARCHER);
				m_lastArcherUser.reset();
			}

			auto roomRef = RoomManager::GetInstance().MakeRunningRoom(userRefVec);
			roomRef->InitializeAllGameObject();
			for (auto& userRef : userRefVec) {
				if (nullptr != userRef)
					userRef->SetIngameRef(roomRef, roomRef->GetCharacterObject(userRef->GetIngameRole()));
			}
			roomRef->Start();

			userRefVec.clear();
		}
		else {
			if (nullptr != warriorUserRef)
				m_lastWarriorUser = warriorUserRef;
			if (nullptr != mageUserRef)
				m_lastMageUser = mageUserRef;
			if (nullptr != tankerUserRef)
				m_lastTankerUser = tankerUserRef;
			if (nullptr != archerUserRef)
				m_lastArcherUser = archerUserRef;
			userRefVec.clear();
			std::this_thread::yield();
		}
#else
		if ((matchSuccessCondition & matchState) == matchSuccessCondition) {
			//match Success
			warriorUserRef->SetIngameRole(ROLE::WARRIOR);
			mageUserRef->SetIngameRole(ROLE::MAGE);
			tankerUserRef->SetIngameRole(ROLE::TANKER);
			archerUserRef->SetIngameRole(ROLE::ARCHER);

			auto roomRef = RoomManager::GetInstance().MakeRunningRoom(userRefVec);
			roomRef->InitializeAllGameObject();
			for (auto& userRef : userRefVec)
				userRef->SetIngameRef(roomRef, roomRef->GetCharacterObject(userRef->GetIngameRole()));
			roomRef->Start();
			m_lastWarriorUser.reset();
			m_lastMageUser.reset();
			m_lastTankerUser.reset();
			m_lastArcherUser.reset();
			userRefVec.clear();
		}
		else {
			m_lastWarriorUser = warriorUserRef;
			m_lastMageUser = mageUserRef;
			m_lastTankerUser = tankerUserRef;
			m_lastArcherUser = archerUserRef;
			userRefVec.clear();
			std::this_thread::yield();
		}
#endif // TEST_MODE_PEOPLE
	}
}

std::shared_ptr<UserSession> Matching::GetCurrentMatchUserSession(MatchQueue& matchQueue, std::shared_ptr<UserSession> lastUserSession)
{
	std::shared_ptr<UserSession> returnUserSession = nullptr;
	returnUserSession = lastUserSession;
	while (true) {
		if (nullptr != returnUserSession) {
			//UserSession이 있지만 disconnect 됐다면, 
			if (UserSession::CONNECT_STATE::DISCONNECTED == returnUserSession->GetConnectState()) {
				returnUserSession = nullptr;
				continue;
			}
			return returnUserSession;
		}
		else {
			auto frontUserSession = matchQueue.GetMatchUser();
			if (frontUserSession.has_value()) {
				returnUserSession = frontUserSession.value();
				continue;
			}
			else return nullptr;
		}
	}
	return nullptr;
}

void Matching::StartMatching()
{
	m_lastWarriorUser.reset();
	m_lastMageUser.reset();
	m_lastTankerUser.reset();
	m_lastArcherUser.reset();
#ifndef ALONE_TEST
	spdlog::info("Matching::StartMatching() - Matching Thread Start");
	ThreadManager::GetInstance().CreateThread(std::thread([this]() {MatchFunc(); }));

#else
	spdlog::info("Matching::StartMatching() - AloneTestMode, Matching Thread do not run");
#endif
}

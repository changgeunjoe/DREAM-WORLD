#include "stdafx.h"
#include "Matching.h"
#include "../ThreadManager/ThreadManager.h"
#include "../Network/UserSession/UserSession.h"
#include "../Room/RoomManager.h"
#include "../Room/Room.h"

#define ALONE_TEST
//테스트할 때, 한 게임당 들어올 인원 수
//#define TEST_MODE_PEOPLE 3

void Matching::InserMatch(std::shared_ptr<UserSession>& userRef, const ROLE& role)
{
#ifdef ALONE_TEST
	spdlog::debug("Matching::InserMatch() - Alone Test Mode");
	userRef->SetRole(role);
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

	std::vector<std::shared_ptr<UserSession>> userRefVec(4);
	while (true) {
		BYTE matchState = 0;
		int currentMatchedUserCnt = 0;
		auto warriorUserRef = m_lastWarriorUser.lock();
		if (nullptr != warriorUserRef) {
			matchState |= static_cast<BYTE>(ROLE::WARRIOR);
			userRefVec.emplace_back(warriorUserRef);
			++currentMatchedUserCnt;
		}
		else {
			auto warriorResult = m_warriorQueue.GetMatchUser();
			if (warriorResult.has_value()) {
				warriorUserRef = warriorResult.value();
				matchState |= static_cast<BYTE>(ROLE::WARRIOR);
				userRefVec.emplace_back(warriorUserRef);
				++currentMatchedUserCnt;
			}
		}

		auto mageUserRef = m_lastMageUser.lock();
		if (nullptr != mageUserRef) {
			matchState |= static_cast<BYTE>(ROLE::MAGE);
			userRefVec.emplace_back(mageUserRef);
			++currentMatchedUserCnt;
		}
		else {
			auto mageResult = m_mageQueue.GetMatchUser();
			if (mageResult.has_value()) {
				mageUserRef = mageResult.value();
				matchState |= static_cast<BYTE>(ROLE::MAGE);
				userRefVec.emplace_back(mageUserRef);
				++currentMatchedUserCnt;
			}
		}

		auto tankerUserRef = m_lastWarriorUser.lock();
		if (nullptr != tankerUserRef) {
			matchState |= static_cast<BYTE>(ROLE::TANKER);
			userRefVec.emplace_back(tankerUserRef);
			++currentMatchedUserCnt;
		}
		else {
			auto tankerResult = m_tankerQueue.GetMatchUser();
			if (tankerResult.has_value()) {
				tankerUserRef = tankerResult.value();
				matchState |= static_cast<BYTE>(ROLE::TANKER);
				userRefVec.emplace_back(tankerUserRef);
				++currentMatchedUserCnt;
			}
		}

		auto archerUserRef = m_lastWarriorUser.lock();
		if (nullptr != archerUserRef) {
			matchState |= static_cast<BYTE>(ROLE::ARCHER);
			userRefVec.emplace_back(archerUserRef);
			++currentMatchedUserCnt;
		}
		else {
			auto archerResult = m_archerQueue.GetMatchUser();
			if (archerResult.has_value()) {
				archerUserRef = archerResult.value();
				matchState |= static_cast<BYTE>(ROLE::ARCHER);
				userRefVec.emplace_back(archerUserRef);
				++currentMatchedUserCnt;
			}
		}

#ifdef TEST_MODE_PEOPLE
		if (TEST_MODE_PEOPLE == currentMatchedUserCnt) {
			if (nullptr != warriorUserRef) {
				warriorUserRef->SetRole(ROLE::WARRIOR);
				m_lastWarriorUser.reset();
			}
			if (nullptr != mageUserRef) {
				mageUserRef->SetRole(ROLE::MAGE);
				m_lastMageUser.reset();
			}
			if (nullptr != tankerUserRef) {
				tankerUserRef->SetRole(ROLE::TANKER);
				m_lastTankerUser.reset();
			}
			if (nullptr != archerUserRef) {
				archerUserRef->SetRole(ROLE::ARCHER);
				m_lastArcherUser.reset();
			}

			auto roomRef = RoomManager::GetInstance().MakeRunningRoom(userRefVec);
			roomRef->InitializeAllGameObject();
			for (auto& userRef : userRefVec) {
				userRef->SetIngameRef(roomRef, roomRef->GetCharacterObject(userRef->GetRole()));
			}
			roomRef->Start();

			userRefVec.clear();
		}
		continue;
#endif // TEST_MODE_PEOPLE


		if ((matchSuccessCondition & matchState) == matchState) {
			//match Success
			warriorUserRef->SetRole(ROLE::WARRIOR);
			mageUserRef->SetRole(ROLE::MAGE);
			tankerUserRef->SetRole(ROLE::TANKER);
			archerUserRef->SetRole(ROLE::ARCHER);

			auto roomRef = RoomManager::GetInstance().MakeRunningRoom(userRefVec);
			for (auto& userRef : userRefVec)
				userRef->SetIngameRef(roomRef, roomRef->GetCharacterObject(userRef->GetRole()));
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
		}
	}
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

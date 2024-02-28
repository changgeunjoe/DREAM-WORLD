#include "stdafx.h"
#include "Matching.h"
#include "../Network/UserSession/UserSession.h"

#define ALONE_TEST

void Matching::InserMatch(std::shared_ptr<UserSession>& userRef, const ROLE& role)
{
#ifdef ALONE_TEST
	spdlog::debug("Matching::InserMatch() - Call");
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

	while (true) {
		BYTE matchState = 0;

		auto warriorUserRef = m_lastWarriorUser.lock();
		if (nullptr != warriorUserRef) {
			matchState |= static_cast<BYTE>(ROLE::WARRIOR);
		}
		else {
			auto warriorResult = m_warriorQueue.GetMatchUser();
			if (warriorResult.has_value()) {
				warriorUserRef = warriorResult.value();
				matchState |= static_cast<BYTE>(ROLE::WARRIOR);
			}
		}

		auto mageUserRef = m_lastMageUser.lock();
		if (nullptr != mageUserRef) {
			matchState |= static_cast<BYTE>(ROLE::MAGE);
		}
		else {
			auto mageResult = m_mageQueue.GetMatchUser();
			if (mageResult.has_value()) {
				mageUserRef = mageResult.value();
				matchState |= static_cast<BYTE>(ROLE::MAGE);
			}
		}

		auto tankerUserRef = m_lastWarriorUser.lock();
		if (nullptr != tankerUserRef) {
			matchState |= static_cast<BYTE>(ROLE::TANKER);
		}
		else {
			auto tankerResult = m_tankerQueue.GetMatchUser();
			if (tankerResult.has_value()) {
				tankerUserRef = tankerResult.value();
				matchState |= static_cast<BYTE>(ROLE::TANKER);
			}
		}

		auto archerUserRef = m_lastWarriorUser.lock();
		if (nullptr != archerUserRef) {
			matchState |= static_cast<BYTE>(ROLE::ARCHER);
		}
		else {
			auto archerResult = m_archerQueue.GetMatchUser();
			if (archerResult.has_value()) {
				archerUserRef = archerResult.value();
				matchState |= static_cast<BYTE>(ROLE::ARCHER);
			}
		}

		if (matchSuccessCondition == matchState) {
			//match Success
		}
		else {
			m_lastWarriorUser = warriorUserRef;
			m_lastMageUser = warriorUserRef;
			m_lastTankerUser = warriorUserRef;
			m_lastArcherUser = warriorUserRef;
		}
	}
}

void Matching::StartMatching()
{
	m_lastWarriorUser.reset();
	m_lastMageUser.reset();
	m_lastTankerUser.reset();
	m_lastArcherUser.reset();
	m_matchThread = std::jthread([this]() {MatchFunc(); });
	spdlog::info("Matching::Start() - Matching Thread Start");
}

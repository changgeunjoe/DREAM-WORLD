#include "stdafx.h"
#include "UserState.h"
#include "../../Network/UserSession/StressUserSession.h"
#include "../Character/CharacterObject.h"

void LobbyState::EnterState()
{
	m_userSession->SendMatchPacket();
	m_userSession->ChangeUserState(USER_STATE::MATCH);
}

InGameState::InGameState(std::shared_ptr<DreamWorld::StressUserSession> userSession) : UserState(userSession), m_currentRole(ROLE::NONE_SELECT)
{
}

void InGameState::EnterState()
{
	//MovePacket
	if (m_currentRole != m_userSession->GetRole()) {
		switch (m_userSession->GetRole())
		{
		case ROLE::WARRIOR:
			m_characterRef = std::make_shared<WarriorObject>();
			break;
		case ROLE::TANKER:
			m_characterRef = std::make_shared<TankerObject>();
			break;
		case ROLE::ARCHER:
			m_characterRef = std::make_shared<ArcherObject>();
			break;
		case ROLE::PRIEST:
			m_characterRef = std::make_shared<MageObject>();
			break;
		default:
			break;
		}
	}
	m_currentRole = m_userSession->GetRole();
	
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution changeBossStage(30, 30 * 3);
	//std::uniform_int_distribution changeBossStage(2, 3);

	int randomBoss = changeBossStage(dre);

	std::uniform_int_distribution gameEnd(randomBoss + 30, randomBoss + 60);
	int randomGameEnd = gameEnd(dre);

	std::chrono::seconds randomBossTime(randomBoss);
	std::chrono::seconds randomgameEndTime(randomGameEnd);

	m_isChangeBoss = false;
	m_changeBossTime = std::chrono::high_resolution_clock::now() + randomBossTime;
	m_isGameEnd = false;
	m_gameEndTime = std::chrono::high_resolution_clock::now() + randomgameEndTime;
	//누군가 다시 보내도 서버에서 무시 함.
}

void InGameState::UpdateState()
{
	if (ROLE::NONE_SELECT == m_userSession->GetRole()) {//Game End
		m_userSession->ChangeUserState(USER_STATE::LOBBY);
		return;
	}

	auto nowTime = std::chrono::high_resolution_clock::now();
	if (!m_isChangeBoss) {
		if (nowTime > m_changeBossTime) {
			m_userSession->SendStageChangeToBoss();
			m_isChangeBoss = true;
		}
	}
	if (!m_isGameEnd) {
		if (nowTime > m_gameEndTime) {
			m_userSession->SendForceGameEnd();
			m_isGameEnd = true;
		}
	}

	auto sendPacket = m_characterRef->GetAttackPacket();
	if (nullptr != sendPacket) {
		m_userSession->Send(sendPacket);
	}
	m_userSession->SendIngamePacket();
}

void InGameState::ExitState()
{
	m_currentRole = ROLE::NONE_SELECT;
	m_userSession->SendGameEndOkayPacket();
}

void MatchState::UpdateState()
{
	if (ROLE::NONE_SELECT != m_userSession->GetRole())
		m_userSession->ChangeUserState(USER_STATE::INGAME);
}

#pragma once
#include "../PCH/stdafx.h"

class UserSession;
class MatchQueue
{
public:
	MatchQueue();
	~MatchQueue() = default;
public:
	std::optional<std::shared_ptr<UserSession>> GetMatchUser();
	void InsertMatchUser(std::shared_ptr<UserSession>& userRef);
	void InsertCancelUser(std::shared_ptr<UserSession>& userRef);

private:
	//취소한 유저가 없을 때, weak_ptr nullptr이 없음
	std::optional<std::shared_ptr<UserSession>> GetCancelUser();
private:
	tbb::concurrent_queue<std::weak_ptr<UserSession>> m_match;
	tbb::concurrent_queue<std::weak_ptr<UserSession>> m_cancel;

	std::weak_ptr<UserSession> m_lastCancelUserWeakPtr;
};


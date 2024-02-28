#include "stdafx.h"
#include "MatchQueue.h"

MatchQueue::MatchQueue()
{
	m_lastCancelUserWeakPtr.reset();
}

std::optional<std::shared_ptr<UserSession>> MatchQueue::GetMatchUser()
{
	while (true)
	{
		std::weak_ptr<UserSession> matchUserWeakPtr;
		bool isSuccess = match.try_pop(matchUserWeakPtr);
		//pop실패
		if (!isSuccess)
			return std::nullopt;

		auto matchUserRef = matchUserWeakPtr.lock();
		//유저가 매칭하고서 나감, 큐에서 다음 유저 탐색
		if (nullptr == matchUserRef)
			continue;

		auto lastCancelUserRef = m_lastCancelUserWeakPtr.lock();
		if (nullptr != lastCancelUserRef) {
			//취소 유저와 매칭 유저가 같을 때는, 다음 유저 탐색
			if (lastCancelUserRef == matchUserRef) {
				lastCancelUserRef.reset();//매칭 취소 했으니 리셋
				continue;
			}
			//취소한 유저와 현재 메칭할 유저가 다를 때
			else return matchUserRef;
		}

		//이전 취소 유저가 없을때, 취소 유저 큐 확인
		auto cancelUserInfo = GetCancelUser();

		//취소 유저가 없음 -> 성공
		if (!cancelUserInfo.has_value())
			return matchUserRef;
		else {//취소 유저가 존재 함.
			auto cancelUserRef = cancelUserInfo.value();
			//현재 취소할 유저와 매칭 유저가 같음, 다음 유저 탐색
			if (cancelUserRef == matchUserRef) {
				continue;
			}
			//취소한 유저와 현재 메칭할 유저가 다를 때
			else {
				//현재 매칭 유저와 취소 유저가 다름, 취소할 유저 저장, 매칭 성공
				lastCancelUserRef = cancelUserRef;
				return matchUserRef;
			}
		}
	}
}

void MatchQueue::InsertMatchUser(std::shared_ptr<UserSession>& userRef)
{
	match.push(userRef);
}

void MatchQueue::InsertCancelUser(std::shared_ptr<UserSession>& userRef)
{
	cancel.push(userRef);
}

std::optional<std::shared_ptr<UserSession>>  MatchQueue::GetCancelUser()
{
	while (true) {
		std::weak_ptr<UserSession> cancelUserWeakPtr;
		bool existCacelUser = cancel.try_pop(cancelUserWeakPtr);
		if (!existCacelUser)
			return std::nullopt;
		auto userRef = cancelUserWeakPtr.lock();
		if (nullptr == userRef)
			continue;
		return userRef;
	}
}

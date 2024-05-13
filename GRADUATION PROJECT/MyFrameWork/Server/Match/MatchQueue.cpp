#include "stdafx.h"
#include "MatchQueue.h"

MatchQueue::MatchQueue()
{
	m_lastCancelUserWeakPtr.reset();
}

std::optional<std::shared_ptr<UserSession>> MatchQueue::GetMatchUser()
{
	/*
	match cancel은 로직 바꿔야 할듯
		cancel큐 순서 꼬이면 매우 곤란해짐 -> 현재 뽑은 대상의 role과 현재 원하는 role을 비교
		취소 패킷 날아오면 non_select되게하고, 다른거 매칭하면 그 role이 세팅되니까 다르면 다음 사람 보기로.
	*/

	while (true)
	{
		std::weak_ptr<UserSession> matchUserWeakPtr;
		bool isSuccess = m_match.try_pop(matchUserWeakPtr);
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
	m_match.push(userRef);
}

void MatchQueue::InsertCancelUser(std::shared_ptr<UserSession>& userRef)
{
	m_cancel.push(userRef);
}

std::optional<std::shared_ptr<UserSession>>  MatchQueue::GetCancelUser()
{
	while (true) {
		std::weak_ptr<UserSession> cancelUserWeakPtr;
		bool existCacelUser = m_cancel.try_pop(cancelUserWeakPtr);
		if (!existCacelUser)
			return std::nullopt;
		auto userRef = cancelUserWeakPtr.lock();
		if (nullptr == userRef)
			continue;
		return userRef;
	}
}

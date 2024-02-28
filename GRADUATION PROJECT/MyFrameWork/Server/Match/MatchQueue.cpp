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
		//pop����
		if (!isSuccess)
			return std::nullopt;

		auto matchUserRef = matchUserWeakPtr.lock();
		//������ ��Ī�ϰ� ����, ť���� ���� ���� Ž��
		if (nullptr == matchUserRef)
			continue;

		auto lastCancelUserRef = m_lastCancelUserWeakPtr.lock();
		if (nullptr != lastCancelUserRef) {
			//��� ������ ��Ī ������ ���� ����, ���� ���� Ž��
			if (lastCancelUserRef == matchUserRef) {
				lastCancelUserRef.reset();//��Ī ��� ������ ����
				continue;
			}
			//����� ������ ���� ��Ī�� ������ �ٸ� ��
			else return matchUserRef;
		}

		//���� ��� ������ ������, ��� ���� ť Ȯ��
		auto cancelUserInfo = GetCancelUser();

		//��� ������ ���� -> ����
		if (!cancelUserInfo.has_value())
			return matchUserRef;
		else {//��� ������ ���� ��.
			auto cancelUserRef = cancelUserInfo.value();
			//���� ����� ������ ��Ī ������ ����, ���� ���� Ž��
			if (cancelUserRef == matchUserRef) {
				continue;
			}
			//����� ������ ���� ��Ī�� ������ �ٸ� ��
			else {
				//���� ��Ī ������ ��� ������ �ٸ�, ����� ���� ����, ��Ī ����
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

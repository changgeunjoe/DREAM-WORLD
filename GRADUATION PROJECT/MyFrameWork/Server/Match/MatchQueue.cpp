#include "stdafx.h"
#include "MatchQueue.h"

MatchQueue::MatchQueue()
{
	m_lastCancelUserWeakPtr.reset();
}

std::optional<std::shared_ptr<UserSession>> MatchQueue::GetMatchUser()
{
	/*
	match cancel�� ���� �ٲ�� �ҵ�
		cancelť ���� ���̸� �ſ� ������� -> ���� ���� ����� role�� ���� ���ϴ� role�� ��
		��� ��Ŷ ���ƿ��� non_select�ǰ��ϰ�, �ٸ��� ��Ī�ϸ� �� role�� ���õǴϱ� �ٸ��� ���� ��� �����.
	*/

	while (true)
	{
		std::weak_ptr<UserSession> matchUserWeakPtr;
		bool isSuccess = m_match.try_pop(matchUserWeakPtr);
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

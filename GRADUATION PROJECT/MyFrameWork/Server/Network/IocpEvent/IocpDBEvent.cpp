#include "stdafx.h"
#include "IocpDBEvent.h"
#include "IocpEventManager.h"
#include "../ExpOver/ExpOver.h"
#include "../UserSession/UserSession.h"
#include "../protocol/protocol.h"

IOCP::DBNotifyEvent::DBNotifyEvent(std::shared_ptr<UserSession>& userRef) : m_userRef(userRef)
{
}

void IOCP::DBNotifyEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	auto userRef = m_userRef.lock();
	if (nullptr == userRef) {
		IocpEventManager::GetInstance().DeleteExpOver(over);
		return;
	}

	const auto& currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
		//DB - login Error, SQL_ERROR ...
	case IOCP_OP_CODE::OP_FAIL_GET_PLAYER_INFO:
	{
		SERVER_PACKET::NotifyPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		sendPacket.type = static_cast<char>(SERVER_PACKET::TYPE::LOGIN_FAIL);
		userRef->DoSend(&sendPacket);
	}
	break;
	case IOCP_OP_CODE::OP_DB_ERROR:
	{
		/*SERVER_PACKET::NotifyPacket sendPacket;
		sendPacket.size = sizeof(SERVER_PACKET::NotifyPacket);
		sendPacket.type = static_cast<char>(SERVER_PACKET::TYPE::LOGIN_FAIL);
		userRef->DoSend(&sendPacket);*/
	}
	break;
	default:
		spdlog::critical("DBNotifyEvent::Execute() - UnDefined OP_CODE - {}", static_cast<int>(currentOpCode));
		break;
	}
}

void IOCP::DBNotifyEvent::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	const auto& currentOpCode = over->GetOpCode();
	spdlog::critical("DBNotifyEvent::Fail() - OP_CODE: {}", static_cast<int>(currentOpCode));
}

IOCP::DBGetPlayerInfoEvent::DBGetPlayerInfoEvent(const wchar_t* name, std::shared_ptr<UserSession>& userRef) : m_userRef(userRef)
{
	//DB���� 20���ڷ� ���� �߱⶧���� ���� �߻� ���Ұŷ� ���� ��.
	HRESULT res = StringCchCopyW(m_buffer, NAME_SIZE, name);
	if (S_OK != res) {
		//Error
	}

}

IOCP::DBGetPlayerInfoEvent::DBGetPlayerInfoEvent(const std::wstring& name, std::shared_ptr<UserSession>& userRef) : m_userRef(userRef)
{
	HRESULT res = StringCchCopyW(m_buffer, NAME_SIZE, name.c_str());
	if (S_OK != res) {
		//Error
	}
}

void IOCP::DBGetPlayerInfoEvent::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	auto userRef = m_userRef.lock();
	//������ ���ٸ� ����
	if (nullptr == userRef) {
		IocpEventManager::GetInstance().DeleteExpOver(over);
		return;
	}
	const auto& currentOpCode = over->GetOpCode();
	switch (currentOpCode)
	{
	case IOCP_OP_CODE::OP_SUCCESS_GET_PLAYER_INFO:
	{
		SERVER_PACKET::LoginPacket sendPacket;
		sendPacket.type = static_cast<unsigned char>(SERVER_PACKET::TYPE::LOGIN_SUCCESS);
		sendPacket.size = sizeof(SERVER_PACKET::LoginPacket);
		m_buffer;
		int nameLen = lstrlenW(m_buffer);
		for (int removeSpace = nameLen - 1; removeSpace > -1; --removeSpace) {
			if (m_buffer[removeSpace] != L' ' && m_buffer[removeSpace] != 0) break;
			m_buffer[removeSpace] = 0;
		}
		StringCchCopyW(sendPacket.nickName, NAME_SIZE, m_buffer);
		//���� ������ �г��� ����
		userRef->LoginSucces(sendPacket.nickName);
		//�������� �α��� ���� ���� �۽�
		userRef->DoSend(&sendPacket);
		//userRef->
		//userSession�� �̸� ����
		//Ŭ���̾�Ʈ�� �̸� ����
	}
	break;
	default:
		spdlog::critical("DBGetPlayerInfoEvent::Execute() - UnDefined OP_CODE - {}", static_cast<int>(currentOpCode));
		break;
	}
	//expOver�� ��ȯ
	IocpEventManager::GetInstance().DeleteExpOver(over);
	//IOCP::EventBase�� shared_ptr��ü���� delete x
}

void IOCP::DBGetPlayerInfoEvent::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	const auto& currentOpCode = over->GetOpCode();
	spdlog::critical("DBGetPlayerInfoEvent::Fail() - OpCode: {}", static_cast<int>(currentOpCode));
}

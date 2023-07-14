#include "stdafx.h"
#include "UserSession.h"
#include "SessionObject/ChracterSessionObject.h"
#include "../Logic/Logic.h"

extern Logic g_logic;


UserSession::UserSession()
{
	m_id = -1;
	Initialize();
}

UserSession::~UserSession()
{
	m_id = -1;
	//if (m_sessionObject != nullptr)
	//	delete m_sessionObject;
	m_sessionObject = nullptr;
}

void UserSession::Initialize()
{
	m_playerState = PLAYER_STATE::FREE;
	m_sessionObject = nullptr;
	m_prevBufferSize = 0;
	ZeroMemory(&m_exOver.m_overlap, sizeof(m_exOver.m_overlap));
}

void UserSession::Recv()
{
	DWORD recv_flag = 0;
	memset(&m_exOver.m_overlap, 0, sizeof(m_exOver.m_overlap));
	m_exOver.m_wsaBuf.len = MAX_BUF_SIZE - m_prevBufferSize;
	m_exOver.m_wsaBuf.buf = m_exOver.m_buffer + m_prevBufferSize;
	int resRet = WSARecv(m_socket, &m_exOver.m_wsaBuf, 1, 0, &recv_flag, &m_exOver.m_overlap, 0);
	if (resRet)
		DisplayWsaGetLastError(WSAGetLastError());
}

void UserSession::Send(void* p)
{
	DWORD sendByte = 0;
	ExpOver* sendOverlap = new ExpOver(reinterpret_cast<char*>(p));
	//std::cout << "send: " << (int)sendOverlap->m_buffer[2] << std::endl;
	int resRet = WSASend(m_socket, &(sendOverlap->m_wsaBuf), 1, &sendByte, 0, &(sendOverlap->m_overlap), 0);
	if (resRet)
		DisplayWsaGetLastError(WSAGetLastError());
	//std::cout << "sendByte: " << sendByte << std::endl;
}

void UserSession::ConstructPacket(int ioByte)
{
	int remain_data = ioByte + m_prevBufferSize;
	char* p = m_exOver.m_buffer;
	while (remain_data > 1) {
		short packet_size;
		memcpy(&packet_size, p, 2);
		if ((int)packet_size <= remain_data) {
			g_logic.ProcessPacket(static_cast<int>(m_id), p);
			p = p + (int)packet_size;
			remain_data = remain_data - (int)packet_size;
		}
		else break;
	}
	m_prevBufferSize = remain_data;
	if (remain_data > 0) {
		std::memcpy(m_exOver.m_buffer, p, remain_data);
	}
	Recv();
}

void UserSession::RegistPlayer(SOCKET& sock, int id)
{
	m_socket = sock;
	m_id = id;
	Recv();
}

ChracterSessionObject* UserSession::SetPlaySessionObject(ChracterSessionObject* pSession)
{
	m_sessionObject = pSession;
	return m_sessionObject;
}

void UserSession::PlayCharacterMove()
{
	if (m_sessionObject != nullptr && m_sessionObject->m_inputDirection != DIRECTION::IDLE) {
		m_sessionObject->AutoMove();
	}
}

std::tuple<int, int, DirectX::XMFLOAT3, DirectX::XMFLOAT3> UserSession::GetPlayCharacterState()
{
	if (m_sessionObject == nullptr) return{ -1, -1, DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(0,0,0) };
	return { m_id, m_sessionObject->GetHp() ,m_sessionObject->GetPos(),m_sessionObject->GetRot() };
}

void UserSession::ResetPlayerToLobbyState()
{
	m_roomId = -1;
	{
		std::lock_guard < std::mutex>lg{ m_playerStateLock };
		m_playerState = PLAYER_STATE::IN_GAME;
	}
	//delete m_sessionObject;
	m_sessionObject = nullptr;
}

void UserSession::ResetSession()
{
	{
		std::lock_guard < std::mutex>lg{ m_playerStateLock };
		m_playerState = PLAYER_STATE::FREE;
	}
	//play Character UserSession
	//if (m_sessionObject != nullptr)
	//	delete m_sessionObject;
	m_sessionObject = nullptr;
	//player Info
	m_role = ROLE::NONE_SELECT;
	m_id = -1;
	m_roomId = -1;
	m_playerName.clear();
	//buffer
	m_prevBufferSize = 0;
	ZeroMemory(m_exOver.m_buffer, MAX_BUF_SIZE);
	//socket
	closesocket(m_socket);
	m_socket = NULL;
}
//
//void UserSession::ChangeDirectionPlayCharacter(DIRECTION d)
//{
//	m_sessionObject->ChangeDirection(d);
//}
//
//void UserSession::StopMovePlayCharacter()
//{
//	m_sessionObject->StopMove();
//}
//
//DirectX::XMFLOAT3 UserSession::GetPositionPlayCharacter()
//{
//	return m_sessionObject->GetPos();
//}
//
//bool UserSession::AdjustPlayCharacterInfo(DirectX::XMFLOAT3& postion)
//{
//	return m_sessionObject->AdjustPlayerInfo(postion);;
//}
//
//void UserSession::RotatePlayCharacter(ROTATE_AXIS axis, float& angle)
//{
//	m_sessionObject->Rotate(axis, angle);
//}
//
//void UserSession::StartMovePlayCharacter(DIRECTION d)
//{
//	m_sessionObject->StartMove(d);
//}
//
//void UserSession::SetMouseInputPlayCharacter(bool left, bool right)
//{
//	m_sessionObject->SetMouseInput(left, right);
//}
//
//bool UserSession::GetLeftAttackPlayCharacter()
//{
//	return m_sessionObject->GetLeftAttack();
//}
//
//short UserSession::GetAttackDamagePlayCharacter()
//{
//	return m_sessionObject->GetAttackDamage();
//}

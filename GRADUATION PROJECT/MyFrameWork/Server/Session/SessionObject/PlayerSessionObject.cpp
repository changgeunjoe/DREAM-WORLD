#include "stdafx.h"
#include "PlayerSessionObject.h"
#include "../Session.h"
#include "../../Logic/Logic.h"

extern  Logic g_logic;

PlayerSessionObject::PlayerSessionObject(Session* session, SOCKET& sock) : SessionObject(session), m_socket(sock)
{
	m_level = 0;
	m_exp = 0;
	m_hp = 0;
	m_maxHp = 0;
	m_attackDamage = 0;
	m_prevBufferSize = 0;
	ZeroMemory(&m_exOver.m_overlap, sizeof(m_exOver.m_overlap));
}

PlayerSessionObject::~PlayerSessionObject()
{
}

void PlayerSessionObject::Recv()
{
	DWORD recv_flag = 0;
	memset(&m_exOver.m_overlap, 0, sizeof(m_exOver.m_overlap));
	m_exOver.m_wsaBuf.len = MAX_BUF_SIZE - m_prevBufferSize;
	m_exOver.m_wsaBuf.buf = m_exOver.m_buffer + m_prevBufferSize;
	WSARecv(m_socket, &m_exOver.m_wsaBuf, 1, 0, &recv_flag, &m_exOver.m_overlap, 0);
}

void PlayerSessionObject::Send(void* p)
{
	ExpOver* sendOverlap = new ExpOver(reinterpret_cast<char*>(p));
	WSASend(m_socket, &sendOverlap->m_wsaBuf, 1, 0, 0, &sendOverlap->m_overlap, 0);
}

void PlayerSessionObject::ConstructPacket(int ioByte)
{
	int remain_data = ioByte + m_prevBufferSize;
	char* p = m_exOver.m_buffer;
	while (remain_data > 0) {
		int packet_size = p[0];
		if (packet_size <= remain_data) {
			g_logic.ProcessPacket(static_cast<int>(m_session->GetId()), p);
			p = p + packet_size;
			remain_data = remain_data - packet_size;
		}
		else break;
	}
	m_prevBufferSize = remain_data;
	if (remain_data > 0) {
		std::memcpy(m_exOver.m_buffer, p, remain_data);
	}
	Recv();
}

bool PlayerSessionObject::AdjustPlayerInfo(DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& rotate)
{
	m_rotateAngle = rotate;
	if (Vector3::Length(Vector3::Subtract(m_position, position)) < 100) { // 일정 값 미만이라면 문제 없음, 이상이라면 스피드 핵?으로 감지하고 위치 서버 위치로 전환
		m_position = position;
		return true;
	}
	return false;
}

void PlayerSessionObject::AutoMove()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto durationTime = std::chrono::duration<double>(currentTime - m_lastMoveTime);
	m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, (double)durationTime.count()));
	m_lastMoveTime = currentTime;
	std::cout << durationTime.count() << "elapsed" << "current Position" << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;
}

void PlayerSessionObject::StartMove()
{
	m_isMove = true;
	m_lastMoveTime = std::chrono::high_resolution_clock::now();

}

void PlayerSessionObject::StopMove()
{
	m_isMove = false;
}

const DirectX::XMFLOAT3 PlayerSessionObject::GetPosition()
{
	return m_position;
}

const DirectX::XMFLOAT3 PlayerSessionObject::GetRotation()
{
	return m_rotateAngle;
}

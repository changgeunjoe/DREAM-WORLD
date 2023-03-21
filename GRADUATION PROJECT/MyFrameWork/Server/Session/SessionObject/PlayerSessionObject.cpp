#include "stdafx.h"
#include "PlayerSessionObject.h"
#include "../Session.h"
#include "../../Logic/Logic.h"
#include "../../IOCPNetwork/protocol/protocol.h"

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
		short packet_size;
		memcpy(&packet_size, p, 2);
		if ((int)packet_size <= remain_data) {
			g_logic.ProcessPacket(static_cast<int>(m_session->GetId()), p);
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

bool PlayerSessionObject::AdjustPlayerInfo(DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& rotate)
{
	m_rotateAngle = rotate;
	if (Vector3::Length(Vector3::Subtract(m_position, position)) < 0.3f) { // 일정 값 미만이라면 문제 없음, 이상이라면 스피드 핵?으로 감지하고 위치 서버 위치로 전환
		m_position = position;
		return true;
	}
	return false;
}

void PlayerSessionObject::AutoMove()
{
	CalcRightVector();
	auto currentTime = std::chrono::high_resolution_clock::now();
	double durationTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastMoveTime).count();
	durationTime = (double)durationTime / 1000.0f;
	//if (DIRECTION::FRONT == (m_inputDirection & DIRECTION::FRONT)) {
	//	if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) {
	//		DirectX::XMFLOAT3 resDir = Vector3::Normalize(Vector3::Subtract(m_directionVector, m_rightVector));
	//		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resDir, ((double)durationTime / 1000.0f) * 50.0f));
	//		//std::cout << "diagonal dir: " << resDir.x << ", " << resDir.z << "distance: " << ((double)durationTime / 1000.0f) * 50.0f << std::endl;
	//		//std::cout << "LF" << std::endl;
	//	}
	//	else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT)) {
	//		DirectX::XMFLOAT3 resDir = Vector3::Normalize(Vector3::Add(m_directionVector, m_rightVector));
	//		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resDir, ((double)durationTime / 1000.0f) * 50.0f));
	//		//std::cout << "diagonal dir: " << resDir.x << ", " << resDir.z << std::endl;
	//		//std::cout << "RF" << std::endl;
	//	}
	//	else {
	//		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, ((double)durationTime / 1000.0f) * 50.0f));
	//		std::cout << "F" << std::endl;
	//	}
	//}
	//else if (DIRECTION::BACK == (m_inputDirection & DIRECTION::BACK)) {
	//	if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) {
	//		DirectX::XMFLOAT3 resDir = Vector3::Normalize(Vector3::Add(m_directionVector, m_rightVector));
	//		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resDir, ((double)durationTime / 1000.0f) * -50.0f));
	//		//std::cout << "diagonal dir: " << resDir.x << ", " << resDir.z << std::endl;
	//		//std::cout << "LB" << std::endl;
	//	}
	//	else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT)) {
	//		DirectX::XMFLOAT3 resDir = Vector3::Normalize(Vector3::Subtract(m_rightVector, m_directionVector));
	//		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(resDir, ((double)durationTime / 1000.0f) * 50.0f));
	//		//std::cout << "diagonal dir: " << resDir.x << ", " << resDir.z << std::endl;
	//		//std::cout << "RB" << std::endl;
	//	}
	//	else {
	//		m_position = Vector3::Subtract(m_position, Vector3::ScalarProduct(m_directionVector, ((double)durationTime / 1000.0f) * 50.0f));
	//		std::cout << "B" << std::endl;
	//	}
	//}
	//else {
	//	if (DIRECTION::LEFT == (m_inputDirection & DIRECTION::LEFT)) {
	//		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_rightVector, ((double)durationTime / 1000.0f) * -50.0f));
	//		std::cout << "L" << std::endl;
	//	}
	//	else if (DIRECTION::RIGHT == (m_inputDirection & DIRECTION::RIGHT)) {
	//		m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_rightVector, ((double)durationTime / 1000.0f) * 50.0f));
	//		std::cout << "R" << std::endl;
	//	}
	//}
	m_position = Vector3::Add(m_position, Vector3::ScalarProduct(m_directionVector, ((double)durationTime / 1000.0f) * 50.0f));
	m_lastMoveTime = currentTime;
	std::cout << "current Position " << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;
	std::cout << "rotate angle" << m_rotateAngle.x << " " << m_rotateAngle.y << " " << m_rotateAngle.z << std::endl;
}

void PlayerSessionObject::StartMove(DIRECTION d)
{
	if (m_inputDirection == DIRECTION::IDLE) {
		m_lastMoveTime = std::chrono::high_resolution_clock::now();
		m_inputDirection = d;

		std::cout << "prev Direction: ";
		if (m_prevDirection == DIRECTION::IDLE)
			std::cout << "IDLE" << std::endl;
		else if (m_prevDirection == DIRECTION::LEFT)
			std::cout << "LEFT" << std::endl;
		else if (m_prevDirection == DIRECTION::RIGHT)
			std::cout << "RIGHT" << std::endl;
		else if (m_prevDirection == DIRECTION::FRONT)
			std::cout << "FRONT" << std::endl;
		else if (m_prevDirection == DIRECTION::BACK)
			std::cout << "BACK" << std::endl;

		std::cout << "input Direction: ";
		if (d == DIRECTION::IDLE)
			std::cout << "IDLE" << std::endl;
		else if (d == DIRECTION::LEFT)
			std::cout << "LEFT" << std::endl;
		else if (d == DIRECTION::RIGHT)
			std::cout << "RIGHT" << std::endl;
		else if (d == DIRECTION::FRONT)
			std::cout << "FRONT" << std::endl;
		else if (d == DIRECTION::BACK)
			std::cout << "BACK" << std::endl;

		switch (d)
		{
		case DIRECTION::FRONT:
			switch (m_prevDirection)
			{
			case DIRECTION::IDLE:
			case DIRECTION::FRONT:
				break;
			case DIRECTION::BACK:
				Rotate(ROTATE_AXIS::Y, 180.0f);
				break;
			case DIRECTION::RIGHT:
				Rotate(ROTATE_AXIS::Y, -90.0f);
				break;
			case DIRECTION::LEFT:
				Rotate(ROTATE_AXIS::Y, 90.0f);
				break;
			default:
				break;
			}
			break;
		case DIRECTION::LEFT:
			switch (m_prevDirection)
			{
			case DIRECTION::IDLE:
			case DIRECTION::FRONT:
				Rotate(ROTATE_AXIS::Y, -90.0f);
				break;
			case DIRECTION::BACK:
				Rotate(ROTATE_AXIS::Y, 90.0f);
				break;
			case DIRECTION::RIGHT:
				Rotate(ROTATE_AXIS::Y, 180.0f);
				break;
			case DIRECTION::LEFT:
				break;
			default:
				break;
			}
			break;
		case DIRECTION::BACK:
			switch (m_prevDirection)
			{
			case DIRECTION::IDLE:
			case DIRECTION::FRONT:
				Rotate(ROTATE_AXIS::Y, 180.0f);
				break;
			case DIRECTION::BACK:
				break;
			case DIRECTION::RIGHT:
				Rotate(ROTATE_AXIS::Y, 90.0f);
				break;
			case DIRECTION::LEFT:
				Rotate(ROTATE_AXIS::Y, -90.0f);
				break;
			default:
				break;
			}
			break;
		case DIRECTION::RIGHT:
			switch (m_prevDirection)
			{
			case DIRECTION::IDLE:
			case DIRECTION::FRONT:
				Rotate(ROTATE_AXIS::Y, 90.0f);
				break;
			case DIRECTION::BACK:
				Rotate(ROTATE_AXIS::Y, -90.0f);
				break;
			case DIRECTION::RIGHT:
				break;
			case DIRECTION::LEFT:
				Rotate(ROTATE_AXIS::Y, 180.0f);
				break;
			default:
				break;
			}
			break;
		}
		return;
	}
	m_inputDirection = (DIRECTION)(m_inputDirection | d);
	switch ((DIRECTION)(m_inputDirection ^ d))
	{
	case DIRECTION::FRONT:
	{
		switch (d)
		{
		case DIRECTION::FRONT:
			break;
		case DIRECTION::LEFT:
			Rotate(ROTATE_AXIS::Y, -45.0f);
			break;
		case DIRECTION::BACK:
			break;
		case DIRECTION::RIGHT:
			Rotate(ROTATE_AXIS::Y, 45.0f);
			break;
		}
	}
	break;
	case DIRECTION::LEFT:
	{
		switch (d)
		{
		case DIRECTION::FRONT:
			Rotate(ROTATE_AXIS::Y, 45.0f);
			break;
		case DIRECTION::LEFT:
			break;
		case DIRECTION::BACK:
			Rotate(ROTATE_AXIS::Y, -45.0f);
			break;
		case DIRECTION::RIGHT:
			break;
		}
	}
	break;
	case DIRECTION::BACK:
	{
		switch (d)
		{
		case DIRECTION::FRONT:
			break;
		case DIRECTION::LEFT:
			Rotate(ROTATE_AXIS::Y, 45.0f);
			break;
		case DIRECTION::BACK:
			break;
		case DIRECTION::RIGHT:
			Rotate(ROTATE_AXIS::Y, -45.0f);
			break;
		}
	}
	break;
	case DIRECTION::RIGHT:
	{
		switch (d)
		{
		case DIRECTION::FRONT:
			Rotate(ROTATE_AXIS::Y, -45.0f);
			break;
		case DIRECTION::LEFT:
			break;
		case DIRECTION::BACK:
			Rotate(ROTATE_AXIS::Y, 45.0f);
			break;
		case DIRECTION::RIGHT:
			break;
		}
	}
	break;
	}
}

void PlayerSessionObject::StopMove()
{
	PrintCurrentTime();
	std::cout << "PlayerSessionObject::StopMove() - Look Dir: " << m_directionVector.x << ", " << m_directionVector.y << ", " << m_directionVector.z << std::endl;
	m_prevDirection = m_inputDirection;
	m_inputDirection = DIRECTION::IDLE;
}

void PlayerSessionObject::ChangeDirection(DIRECTION d)
{
	m_inputDirection = (DIRECTION)(m_inputDirection ^ d);
	switch (d)
	{
	case DIRECTION::FRONT:
	{
		switch (m_inputDirection)
		{
		case DIRECTION::FRONT:
			break;
		case DIRECTION::LEFT:
			Rotate(ROTATE_AXIS::Y, -45.0f);
			break;
		case DIRECTION::BACK:
			break;
		case DIRECTION::RIGHT:
			Rotate(ROTATE_AXIS::Y, 45.0f);
			break;
		}
	}
	break;
	case DIRECTION::LEFT:
	{
		switch (m_inputDirection)
		{
		case DIRECTION::FRONT:
			Rotate(ROTATE_AXIS::Y, 45.0f);
			break;
		case DIRECTION::LEFT:
			break;
		case DIRECTION::BACK:
			Rotate(ROTATE_AXIS::Y, -45.0f);
			break;
		case DIRECTION::RIGHT:
			break;
		}
	}
	break;
	case DIRECTION::BACK:
	{
		switch (m_inputDirection)
		{
		case DIRECTION::FRONT:
			break;
		case DIRECTION::LEFT:
			Rotate(ROTATE_AXIS::Y, 45.0f);
			break;
		case DIRECTION::BACK:
			break;
		case DIRECTION::RIGHT:
			Rotate(ROTATE_AXIS::Y, -45.0f);
			break;
		}
	}
	break;
	case DIRECTION::RIGHT:
	{
		switch (m_inputDirection)
		{
		case DIRECTION::FRONT:
			Rotate(ROTATE_AXIS::Y, -45.0f);
			break;
		case DIRECTION::LEFT:
			break;
		case DIRECTION::BACK:
			Rotate(ROTATE_AXIS::Y, 45.0f);
			break;
		case DIRECTION::RIGHT:
			break;
		}
	}
	break;
	}
}

void PlayerSessionObject::Rotate(ROTATE_AXIS axis, float angle)
{
	//std::cout << "rotate axis: " << (int)axis << " angle: " << angle << std::endl;
	DirectX::XMFLOAT3 upVec = DirectX::XMFLOAT3(0, 1, 0);
	switch (axis)
	{
	case X:
	{

	}
	break;
	case Y:
	{
		DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&upVec), DirectX::XMConvertToRadians(angle));
		m_worldMatrix = Matrix4x4::Multiply(mtxRotate, m_worldMatrix);
		/*std::cout << "matrix\n"
			<< m_worldMatrix._11 << "   " << m_worldMatrix._12 << "   " << m_worldMatrix._13 << std::endl
			<< m_worldMatrix._21 << "   " << m_worldMatrix._22 << "   " << m_worldMatrix._23 << std::endl
			<< m_worldMatrix._31 << "   " << m_worldMatrix._32 << "   " << m_worldMatrix._33 << std::endl;*/

		m_directionVector = Vector3::Normalize(XMFLOAT3(m_worldMatrix._31, m_worldMatrix._32, m_worldMatrix._33));

		m_rotateAngle.y += angle;
	}
	break;
	case Z:
	{

	}
	break;
	default:
		break;
	}
	//std::cout << "current direction " << m_directionVector.x << " " << m_directionVector.y << " " << m_directionVector.z << std::endl;
	//std::cout << "rotate angle" << m_rotateAngle.x << " " << m_rotateAngle.y << " " << m_rotateAngle.z << std::endl;
}

const DirectX::XMFLOAT3 PlayerSessionObject::GetPosition()
{
	return m_position;
}

const DirectX::XMFLOAT3 PlayerSessionObject::GetRotation()
{
	return m_rotateAngle;
}

char* PlayerSessionObject::GetPlayerInfo()
{
	SERVER_PACKET::AddPlayerPacket* playerInfo = new SERVER_PACKET::AddPlayerPacket;
	playerInfo->userId = m_session->GetId();
	memcpy(playerInfo->name, m_playerName.c_str(), m_playerName.size() * 2);
	playerInfo->position = m_position;
	playerInfo->rotate = m_rotateAngle;
	playerInfo->name[m_playerName.size()] = 0;
	playerInfo->type = SERVER_PACKET::ADD_PLAYER;
	playerInfo->size = sizeof(SERVER_PACKET::AddPlayerPacket);
	return reinterpret_cast<char*>(playerInfo);
}
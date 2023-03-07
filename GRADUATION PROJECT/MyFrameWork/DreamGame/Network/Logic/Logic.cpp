#include "stdafx.h"
#include "Logic.h"
#include "../../../Server/IOCPNetwork/protocol/protocol.h"
#include "../../GameFramework.h"
#include "../../Scene.h"
#include "../../GameobjectManager.h"

extern CGameFramework gGameFramework;


Logic::Logic()
{
	m_KeyInput = new CKeyInput();
}

Logic::~Logic()
{
	delete m_KeyInput;
}

void Logic::ProcessPacket(char* p)
{
	static XMFLOAT3 upVec = XMFLOAT3(0, 1, 0);
	switch (p[1])
	{
	case SERVER_PACKET::MOVE_KEY_DOWN:
	{
		SERVER_PACKET::MovePacket* recvPacket = reinterpret_cast<SERVER_PACKET::MovePacket*>(p);
		auto findRes = find_if(m_inGamePlayerSession.begin(), m_inGamePlayerSession.end(), [&recvPacket](auto& fObj) {
			if (fObj.m_id == recvPacket->userId)
				return true;
			return false;
			});
		if (findRes != m_inGamePlayerSession.end()) {
			DIRECTION currentDir = findRes->m_currentDirection;
#if _DEBUG
			std::cout << "current Direction: ";
			if (currentDir == DIRECTION::IDLE)
				std::cout << "IDLE" << std::endl;
			else if (currentDir == DIRECTION::LEFT)
				std::cout << "LEFT" << std::endl;
			else if (currentDir == DIRECTION::RIGHT)
				std::cout << "RIGHT" << std::endl;
			else if (currentDir == DIRECTION::FRONT)
				std::cout << "FRONT" << std::endl;
			else if (currentDir == DIRECTION::BACK)
				std::cout << "BACK" << std::endl;

			std::cout << "prev Direction: ";
			if (findRes->m_prevDirection == DIRECTION::IDLE)
				std::cout << "IDLE" << std::endl;
			else if (findRes->m_prevDirection == DIRECTION::LEFT)
				std::cout << "LEFT" << std::endl;
			else if (findRes->m_prevDirection == DIRECTION::RIGHT)
				std::cout << "RIGHT" << std::endl;
			else if (findRes->m_prevDirection == DIRECTION::FRONT)
				std::cout << "FRONT" << std::endl;
			else if (findRes->m_prevDirection == DIRECTION::BACK)
				std::cout << "BACK" << std::endl;

			std::cout << "input Direction: ";
			if (recvPacket->direction == DIRECTION::IDLE)
				std::cout << "IDLE" << std::endl;
			else if (recvPacket->direction == DIRECTION::LEFT)
				std::cout << "LEFT" << std::endl;
			else if (recvPacket->direction == DIRECTION::RIGHT)
				std::cout << "RIGHT" << std::endl;
			else if (recvPacket->direction == DIRECTION::FRONT)
				std::cout << "FRONT" << std::endl;
			else if (recvPacket->direction == DIRECTION::BACK)
				std::cout << "BACK" << std::endl;
#endif
			findRes->m_currentDirection = (DIRECTION)(findRes->m_currentDirection | recvPacket->direction);
			if (currentDir == DIRECTION::IDLE) {
				switch (recvPacket->direction)
				{
				case DIRECTION::FRONT:
					switch (findRes->m_prevDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						break;
					case DIRECTION::BACK:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 180.0f);
						findRes->m_rotateAngle.y += 180.0f;
						break;
					case DIRECTION::RIGHT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, -90.0f);
						findRes->m_rotateAngle.y += -90.0f;
						break;
					case DIRECTION::LEFT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 90.0f);
						findRes->m_rotateAngle.y += 90.0f;
						break;
					default:
						break;
					}
					break;
				case DIRECTION::LEFT:
					switch (findRes->m_prevDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, -90.0f);
						findRes->m_rotateAngle.y += -90.0f;
						break;
					case DIRECTION::BACK:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 90.0f);
						findRes->m_rotateAngle.y += 90.0f;
						break;
					case DIRECTION::RIGHT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 180.0f);
						findRes->m_rotateAngle.y += 180.0f;
						break;
					case DIRECTION::LEFT:
						break;
					default:
						break;
					}
					break;
				case DIRECTION::BACK:
					switch (findRes->m_prevDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 180.0f);
						findRes->m_rotateAngle.y += 180.0f;
						break;
					case DIRECTION::BACK:
						break;
					case DIRECTION::RIGHT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 90.0f);
						findRes->m_rotateAngle.y += 90.0f;
						break;
					case DIRECTION::LEFT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, -90.0f);
						findRes->m_rotateAngle.y += -90.0f;
						break;
					default:
						break;
					}
					break;
				case DIRECTION::RIGHT:
					switch (findRes->m_prevDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 90.0f);
						findRes->m_rotateAngle.y += 90.0f;
						break;
					case DIRECTION::BACK:
						findRes->m_currentPlayGameObject->Rotate(&upVec, -90.0f);
						findRes->m_rotateAngle.y += -90.0f;
						break;
					case DIRECTION::RIGHT:
						break;
					case DIRECTION::LEFT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 180.0f);
						findRes->m_rotateAngle.y += 180.0f;
						break;
					default:
						break;
					}
					break;
				}
			}
			else {
				switch (currentDir)
				{
				case DIRECTION::FRONT:
				{
					switch (recvPacket->direction)
					{
					case DIRECTION::FRONT:
						break;
					case DIRECTION::LEFT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						findRes->m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::BACK:
						break;
					case DIRECTION::RIGHT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						findRes->m_rotateAngle.y += 45.0f;
						break;
					}
				}
				break;
				case DIRECTION::LEFT:
				{
					switch (recvPacket->direction)
					{
					case DIRECTION::FRONT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						findRes->m_rotateAngle.y += 45.0f;
						break;
					case DIRECTION::LEFT:
						break;
					case DIRECTION::BACK:
						findRes->m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						findRes->m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::RIGHT:
						break;
					}
				}
				break;
				case DIRECTION::BACK:
				{
					switch (recvPacket->direction)
					{
					case DIRECTION::FRONT:
						break;
					case DIRECTION::LEFT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						findRes->m_rotateAngle.y += 45.0f;
						break;
					case DIRECTION::BACK:
						break;
					case DIRECTION::RIGHT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						findRes->m_rotateAngle.y += -45.0f;
						break;
					}
				}
				break;
				case DIRECTION::RIGHT:
				{
					switch (recvPacket->direction)
					{
					case DIRECTION::FRONT:
						findRes->m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						findRes->m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::LEFT:
						break;
					case DIRECTION::BACK:
						findRes->m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						findRes->m_rotateAngle.y += 45.0f;
						break;
					case DIRECTION::RIGHT:
						break;
					}
				}
				break;
				}
			}
		}
		else cout << "not found array" << endl;
#ifdef _DEBUG
		PrintCurrentTime();
		std::cout << "Logic::ProcessPacket() - SERVER_PACKET::MOVE_KEY_DOWN - MOVE_KEY_DOWN ID: " << recvPacket->userId << std::endl;
#endif

	}
	break;
	case SERVER_PACKET::MOVE_KEY_UP:
	{
		SERVER_PACKET::MovePacket* recvPacket = reinterpret_cast<SERVER_PACKET::MovePacket*>(p);
		auto findRes = find_if(m_inGamePlayerSession.begin(), m_inGamePlayerSession.end(), [&recvPacket](auto& fObj) {
			if (fObj.m_id == recvPacket->userId)
				return true;
			return false;
			});
		if (findRes != m_inGamePlayerSession.end()) {
			findRes->m_currentDirection = (DIRECTION)(findRes->m_currentDirection ^ recvPacket->direction);
			switch (recvPacket->direction)
			{
			case DIRECTION::FRONT:
			{
				switch (findRes->m_currentDirection)
				{
				case DIRECTION::FRONT:
					break;
				case DIRECTION::LEFT:
					findRes->m_currentPlayGameObject->Rotate(&upVec, -45.0f);
					findRes->m_rotateAngle.y += -45.0f;
					break;
				case DIRECTION::BACK:
					break;
				case DIRECTION::RIGHT:
					findRes->m_currentPlayGameObject->Rotate(&upVec, 45.0f);
					findRes->m_rotateAngle.y += 45.0f;
					break;
				}
			}
			break;
			case DIRECTION::LEFT:
			{
				switch (findRes->m_currentDirection)
				{
				case DIRECTION::FRONT:
					findRes->m_currentPlayGameObject->Rotate(&upVec, 45.0f);
					findRes->m_rotateAngle.y += 45.0f;
					break;
				case DIRECTION::LEFT:
					break;
				case DIRECTION::BACK:
					findRes->m_currentPlayGameObject->Rotate(&upVec, -45.0f);
					findRes->m_rotateAngle.y += -45.0f;
					break;
				case DIRECTION::RIGHT:
					break;
				}
			}
			break;
			case DIRECTION::BACK:
			{
				switch (findRes->m_currentDirection)
				{
				case DIRECTION::FRONT:
					break;
				case DIRECTION::LEFT:
					findRes->m_currentPlayGameObject->Rotate(&upVec, 45.0f);
					findRes->m_rotateAngle.y += 45.0f;
					break;
				case DIRECTION::BACK:
					break;
				case DIRECTION::RIGHT:
					findRes->m_currentPlayGameObject->Rotate(&upVec, -45.0f);
					findRes->m_rotateAngle.y += -45.0f;
					break;
				}
			}
			break;
			case DIRECTION::RIGHT:
			{
				switch (findRes->m_currentDirection)
				{
				case DIRECTION::FRONT:
					findRes->m_currentPlayGameObject->Rotate(&upVec, -45.0f);
					findRes->m_rotateAngle.y += -45.0f;
					break;
				case DIRECTION::LEFT:
					break;
				case DIRECTION::BACK:
					findRes->m_currentPlayGameObject->Rotate(&upVec, 45.0f);
					findRes->m_rotateAngle.y += 45.0f;
					break;
				case DIRECTION::RIGHT:
					break;
				}
			}
			break;
			}
		}
		else cout << "not found array" << endl;
#ifdef _DEBUG
		PrintCurrentTime();
		std::cout << "Logic::ProcessPacket() - SERVER_PACKET::MOVE_KEY_UP - MOVE_KEY_UP ID: " << recvPacket->userId << std::endl;
#endif
	}
	break;
	case SERVER_PACKET::ROTATE:
	{
		SERVER_PACKET::RotatePacket* recvPacket = reinterpret_cast<SERVER_PACKET::RotatePacket*>(p);
		auto findRes = find_if(m_inGamePlayerSession.begin(), m_inGamePlayerSession.end(), [&recvPacket](auto& fObj) {
			if (fObj.m_id == recvPacket->userId)
				return true;
			return false;
			});
		if (findRes != m_inGamePlayerSession.end()) {
			switch (recvPacket->axis)
			{
			case ROTATE_AXIS::X:
			{

			}
			break;
			case ROTATE_AXIS::Y:
			{
				findRes->m_currentPlayGameObject->Rotate(&upVec, recvPacket->angle);
				findRes->m_rotateAngle.y += recvPacket->angle;
#ifdef _DEBUG
				PrintCurrentTime();
				std::cout << "Logic::ProcessPacket() - SERVER_PACKET::ROTATE - ROTATE ID: " << recvPacket->userId << std::endl;
				cout << "Rotate axis Y angle: " << recvPacket->angle << endl;
#endif
			}
			break;
			case ROTATE_AXIS::Z:
			{

			}
			break;
			default:
				break;
			}
		}
	}
	break;
	case SERVER_PACKET::STOP:
	{
		XMFLOAT3 upVec = XMFLOAT3(0, 1, 0);
		XMFLOAT3 rightVec = XMFLOAT3(1, 0, 0);
		XMFLOAT3 dirVec = XMFLOAT3(0, 0, 1);
		SERVER_PACKET::StopPacket* recvPacket = reinterpret_cast<SERVER_PACKET::StopPacket*>(p);
		auto findRes = find_if(m_inGamePlayerSession.begin(), m_inGamePlayerSession.end(), [&recvPacket](auto& fObj) {
			if (fObj.m_id == recvPacket->userId)
				return true;
			return false;
			});
		if (findRes != m_inGamePlayerSession.end()) {
			if (findRes->m_id != myId)
				findRes->m_prevDirection = findRes->m_currentDirection;
			findRes->m_currentDirection = DIRECTION::IDLE;
			findRes->m_currentPlayGameObject->SetPosition(recvPacket->position);
			if (abs(findRes->m_rotateAngle.y - recvPacket->rotate.y) > FLT_EPSILON) {
#ifdef _DEBUG
				PrintCurrentTime();
				cout << "Logic::ProcessPacket() - SERVER_PACKET::STOP - Rotation: current(" << findRes->m_rotateAngle.y << ") - new(" << recvPacket->rotate.y << ")" << endl;
#endif
				findRes->m_currentPlayGameObject->Rotate(&upVec, recvPacket->rotate.y - findRes->m_rotateAngle.y);
			}
#ifdef _DEBUG
			PrintCurrentTime();
			std::cout << "Logic::ProcessPacket() - SERVER_PACKET::STOP - STOP ID: " << recvPacket->userId << std::endl;
			cout << "Position: " << recvPacket->position.x << ", " << recvPacket->position.y << ", " << recvPacket->position.z << endl;
			cout << "Rotation: " << recvPacket->rotate.x << ", " << recvPacket->rotate.y << ", " << recvPacket->rotate.z << endl;
			cout << "STOP ID: " << recvPacket->userId << endl;
#endif
		}
	}
	break;
	case SERVER_PACKET::LOGIN_OK:
	{
		SERVER_PACKET::LoginPacket* recvPacket = reinterpret_cast<SERVER_PACKET::LoginPacket*>(p);
		wstring wst_name = recvPacket->name;
		m_inGamePlayerSession[0].m_id = myId = recvPacket->userID;
		m_inGamePlayerSession[0].SetName(wst_name);
		gGameFramework.m_pScene->m_pObjectManager->SetPlayCharacter(&m_inGamePlayerSession[0]);
#ifdef _DEBUG
		PrintCurrentTime();
		std::wcout << "Logic::ProcessPacket() - SERVER_PACKET::LOGIN_OK - " << "user Name: " << wst_name << std::endl;
#endif
	}
	break;
	case SERVER_PACKET::ADD_PLAYER:
	{
		XMFLOAT3 rightVec = XMFLOAT3(1, 0, 0);
		XMFLOAT3 dirVec = XMFLOAT3(0, 0, 1);

		SERVER_PACKET::AddPlayerPacket* recvPacket = reinterpret_cast<SERVER_PACKET::AddPlayerPacket*>(p);
		for (auto& pSession : m_inGamePlayerSession) {
			if (-1 == pSession.m_id) {
				pSession.m_id = recvPacket->userId;
				pSession.SetName(recvPacket->name);
				gGameFramework.m_pScene->m_pObjectManager->SetPlayCharacter(&pSession);
				pSession.m_currentPlayGameObject->SetPosition(recvPacket->position);
				pSession.m_currentPlayGameObject->Rotate(&upVec, recvPacket->rotate.y);
				pSession.m_rotateAngle.y = recvPacket->rotate.y;
#ifdef _DEBUG
				PrintCurrentTime();
				cout << "CLIENT::Logic::" << endl;
				cout << "AddPlayer ID: " << recvPacket->userId << endl;
				cout << "Position: " << recvPacket->position.x << ", " << recvPacket->position.y << ", " << recvPacket->position.z << endl;
				cout << "Rotate: " << recvPacket->rotate.x << ", " << recvPacket->rotate.y << ", " << recvPacket->rotate.z << endl;
#endif
				break;
			}
		}
	}
	break;
	default:
		break;
	}
}

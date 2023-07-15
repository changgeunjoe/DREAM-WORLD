#include "stdafx.h"
#include "Logic.h"
//#include "../../../Server/IOCPNetwork/protocol/protocol.h"
#include "../../GameFramework.h"
#include "../../Scene.h"
#include "../../GameobjectManager.h"
#include "../../Animation.h"
#include "../Room/RoomManger.h"
#include "../../Character.h"
#include "../NetworkHelper.h"


extern CGameFramework gGameFramework;
extern RoomManger g_RoomManager;
extern NetworkHelper g_NetworkHelper;
extern bool GameEnd;

extern HWND g_wnd;
extern int g_cmd;

using namespace chrono;

Logic::Logic()
{
	m_KeyInput = new CKeyInput();
	attckPacketRecvTime = chrono::high_resolution_clock::now();
}

Logic::~Logic()
{
	delete m_KeyInput;
}

void Logic::ProcessPacket(char* p)
{
	static XMFLOAT3 upVec = XMFLOAT3(0, 1, 0);
	switch (p[2])
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
			findRes->m_currentDirection = (DIRECTION)(findRes->m_currentDirection | recvPacket->direction);
			findRes->m_currentPlayGameObject->SetMoveState(true);
			//cout << "Move Player Id: " << findRes->m_id << endl;
		}
		else cout << "not found array" << endl;
#ifdef _DEBUG
		PrintCurrentTime();
		//std::cout << "Logic::ProcessPacket() - SERVER_PACKET::MOVE_KEY_DOWN - ID: " << recvPacket->userId << std::endl;

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
			//cout << "Move Player Id: " << findRes->m_id << endl;
		}
		else cout << "not found array" << endl;
#ifdef _DEBUG
		PrintCurrentTime();
		//	std::cout << "Logic::ProcessPacket() - SERVER_PACKET::MOVE_KEY_UP - MOVE_KEY_UP ID: " << recvPacket->userId << std::endl;
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
				findRes->m_ownerRotateAngle.x = recvPacket->angle;
				findRes->m_currentPlayGameObject->SetProjectileY(findRes->m_ownerRotateAngle.x);
			}
			break;
			case ROTATE_AXIS::Y:
			{
				findRes->m_currentPlayGameObject->Rotate(&upVec, recvPacket->angle);
				findRes->m_ownerRotateAngle.y += recvPacket->angle;
#ifdef _DEBUG
				PrintCurrentTime();
				//	std::cout << "Logic::ProcessPacket() - SERVER_PACKET::ROTATE - ROTATE ID: " << recvPacket->userId << std::endl;
				//	cout << "Rotate axis Y angle: " << recvPacket->angle << endl;
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
			findRes->m_currentPlayGameObject->SetMoveState(false);
			// if (abs(findRes->m_rotateAngle.y - recvPacket->rotate.y) > FLT_EPSILON) {
				// PrintCurrentTime();
				// cout << "Logic::ProcessPacket() - SERVER_PACKET::STOP - Rotation: current(" << findRes->m_rotateAngle.y << ") - new(" << recvPacket->rotate.y << ")" << endl;
				// findRes->m_currentPlayGameObject->Rotate(&upVec, recvPacket->rotate.y - findRes->m_rotateAngle.y);
				// findRes->m_rotateAngle.y += recvPacket->rotate.y - findRes->m_rotateAngle.y;
			// }
#ifdef _DEBUG
		//	PrintCurrentTime();
		//	std::cout << "Logic::ProcessPacket() - SERVER_PACKET::STOP - STOP ID: " << recvPacket->userId << std::endl;
		//	cout << "Position: " << recvPacket->position.x << ", " << recvPacket->position.y << ", " << recvPacket->position.z << endl;
		//	// cout << "Rotation: " << recvPacket->rotate.x << ", " << recvPacket->rotate.y << ", " << recvPacket->rotate.z << endl;
		//	cout << "STOP ID: " << recvPacket->userId << endl;
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
		//gGameFramework.m_pScene->m_pObjectManager->SetPlayCharacter(&m_inGamePlayerSession[0]);
		EndDialog(loginWnd, IDCANCEL);
		ShowWindow(g_wnd, g_cmd);
		UpdateWindow(g_wnd);
#ifdef _DEBUG
		PrintCurrentTime();
		std::wcout << "Logic::ProcessPacket() - SERVER_PACKET::LOGIN_OK - " << "user Name: " << wst_name << std::endl;
		std::wcout << "Logic::ProcessPacket() - SERVER_PACKET::LOGIN_OK - " << "user ID: " << recvPacket->userID << std::endl;
#endif
	}
	break;
	case SERVER_PACKET::ADD_PLAYER:
	{
		XMFLOAT3 rightVec = XMFLOAT3(1, 0, 0);
		XMFLOAT3 dirVec = XMFLOAT3(0, 0, 1);

		SERVER_PACKET::AddPlayerPacket* recvPacket = reinterpret_cast<SERVER_PACKET::AddPlayerPacket*>(p);
#ifdef _DEBUG
		cout << "AddPlayer ID: " << recvPacket->userId << endl;
		cout << "myID: " << myId << endl;
#endif
		if (recvPacket->userId == myId) {
			m_inGamePlayerSession[0].m_id = recvPacket->userId;
			m_inGamePlayerSession[0].SetName(recvPacket->name);
			m_inGamePlayerSession[0].SetRole((ROLE)recvPacket->role);
			gGameFramework.m_pScene->m_pObjectManager->SetPlayCharacter(&m_inGamePlayerSession[0]);
			gGameFramework.m_pScene->m_pObjectManager->SetPlayerCamera(m_inGamePlayerSession[0]);
			m_inGamePlayerSession[0].m_currentPlayGameObject->SetPosition(recvPacket->position);
			m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, recvPacket->rotate.y);
			m_inGamePlayerSession[0].m_ownerRotateAngle.y = recvPacket->rotate.y;
		}
		else {
			for (int i = 1; i < 4; i++) {
				auto& pSession = m_inGamePlayerSession[i];
				if (-1 == pSession.m_id) {
					pSession.m_id = recvPacket->userId;
					pSession.SetName(recvPacket->name);
					pSession.SetRole((ROLE)recvPacket->role);
					gGameFramework.m_pScene->m_pObjectManager->SetPlayCharacter(&pSession);
					pSession.m_currentPlayGameObject->SetPosition(recvPacket->position);
					pSession.m_currentPlayGameObject->Rotate(&upVec, recvPacket->rotate.y);
					pSession.m_ownerRotateAngle.y = recvPacket->rotate.y;
					//#ifdef _DEBUG
					//					PrintCurrentTime();
					//					cout << "CLIENT::Logic::" << endl;
					//					cout << "AddPlayer ID: " << recvPacket->userId << endl;
					//					cout << "Position: " << recvPacket->position.x << ", " << recvPacket->position.y << ", " << recvPacket->position.z << endl;
					//					cout << "Rotate: " << recvPacket->rotate.x << ", " << recvPacket->rotate.y << ", " << recvPacket->rotate.z << endl;
					//#endif
					break;
				}
			}
		}
	}
	break;
	case SERVER_PACKET::MOUSE_INPUT:
	{
		SERVER_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<SERVER_PACKET::MouseInputPacket*>(p);

		auto findRes = find_if(m_inGamePlayerSession.begin(), m_inGamePlayerSession.end(), [&recvPacket](auto& fObj) {
			if (fObj.m_id == recvPacket->userId)
				return true;
			return false;
			});
		if (findRes != m_inGamePlayerSession.end()) {
			if (recvPacket->LClickedButton == true) {
				findRes->m_currentPlayGameObject->SetLButtonClicked(true);
			}
			else {
				findRes->m_currentPlayGameObject->SetLButtonClicked(false);
			}

			if (recvPacket->RClickedButton == true) {
				findRes->m_currentPlayGameObject->SetRButtonClicked(true);
			}
			else {
				findRes->m_currentPlayGameObject->SetRButtonClicked(false);
				findRes->m_currentPlayGameObject->RbuttonUp();
			}

			findRes->m_currentPlayGameObject->m_LMouseInput = recvPacket->LClickedButton;
			findRes->m_currentPlayGameObject->m_RMouseInput = recvPacket->RClickedButton;
		}
	}
	break;

	case SERVER_PACKET::CREATE_ROOM_SUCCESS: //  룸 생성 성공 패킷 - 대기 방으로 UI변경하고 방장임
	{
		SERVER_PACKET::CreateRoomResultPacket* recvPacket = reinterpret_cast<SERVER_PACKET::CreateRoomResultPacket*>(p);
		recvPacket->roomName;
	}
	break;

	case SERVER_PACKET::CREATE_ROOM_FAILURE: // 룸 생성 실패 패킷 - 실패했다고 notify UI 띄워주셈
	{
		SERVER_PACKET::CreateRoomResultPacket* recvPacket = reinterpret_cast<SERVER_PACKET::CreateRoomResultPacket*>(p);
	}
	break;

	case SERVER_PACKET::REQUEST_ROOM_LIST: // 방 리스트 출력 중 하나
	{
		gGameFramework.m_pLobbyScene->m_pObjectManager->SetSection(1);
		// 방 리스트 정보
		SERVER_PACKET::RoomInfoPacket* recvPacket = reinterpret_cast<SERVER_PACKET::RoomInfoPacket*>(p);
		//recvPacket->playerName;
		//recvPacket->role;
		//recvPacket->roomId;
		//recvPacket->roomName;
		wstring playerName[4] = { recvPacket->playerName[0], recvPacket->playerName[1], recvPacket->playerName[2], recvPacket->playerName[3] };
		g_RoomManager.InsertRoom(string{ recvPacket->roomId }, wstring{ recvPacket->roomName }, playerName, recvPacket->role);
#if _DEBUG
		wstring roomName{ recvPacket->roomName };
		wcout << "방 이름: " << roomName << " 방 아이디: " << recvPacket->roomId << endl;
#endif // 0
	}
	break;

	case SERVER_PACKET::REQUEST_ROOM_LIST_END: // 방 리스트의 끝
	{
		gGameFramework.m_pLobbyScene->m_pObjectManager->SetSection(1);
		// 마지막 방 리스트 정보
		SERVER_PACKET::RoomInfoPacket* recvPacket = reinterpret_cast<SERVER_PACKET::RoomInfoPacket*>(p);
		recvPacket->playerName;
		recvPacket->role;
		recvPacket->roomId;
		recvPacket->roomName;
		wstring playerName[4] = { recvPacket->playerName[0], recvPacket->playerName[1], recvPacket->playerName[2], recvPacket->playerName[3] };
		g_RoomManager.InsertRoom(string{ recvPacket->roomId }, wstring{ recvPacket->roomName }, playerName, recvPacket->role);
#if _DEBUG
		wstring roomName{ recvPacket->roomName };
		wcout << "마지막 방 이름: " << roomName << " 방 아이디: " << recvPacket->roomId << endl;
#endif // 0

	}
	break;

	case SERVER_PACKET::REQUEST_ROOM_LIST_NONE: // 방 리스트가 존재하지 않음
	{
		gGameFramework.m_pLobbyScene->m_pObjectManager->SetSection(1);
		//방 리스트 없는 상황 보여주면 됨
		SERVER_PACKET::NotifyPacket* recvPacket = reinterpret_cast<SERVER_PACKET::NotifyPacket*>(p);
		cout << "방 없음" << endl;
	}
	break;

	case SERVER_PACKET::ACCEPT_ENTER_ROOM: // 방 입장 확인
	{
		//UI 대기방 입장 상태로 변경
		SERVER_PACKET::NotifyPacket* recvPacket = reinterpret_cast<SERVER_PACKET::NotifyPacket*>(p);
	}
	break;

	case SERVER_PACKET::REJECT_ENTER_ROOM: // 방 입장 거부 당함
	{
		// 대기방 입장 거부 당하는 UI
		SERVER_PACKET::NotifyPacket* recvPacket = reinterpret_cast<SERVER_PACKET::NotifyPacket*>(p);

	}
	break;

	case SERVER_PACKET::NOT_FOUND_ROOM: // 신청한 방이 사라짐 -> 방이 출발했던가, 방이 사라졌던가
	{
		// 존재하지 않는 방입니다. UI출력
		SERVER_PACKET::NotifyPacket* recvPacket = reinterpret_cast<SERVER_PACKET::NotifyPacket*>(p);

	}
	break;

	case SERVER_PACKET::PLAYER_APPLY_ROOM: // 신청자 정보 방장한테 전송됨(이 패킷은 방장한테만? 아니면 룸 사람들한테까지 멀티 캐스트)
	{
		// 대기방에 사람 추가 UI해주면 됨
		SERVER_PACKET::PlayerApplyRoomPacket* recvPacket = reinterpret_cast<SERVER_PACKET::PlayerApplyRoomPacket*>(p);
		recvPacket->name;
		recvPacket->role;
	}
	break;

	case SERVER_PACKET::PLAYER_CANCEL_ROOM: // 신청자가 방 신청 취소
	{
		// 대기방의 신청자 명단에서 제거
		SERVER_PACKET::PlayerApplyRoomPacket* recvPacket = reinterpret_cast<SERVER_PACKET::PlayerApplyRoomPacket*>(p);
		recvPacket->name;
		recvPacket->role;
	}
	break;
	case SERVER_PACKET::INTO_GAME:
	{
		//로딩창이면 좋을듯?
		gGameFramework.m_bLobbyScene = false;
		gGameFramework.m_pLobbyScene->m_pObjectManager->SetInMatching(false);
		gGameFramework.m_pLobbyScene->m_pObjectManager->SetUIActive();
		gGameFramework.m_pScene->m_pObjectManager->SetStoryTime();
		m_MonsterSession.m_isVisible = true;
		cout << "start boss ratate angle: " << m_MonsterSession.m_rotateAngle.y << endl;
		for (int i = 0; i < 4; i++) {
			cout << "Session Index: " << i << " User Id: " << m_inGamePlayerSession[i].m_id << " User Role: " << (int)m_inGamePlayerSession[i].GetRole() << endl;
		}
	}
	break;
	case SERVER_PACKET::BOSS_CHANGE_STATE_MOVE_DES:
	{
		std::cout << "ProcessPacket()::SERVER_PACKET::BOSS_CHANGE_STATE_MOVE_DES" << std::endl;
		SERVER_PACKET::BossChangeStateMovePacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossChangeStateMovePacket*>(p);
		auto durationTime = chrono::duration_cast<milliseconds>(chrono::high_resolution_clock::now() - attckPacketRecvTime);
		//attckPacketRecvTime = chrono::high_resolution_clock::now();		
		//cout << "boss Move Packet Recv ElapsedTime: " << durationTime << endl;
		recvPacket->desPos; //여기 목적지 까지 보스 몬스터 이동 시키면 됩니다
		recvPacket->bossPos;
		recvPacket->t;
		m_MonsterSession.m_currentPlayGameObject->m_xmf3Destination = recvPacket->desPos;
		if (!m_MonsterSession.m_currentPlayGameObject->GetMoveState())
		{
			m_MonsterSession.m_currentPlayGameObject->SetMoveState(true);
			m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ATTACK::ATTACK_COUNT;
			m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->SetTrackEnable(0, 2);
		}
	}
	break;
	case SERVER_PACKET::SHOOTING_ARROW://화살
	{
		SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<SERVER_PACKET::ShootingObject*>(p);
		recvPacket->dir;
		recvPacket->srcPos;
		recvPacket->speed;
	}
	break;
	case SERVER_PACKET::SHOOTING_BALL://공
	{
		SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<SERVER_PACKET::ShootingObject*>(p);
		recvPacket->dir;
		recvPacket->srcPos;
		recvPacket->speed;
	}
	break;
	case SERVER_PACKET::GAME_STATE_S:
	{
		SERVER_PACKET::GameState_STAGE1* recvPacket = reinterpret_cast<SERVER_PACKET::GameState_STAGE1*>(p);
		//Player Session
		for (int i = 0; i < 4; i++) {//그냥 4개 여서 도는 for문 주의			
			if (-1 != recvPacket->userState[i].userId) {
				//	std::cout << "ProcessPacket()::SERVER_PACKET::GAME_STATE - User ID: " << recvPacket->userState[i].userId << " HP: " << recvPacket->userState[i].hp << std::endl;
					//playerSession에서 해당 플레이어 탐색
				auto findRes = find_if(m_inGamePlayerSession.begin(), m_inGamePlayerSession.end(), [&recvPacket, &i](auto& fObj) {
					if (fObj.m_id == recvPacket->userState[i].userId)
						return true;
					return false;
					});
				if (findRes == m_inGamePlayerSession.end())continue;
				findRes->m_currentPlayGameObject->InterpolateMove(recvPacket->time, recvPacket->userState[i].pos);
				float maxHp = findRes->m_currentPlayGameObject->GetMaxCurrentHP();
				findRes->m_currentPlayGameObject->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
			}
		}
		//small monster
		for (int i = 0; i < 15; i++) {
			if (m_SmallMonsterSession[i].m_currentPlayGameObject->GetCurrentHP() < 0.0f) {
				m_SmallMonsterSession[i].m_currentPlayGameObject->InterpolateMove(recvPacket->time, recvPacket->smallMonster[i].pos);
				m_SmallMonsterSession[i].m_currentPlayGameObject->SetCurrentHP(recvPacket->smallMonster[i].hp);
				float maxHp = m_SmallMonsterSession[i].m_currentPlayGameObject->GetMaxCurrentHP();
				m_SmallMonsterSession[i].m_currentPlayGameObject->SetCurrentHP(recvPacket->smallMonster[i].hp / maxHp * 100.0f);
			}
		}
		recvPacket->time;
		recvPacket->userState;
	}
	break;
	case SERVER_PACKET::GAME_STATE_B:
	{
		//std::cout << "ProcessPacket()::SERVER_PACKET::GAME_STATE" << std::endl;
		SERVER_PACKET::GameState_BOSS* recvPacket = reinterpret_cast<SERVER_PACKET::GameState_BOSS*>(p);
		if (recvPacket->bossState.hp != 2500) {
			//std::cout << "ProcessPacket()::SERVER_PACKET::GAME_STATE - Boss HP: " << recvPacket->bossState.hp << std::endl;

		}
		//m_MonsterSession.m_currentPlayGameObject->m_UIScale = static_cast<float>(recvPacket->bossState.hp) / 250.0f;//maxHp 2500입니다
		m_MonsterSession.m_currentPlayGameObject->SetCurrentHP(static_cast<float>(recvPacket->bossState.hp) / 25.0f);//maxHp 2500입니다
		if (m_MonsterSession.m_currentPlayGameObject->GetCurrentHP() < FLT_EPSILON)
		{
			GameEnd = true;
			break;
		}
		m_MonsterSession.m_currentPlayGameObject->InterpolateMove(recvPacket->time, recvPacket->bossState.pos);
		//Player Session
		for (int i = 0; i < 4; i++) {//그냥 4개 여서 도는 for문 주의			
			if (-1 != recvPacket->userState[i].userId) {
				//	std::cout << "ProcessPacket()::SERVER_PACKET::GAME_STATE - User ID: " << recvPacket->userState[i].userId << " HP: " << recvPacket->userState[i].hp << std::endl;
					//playerSession에서 해당 플레이어 탐색
				auto findRes = find_if(m_inGamePlayerSession.begin(), m_inGamePlayerSession.end(), [&recvPacket, &i](auto& fObj) {
					if (fObj.m_id == recvPacket->userState[i].userId)
						return true;
					return false;
					});
				if (findRes == m_inGamePlayerSession.end())continue;
				findRes->m_currentPlayGameObject->InterpolateMove(recvPacket->time, recvPacket->userState[i].pos);
				float maxHp = findRes->m_currentPlayGameObject->GetMaxCurrentHP();
				findRes->m_currentPlayGameObject->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
			}
		}
	}
	break;
	case SERVER_PACKET::BOSS_ATTACK:
	{
		auto durationTime = chrono::duration_cast<milliseconds>(chrono::high_resolution_clock::now() - attckPacketRecvTime);
		attckPacketRecvTime = chrono::high_resolution_clock::now();
		std::cout << "Attack Packet recv ElapsedTime: " << durationTime << endl;
		SERVER_PACKET::BossAttackPacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossAttackPacket*>(p);

		if (!GameEnd)
		{
			switch (recvPacket->bossAttackType)
			{
			case BOSS_ATTACK::ATTACK_PUNCH:
				if (m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ATTACK::ATTACK_PUNCH)
				{
					m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ATTACK::ATTACK_PUNCH;
					m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->SetTrackEnable(7, 2);
				}
				break;
			case BOSS_ATTACK::ATTACK_SPIN:
				if (m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ATTACK::ATTACK_SPIN)
				{
					m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ATTACK::ATTACK_SPIN;
					m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->SetTrackEnable(2, 2);
				}
				break;
			case BOSS_ATTACK::ATTACK_KICK:
				if (m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ATTACK::ATTACK_KICK)
				{
					m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ATTACK::ATTACK_KICK;
					m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->SetTrackEnable(5, 2);
				}
				break;
			}
			m_MonsterSession.m_currentPlayGameObject->SetMoveState(false);
			cout << "ProcessPacket::SERVER_PACKET::BOSS_ATTACK - recvPacket: " << (int)recvPacket->bossAttackType << endl;
		}
	}
	break;
	case SERVER_PACKET::HIT_BOSS_MAGE:
	{
		SERVER_PACKET::BossHitObject* recvPacket = reinterpret_cast<SERVER_PACKET::BossHitObject*>(p);
		m_MonsterSession.m_currentPlayGameObject->m_xmfHitPosition = recvPacket->pos;
	}
	break;
	case SERVER_PACKET::GAME_END:
	{
		//게임 종료 패킷 수신
		//지금은 바로 게임 종료 확인하는 패킷 서버로 전송하게 구현함
		m_MonsterSession.m_currentPlayGameObject->SetCurrentHP(0.0f);
		GameEnd = true;

	}
	break;
	case SERVER_PACKET::BOSS_MOVE_NODE:
	{
		// 현재 시점에서 경로 Clear
		//std::queue<int> emptyQueue;
		//std::swap(m_MonsterSession.m_currentPlayGameObject->m_BossRoute, emptyQueue);


		SERVER_PACKET::BossMoveNodePacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossMoveNodePacket*>(p);
		if (gGameFramework.m_pScene == nullptr) return;
		std::queue<int> recvNodeQueue;
		//Role로 변경했음 이거 참고 바람
		m_MonsterSession.m_currentPlayGameObject->m_roleDesPlayer = recvPacket->targetRole;
		//std::cout << "recv aggro Id: " << recvPacket->desPlayerId << std::endl;
		if (recvPacket->nodeCnt == -1) {
			m_MonsterSession.m_currentPlayGameObject->m_lockBossRoute.lock();
			m_MonsterSession.m_currentPlayGameObject->m_BossRoute.swap(recvNodeQueue);
			m_MonsterSession.m_currentPlayGameObject->m_lockBossRoute.unlock();
		}
		else if (recvPacket->nodeCnt > -1) {
			cout << "보스 이동 인덱스 : ";
			vector<int> triangleIdxVec;
			for (int i = 0; i < recvPacket->nodeCnt; i++) {
				//보스가 이동할 노드 데이터
				// 받아온 노드들 벡터에 새로 넣기
				cout << recvPacket->node[i] << ", ";
				recvNodeQueue.push(recvPacket->node[i]);
				triangleIdxVec.push_back(recvPacket->node[i]);
			}
			cout << endl;
			//gGameFramework.m_pScene->m_pObjectManager->m_VecNodeQueue.push_back(recvPacket->node[i]);				
			//AStart Node Mesh
			gGameFramework.m_pScene->m_pObjectManager->m_nodeLock.lock();
			gGameFramework.m_pScene->m_pObjectManager->m_VecNodeQueue.swap(triangleIdxVec);
			gGameFramework.m_pScene->m_pObjectManager->m_nodeLock.unlock();

			//boss Move Node Data
			m_MonsterSession.m_currentPlayGameObject->m_lockBossRoute.lock();
			m_MonsterSession.m_currentPlayGameObject->m_BossRoute.swap(recvNodeQueue);
			m_MonsterSession.m_currentPlayGameObject->m_lockBossRoute.unlock();
		}

		if (!m_MonsterSession.m_currentPlayGameObject->GetMoveState())
		{
			cout << "SERVER_PACKET::BOSS_MOVE_NODE - SetMoveState True" << endl;
			m_MonsterSession.m_currentPlayGameObject->SetMoveState(true);
			m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ATTACK::ATTACK_COUNT;
			m_MonsterSession.m_currentPlayGameObject->m_pSkinnedAnimationController->SetTrackEnable(0, 2);
		}
		//m_MonsterSession.m_currentPlayGameObject->SetMoveState(true);
		//cout << endl;
	}
	break;
	case SERVER_PACKET::PRE_EXIST_LOGIN://이미 존재하는 플레이어가 있기 때문에, 지금 들어온 플레이어(내 클라이언트는) 접속 해제 패킷을 수신
	{
		//알림 메세지 띄우고, 다시 접속은 해야하네...;;
		EndDialog(loginWnd, IDCANCEL);
		//ShowWindow(g_wnd, g_cmd);//이 윈도우가 아닌 로그인 중복 확인창 띄워야 됨
		//UpdateWindow(g_wnd);
	}
	break;
	case SERVER_PACKET::DUPLICATED_LOGIN://접속해 있었지만(내 클라이언트) 중복로그인을 함
	{
		EndDialog(g_wnd, IDCANCEL);

	}
	break;
	case SERVER_PACKET::SKILL_INPUT:
	{
		SERVER_PACKET::SkillInputPacket* recvPacket = reinterpret_cast<SERVER_PACKET::SkillInputPacket*>(p);

		auto findRes = find_if(m_inGamePlayerSession.begin(), m_inGamePlayerSession.end(), [&recvPacket](auto& fObj) {
			if (fObj.m_id == recvPacket->userId)
				return true;
			return false;
			});
		if (findRes != m_inGamePlayerSession.end()) {
			if (recvPacket->qSkill == true) {
				static_cast<Character*>(findRes->m_currentPlayGameObject)->FirstSkillDown();
			}

			if (recvPacket->eSkill == true) {
				static_cast<Character*>(findRes->m_currentPlayGameObject)->SecondSkillDown();
			}
		}
	}
	break;



	default:
	{
		std::cout << "Unknown Packet Recv" << std::endl;
	}
	break;
	}
}

XMFLOAT3 Logic::GetPostion(ROLE r)
{
	return gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(r)->GetPosition();
	// TODO: 여기에 return 문을 삽입합니다.
}

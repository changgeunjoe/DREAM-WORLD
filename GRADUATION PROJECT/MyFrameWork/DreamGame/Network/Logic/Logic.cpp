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
	case SERVER_PACKET::GAME_STATE:
	{
		//std::cout << "ProcessPacket()::SERVER_PACKET::GAME_STATE" << std::endl;
		SERVER_PACKET::GameState* recvPacket = reinterpret_cast<SERVER_PACKET::GameState*>(p);
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

		Monster* monsterObject = dynamic_cast<Monster*>(m_MonsterSession.m_currentPlayGameObject);

		utc_clock::time_point currentUTC_Time = utc_clock::now();
		XMFLOAT3 bossPos = m_MonsterSession.m_currentPlayGameObject->GetPosition();
		XMFLOAT3 bossLookVector = monsterObject->GetLook();
		XMFLOAT3 bossRightVector = monsterObject->GetRight();
		high_resolution_clock::time_point h_t = high_resolution_clock::now();
		XMFLOAT3 bossInterpolationVector = Vector3::Subtract(recvPacket->bossState.pos, bossPos);

		double bosDurationTime = duration_cast<microseconds>(currentUTC_Time - recvPacket->time).count();
		bosDurationTime = (double)bosDurationTime / 1000.0f;//microseconds to milli
		//cout << "duration UTC: " << bosDurationTime << "mili" << endl;
		bosDurationTime = (double)bosDurationTime / 1000.0f;//milliseconds ti sec
		//cout << "boss duration UTC: " << bosDurationTime << "second" << endl;

		float bossPosDistance = Vector3::Length(bossInterpolationVector);
		float bossInterpolationDistance = bossPosDistance - (float)bosDurationTime * 50.0f;//length - v*t

		//cout << "bossPosDistance: " << bossPosDistance << endl;
		//cout << "bossInterplationDis: " << bossInterpolationDistance << endl;

		//boss Session

		//rot
		bool OnRight = (Vector3::DotProduct(bossRightVector, Vector3::Normalize(recvPacket->bossState.directionVector)) > 0) ? true : false;	// 목적지가 올느쪽 왼
		float bossRotBetweenAngle = Vector3::Angle(Vector3::Normalize(recvPacket->bossState.directionVector), bossLookVector);
		float bossInterpolationAngle = abs(bossRotBetweenAngle) - bosDurationTime * 90.0f;
		//PrintCurrentTime();
		//cout << endl << "bossBetweenAngle: " << bossRotBetweenAngle << endl;
		//cout << "bossBetweenAngle Interpolation: " << bossInterpolationAngle << endl;
		if (bossInterpolationAngle < DBL_EPSILON || bossInterpolationDistance < 0)
			m_MonsterSession.m_currentPlayGameObject->m_interpolationRotateAngleY = 0.0;
		else if (abs(bossInterpolationAngle) > 5.0f) {
			OnRight ? m_MonsterSession.m_currentPlayGameObject->Rotate(0, bossRotBetweenAngle, 0) :
				m_MonsterSession.m_currentPlayGameObject->Rotate(0, -bossRotBetweenAngle, 0);
		}
		else {
			OnRight ? m_MonsterSession.m_currentPlayGameObject->m_interpolationRotateAngleY = bossRotBetweenAngle :
				m_MonsterSession.m_currentPlayGameObject->m_interpolationRotateAngleY = -bossRotBetweenAngle;
		}


		//float bossRotBetweenAngle = -recvPacket->bossState.rot.y - bossRot.y;
		//cout << "recv Boss rot Y: " << -recvPacket->bossState.rot.y << endl;
		//cout << "currentBos rot Y: " << bossRot.y << endl;
		//cout << "bossBetweenAngle: " << bossRotBetweenAngle << endl;
		//if (bossInterpolationAngle < DBL_EPSILON || bossInterpolationDistance < 0)
		//	m_MonsterSession.m_currentPlayGameObject->m_interpolationRotateAngleY = 0.0;
		//else if (abs(bossInterpolationAngle) > 5.0f) {
		//	m_MonsterSession.m_currentPlayGameObject->Rotate(0, bossRotBetweenAngle, 0);
		//}
		//else {
		//	m_MonsterSession.m_currentPlayGameObject->m_interpolationRotateAngleY = bossRotBetweenAngle;
		//}


		//pos
		if (bossPosDistance < DBL_EPSILON) {
			m_MonsterSession.m_currentPlayGameObject->m_interpolationDistance = 0.0f;
		}
		else if (bossInterpolationDistance < 0) {
			m_MonsterSession.m_currentPlayGameObject->m_interpolationDistance = 0.0f;
			m_MonsterSession.m_currentPlayGameObject->m_interpolationVector = XMFLOAT3{ 0,0,0 };
		}
		else if (abs(bossInterpolationDistance) > 10.0f) {
			m_MonsterSession.m_currentPlayGameObject->SetPosition(recvPacket->bossState.pos);
		}
		else if (abs(bossInterpolationDistance) < 5.0f) {
			m_MonsterSession.m_currentPlayGameObject->m_interpolationDistance = 0.0f;
		}
		else {
			m_MonsterSession.m_currentPlayGameObject->m_interpolationDistance = abs(bossInterpolationDistance);
			m_MonsterSession.m_currentPlayGameObject->m_interpolationVector = Vector3::Normalize(bossInterpolationVector);
		}


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
				utc_clock::time_point playerCurrentUTC_Time = utc_clock::now();
				XMFLOAT3 playerPos = findRes->m_currentPlayGameObject->GetPosition();

				XMFLOAT3 playerInterpolationVector = Vector3::Subtract(recvPacket->userState[i].pos, playerPos);

				//cout << "current UTC: " << playerCurrentUTC_Time << endl;
				double playerDurationTime = duration_cast<microseconds>(playerCurrentUTC_Time - recvPacket->time).count();
				playerDurationTime = (double)playerDurationTime / 1000.0f;//microseconds to mill
				//cout << "duration UTC: " << playerDurationTime << "mili" << endl;
				playerDurationTime = (double)playerDurationTime / 1000.0f;//milliseconds to sec
				//cout << "duration UTC: " << playerDurationTime << "second" << endl;

				float playerPosDistance = Vector3::Length(playerInterpolationVector);
				float playerInterpolationDistance = playerPosDistance - (float)playerDurationTime * 50.0f;//length - v*t

				//	cout << "player PosDistance: " << playerPosDistance << endl;
				//	cout << "player InterplationDis: " << playerInterpolationDistance << endl;

				if (playerPosDistance < DBL_EPSILON) {
					findRes->m_currentPlayGameObject->m_interpolationDistance = 0.0f;
				}
				else if (bossInterpolationDistance < 0) {
					findRes->m_currentPlayGameObject->m_interpolationDistance = 0.0f;
					findRes->m_currentPlayGameObject->m_interpolationVector = XMFLOAT3{ 0,0,0 };
				}
				else if (abs(playerInterpolationDistance) > 10.0f) {
					//cout << "client playerPos: " << playerPos.x << ", " << playerPos.z << endl;
					//cout << "server playerPos: " << recvPacket->userState[i].pos.x << ", " << recvPacket->userState[i].pos.z << endl;
					findRes->m_currentPlayGameObject->SetPosition(recvPacket->userState[i].pos);
				}
				else if (abs(playerInterpolationDistance) < 5.0f) {
					findRes->m_currentPlayGameObject->m_interpolationDistance = 0.0f;
				}
				else {
					findRes->m_currentPlayGameObject->m_interpolationDistance = abs(playerInterpolationDistance);
					findRes->m_currentPlayGameObject->m_interpolationVector = Vector3::Normalize(playerInterpolationVector);
				}
				float maxHp = findRes->m_currentPlayGameObject->GetMaxCurrentHP();
				findRes->m_currentPlayGameObject->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
				//switch (findRes->GetRole())
				//{
				//case ROLE::ARCHER: 
				//{

				//}
				//break;
				//case ROLE::PRIEST: break;
				//case ROLE::TANKER: break;
				//case ROLE::WARRIOR: break;
				//}

				//recvPacket->userState[i].hp;
				//recvPacket->userState[i].rot;
			}
		}
	}
	break;
	case SERVER_PACKET::BOSS_ATTACK:
	{
		auto durationTime = chrono::duration_cast<milliseconds>(chrono::high_resolution_clock::now() - attckPacketRecvTime);
		attckPacketRecvTime = chrono::high_resolution_clock::now();
		cout << "Attack Packet recv ElapsedTime: " << durationTime << endl;
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
	default:
	{
		std::cout << "Unknown Packet Recv" << std::endl;
	}
	break;
	}
}

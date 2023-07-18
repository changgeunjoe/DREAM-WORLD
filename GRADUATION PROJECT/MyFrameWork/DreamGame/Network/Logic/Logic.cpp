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
		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->AddDirection(recvPacket->direction);
		possessObj->SetMoveState(true);
	}
	break;
	case SERVER_PACKET::MOVE_KEY_UP:
	{
		SERVER_PACKET::MovePacket* recvPacket = reinterpret_cast<SERVER_PACKET::MovePacket*>(p);
		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->RemoveDIrection(recvPacket->direction);
	}
	break;
	case SERVER_PACKET::ROTATE:
	{
		SERVER_PACKET::RotatePacket* recvPacket = reinterpret_cast<SERVER_PACKET::RotatePacket*>(p);
		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		XMFLOAT3 rotateAngle = possessObj->GetRotateAxis();
		switch (recvPacket->axis)
		{
		case ROTATE_AXIS::X:
		{
			//findRes->m_ownerRotateAngle.x = recvPacket->angle;
			//findRes->m_currentPlayGameObject->SetProjectileY(findRes->m_ownerRotateAngle.x);
		}
		break;
		case ROTATE_AXIS::Y:
		{
			possessObj->Rotate(&upVec, recvPacket->angle);
			rotateAngle.y += recvPacket->angle;
			possessObj->SetRotateAxis(rotateAngle);
			//findRes->m_ownerRotateAngle.y += recvPacket->angle;
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
	break;
	case SERVER_PACKET::STOP:
	{
		XMFLOAT3 upVec = XMFLOAT3(0, 1, 0);
		XMFLOAT3 rightVec = XMFLOAT3(1, 0, 0);
		XMFLOAT3 dirVec = XMFLOAT3(0, 0, 1);
		SERVER_PACKET::StopPacket* recvPacket = reinterpret_cast<SERVER_PACKET::StopPacket*>(p);
		if ((ROLE)recvPacket->role != myRole) {
			Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
			possessObj->SetStopDirection();
			possessObj->SetPosition(recvPacket->position);
			possessObj->SetMoveState(false);
		}
	}
	break;
	case SERVER_PACKET::LOGIN_OK:
	{
		SERVER_PACKET::LoginPacket* recvPacket = reinterpret_cast<SERVER_PACKET::LoginPacket*>(p);
		wstring wst_name = recvPacket->name;
		myId = recvPacket->userID;
		//m_inGamePlayerSession[0].m_id = myId = recvPacket->userID;
		//m_inGamePlayerSession[0].SetName(wst_name);
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
		cout << "AddPlayer role: " << recvPacket->role << endl;
		cout << "myID: " << myId << endl;
#endif
		GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->SetPosition(recvPacket->position);
		possessObj->Rotate(&upVec, recvPacket->rotate.y);
		//이름 세팅 함수 필요
		if (recvPacket->userId == myId) {
			myRole = (ROLE)recvPacket->role;
			gGameFramework.GetScene()->GetObjectManager()->SetPlayerCamera(possessObj);
			//camera Set func필요
			//m_inGamePlayerSession[0].SetName(recvPacket->name);
		}
	}
	break;
	case SERVER_PACKET::MOUSE_INPUT:
	{
		SERVER_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<SERVER_PACKET::MouseInputPacket*>(p);
		GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		if (recvPacket->LClickedButton == true) {
			possessObj->SetLButtonClicked(true);
		}
		else {
			possessObj->SetLButtonClicked(false);
		}

		if (recvPacket->RClickedButton == true) {
			possessObj->SetRButtonClicked(true);
		}
		else {
			possessObj->SetRButtonClicked(false);
			possessObj->RbuttonUp();
		}

		possessObj->m_LMouseInput = recvPacket->LClickedButton;
		possessObj->m_RMouseInput = recvPacket->RClickedButton;
	}
	break;
	case SERVER_PACKET::INTO_GAME:
	{
		//로딩창이면 좋을듯?
		gGameFramework.m_bLobbyScene = false;
		//gGameFramework		
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
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->m_xmf3Destination = recvPacket->desPos;
		if (!bossMonster->GetMoveState())
		{
			bossMonster->SetMoveState(true);
			bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_MOVE;
			bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_MOVE, 2);
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
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->userState[i].role);
				possessObj->InterpolateMove(recvPacket->time, recvPacket->userState[i].pos);
				float maxHp = possessObj->GetMaxCurrentHP();
				possessObj->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
			}
		}
		//small monster
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		for (int i = 0; i < 15; i++) {
			smallMonsterArr[i]->InterpolateMove(recvPacket->time, recvPacket->smallMonster[i].pos);
			smallMonsterArr[i]->SetLook(recvPacket->smallMonster[i].directionVector);
			if (smallMonsterArr[i]->GetCurrentHP() < 0.0f) {
				smallMonsterArr[i]->SetCurrentHP(recvPacket->smallMonster[i].hp);
				float maxHp = smallMonsterArr[i]->GetMaxCurrentHP();
				smallMonsterArr[i]->SetCurrentHP(recvPacket->smallMonster[i].hp / maxHp * 100.0f);
				if (recvPacket->smallMonster[i].idxSize == 0) {
					smallMonsterArr[i]->ResetNearMonster();
				}
				else {
					std::set<int> nearSet;
					for (int n = 0; n < recvPacket->smallMonster[i].idxSize; n++) {
						nearSet.insert(recvPacket->smallMonster[i].nearIdx[n]);
					}
					smallMonsterArr[i]->SetNearMonster(nearSet);
				}
			}
		}
	}
	break;
	case SERVER_PACKET::SMALL_MONSTER_MOVE:
	{
		SERVER_PACKET::SmallMonsterMovePacket* recvPacket = reinterpret_cast<SERVER_PACKET::SmallMonsterMovePacket*>(p);
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		for (int i = 0; i < 15; i++) {
			smallMonsterArr[i]->SetDesPos(recvPacket->desPositions[i]);
			if (smallMonsterArr[i]->GetCurrentHP() < 0.0f) {
			}
		}
	}
	break;
	case SERVER_PACKET::GAME_STATE_B:
	{
		//std::cout << "ProcessPacket()::SERVER_PACKET::GAME_STATE" << std::endl;
		SERVER_PACKET::GameState_BOSS* recvPacket = reinterpret_cast<SERVER_PACKET::GameState_BOSS*>(p);
		if (recvPacket->bossState.hp != 2500) {
			//std::cout << "ProcessPacket()::SERVER_PACKET::GAME_STATE - Boss HP: " << recvPacket->bossState.hp << std::endl;

		}
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->m_UIScale = static_cast<float>(recvPacket->bossState.hp) / 250.0f;//maxHp 2500입니다
		bossMonster->SetCurrentHP(static_cast<float>(recvPacket->bossState.hp) / 25.0f);//maxHp 2500입니다
		if (bossMonster->GetCurrentHP() < FLT_EPSILON)
		{
			GameEnd = true;
			break;
		}
		bossMonster->InterpolateMove(recvPacket->time, recvPacket->bossState.pos);
		//Player Session
		for (int i = 0; i < 4; i++) {//그냥 4개 여서 도는 for문 주의			
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->userState[i].role);
				possessObj->InterpolateMove(recvPacket->time, recvPacket->userState[i].pos);
				float maxHp = possessObj->GetMaxCurrentHP();
				possessObj->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
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
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();

		if (!GameEnd)
		{
			switch (recvPacket->bossAttackType)
			{
			case BOSS_ATTACK::ATTACK_PUNCH:
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_RIGHT_PUNCH)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_RIGHT_PUNCH;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_RIGHT_PUNCH, 2);
				}
				break;
			case BOSS_ATTACK::ATTACK_SPIN:
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_SPIN_ATTACK)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_SPIN_ATTACK;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_SPIN_ATTACK, 2);
				}
				break;
			case BOSS_ATTACK::ATTACK_KICK:
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_KICK_ATTACK)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_KICK_ATTACK;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_KICK_ATTACK, 2);
				}
				break;
			case BOSS_ATTACK::SKILL_DASH:
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_DASH_SKILL)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_DASH_SKILL;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_DASH_SKILL, 2);
					XMFLOAT3 xmf3BossPos = bossMonster->GetPosition();
					XMFLOAT3 xmf3BossLook = bossMonster->GetLook();
					XMFLOAT3 xmf3SkillPos = Vector3::Add(xmf3BossPos, xmf3BossLook, 160.0f);	// 보스가 스킬 써서 이동하는 거리의 1/2만큼 이동
					static_cast<Monster*>(bossMonster)->m_pSkillRange->m_bActive = true;
					static_cast<Monster*>(bossMonster)->m_pSkillRange->SetPosition(xmf3SkillPos);
				}
				break;
			case BOSS_ATTACK::SKILL_PUNCH:
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_PUNCHING_SKILL)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_PUNCHING_SKILL;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_PUNCHING_SKILL, 2);
				}
				break;
			case BOSS_ATTACK::SKILL_CASTING:
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_CAST_SPELL)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_CAST_SPELL;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_CAST_SPELL, 2);
				}
				break;
			}
			bossMonster->SetMoveState(false);
			cout << "ProcessPacket::SERVER_PACKET::BOSS_ATTACK - recvPacket: " << (int)recvPacket->bossAttackType << endl;
		}
	}
	break;
	case SERVER_PACKET::HIT_BOSS_MAGE:
	{
		/*	SERVER_PACKET::BossHitObject* recvPacket = reinterpret_cast<SERVER_PACKET::BossHitObject*>(p);
			m_MonsterSession.m_currentPlayGameObject->m_xmfHitPosition = recvPacket->pos;*/
	}
	break;
	case SERVER_PACKET::GAME_END:
	{
		//게임 종료 패킷 수신
		//지금은 바로 게임 종료 확인하는 패킷 서버로 전송하게 구현함
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->SetCurrentHP(0.0f);
		GameEnd = true;

	}
	break;
	case SERVER_PACKET::BOSS_MOVE_NODE:
	{
		// 현재 시점에서 경로 Clear
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		std::queue<int> emptyQueue;
		std::swap(bossMonster->m_BossRoute, emptyQueue);


		SERVER_PACKET::BossMoveNodePacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossMoveNodePacket*>(p);
		if (gGameFramework.m_pScene == nullptr) return;
		std::queue<int> recvNodeQueue;
		//Role로 변경했음 이거 참고 바람
		bossMonster->m_roleDesPlayer = recvPacket->targetRole;
		//std::cout << "recv aggro Id: " << recvPacket->desPlayerId << std::endl;
		if (recvPacket->nodeCnt == -1) {
			bossMonster->m_lockBossRoute.lock();
			bossMonster->m_BossRoute.swap(recvNodeQueue);
			bossMonster->m_lockBossRoute.unlock();
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
			bossMonster->m_lockBossRoute.lock();
			bossMonster->m_BossRoute.swap(recvNodeQueue);
			bossMonster->m_lockBossRoute.unlock();
		}

		if (!bossMonster->GetMoveState())
		{
			cout << "SERVER_PACKET::BOSS_MOVE_NODE - SetMoveState True" << endl;
			bossMonster->SetMoveState(true);
			bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_MOVE;
			bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_MOVE, 2);
		}
		bossMonster->SetMoveState(true);
		cout << endl;
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
	//case SERVER_PACKET::DUPLICATED_LOGIN://접속해 있었지만(내 클라이언트) 중복로그인을 함 - 기능 삭제
	//{
	//	EndDialog(g_wnd, IDCANCEL);

	//}
	//break;
	case SERVER_PACKET::SKILL_INPUT:
	{
		SERVER_PACKET::SkillInputPacket* recvPacket = reinterpret_cast<SERVER_PACKET::SkillInputPacket*>(p);
		Character* possessChracter = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->role);
		if (recvPacket->qSkill == true) {
			possessChracter->FirstSkillDown();
		}
		if (recvPacket->eSkill == true) {
			possessChracter->SecondSkillDown();
		}
	}
	break;
	case SERVER_PACKET::STAGE_CHANGING_BOSS:
	{
		//키 인풋 막아주기
		//로딩 창 띄워주기

	}
	break;
	case SERVER_PACKET::STAGE_START_BOSS:
	{
		//로딩 창 지우기
		//키 인풋 할 수 있게 변경
		SERVER_PACKET::GameState_BOSS_INIT* recvPacket = reinterpret_cast<SERVER_PACKET::GameState_BOSS_INIT*>(p);
		for (int i = 0; i < 4; i++) {//그냥 4개 여서 도는 for문 주의			
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->userState[i].role);
				possessObj->SetPosition(recvPacket->userState[i].pos);
				float maxHp = possessObj->GetMaxCurrentHP();
				possessObj->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
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

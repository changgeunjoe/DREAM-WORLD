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
	if (GameEnd) return;
	switch (p[2])
	{
	case SERVER_PACKET::MOVE_KEY_DOWN:
	{
		SERVER_PACKET::MovePacket* recvPacket = reinterpret_cast<SERVER_PACKET::MovePacket*>(p);
		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->AddDirection(recvPacket->direction);
		possessObj->SetMoveState(true);
		//possessObj->InterpolateMove(recvPacket->time, recvPacket->position, recvPacket->moveVec);
		//params serverTime, serverPos, serverMoveVec

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
		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->SetStopDirection();
		possessObj->SetPosition(recvPacket->position);
		possessObj->SetMoveState(false);
		//if ((ROLE)recvPacket->role != myRole) {
		//}
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
		//�̸� ���� �Լ� �ʿ�
		if (recvPacket->userId == myId) {
			myRole = (ROLE)recvPacket->role;
			gGameFramework.GetScene()->GetObjectManager()->SetPlayerCamera(possessObj);
			//camera Set func�ʿ�
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
		//�ε�â�̸� ������?
		gGameFramework.m_bLobbyScene = false;
		gGameFramework.GetScene()->GetObjectManager()->SetCharactersStage1Postion();
		gGameFramework.GetLobbyScene()->GetObjectManager()->ResetLobbyUI();
	}
	break;
	case SERVER_PACKET::BOSS_CHANGE_STATE_MOVE_DES:
	{
		std::cout << "ProcessPacket()::SERVER_PACKET::BOSS_CHANGE_STATE_MOVE_DES" << std::endl;
		SERVER_PACKET::BossChangeStateMovePacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossChangeStateMovePacket*>(p);
		auto durationTime = chrono::duration_cast<milliseconds>(chrono::high_resolution_clock::now() - attckPacketRecvTime);
		//attckPacketRecvTime = chrono::high_resolution_clock::now();		
		//cout << "boss Move Packet Recv ElapsedTime: " << durationTime << endl;
		recvPacket->desPos; //���� ������ ���� ���� ���� �̵� ��Ű�� �˴ϴ�
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
	case SERVER_PACKET::SHOOTING_ARROW://ȭ��
	{
		SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<SERVER_PACKET::ShootingObject*>(p);

		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(ROLE::ARCHER);
		static_cast<Archer*>(possessObj)->ShootArrow(recvPacket->dir);
	}
	break;
	case SERVER_PACKET::SHOOTING_BALL://��
	{
		SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<SERVER_PACKET::ShootingObject*>(p);

		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(ROLE::PRIEST);
		static_cast<Priest*>(possessObj)->Attack(recvPacket->dir);
	}
	break;
	case SERVER_PACKET::EXECUTE_LIGHTNING:
	{
		SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<SERVER_PACKET::ShootingObject*>(p);
		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(ROLE::PRIEST);
		gGameFramework.m_pScene->m_pObjectManager->SetLightningEffect(recvPacket->dir);
	}
	break;
	case SERVER_PACKET::GAME_STATE_S:
	{
		SERVER_PACKET::GameState_STAGE1* recvPacket = reinterpret_cast<SERVER_PACKET::GameState_STAGE1*>(p);
		//Player Session
		//auto clientUtcTime = std::chrono::utc_clock::now();		
		//std::cout << "Logic::ProcessPacket() - SERVER_PACKET::GAME_STATE_S, utcTime: " << clientUtcTime << std::endl;
		for (int i = 0; i < 4; i++) {//�׳� 4�� ���� ���� for�� ����
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->userState[i].role);
				possessObj->InterpolateMove(recvPacket->userTime, recvPacket->userState[i].pos, recvPacket->userState[i].moveVec);
				float maxHp = possessObj->GetMaxHP();
				possessObj->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
			}
		}
		//small monster		
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		for (int i = 0; i < 15; i++) {
			smallMonsterArr[i]->InterpolateMove(recvPacket->monsterTime, recvPacket->smallMonster[i].pos, recvPacket->smallMonster[i].moveVec);
			//smallMonsterArr[i]->SetLook(recvPacket->smallMonster[i].moveVec);
			//smallMonsterArr[i]->SetCurrentHP(recvPacket->smallMonster[i].hp);
			//float maxHp = smallMonsterArr[i]->GetMaxCurrentHP(); //conflict
			float maxHp = smallMonsterArr[i]->GetMaxHP();
			smallMonsterArr[i]->SetCurrentHP(recvPacket->smallMonster[i].hp / maxHp * 100.0f);
			if(recvPacket->smallMonster[i].hp == 150)
				smallMonsterArr[i]->SetTempHp(recvPacket->smallMonster[i].hp / maxHp * 100.0f);
			smallMonsterArr[i]->SetAliveState(recvPacket->smallMonster[i].isAlive);

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
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		float fMaxHp = bossMonster->GetMaxHP();

		bossMonster->m_UIScale = static_cast<float>(recvPacket->bossState.hp) / fMaxHp * 100.0f;//maxHp 2500�Դϴ�
		bossMonster->SetCurrentHP(static_cast<float>(recvPacket->bossState.hp) / fMaxHp * 100.0f);//maxHp 2500�Դϴ�
		if (recvPacket->bossState.hp == 6500)
			bossMonster->SetTempHp(recvPacket->bossState.hp / fMaxHp * 100.0f);

		//bossMonster->m_desDirecionVec = recvPacket->bossState.moveVec;
		//bossMonster->m_serverDesDirecionVec = recvPacket->bossState.desVec;

		if (bossMonster->GetCurrentHP() < FLT_EPSILON)
		{
			GameEnd = true;
			break;
		}
		bossMonster->InterpolateMove(recvPacket->time, recvPacket->bossState.pos, recvPacket->bossState.moveVec);
		//Player Session
		for (int i = 0; i < 4; i++) {//�׳� 4�� ���� ���� for�� ����			
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->userState[i].role);
				possessObj->InterpolateMove(recvPacket->time, recvPacket->userState[i].pos, recvPacket->userState[i].moveVec);
				float maxHp = possessObj->GetMaxHP();
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
			{
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_RIGHT_PUNCH)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_RIGHT_PUNCH;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_RIGHT_PUNCH, 2);
				}
			}
			break;
			case BOSS_ATTACK::ATTACK_SPIN:
			{
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_SPIN_ATTACK)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_SPIN_ATTACK;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_SPIN_ATTACK, 2);
				}
			}
			break;
			case BOSS_ATTACK::ATTACK_KICK:
			{
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_KICK_ATTACK)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_KICK_ATTACK;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_KICK_ATTACK, 2);
				}
			}
			break;
			//case BOSS_ATTACK::SKILL_DASH:
			//{
			//	if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_DASH_SKILL)
			//	{
			//		bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_DASH_SKILL;
			//		bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_DASH_SKILL, 2);
			//		XMFLOAT3 xmf3BossPos = bossMonster->GetPosition();
			//		XMFLOAT3 xmf3BossLook = bossMonster->GetLook();
			//		XMFLOAT3 xmf3SkillPos = Vector3::Add(xmf3BossPos, xmf3BossLook, 160.0f);	// ������ ��ų �Ἥ �̵��ϴ� �Ÿ��� 1/2��ŭ �̵�
			//		static_cast<Monster*>(bossMonster)->m_pSkillRange->m_bActive = true;
			//		static_cast<Monster*>(bossMonster)->m_pSkillRange->SetPosition(xmf3SkillPos);
			//	}
			//}
			//	break;
			//case BOSS_ATTACK::SKILL_PUNCH:
			//{
			//	if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_PUNCHING_SKILL)
			//	{
			//		bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_PUNCHING_SKILL;
			//		bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_PUNCHING_SKILL, 2);
			//	}
			//}
			//break;
			//case BOSS_ATTACK::SKILL_CASTING:
			//{
			//	if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_CAST_SPELL)
			//	{
			//		bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_CAST_SPELL;
			//		bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_CAST_SPELL, 2);
			//	}
			//}
			//break;
			case BOSS_ATTACK::ATTACK_FLOOR_BOOM:
			{
				if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_CAST_SPELL)
				{
					bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_CAST_SPELL;
					bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_CAST_SPELL, 2);
					bossMonster->m_pSkillRange->m_bActive = true;
					bossMonster->m_pSkillRange->m_bBossSkillActive = true;
					bossMonster->m_pSkillRange->m_fBossSkillTime = gGameFramework.GetScene()->GetObjectManager()->GetTotalProgressTime();
					bossMonster->m_pSkillRange->SetPosition(bossMonster->GetPosition());
				}
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
		//���� ���� ��Ŷ ����
		//������ �ٷ� ���� ���� Ȯ���ϴ� ��Ŷ ������ �����ϰ� ������
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->SetCurrentHP(0.0f);
		GameEnd = true;


	}
	break;
	case SERVER_PACKET::BOSS_MOVE_NODE:
	{
		// ���� �������� ��� Clear
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();


		SERVER_PACKET::BossMoveNodePacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossMoveNodePacket*>(p);
		if (gGameFramework.m_pScene == nullptr) return;

		//Role�� �������� �̰� ���� 		
		bossMonster->m_roleDesPlayer = recvPacket->targetRole;
		////std::cout << "recv aggro Id: " << recvPacket->desPlayerId << std::endl;
		//if (recvPacket->nodeCnt == -1) {		
		//}
		//else if (recvPacket->nodeCnt > 0) {
		//	cout << "���� �̵� �ε��� : ";
		//	vector<int> triangleIdxVec;
		//	std::list<int> recvNodeQueue;
		//	for (int i = 0; i < recvPacket->nodeCnt; i++) {
		//		//������ �̵��� ��� ������
		//		// �޾ƿ� ���� ���Ϳ� ���� �ֱ�
		//		cout << recvPacket->node[i] << ", ";
		//		recvNodeQueue.push_back(recvPacket->node[i]);
		//		triangleIdxVec.push_back(recvPacket->node[i]);
		//	}
		//	bossMonster->m_astarIdx = recvPacket->node[0];
		//	cout << endl;
		//	//gGameFramework.m_pScene->m_pObjectManager->m_VecNodeQueue.push_back(recvPacket->node[i]);
		//	//AStart Node Mesh
		//	gGameFramework.m_pScene->m_pObjectManager->m_nodeLock.lock();
		//	gGameFramework.m_pScene->m_pObjectManager->m_VecNodeQueue.swap(triangleIdxVec);
		//	gGameFramework.m_pScene->m_pObjectManager->m_nodeLock.unlock();
		//
		//	//boss Move Node Data
		//	bossMonster->m_lockBossRoute.lock();
		//	bossMonster->m_BossRoute = recvNodeQueue;
		//	bossMonster->m_lockBossRoute.unlock();
		//}

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
	case SERVER_PACKET::PRE_EXIST_LOGIN://�̹� �����ϴ� �÷��̾ �ֱ� ������, ���� ���� �÷��̾�(�� Ŭ���̾�Ʈ��) ���� ���� ��Ŷ�� ����
	{
		//�˸� �޼��� ����, �ٽ� ������ �ؾ��ϳ�...;;
		EndDialog(loginWnd, IDCANCEL);
		//ShowWindow(g_wnd, g_cmd);//�� �����찡 �ƴ� �α��� �ߺ� Ȯ��â ����� ��
		//UpdateWindow(g_wnd);
	}
	break;
	//case SERVER_PACKET::DUPLICATED_LOGIN://������ �־�����(�� Ŭ���̾�Ʈ) �ߺ��α����� �� - ��� ����
	//{
	//	EndDialog(g_wnd, IDCANCEL);

	//}
	//break;	
	case SERVER_PACKET::STAGE_CHANGING_BOSS:
	{
		//Ű ��ǲ �����ֱ�
		//�ε� â ����ֱ�
		gGameFramework.GetScene()->GetObjectManager()->SetPortalCheck(true);
	}
	break;
	case SERVER_PACKET::STAGE_START_BOSS:
	{
		//�ε� â �����
		//Ű ��ǲ �� �� �ְ� ����
		SERVER_PACKET::GameState_BOSS_INIT* recvPacket = reinterpret_cast<SERVER_PACKET::GameState_BOSS_INIT*>(p);
		for (int i = 0; i < 4; i++) {//�׳� 4�� ���� ���� for�� ����			
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->userState[i].role);
				possessObj->SetPosition(recvPacket->userState[i].pos);
				float maxHp = possessObj->GetMaxHP();
				possessObj->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
			}
		}
	}
	break;
	case SERVER_PACKET::HEAL_START:
	{
		//������ ����Ʈ ����

		Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::PRIEST);
		possessObj->StartEffect(0);

	}
	break;
	case SERVER_PACKET::HEAL_END:
	{
		Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::PRIEST);
		possessObj->EndEffect(0);
		//������ ����Ʈ ��
	}
	break;
	case SERVER_PACKET::SHIELD_START:
	{
		//���� ����Ʈ ��� �ؼ� ����
		Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
		possessObj->StartEffect(0);
	}
	break;
	case SERVER_PACKET::SHIELD_END:
	{
		//���� ��
		Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
		possessObj->EndEffect(0);
	}
	break;
	case SERVER_PACKET::NOTIFY_HEAL_HP:
	{
		SERVER_PACKET::NotifyHealPacket* recvPacket = reinterpret_cast<SERVER_PACKET::NotifyHealPacket*>(p);
		for (int i = 0; i < 4; i++) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->applyHealPlayerInfo[i].role);
			if (possessObj)
			{
				float maxHp = possessObj->GetMaxHP();
				possessObj->SetCurrentHP(recvPacket->applyHealPlayerInfo[i].hp / maxHp * 100.0f);
			}
		}
	}
	break;
	case SERVER_PACKET::NOTIFY_SHIELD_APPLY:
	{
		SERVER_PACKET::NotifyShieldPacket* recvPacket = reinterpret_cast<SERVER_PACKET::NotifyShieldPacket*>(p);
		for (int i = 0; i < 4; i++) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->applyShieldPlayerInfo[i].role);
			if (possessObj)
			{
				possessObj->SetShield(recvPacket->applyShieldPlayerInfo[i].shield);	//�ִ� �ǵ� 200
				possessObj->m_pHPBarUI->SetShield(recvPacket->applyShieldPlayerInfo[i].shield / possessObj->GetMaxHP() * 100.0f);
				possessObj->m_pHPBarUI->SetCurrentHP(possessObj->GetCurrentHP() / possessObj->GetMaxHP() * 100.0f);
			}
		}
	}
	break;
	case SERVER_PACKET::FIRST_SEND:
	{
		g_NetworkHelper.SendFirstPacket();
	}
	break;
	case SERVER_PACKET::TIME_SYNC:
	{
		SERVER_PACKET::TimeSyncPacket* recvPacket = reinterpret_cast<SERVER_PACKET::TimeSyncPacket*>(p);
		auto clientCurrentTime = std::chrono::utc_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::microseconds>(clientCurrentTime - recvPacket->serverTime).count();
		g_NetworkHelper.SendAdaptTime(diff, clientCurrentTime);
	}
	break;

	case SERVER_PACKET::NOTIFY_LATENCY:
	{
		SERVER_PACKET::TimeLatencyNotifyPacket* recvPacket = reinterpret_cast<SERVER_PACKET::TimeLatencyNotifyPacket*>(p);
		C2S_DiffTime = recvPacket->diff;
	}
	break;
	case SERVER_PACKET::PLAYER_ATTACK_RESULT:
	{
		SERVER_PACKET::PlayerAttackMonsterDamagePacket* recvPacket = reinterpret_cast<SERVER_PACKET::PlayerAttackMonsterDamagePacket*>(p);
		recvPacket->role;//�� �����̸�
		recvPacket->damage;//�������� ����ϼ����� -> ��� ������ �� ����
		//for (int i = 0; i < recvPacket->attackedMonsterCnt; i++) {//���� �� ��Ŷ�� ���� ����
		//	recvPacket->monsterIdx[i];//�迭 15��¥�� ���� idx�迭  
		//	gGameFramework.GetScene()->GetObjectManager()->AddDamageFontToUiLayer(XMFLOAT3(
		//		gGameFramework.GetScene()->GetObjectManager()->m_ppNormalMonsterObject[recvPacket->monsterIdx[i]]->GetPosition().x,
		//		gGameFramework.GetScene()->GetObjectManager()->m_ppNormalMonsterObject[recvPacket->monsterIdx[i]]->GetPosition().y+20,
		//		gGameFramework.GetScene()->GetObjectManager()->m_ppNormalMonsterObject[recvPacket->monsterIdx[i]]->GetPosition().z
		//	), recvPacket->damage);
		//}
		recvPacket->monsterIdx;//�迭 15��¥�� ���� idx�迭
		recvPacket->attackedMonsterCnt;//���� �� ��Ŷ�� ���� ����
	}
	break;
	case SERVER_PACKET::SMALL_MONSTER_ATTACK:
	{
		SERVER_PACKET::SmallMonsterAttackPlayerPacket* recvPacket = reinterpret_cast<SERVER_PACKET::SmallMonsterAttackPlayerPacket*>(p);
		if (recvPacket->attackedRole == myRole) {
			//�ǰ� �����̻� ����
		}

		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		smallMonsterArr[recvPacket->attackMonsterIdx]->SetOnAttack(true); //��� �÷��̾ ���ؼ� �� ���� ���� �ִϸ��̼� �缺
	}
	break;
	case SERVER_PACKET::COMMON_ATTACK_START:
	{
		//SERVER_PACKET::CommonAttackPacket* recvPacket = reinterpret_cast<SERVER_PACKET::CommonAttackPacket*>(p);
		//Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->role);
		//possessObj->SetLButtonClicked(true);
	}
	break;
	case SERVER_PACKET::START_ANIMATION_Q:
	{
		SERVER_PACKET::CommonAttackPacket* recvPacket = reinterpret_cast<SERVER_PACKET::CommonAttackPacket*>(p);
		Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->FirstSkillDown();
	}
	break;
	case SERVER_PACKET::START_ANIMATION_E:
	{
		SERVER_PACKET::CommonAttackPacket* recvPacket = reinterpret_cast<SERVER_PACKET::CommonAttackPacket*>(p);
		Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->SecondSkillDown();
	}
	break;
	//projectile packet
	case SERVER_PACKET::MONSTER_DAMAGED_ARROW:
	{
		SERVER_PACKET::ProjectileDamagePacket* recvPacket = reinterpret_cast<SERVER_PACKET::ProjectileDamagePacket*>(p);
		if (myRole == ROLE::ARCHER) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);
			possessObj->m_ppProjectiles[recvPacket->projectileId]->m_bActive = false;

			recvPacket->damage;//������
			recvPacket->position;//������ ��Ʈ ��ġ
		}
		recvPacket->projectileId;//���� ȸ���� ȭ�� ���̵�
	}
	break;
	case SERVER_PACKET::MONSTER_DAMAGED_ARROW_SKILL:
	{
		SERVER_PACKET::ProjectileDamagePacket* recvPacket = reinterpret_cast<SERVER_PACKET::ProjectileDamagePacket*>(p);
		if (myRole == ROLE::ARCHER) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);
			static_cast<Archer*>(possessObj)->m_ppArrowForQSkill[recvPacket->projectileId]->m_bActive = false;


			recvPacket->damage;//������
			recvPacket->position;//������ ��Ʈ ��ġ
		}
		;//��� ��ų�ε� �ʿ� �Ϸ��� �𸣰���
	}
	break;
	case SERVER_PACKET::MONSTER_DAMAGED_BALL:
	{
		SERVER_PACKET::ProjectileDamagePacket* recvPacket = reinterpret_cast<SERVER_PACKET::ProjectileDamagePacket*>(p);
		if (myRole == ROLE::PRIEST) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::PRIEST);
			possessObj->m_ppProjectiles[recvPacket->projectileId]->m_bActive = false;

			recvPacket->damage;//������
			recvPacket->position;//������ ��Ʈ ��ġ
		}
		;//���� ȸ���� �������� ���̵�
	}
	break;
	case SERVER_PACKET::PLAYER_ATTACK_RESULT_BOSS:
	{
		SERVER_PACKET::PlayerAttackBossDamagePacket* recvPacket = reinterpret_cast<SERVER_PACKET::PlayerAttackBossDamagePacket*>(p);
		recvPacket->damage;//���� Ŭ���̾�Ʈ ���� ��ġ���ٰ� ��� ������ ��Ʈ�� ������
	}
	break;
	case SERVER_PACKET::BOSS_ATTACK_PALYER:
	{
		SERVER_PACKET::BossAttackPlayerPacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossAttackPlayerPacket*>(p);
		recvPacket->currentHp;//�������� �ǰݴ��Ͽ� ���� ���� HP�̰� �ʿ� �ֳ� ����... => ���߿� notift�� �����ҵ�?
	}
	break;
	case  SERVER_PACKET::BOSS_CHANGE_DIRECION:
	{
		SERVER_PACKET::BossDirectionPacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossDirectionPacket*>(p);
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->m_desDirecionVec = recvPacket->directionVec;
	}
	break;
	case SERVER_PACKET::METEO_PLAYER_ATTACK:
	{
		SERVER_PACKET::BossAttackPlayerPacket* recvPacket = reinterpret_cast<SERVER_PACKET::BossAttackPlayerPacket*>(p);
		GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(GetMyRole());
		if (possessObj != nullptr) {
			float maxHp = possessObj->GetMaxHP();
			possessObj->SetCurrentHP(recvPacket->currentHp / maxHp * 100.0f);
		}
	}
	break;
	case SERVER_PACKET::METEO_DESTROY:
	{
		SERVER_PACKET::DestroyedMeteoPacket* recvPacket = reinterpret_cast<SERVER_PACKET::DestroyedMeteoPacket*>(p);
		vector<RockSpike*> ppRockSpike = gGameFramework.GetScene()->GetObjectManager()->GetRockSpikeArr();
		if (ppRockSpike[recvPacket->idx] != nullptr)
			ppRockSpike[recvPacket->idx]->m_bActive = false;
		//�� ���׿� ������ off
	}
	break;
	case SERVER_PACKET::METEO_CREATE:
	{
		SERVER_PACKET::MeteoStartPacket* recvPacket = reinterpret_cast<SERVER_PACKET::MeteoStartPacket*>(p);
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		vector<RockSpike*> ppRockSpike = gGameFramework.GetScene()->GetObjectManager()->GetRockSpikeArr();
		bossMonster->SetMoveState(false);
		//���� �̵� ���߰� �ִϸ��̼� �������ּ���.
		if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_CAST_SPELL)
		{
			bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_CAST_SPELL;
			bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_CAST_SPELL, 2);
		}
		for (int i = 0; i < 10; i++) {
			if (ppRockSpike[i] == nullptr) continue;
			if (ppRockSpike[i]->m_bActive == true) continue;
			XMFLOAT3 rockPosition = recvPacket->meteoInfo[i].pos;
			ppRockSpike[i]->m_bActive = true;
			ppRockSpike[i]->SetPosition(rockPosition);
			ppRockSpike[i]->SetSpeed(recvPacket->meteoInfo[i].speed);
			rockPosition.y = 0.1;

			if (ppRockSpike[i]->m_pAttackedArea != nullptr)
				ppRockSpike[i]->m_pAttackedArea->SetPosition(rockPosition);
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
	// TODO: ���⿡ return ���� �����մϴ�.
}

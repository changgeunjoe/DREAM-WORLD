#include "stdafx.h"
#include "Logic.h"
//#include "../../../Server/IOCPNetwork/protocol/protocol.h"
#include "../../GameFramework.h"
#include "../../Projectile.h"
#include "../../Scene.h"
#include "../../GameobjectManager.h"
#include "../../Animation.h"
#include "../Room/RoomManger.h"
#include "../../Character.h"
#include "../../MeleeCharacter.h"
#include "../../RangedCharacter.h"
#include "../../sound/GameSound.h"
#include "../NetworkHelper.h"
#include"../../EffectObject.h"
#include "../../Player.h"
#include "../../Monster.h"
#include "../../../Server/Network/protocol/protocol.h"


extern CGameFramework gGameFramework;
extern RoomManger g_RoomManager;
extern NetworkHelper g_NetworkHelper;
extern GameSound g_sound;
extern bool GameEnd;

extern HWND g_wnd;
extern int g_cmd;

using namespace chrono;

Logic::Logic()
	:m_isReadySyncPacket(true)
{
	m_KeyInput = new CKeyInput();
	attckPacketRecvTime = chrono::high_resolution_clock::now();
	m_requestTime = std::chrono::high_resolution_clock::now();
	m_responseTime = std::chrono::high_resolution_clock::now();
}

Logic::~Logic()
{
	delete m_KeyInput;
}

void Logic::ProcessPacket(const PacketHeader* packetHeader)
{
	static XMFLOAT3 upVec = XMFLOAT3(0, 1, 0);
	if (GameEnd) return;
	if (gGameFramework.m_bLobbyScene)
	{
		switch (static_cast<SERVER_PACKET::TYPE>(packetHeader->type))
		{
		case SERVER_PACKET::TYPE::INTO_GAME:
		case SERVER_PACKET::TYPE::LOGIN_SUCCESS:
		case SERVER_PACKET::TYPE::ADD_PLAYER:
		case SERVER_PACKET::TYPE::PRE_EXIST_LOGIN:
		case SERVER_PACKET::TYPE::TIME_SYNC_RESPONSE:
			break;
		default:
			return;
		}
	}
	switch (static_cast<SERVER_PACKET::TYPE>(packetHeader->type))
	{
	case SERVER_PACKET::TYPE::MOVE_KEY_DOWN:
	{
		const SERVER_PACKET::MovePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::MovePacket*>(packetHeader);
		//if (recvPacket->role == myRole) return;
		Player* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->AddDirection(recvPacket->direction);
		possessObj->SetMoveState(true);
	}
	break;
	case SERVER_PACKET::TYPE::MOVE_KEY_UP:
	{
		const SERVER_PACKET::MovePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::MovePacket*>(packetHeader);
		//if (recvPacket->role == myRole) return;
		Player* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->RemoveDIrection(recvPacket->direction);
	}
	break;
	case SERVER_PACKET::TYPE::ROTATE:
	{
		const SERVER_PACKET::RotatePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::RotatePacket*>(packetHeader);
		Player* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
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
	case SERVER_PACKET::TYPE::STOP:
	{
		XMFLOAT3 upVec = XMFLOAT3(0, 1, 0);
		XMFLOAT3 rightVec = XMFLOAT3(1, 0, 0);
		XMFLOAT3 dirVec = XMFLOAT3(0, 0, 1);
		const SERVER_PACKET::StopPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::StopPacket*>(packetHeader);
		Player* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);

		possessObj->SetStopDirection();
		possessObj->SetMoveState(false);
	}
	break;
	case SERVER_PACKET::TYPE::LOGIN_SUCCESS:
	{
		const SERVER_PACKET::LoginPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::LoginPacket*>(packetHeader);
		wstring wst_name = recvPacket->nickName;
		//m_inGamePlayerSession[0].m_id = myId = recvPacket->userID;
		//m_inGamePlayerSession[0].SetName(wst_name);
		//gGameFramework.m_pScene->m_pObjectManager->SetPlayCharacter(&m_inGamePlayerSession[0]);
		EndDialog(loginWnd, IDCANCEL);
		ShowWindow(g_wnd, g_cmd);
		UpdateWindow(g_wnd);
#ifdef _DEBUG
		PrintCurrentTime();
		//std::wcout << "Logic::ProcessPacket() - const SERVER_PACKET::LOGIN_OK - " << "user Name: " << wst_name << std::endl;
		//std::wcout << "Logic::ProcessPacket() - const SERVER_PACKET::LOGIN_OK - " << "user ID: " << recvPacket->userID << std::endl;
#endif
	}
	break;
	case SERVER_PACKET::TYPE::ADD_PLAYER:
	{
		XMFLOAT3 rightVec = XMFLOAT3(1, 0, 0);
		XMFLOAT3 dirVec = XMFLOAT3(0, 0, 1);

		//const SERVER_PACKET::AddPlayerPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::AddPlayerPacket*>(packetHeader);
		//GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		//possessObj->SetPosition(recvPacket->position);
		//possessObj->Rotate(&upVec, recvPacket->rotate.y);
		////�̸� ���� �Լ� �ʿ�
		//if (recvPacket->userId == myId) {
		//	myRole = (ROLE)recvPacket->role;
		//GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		//	gGameFramework.GetScene()->GetObjectManager()->SetPlayerCamera(possessObj);
		//	//camera Set func�ʿ�
		//	//m_inGamePlayerSession[0].SetName(recvPacket->name);
		//}
	}
	break;
	case SERVER_PACKET::TYPE::MOUSE_INPUT:
	{
		const SERVER_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::MouseInputPacket*>(packetHeader);
		GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->role);
		if (recvPacket->leftClickedButton == true) {
			possessObj->SetLButtonClicked(true);
		}
		else {
			possessObj->SetLButtonClicked(false);
		}

		if (recvPacket->rightClickedButton == true) {
			possessObj->SetRButtonClicked(true);
		}
		else {
			possessObj->SetRButtonClicked(false);
			possessObj->RbuttonUp();
		}

		possessObj->m_LMouseInput = recvPacket->leftClickedButton;
		possessObj->m_RMouseInput = recvPacket->rightClickedButton;
	}
	break;
	case SERVER_PACKET::TYPE::INTO_GAME:
	{
		//�ε�â�̸� ������?
		const SERVER_PACKET::IntoGamePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::IntoGamePacket*>(packetHeader);
		auto normalMonsterObj = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		for (int i = 0; i < 15; ++i) {
			normalMonsterObj[recvPacket->monsterData[i].id]->SetLook(recvPacket->monsterData[i].lookVector);
			normalMonsterObj[recvPacket->monsterData[i].id]->SetPosition(recvPacket->monsterData[i].position);
			normalMonsterObj[recvPacket->monsterData[i].id]->SetMaxHP(recvPacket->monsterData[i].maxHp);
			normalMonsterObj[recvPacket->monsterData[i].id]->SetCurrentHP(100.0f);
			normalMonsterObj[recvPacket->monsterData[i].id]->SetTempHp(100.0f);
			//break;
		}
		GameObject* possessObj = gGameFramework.GetScene()->m_pObjectManager->GetChracterInfo(recvPacket->role);
		gGameFramework.GetScene()->GetObjectManager()->SetPlayerCamera(possessObj);
		gGameFramework.m_bLobbyScene = false;
		gGameFramework.GetScene()->GetObjectManager()->m_bSceneSwap = true;
		gGameFramework.GetScene()->GetObjectManager()->SetCharactersStage1Postion();
		gGameFramework.GetLobbyScene()->GetObjectManager()->ResetLobbyUI();
	}
	break;
	case SERVER_PACKET::TYPE::BOSS_CHANGE_STATE_MOVE_DES:
	{
		//const SERVER_PACKET::TYPE::BossChangeStateMovePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossChangeStateMovePacket*>(packetHeader);
		//auto durationTime = chrono::duration_cast<milliseconds>(chrono::high_resolution_clock::now() - attckPacketRecvTime);		
		//recvPacket->desPos; //���� ������ ���� ���� ���� �̵� ��Ű�� �˴ϴ�
		//recvPacket->bossPos;
		//recvPacket->t;
		//Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		//bossMonster->m_xmf3Destination = recvPacket->desPos;
		//if (!bossMonster->GetMoveState())
		//{
		//	bossMonster->SetMoveState(true);
		//	bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_MOVE;
		//	bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_MOVE, 2);
		//}
	}
	break;
	case SERVER_PACKET::TYPE::PLAYER_DIE:
	{
		const SERVER_PACKET::PlayerDiePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::PlayerDiePacket*>(packetHeader);
		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(recvPacket->role);
		possessObj->SetAliveState(false);
	}
	break;
	case SERVER_PACKET::TYPE::PLAYER_DAMAGED:
	{
		const SERVER_PACKET::PlayerDamagedPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::PlayerDamagedPacket*>(packetHeader);
		recvPacket->restHp;
		recvPacket->restShield;
		recvPacket->role;
		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(recvPacket->role);
		float maxHp = possessObj->GetMaxHP();
		possessObj->SetCurrentHP(recvPacket->restHp / maxHp * 100.0f);
		possessObj->m_pHPBarUI->SetCurrentHP(recvPacket->restHp / maxHp * 100.0f);
		possessObj->SetShield(recvPacket->restShield);
		possessObj->m_pHPBarUI->SetShield(recvPacket->restShield);
	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_DIE:
	{
		const SERVER_PACKET::SmallMonsterPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::SmallMonsterPacket*>(packetHeader);
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		smallMonsterArr[recvPacket->id]->SetAliveState(false);
	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_DAMAGED:
	{
		const SERVER_PACKET::SmallMonsterDamagedPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::SmallMonsterDamagedPacket*>(packetHeader);
		recvPacket->id;
		recvPacket->restHp;
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();

		float maxHp = smallMonsterArr[recvPacket->id]->GetMaxHP();
		smallMonsterArr[recvPacket->id]->SetCurrentHP(recvPacket->restHp / maxHp * 100.0f);
		//smallMonsterArr[recvPacket->id]->m_pHPBarUI->SetCurrentHP(recvPacket->restHp / maxHp * 100.0f);
	}
	break;
	case SERVER_PACKET::TYPE::SHOOTING_ARROW://ȭ��
	{
		const SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<const SERVER_PACKET::ShootingObject*>(packetHeader);

		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(ROLE::ARCHER);
		static_cast<Archer*>(possessObj)->ShootArrow(recvPacket->direction);
	}
	break;
	case SERVER_PACKET::TYPE::SHOOTING_ICE_LANCE://���̽� ����
	{
		const SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<const SERVER_PACKET::ShootingObject*>(packetHeader);

		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(ROLE::MAGE);
		static_cast<Mage*>(possessObj)->Attack(recvPacket->direction);
	}
	break;
	case SERVER_PACKET::TYPE::EXECUTE_LIGHTNING:
	{
		const SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<const SERVER_PACKET::ShootingObject*>(packetHeader);
		gGameFramework.m_pScene->m_pObjectManager->SetLightningEffect(recvPacket->direction);
	}
	break;
	case SERVER_PACKET::TYPE::GAME_STATE_STAGE:
	{
		const SERVER_PACKET::GameState_STAGE* recvPacket = reinterpret_cast<const SERVER_PACKET::GameState_STAGE*>(packetHeader);
		//Player Session		
		for (int i = 0; i < 4; i++) {
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(recvPacket->userState[i].role);
				float maxHp = possessObj->GetMaxHP();
				possessObj->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
				possessObj->m_pHPBarUI->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
				//Sheild�����͵� �־���� ��.

				possessObj->SetInterpolateData(recvPacket->userState[i].time, recvPacket->userState[i].position);
			}
		}
		//small monster
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		for (int i = 0; i < 15; i++) {
			int monsterIdx = recvPacket->smallMonster[i].idx;
			if (!smallMonsterArr[monsterIdx]->GetAliveState()) continue;

			smallMonsterArr[monsterIdx]->SetAliveState(recvPacket->smallMonster[monsterIdx].isAlive);
			float maxHp = smallMonsterArr[monsterIdx]->GetMaxHP();
			smallMonsterArr[monsterIdx]->SetCurrentHP(recvPacket->smallMonster[monsterIdx].hp / maxHp * 100.0f);
			//smallMonsterArr[monsterIdx]->m_pHPBarUI->SetCurrentHP(recvPacket->smallMonster[monsterIdx].hp / maxHp * 100.0f);

			smallMonsterArr[monsterIdx]->SetInterpolateData(recvPacket->smallMonster[monsterIdx].time, recvPacket->smallMonster[monsterIdx].position);
			break;
		}
	}
	break;

	case SERVER_PACKET::TYPE::SMALL_MONSTER_ATTACK:
	{
		const SERVER_PACKET::SmallMonsterAttackPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::SmallMonsterAttackPacket*>(packetHeader);
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		smallMonsterArr[recvPacket->id]->SetOnAttack(true);
	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_MOVE:
	{
		const SERVER_PACKET::SmallMonsterPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::SmallMonsterPacket*>(packetHeader);
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		smallMonsterArr[recvPacket->id]->SetMoveState(true);
	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_STOP:
	{
		const SERVER_PACKET::SmallMonsterPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::SmallMonsterPacket*>(packetHeader);
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		smallMonsterArr[recvPacket->id]->SetMoveState(false);
	}
	break;
	case SERVER_PACKET::TYPE::SMALL_MONSTER_SET_DESTINATION:
	{
		const SERVER_PACKET::SmallMonsterDestinationPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::SmallMonsterDestinationPacket*>(packetHeader);
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		smallMonsterArr[recvPacket->id]->SetDesPos(recvPacket->destinationPosition);
	}
	break;
	//case SERVER_PACKET::TYPE::BOSS_ATTACK:
	//{
	//	auto durationTime = chrono::duration_cast<milliseconds>(chrono::high_resolution_clock::now() - attckPacketRecvTime);
	//	attckPacketRecvTime = chrono::high_resolution_clock::now();
	//	//std::cout << "Attack Packet recv ElapsedTime: " << durationTime << endl;
	//	const SERVER_PACKET::BossAttackPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossAttackPacket*>(packetHeader);
	//	Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();

	//	if (!GameEnd)
	//	{
	//		switch (recvPacket->bossAttackType)
	//		{
	//		case BOSS_ATTACK::ATTACK_PUNCH:
	//		{
	//			if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_RIGHT_PUNCH)
	//			{
	//				bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_RIGHT_PUNCH;
	//				bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_RIGHT_PUNCH, 2);
	//			}
	//		}
	//		break;
	//		case BOSS_ATTACK::ATTACK_SPIN:
	//		{
	//			if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_SPIN_ATTACK)
	//			{
	//				bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_SPIN_ATTACK;
	//				bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_SPIN_ATTACK, 2);
	//			}
	//		}
	//		break;
	//		case BOSS_ATTACK::ATTACK_KICK:
	//		{
	//			if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_KICK_ATTACK)
	//			{
	//				bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_KICK_ATTACK;
	//				bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_KICK_ATTACK, 2);
	//			}
	//		}
	//		break;
	//		//case BOSS_ATTACK::SKILL_DASH:
	//		//{
	//		//	if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_DASH_SKILL)
	//		//	{
	//		//		bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_DASH_SKILL;
	//		//		bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_DASH_SKILL, 2);
	//		//		XMFLOAT3 xmf3BossPos = bossMonster->GetPosition();
	//		//		XMFLOAT3 xmf3BossLook = bossMonster->GetLook();
	//		//		XMFLOAT3 xmf3SkillPos = Vector3::Add(xmf3BossPos, xmf3BossLook, 160.0f);	// ������ ��ų �Ἥ �̵��ϴ� �Ÿ��� 1/2��ŭ �̵�
	//		//		static_cast<Monster*>(bossMonster)->m_pSkillRange->m_bActive = true;
	//		//		static_cast<Monster*>(bossMonster)->m_pSkillRange->SetPosition(xmf3SkillPos);
	//		//	}
	//		//}
	//		//	break;
	//		//case BOSS_ATTACK::SKILL_PUNCH:
	//		//{
	//		//	if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_PUNCHING_SKILL)
	//		//	{
	//		//		bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_PUNCHING_SKILL;
	//		//		bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_PUNCHING_SKILL, 2);
	//		//	}
	//		//}
	//		//break;
	//		//case BOSS_ATTACK::SKILL_CASTING:
	//		//{
	//		//	if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_CAST_SPELL)
	//		//	{
	//		//		bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_CAST_SPELL;
	//		//		bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_CAST_SPELL, 2);
	//		//	}
	//		//}
	//		//break;
	//		case BOSS_ATTACK::ATTACK_FLOOR_BOOM:
	//		{
	//			if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_CAST_SPELL)
	//			{
	//				bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_CAST_SPELL;
	//				bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_CAST_SPELL, 2);

	//				bossMonster->m_pSkillRange->m_bActive = true;
	//				bossMonster->m_pSkillRange->m_bBossSkillActive = true;
	//				bossMonster->m_pSkillRange->m_fBossSkillTime = gGameFramework.GetScene()->GetObjectManager()->GetTotalProgressTime();
	//				XMFLOAT3 BossPosition = bossMonster->GetPosition();
	//				BossPosition.y = 0.2f;
	//				bossMonster->m_pSkillRange->SetPosition(BossPosition);
	//			}
	//		}
	//		break;
	//		}
	//		bossMonster->SetMoveState(false);
	//		g_sound.Pause("BossMoveSound");
	//		//cout << "ProcessPacket::const SERVER_PACKET::BOSS_ATTACK - recvPacket: " << (int)recvPacket->bossAttackType << endl;
	//	}
	//}
	//break;
	case SERVER_PACKET::TYPE::GAME_END:
	{
		//���� ���� ��Ŷ ����
		//������ �ٷ� ���� ���� Ȯ���ϴ� ��Ŷ ������ �����ϰ� ������
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->SetCurrentHP(0.0f);
		GameEnd = true;
	}
	break;
	case SERVER_PACKET::TYPE::BOSS_MOVE_NODE:
	{
		// ���� �������� ��� Clear
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();


		const SERVER_PACKET::BossMoveNodePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossMoveNodePacket*>(packetHeader);
		if (gGameFramework.m_pScene == nullptr) return;

		//Role�� �������� �̰� ���� 		
		bossMonster->m_roleDesPlayer = recvPacket->targetRole;
		//std::cout << "recv aggro Id: " << recvPacket->desPlayerId << std::endl;
		if (recvPacket->nodeCnt > 0) {
			//cout << "���� �̵� �ε��� : ";
			vector<int> triangleIdxVec;
			std::list<int> recvNodeQueue;
			for (int i = 0; i < recvPacket->nodeCnt; i++) {
				//������ �̵��� ��� ������
				// �޾ƿ� ���� ���Ϳ� ���� �ֱ�
				//cout << recvPacket->node[i] << ", ";
				//recvNodeQueue.push_back(recvPacket->node[i]);
				triangleIdxVec.push_back(recvPacket->node[i]);
			}
			bossMonster->m_astarIdx = recvPacket->node[0];
			//cout << endl;
			//gGameFramework.m_pScene->m_pObjectManager->m_VecNodeQueue.push_back(recvPacket->node[i]);
			//AStart Node Mesh
			gGameFramework.m_pScene->m_pObjectManager->m_nodeLock.lock();
			gGameFramework.m_pScene->m_pObjectManager->m_VecNodeQueue.swap(triangleIdxVec);
			gGameFramework.m_pScene->m_pObjectManager->m_nodeLock.unlock();

			//boss Move Node Data
			//bossMonster->m_lockBossRoute.lock();
			//bossMonster->m_BossRoute = recvNodeQueue;
			//bossMonster->m_lockBossRoute.unlock();
		}

		if (!bossMonster->GetMoveState())
		{
			//cout << "const SERVER_PACKET::BOSS_MOVE_NODE - SetMoveState True" << endl;
			bossMonster->SetMoveState(true);
			bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_MOVE;
			bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_MOVE, 2);
		}
		bossMonster->SetMoveState(true);
		g_sound.Play("BossMoveSound", gGameFramework.GetScene()->GetObjectManager()->GetBossMonster()->CalculateDistanceSound());
		// cout << endl;
	}
	break;
	case SERVER_PACKET::TYPE::PRE_EXIST_LOGIN://�̹� �����ϴ� �÷��̾ �ֱ� ������, ���� ���� �÷��̾�(�� Ŭ���̾�Ʈ��) ���� ���� ��Ŷ�� ����
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
	case SERVER_PACKET::TYPE::STAGE_CHANGING_BOSS:
	{
		//Ű ��ǲ �����ֱ�
		//�ε� â ����ֱ�
		gGameFramework.GetScene()->GetObjectManager()->SetPortalCheck(true);
	}
	break;
	case SERVER_PACKET::TYPE::STAGE_START_BOSS:
	{
		//�ε� â �����
		//Ű ��ǲ �� �� �ְ� ����
		const SERVER_PACKET::GameState_BOSS_INIT* recvPacket = reinterpret_cast<const SERVER_PACKET::GameState_BOSS_INIT*>(packetHeader);
		for (int i = 0; i < 4; i++) {//�׳� 4�� ���� ���� for�� ����			
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->userState[i].role);
				//possessObj->SetPosition(recvPacket->userState[i].pos);
				float maxHp = possessObj->GetMaxHP();
				possessObj->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);

				if (possessObj->m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_IDLE)
				{
					possessObj->m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_IDLE;
					possessObj->m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_IDLE;
					possessObj->m_pSkinnedAnimationController->SetTrackEnable(possessObj->m_pSkinnedAnimationController->m_CurrentAnimations);
					possessObj->m_pSkinnedAnimationController->ResetTrack();
				}
			}
		}
	}
	break;
	case SERVER_PACKET::TYPE::HEAL_START:
	{
		//������ ����Ʈ ����
		Player* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::MAGE);
		possessObj->StartEffect(0);

	}
	break;
	case SERVER_PACKET::TYPE::HEAL_END:
	{
		Player* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::MAGE);
		possessObj->EndEffect(0);
		//������ ����Ʈ ��
	}
	break;
	case SERVER_PACKET::TYPE::SHIELD_START:
	{
		//���� ����Ʈ ��� �ؼ� ����
		Player* TankerObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
		TankerObj->StartEffect(0);

		for (int i = 0; i < 4; ++i)
		{
			Player* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(static_cast<ROLE>(0x01 << i));
			possessObj->SetShield(200.0f);
			possessObj->m_pHPBarUI->SetShield(200.0f);
		}
	}
	break;
	case SERVER_PACKET::TYPE::SHIELD_END:
	{
		//���� ��
		Player* TankerObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
		TankerObj->EndEffect(0);

		for (int i = 0; i < 4; ++i)
		{
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(static_cast<ROLE>(0x01 << i));
			possessObj->SetShield(0.0f);
			possessObj->m_pHPBarUI->SetShield(0.0f);
		}
	}
	break;
	case SERVER_PACKET::TYPE::NOTIFY_HEAL_HP:
	{
		const SERVER_PACKET::NotifyHealPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::NotifyHealPacket*>(packetHeader);
		for (int i = 0; i < 4; i++) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(recvPacket->applyHealPlayerInfo[i].role);
			if (possessObj)
			{
				float maxHp = possessObj->GetMaxHP();
				possessObj->SetCurrentHP(recvPacket->applyHealPlayerInfo[i].hp / maxHp * 100.0f);
			}
		}
	}
	break;
	//case SERVER_PACKET::TYPE::NOTIFY_SHIELD_APPLY:
	//{
	//	const SERVER_PACKET::NotifyShieldPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::NotifyShieldPacket*>(packetHeader);
	//	for (int i = 0; i < 4; i++) {
	//		if (recvPacket->applyShieldPlayerInfo[i].shield < FLT_EPSILON) continue;
	//		Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->applyShieldPlayerInfo[i].role);
	//		if (possessObj)
	//		{
	//			possessObj->SetShield(recvPacket->applyShieldPlayerInfo[i].shield);	//�ִ� �ǵ� 200
	//			possessObj->m_pHPBarUI->SetShield(recvPacket->applyShieldPlayerInfo[i].shield);
	//			possessObj->m_pHPBarUI->SetCurrentHP(possessObj->GetCurrentHP());
	//		}
	//	}
	//}
	//break;
	case SERVER_PACKET::TYPE::PLAYER_ATTACK_RESULT:
	{
		const SERVER_PACKET::PlayerAttackMonsterDamagePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::PlayerAttackMonsterDamagePacket*>(packetHeader);
		NormalMonster** smallMonsterArr = gGameFramework.GetScene()->GetObjectManager()->GetNormalMonsterArr();
		if (recvPacket->role == ROLE::TANKER)
		{
			Player* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
			if (possessObj->GetESkillState() == false) return;
			for (int i = 0; i < recvPacket->attackedMonsterCnt; ++i)
			{
				int index = recvPacket->monsterIdx[i];
				if (smallMonsterArr[index] == nullptr) continue;
				gGameFramework.GetScene()->GetObjectManager()->GetTankerAttackEffect()->m_xmf3TargetPosition = smallMonsterArr[index]->GetPosition();
				gGameFramework.GetScene()->GetObjectManager()->GetTankerAttackEffect()->SetActive(true);
				gGameFramework.GetScene()->GetObjectManager()->GetTankerAttackEffect()->m_fEffectLifeTime = 3.0f;
			}
		}
	}
	break;
	case SERVER_PACKET::TYPE::COMMON_ATTACK_START:
	{
		//const SERVER_PACKET::CommonAttackPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::CommonAttackPacket*>(packetHeader);
		//Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->role);
		//possessObj->SetLButtonClicked(true);
	}
	break;
	case SERVER_PACKET::TYPE::START_ANIMATION_Q:
	{
		const SERVER_PACKET::NotifyPlayerAnimationPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::NotifyPlayerAnimationPacket*>(packetHeader);
		Player* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->RecvFirstSkill(recvPacket->time);
	}
	break;
	case SERVER_PACKET::TYPE::START_ANIMATION_E:
	{
		const SERVER_PACKET::NotifyPlayerAnimationPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::NotifyPlayerAnimationPacket*>(packetHeader);
		Player* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo((ROLE)recvPacket->role);
		possessObj->RecvSecondSkill(recvPacket->time);
	}
	break;
	//projectile packet
	/*case SERVER_PACKET::TYPE::MONSTER_DAMAGED_ARROW:
	{
		const SERVER_PACKET::ProjectileDamagePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::ProjectileDamagePacket*>(packetHeader);
		if (myRole == ROLE::ARCHER) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);
			possessObj->m_ppProjectiles[recvPacket->projectileId]->m_bActive = false;
		}
	}
	break;*/
	/*case SERVER_PACKET::TYPE::MONSTER_DAMAGED_ARROW_SKILL:
	{
		const SERVER_PACKET::ProjectileDamagePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::ProjectileDamagePacket*>(packetHeader);
		if (myRole == ROLE::ARCHER) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);
			static_cast<Archer*>(possessObj)->m_ppArrowForQSkill[recvPacket->projectileId]->m_bActive = false;
		}
	}
	break;*/
	/*case SERVER_PACKET::TYPE::MONSTER_DAMAGED_BALL:
	{
		const SERVER_PACKET::ProjectileDamagePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::ProjectileDamagePacket*>(packetHeader);
		if (myRole == ROLE::PRIEST) {
			Character* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::PRIEST);
			possessObj->m_ppProjectiles[recvPacket->projectileId]->m_bActive = false;
		}
	}
	break;*/
	case SERVER_PACKET::TYPE::PLAYER_ATTACK_RESULT_BOSS:
	{
		const SERVER_PACKET::PlayerAttackBossDamagePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::PlayerAttackBossDamagePacket*>(packetHeader);
		recvPacket->damage;//���� Ŭ���̾�Ʈ ���� ��ġ���ٰ� ��� ������ ��Ʈ�� ������
		if (recvPacket->role == ROLE::TANKER) {
			gGameFramework.GetScene()->GetObjectManager()->GetTankerAttackEffect()->m_xmf3TargetPosition = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster()->GetPosition();
			gGameFramework.GetScene()->GetObjectManager()->GetTankerAttackEffect()->SetActive(true);
			gGameFramework.GetScene()->GetObjectManager()->GetTankerAttackEffect()->m_fEffectLifeTime = 3.0f;
		};//����
	}
	break;
	case SERVER_PACKET::TYPE::BOSS_ATTACK_PALYER:
	{
		const SERVER_PACKET::BossAttackPlayerPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossAttackPlayerPacket*>(packetHeader);
		g_sound.Play("BossMonsterAttack", gGameFramework.GetScene()->GetObjectManager()->GetBossMonster()->CalculateDistanceSound());
		recvPacket->currentHp;//�������� �ǰݴ��Ͽ� ���� ���� HP�̰� �ʿ� �ֳ� ����... => ���߿� notift�� �����ҵ�?
	}
	break;
	case  SERVER_PACKET::TYPE::BOSS_CHANGE_DIRECION:
	{
		const SERVER_PACKET::BossDirectionPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossDirectionPacket*>(packetHeader);
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->m_desDirecionVec = recvPacket->directionVec;
	}
	break;
	case SERVER_PACKET::TYPE::METEO_PLAYER_ATTACK:
	{
		const SERVER_PACKET::BossAttackPlayerPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossAttackPlayerPacket*>(packetHeader);
		GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(GetMyRole());
		if (possessObj != nullptr) {
			float maxHp = possessObj->GetMaxHP();
			possessObj->SetCurrentHP(recvPacket->currentHp / maxHp * 100.0f);
		}
	}
	break;
	case SERVER_PACKET::TYPE::METEO_DESTROY:
	{
		const SERVER_PACKET::DestroyedMeteoPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::DestroyedMeteoPacket*>(packetHeader);
		vector<RockSpike*> ppRockSpike = gGameFramework.GetScene()->GetObjectManager()->GetRockSpikeArr();
		for (int i = 0; i < 10; i++) {
			string sound = "RockSpkieSound";
			sound = sound + to_string(i);
			g_sound.Play(sound, ppRockSpike[i]->CalculateDistanceSound() * 0.65);
		}
		if (ppRockSpike[recvPacket->idx] != nullptr)
			ppRockSpike[recvPacket->idx]->m_bActive = false;
		//�� ���׿� ������ off
	}
	break;
	//case SERVER_PACKET::TYPE::METEO_CREATE:
	//{
	//	const SERVER_PACKET::MeteoStartPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::MeteoStartPacket*>(packetHeader);
	//	Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
	//	vector<RockSpike*> ppRockSpike = gGameFramework.GetScene()->GetObjectManager()->GetRockSpikeArr();
	//	bossMonster->SetMoveState(false);
	//	g_sound.Pause("BossMoveSound");
	//	g_sound.Play("BossSKillSound", 0.9f);
	//	//���� �̵� ���߰� �ִϸ��̼� �������ּ���.
	//	if (bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_CAST_SPELL)
	//	{
	//		bossMonster->m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_CAST_SPELL;
	//		bossMonster->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_CAST_SPELL, 2);
	//	}
	//	for (int i = 0; i < 10; i++) {
	//		if (ppRockSpike[i] == nullptr) continue;
	//		if (ppRockSpike[i]->m_bActive == true) continue;
	//		//XMFLOAT3 rockPosition = recvPacket->meteoInfo[i].pos;
	//		ppRockSpike[i]->m_bActive = true;
	//		ppRockSpike[i]->SetPosition(rockPosition);
	//		ppRockSpike[i]->SetSpeed(recvPacket->meteoInfo[i].speed);
	//		rockPosition.y = 0.2;

	//		if (ppRockSpike[i]->m_pAttackedArea != nullptr)
	//			ppRockSpike[i]->m_pAttackedArea->SetPosition(rockPosition);
	//	}
	//}
	//break;
	case SERVER_PACKET::TYPE::TIME_SYNC_RESPONSE:
	{
		m_responseTime = std::chrono::high_resolution_clock::now();
		const SERVER_PACKET::TimeSyncPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::TimeSyncPacket*>(packetHeader);
		auto RTT = std::chrono::duration_cast<microseconds>(m_responseTime - m_requestTime).count();
		if (RTT < 0) return;
		RTT = RTT / 2;
		m_RTT = (float)RTT / 1000.0f;
		auto requestDiff = std::chrono::duration_cast<microseconds>(recvPacket->time - m_requestTime).count();
		C2S_DiffTime = requestDiff - RTT;
		SetReadySync(true);
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

void Logic::SetrequestTime()
{
	m_requestTime = std::chrono::high_resolution_clock::now();
}

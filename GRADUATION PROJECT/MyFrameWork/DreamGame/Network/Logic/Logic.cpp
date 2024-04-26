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
		//로딩창이면 좋을듯?
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
	case SERVER_PACKET::TYPE::SHOOTING_ARROW://화살
	{
		const SERVER_PACKET::ShootingObject* recvPacket = reinterpret_cast<const SERVER_PACKET::ShootingObject*>(packetHeader);

		Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(ROLE::ARCHER);
		static_cast<Archer*>(possessObj)->ShootArrow(recvPacket->direction);
	}
	break;
	case SERVER_PACKET::TYPE::SHOOTING_ICE_LANCE://아이스 렌스
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
				//Sheild데이터도 넣어줘야 됨.

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

	case SERVER_PACKET::TYPE::GAME_END:
	{
		//게임 종료 패킷 수신
		//지금은 바로 게임 종료 확인하는 패킷 서버로 전송하게 구현함
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->SetCurrentHP(0.0f);
		GameEnd = true;
	}
	break;

	case SERVER_PACKET::TYPE::PRE_EXIST_LOGIN://이미 존재하는 플레이어가 있기 때문에, 지금 들어온 플레이어(내 클라이언트는) 접속 해제 패킷을 수신
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

	case SERVER_PACKET::TYPE::HEAL_START:
	{
		//힐장판 이펙트 시작
		Player* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::MAGE);
		possessObj->StartEffect(0);

	}
	break;
	case SERVER_PACKET::TYPE::HEAL_END:
	{
		Player* possessObj = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::MAGE);
		possessObj->EndEffect(0);
		//힐장판 이펙트 끝
	}
	break;
	case SERVER_PACKET::TYPE::SHIELD_START:
	{
		//쉴드 이펙트 출발 해서 적용
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
		//쉴드 끝
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

	case SERVER_PACKET::TYPE::START_STAGE_BOSS:
	{
		//로딩 창 지우기
		//키 인풋 할 수 있게 변경
		const SERVER_PACKET::BossStageInitPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossStageInitPacket*>(packetHeader);
		gGameFramework.GetScene()->GetObjectManager()->SetPortalCheck(true);
		for (int i = 0; i < 4; i++) {//그냥 4개 여서 도는 for문 주의
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				GameObject* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo((ROLE)recvPacket->userState[i].role);
				possessObj->SetPosition(recvPacket->userState[i].position);
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
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->SetPosition(recvPacket->bossPosition);
		bossMonster->SetLook(recvPacket->bossLookVector);
		bossMonster->SetMaxHP(recvPacket->bossHp);
		bossMonster->SetCurrentHP(100.0f);
		bossMonster->SetTempHp(100.0f);

	}
	break;

	case SERVER_PACKET::TYPE::GAME_STATE_BOSS:
	{
		const SERVER_PACKET::GameState_BOSS* recvPacket = reinterpret_cast<const SERVER_PACKET::GameState_BOSS*>(packetHeader);
		for (int i = 0; i < 4; i++) {
			if (recvPacket->userState[i].role != ROLE::NONE_SELECT) {
				Character* possessObj = gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(recvPacket->userState[i].role);
				float maxHp = possessObj->GetMaxHP();
				possessObj->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
				possessObj->m_pHPBarUI->SetCurrentHP(recvPacket->userState[i].hp / maxHp * 100.0f);
				//Sheild데이터도 넣어줘야 됨.

				possessObj->SetInterpolateData(recvPacket->userState[i].time, recvPacket->userState[i].position);
			}
		}
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->SetInterpolateData(recvPacket->bossState.time, recvPacket->bossState.position);
		float maxHp = bossMonster->GetMaxHP();
		bossMonster->SetCurrentHP(recvPacket->bossState.hp / maxHp * 100.0f);
		//recvPacket->bossState.hp

	}
	break;

	case SERVER_PACKET::TYPE::BOSS_MOVE_DESTINATION:
	{
		const SERVER_PACKET::BossMoveDestnationPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossMoveDestnationPacket*>(packetHeader);
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->InsertEvent(std::make_shared<BossSetDestinationPositionEvent>(recvPacket->destination));
	}
	break;

	//case SERVER_PACKET::TYPE::BOSS_STOP:
	//{
	//	//const SERVER_PACKET::BossStopPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossStopPacket*>(packetHeader);
	//	Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
	//	bossMonster->SetMoveState(false);
	//	g_sound.Pause("BossMoveSound");
	//}
	//break;

	case SERVER_PACKET::TYPE::BOSS_ON_SAME_NODE:
	{
		const SERVER_PACKET::BossOnSameNodePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossOnSameNodePacket*>(packetHeader);
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		bossMonster->InsertEvent(std::make_shared<BossSetAggroPositionEvent>(recvPacket->destination));
	}
	break;

	/*case SERVER_PACKET::TYPE::BOSS_ROTATE:
	{
		const SERVER_PACKET::BossRotatePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossRotatePacket*>(packetHeader);
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		recvPacket->angle;
	}
	break;*/

	case SERVER_PACKET::TYPE::BOSS_ATTACK:
	{
		const SERVER_PACKET::BossAttackPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossAttackPacket*>(packetHeader);
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		if (BOSS_ATTACK::ATTACK_FIRE == recvPacket->attackType) {
			bossMonster->InsertEvent(std::make_shared<BossFireAttackEvent>());
		}
		else if (BOSS_ATTACK::ATTACK_SPIN == recvPacket->attackType) {
			bossMonster->InsertEvent(std::make_shared<BossSpinAttackEvent>());
		}
		bossMonster->SetMoveState(false);
		g_sound.Pause("BossMoveSound");
	}
	break;

	case SERVER_PACKET::TYPE::BOSS_DIRECTION_ATTACK:
	{
		const SERVER_PACKET::BossDirectionAttackPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossDirectionAttackPacket*>(packetHeader);
		recvPacket->attackType;
		;
		Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
		if (BOSS_ATTACK::ATTACK_KICK == recvPacket->attackType) {
			bossMonster->InsertEvent(std::make_shared<BossKickAttackEvent>(recvPacket->attackVector));
		}
		else if (BOSS_ATTACK::ATTACK_PUNCH == recvPacket->attackType) {
			bossMonster->InsertEvent(std::make_shared<BossPunchAttackEvent>(recvPacket->attackVector));
		}
		bossMonster->SetMoveState(false);
		g_sound.Pause("BossMoveSound");
	}
	break;

	case SERVER_PACKET::TYPE::BOSS_ATTACK_METEOR:
	{
		//bossMonster->InsertEvent(std::make_shared<BossSpinAttackEvent>());
		//const SERVER_PACKET::BossStageInitPacket* recvPacket = reinterpret_cast<const SERVER_PACKET::BossStageInitPacket*>(packetHeader);
		//Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
	}
	break;

	default:
	{
		std::cout << "Unknown Packet Recv: " << (int)packetHeader->type << std::endl;
	}
	break;
	}
}

XMFLOAT3 Logic::GetPostion(ROLE r)
{
	return gGameFramework.m_pScene->m_pObjectManager->GetChracterInfo(r)->GetPosition();
	// TODO: 여기에 return 문을 삽입합니다.
}

void Logic::SetrequestTime()
{
	m_requestTime = std::chrono::high_resolution_clock::now();
}

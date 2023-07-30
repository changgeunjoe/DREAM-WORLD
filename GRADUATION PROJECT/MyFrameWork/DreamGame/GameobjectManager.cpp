#include "stdafx.h"
#include "GameobjectManager.h"
#include "Animation.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"
#include "DepthRenderShaderComponent.h"
#include "TextureToViewportComponent.h"
#include "UiShaderComponent.h"
#include "MultiSpriteShaderComponent.h"
#include "InstancingShaderComponent.h"
#include "TrailShaderComponent.h"
#include "TrailComponent.h"
#include"TerrainShaderComponent.h"
#include"EffectShaderComponent.h"
#include "./Network/MapData/MapData.h"
#include"EffectObject.h"
#include"DebuffObject.h"
#include"UILayer.h"
#include "GameFramework.h"
#include "Character.h"
#include "sound/GameSound.h"

extern NetworkHelper g_NetworkHelper;
extern Logic g_Logic;
extern bool GameEnd;
extern MapData g_bossMapData;
extern MapData g_stage1MapData;
extern CGameFramework gGameFramework;
extern MapData g_stage1MapData;
extern GameSound g_sound;



template<typename S>

S* ComponentType(component_id componentID)
{
	if (typeid(S).name() == typeid(RenderComponent).name())
	{
		componentID = RENDER_COMPONENT;
	}
	return 0;
}
template<typename T>
T* InsertComponent()
{
	component_id Component{};
	ComponentType<RenderComponent>(Component);
	return 0;
}

GameobjectManager::GameobjectManager(CCamera* pCamera)
{
	m_pLight = new CLight();
	m_pCamera = pCamera;
}

GameobjectManager::~GameobjectManager()
{
}

void GameobjectManager::Animate(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;
	m_fTimeElapsed = fTimeElapsed;
	//<<<<<<< Updated upstream
		/*m_pConditionUIObject->SetColor(XMFLOAT4(0, 0, 0, sin(m_fTime)+0.2));
		*/
		//SortEffect(); // 게임 오브젝트를 카메라와 거리별로 sort하는 함수입니다.->이펙트가 블랜드가 꼬이는 걸 막기위한 소트

	//SceneSwapAnimate(fTimeElapsed);

	//if (int(m_fTime) % 5 > 3)
	//{
	//	AddDamageFontToUiLayer();
	//}
	//Effect

	m_pMonsterCubeObject->SetPosition(m_pCamera->GetPosition());
	m_pSkyboxObject->SetPosition(m_pCamera->GetPosition());
	m_pLight->UpdatePosition(XMFLOAT3(m_pCamera->GetPosition().x,
		m_pCamera->GetPosition().y + 600, m_pCamera->GetPosition().z));

	if (m_pNPCPressGObject)//23.04.18 몬스터 체력바 -> 카메라를 바라 보도록 .ccg
	{
		m_pNPCPressGObject->SetLookAt(m_pCamera->GetPosition());
		m_pNPCPressGObject->SetPosition(XMFLOAT3(m_pAngelNPCObject->GetPosition().x,
			m_pAngelNPCObject->GetPosition().y + 23, m_pAngelNPCObject->GetPosition().z));
		m_pNPCPressGObject->Rotate(0, 180, 0);
		m_pNPCPressGObject->SetScale(1, 1, 1);
		//m_pNPCPressGObject->SetCurrentHP(m_pAngelNPCObject->GetCurrentHP());
	}

	//MonsterHpBarAnimate(fTimeElapsed);
	if (m_pMonsterHPBarObject)//23.04.18 몬스터 체력바 -> 카메라를 바라 보도록 .ccg
	{
		m_pMonsterHPBarObject->SetLookAt(m_pCamera->GetPosition());
		m_pMonsterHPBarObject->SetPosition(XMFLOAT3(m_pMonsterObject->GetPosition().x,
			m_pMonsterObject->GetPosition().y + 70, m_pMonsterObject->GetPosition().z));
		m_pMonsterHPBarObject->Rotate(0, 180, 0);
		m_pMonsterHPBarObject->SetScale(10, 1, 1);
		m_pMonsterHPBarObject->SetCurrentHP(m_pMonsterObject->GetCurrentHP());
	}
	for (int i = 0; i < m_pNormalMonsterHPBarObject.size(); i++) {
		if (m_pNormalMonsterHPBarObject[i])//23.04.18 몬스터 체력바 -> 카메라를 바라 보도록 .ccg
		{
			m_pNormalMonsterHPBarObject[i]->SetLookAt(m_pCamera->GetPosition());
			m_pNormalMonsterHPBarObject[i]->SetPosition(XMFLOAT3(m_ppNormalMonsterObject[i]->GetPosition().x,
				m_ppNormalMonsterObject[i]->GetPosition().y + 30, m_ppNormalMonsterObject[i]->GetPosition().z));
			m_pNormalMonsterHPBarObject[i]->Rotate(0, 180, 0);
			m_pNormalMonsterHPBarObject[i]->SetScale(3, 0.5, 1);
			m_pNormalMonsterHPBarObject[i]->SetCurrentHP(m_ppNormalMonsterObject[i]->GetCurrentHP());
		}
	}
	XMFLOAT3 mfHittmp = m_pMonsterObject->m_xmfHitPosition;
	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
		if (m_ppParticleObjects[i]->m_bActive)
		{
			m_ppParticleObjects[i]->SetLookAt(m_pCamera->GetPosition());
			//m_ppParticleObjects[i]->SetPosition(XMFLOAT3(mfHittmp.x, mfHittmp.y+20, mfHittmp.z));
			m_ppParticleObjects[i]->SetScale(m_ppParticleObjects[i]->m_xmf3Scale.x,
				m_ppParticleObjects[i]->m_xmf3Scale.y, m_ppParticleObjects[i]->m_xmf3Scale.z);
			m_ppParticleObjects[i]->Rotate(0, 180, 0);
			m_ppParticleObjects[i]->AnimateRowColumn(fTimeElapsed);
		}
	}
	SceneSwapAnimate(fTimeElapsed);
	//if (int(m_fTime) % 5 > 3)
	//{
	//	AddDamageFontToUiLayer();
	//}
	//Effect
	if (m_bPickingenemy) {
		if (m_pSelectedObject != nullptr) {
			if (g_Logic.GetMyRole() == ROLE::PRIEST) {

				m_pLightEffectObject->AnimateEffect(m_pCamera, XMFLOAT3(m_pSelectedObject->GetPosition().x,
					m_pSelectedObject->GetPosition().y + 10, m_pSelectedObject->GetPosition().z), fTimeElapsed, m_fTime * 5);
				m_pLightningSpriteObject->SetPosition(XMFLOAT3(
					m_pSelectedObject->GetPosition().x,
					m_pSelectedObject->GetPosition().y + 50,
					m_pSelectedObject->GetPosition().z));

				m_pLightEffectObject->m_fEffectLifeTime = 2.0f;

				m_pLightningSpriteObject->m_bActive = m_pLightEffectObject->m_bActive = true;
				XMFLOAT3 TargetPosition = m_pSelectedObject->GetPosition();
				g_NetworkHelper.Send_SkillExecute_E(TargetPosition);
				g_sound.NoLoopPlay("LightningSound", m_pLightningSpriteObject->CalculateDistanceSound() + 0.3);
			}
			m_bPickingenemy = false;
		}
		//m_pEffectObject->AnimateEffect(m_pCamera, m_pSelectedObject->GetPosition(), fTimeElapsed, m_fTime * 10);
		//라이트닝

		//m_pLightningSpriteObject->SetScale(7.0f, 20.0f, 7.0f);
		//m_pSheildEffectObject->AnimateEffect(m_pCamera, m_pSelectedObject->GetPosition(), fTimeElapsed, m_fTime * 10);
	}

	if (m_pLightEffectObject) {
		if (m_pLightEffectObject->m_bActive) {
			if (m_pLightEffectObject->m_fEffectLifeTime > FLT_EPSILON) {
				m_pLightEffectObject->SetActive(m_pLightEffectObject->m_bActive);
				m_pLightEffectObject->AnimateEffect(m_pCamera, m_LightningTargetPos, fTimeElapsed, m_fTime * 10);
				m_pLightningSpriteObject->m_bActive = m_pLightEffectObject->m_bActive;
				if (m_pLightEffectObject->m_bActive == false) {
					m_pSelectedObject = nullptr;
				}
			}
		}
	}

	for (auto& effect : m_ppShieldEffectObject)
	{
		if (effect == nullptr) continue;
		if (effect->m_bActive == false) continue;
		Character* possessChracter = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(effect->m_hostObject);
		effect->AnimateEffect(m_pCamera, possessChracter->GetPosition(), fTimeElapsed, m_fTime * 10);
	}

	for (auto& effect : m_ppHealingEffectObject)
	{
		if (effect == nullptr) continue;
		if (effect->m_bActive == false) continue;
		Character* possessChracter = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(effect->m_hostObject);
		effect->AnimateEffect(m_pCamera, possessChracter->GetPosition(), fTimeElapsed, m_fTime * 10);
	}


	m_pPortalEffectObject->AnimateEffect(m_pCamera, XMFLOAT3(0, 0, 0), fTimeElapsed, m_fTime * 5);
	if (m_bTest) {
		//m_pPreistAttackEffectObject->AnimateEffect(m_pCamera, XMFLOAT3(0, 20, 0), fTimeElapsed, m_fTime * 5);
		m_pTankerAttackEffectObject->AnimateEffect(m_pCamera, XMFLOAT3(0, 20, 0), fTimeElapsed, m_fTime * 5);

	}
	//Effect
	// if (m_pSelectedObject) {
		// 힐 이펙트
		// m_pLightEffectObject->AnimateEffect(m_pCamera, m_pSelectedObject->GetPosition(), fTimeElapsed, m_fTime * 10);
	// }
	//m_pDebuffObject->AnimateEffect(m_pCamera, g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), fTimeElapsed, m_fTime * 10);

	//sword effect
	//m_pSwordFireObject->SetPosition(XMFLOAT3(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().x,
	//	m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().y,
	//	m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().z));
	//g_Logic.m_MonsterSession.m_currentPlayGameObject->Animate(fTimeElapsed);
	//auto pos = g_Logic.m_MonsterSession.m_currentPlayGameObject->GetPosition();
	//cout << "GameobjectManager::Boss Position: " << pos.x << ", 0, " << pos.z << endl;


	PlayerCurrentHpAnimate(fTimeElapsed);
	PlayerConditionAnimate(fTimeElapsed);
	NormalMonsterConditionAnimate(fTimeElapsed);
	BossConditionAnimate(fTimeElapsed);
	ChangeStage1ToStage2(fTimeElapsed);//포탈과 상호작용시에 스테이지 1이 사라지고 보스스테이지 2가 나오는 함수
	CharacterUIAnimate(fTimeElapsed);
	TrailAnimate(fTimeElapsed);

	//TextUI Update
	{//서순 중요 AddText
		m_pUILayer->Update(fTimeElapsed, m_bNPCinteraction, m_bNPCscreen);
		AddTextToUILayer(m_iTEXTiIndex);
	}

}

void GameobjectManager::CharacterUIAnimate(float fTimeElapsed)//나중에 처리
{
	m_pArcherObject->m_pHPBarUI->SetCurrentHP(m_pArcherObject->GetCurrentHP());
	m_pWarriorObject->m_pHPBarUI->SetCurrentHP(m_pWarriorObject->GetCurrentHP());
	m_pTankerObject->m_pHPBarUI->SetCurrentHP(m_pTankerObject->GetCurrentHP());
	m_pPriestObject->m_pHPBarUI->SetCurrentHP(m_pPriestObject->GetCurrentHP());

	if (g_Logic.GetMyRole() == ROLE::ARCHER) {
		m_pArcherObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.48, 1.005));
		m_pArcherObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.5, 1.005));
		m_pTankerObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.38, 1.005));
		m_pTankerObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.4, 1.005));
		m_pPriestObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.26, 1.005));
		m_pPriestObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.28, 1.005));
		m_pWarriorObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.14, 1.005));
		m_pWarriorObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.16, 1.005));
		m_pArcherObject->m_pSkillQUI->SetPosition(XMFLOAT3(-0.975, 0.0, 1.005));
		m_pArcherObject->m_pSkillEUI->SetPosition(XMFLOAT3(-0.975, -0.1, 1.005));
	}
	else if (g_Logic.GetMyRole() == ROLE::TANKER) {
		m_pTankerObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.46, 1.005));
		m_pTankerObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.48, 1.005));
		m_pArcherObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.36, 1.005));
		m_pArcherObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.38, 1.005));
		m_pPriestObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.24, 1.005));
		m_pPriestObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.26, 1.005));
		m_pWarriorObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.12, 1.005));
		m_pWarriorObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.14, 1.005));
		m_pTankerObject->m_pSkillQUI->SetPosition(XMFLOAT3(0.975, 0.1, 1.005));
		m_pTankerObject->m_pSkillEUI->SetPosition(XMFLOAT3(0.975, 0.0, 1.005));
	}
	else if (g_Logic.GetMyRole() == ROLE::PRIEST) {
		m_pPriestObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.46, 1.005));
		m_pPriestObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.48, 1.005));
		m_pTankerObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.36, 1.005));
		m_pTankerObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.38, 1.005));
		m_pArcherObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.24, 1.005));
		m_pArcherObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.26, 1.005));
		m_pWarriorObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.12, 1.005));
		m_pWarriorObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.14, 1.005));
		m_pPriestObject->m_pSkillQUI->SetPosition(XMFLOAT3(0.975, 0.1, 1.005));
		m_pPriestObject->m_pSkillEUI->SetPosition(XMFLOAT3(0.975, 0.0, 1.005));
	}
	else if (g_Logic.GetMyRole() == ROLE::WARRIOR) {
		m_pWarriorObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.46, 1.005));
		m_pWarriorObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.48, 1.005));
		m_pTankerObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.36, 1.005));
		m_pTankerObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.38, 1.005));
		m_pPriestObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.24, 1.005));
		m_pPriestObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.26, 1.005));
		m_pArcherObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.12, 1.005));
		m_pArcherObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.14, 1.005));
		m_pWarriorObject->m_pSkillQUI->SetPosition(XMFLOAT3(0.975, 0.1, 1.005));
		m_pWarriorObject->m_pSkillEUI->SetPosition(XMFLOAT3(0.975, 0.0, 1.005));
	}
}

void GameobjectManager::TrailAnimate(float fTimeElapsed)
{
	for (int i = 0; i < m_pTrailArrowComponent.size(); i++) {
		if (m_pTrailArrowComponent[i]) {
			if (m_pArrowObjects[i]->m_bActive)
			{
				m_pTrailArrowComponent[i]->m_fRenderTime = 0;
			}
			if (m_pTrailArrowComponent[i]->m_fRenderTime > 3) {

			}
			else {
				XMFLOAT3 xmf3ObjectPos = m_pArrowObjects[i]->GetPosition();
				m_pTrailArrowComponent[i]->AddTrail(xmf3ObjectPos, Vector3::Add(xmf3ObjectPos, XMFLOAT3(0.0f, 0.0f, 1.0f), 1.5f));
			}
		}
	}
	for (int i = 0; i < m_pTrailIceRanceComponent.size(); i++) {
		if (m_pTrailIceRanceComponent[i]) {
			XMFLOAT3 xmf3ObjectPos = m_pEnergyBallObjects[i]->GetPosition();
			m_pTrailIceRanceComponent[i]->AddTrail(xmf3ObjectPos, Vector3::Add(xmf3ObjectPos, XMFLOAT3(0.0f, 0.0f, 1.0f), 1.5f));
		}
	}

	if (m_pTrailComponent)
	{
		m_pTrailComponent->AddTrail(XMFLOAT3(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().x,
			m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().y,
			m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().z),
			XMFLOAT3(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().x,
				m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().y,
				m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().z));
	}

}

void GameobjectManager::StoryUIAnimate(float fTimeElapsed)
{

}

void GameobjectManager::ConditionAnimate(float fTimeElapsed)
{
	//m_pConditionUIObject->m_fTime+= fTimeElapsed;
	//m_pConditionUIObject->SetColor(XMFLOAT4(10, 0, 0, 0));
}

void GameobjectManager::SceneSwapAnimate(float fTimeElapsed)
{
	if (m_bSceneSwap)
	{
		float mftime = float(fTimeElapsed / 2);
		m_pSceneChangeUIObject->m_fTime += mftime;
		m_pSceneChangeUIObject->SetColor(XMFLOAT4(0, 0, 0, sin(m_pSceneChangeUIObject->m_fTime)));
		if (sin(m_pSceneChangeUIObject->m_fTime) < 0) {
			m_bSceneSwap = false;
			m_pSceneChangeUIObject->m_fTime = 0;
		}
	}
}

void GameobjectManager::MonsterHpBarAnimate(float fTimeElapsed)
{

	/*GameObject* mpHpBar;
	for (int i = 0; i < m_ppGameObjects.size(); i++) {
		if (static_cast<Character*>(m_ppGameObjects[i])->m_pHPBarObject) {
			static_cast<Character*>(m_ppGameObjects[i])->m_pHPBarObject = static_cast<Character*>(m_ppGameObjects[i])->m_pHPBarObject;
			static_cast<Character*>(m_ppGameObjects[i])->m_pHPBarObject->SetLookAt(m_pCamera->GetPosition());
			static_cast<Character*>(m_ppGameObjects[i])->m_pHPBarObject->SetPosition(XMFLOAT3(m_ppGameObjects[i]->GetPosition().x,
				m_ppGameObjects[i]->GetPosition().y + 70, m_ppGameObjects[i]->GetPosition().z));
			static_cast<Character*>(m_ppGameObjects[i])->m_pHPBarObject->Rotate(0, 180, 0);
			mpHpBar->SetScale(10, 1, 1);
			mpHpBar->SetCurrentHP(m_ppGameObjects[i]->GetCurrentHP());
		}
	}*/
}

void GameobjectManager::NormalMonsterConditionAnimate(float fTimeElapsed)
{
	if (!m_bGameStart) {
		SetTempHP();
	}

	for (int i = 0; i < 15; i++) {
		if (m_ppNormalMonsterObject[i]->GetCurrentHP() != m_ppNormalMonsterObject[i]->GetTempHP() && m_ppNormalMonsterObject[i]->GetTempHP() > 0) {
			//m_ppNormalMonsterObject[i]->SetColor(XMFLOAT4(1,0,0,0.00012));
			g_sound.NoLoopPlay("MonsterAttackedSound", m_ppNormalMonsterObject[i]->CalculateDistanceSound());
			AddDamageFontToUiLayer(XMFLOAT3(m_ppNormalMonsterObject[i]->GetPosition().x,
				m_ppNormalMonsterObject[i]->GetPosition().y + 20,
				m_ppNormalMonsterObject[i]->GetPosition().z), int(m_ppNormalMonsterObject[i]->GetTempHP() - m_ppNormalMonsterObject[i]->GetCurrentHP()));
			m_ppNormalMonsterObject[i]->m_bAttacked = true;
		}
		if (m_ppNormalMonsterObject[i]->m_bAttacked && m_ppNormalMonsterObject[i]->m_fConditionTime < 0.23) {
			m_ppNormalMonsterObject[i]->SetColor(XMFLOAT4(0.8, 0.3, 0.1, 0.00012));
			m_ppNormalMonsterObject[i]->m_fConditionTime += fTimeElapsed;
		}
		else {
			if (m_ppNormalMonsterObject[i]->GetTempHP() <= 0) {
				if (m_ppNormalMonsterObject[i]->m_bActive) {
					m_ppNormalMonsterObject[i]->DieMonster(fTimeElapsed / 10);
				}
				if (m_ppNormalMonsterObject[i]->m_xmf4Color.w > 0.95) {
					m_ppNormalMonsterObject[i]->m_bActive = false;
				}

			}
			else {
				m_ppNormalMonsterObject[i]->m_bAttacked = false;
				m_ppNormalMonsterObject[i]->SetColor(XMFLOAT4(0, 0, 0, 0.00));
				m_ppNormalMonsterObject[i]->m_fConditionTime = 0;
			}
		}
		m_ppNormalMonsterObject[i]->SetTempHp(m_ppNormalMonsterObject[i]->GetCurrentHP());
	}

	//for (int i = 0; i < 15; i++) {
	//	

	//	m_ppNormalMonsterObject[i]->SetTempHp(m_ppNormalMonsterObject[i]->GetCurrentHP());
	//}
	//for (int i = 0; i < 15; i++) {
	//	


}

void GameobjectManager::PlayerConditionAnimate(float fTimeElapsed)
{
	if (g_Logic.GetMyRole()) {
		Character* myPlayCharacter = GetChracterInfo(g_Logic.GetMyRole());
		if (myPlayCharacter->m_fConditionTime < 1.2) {
			if (myPlayCharacter->GetCurrentHP() > myPlayCharacter->GetTempHP()) {
				m_pConditionUIObject->SetColor(XMFLOAT4(0, 0.7, 0.2, 0));
				myPlayCharacter->m_nCondition = 1;
			}
			if (myPlayCharacter->GetCurrentHP() < myPlayCharacter->GetTempHP()) {
				m_pConditionUIObject->SetColor(XMFLOAT4(1, 0, 0, 0));
				myPlayCharacter->m_nCondition = 1;
			}
			myPlayCharacter->m_fConditionTime += fTimeElapsed;
		}
		else
		{
			myPlayCharacter->m_nCondition = 0;
			myPlayCharacter->m_fConditionTime = 0;
			m_pConditionUIObject->SetColor(XMFLOAT4(0, 0, 0, -1));
		}

		myPlayCharacter->SetTempHp(myPlayCharacter->GetCurrentHP());
	}
}

void GameobjectManager::BossConditionAnimate(float fTimeElapsed)
{
	if (!m_bGameStart) {
		SetTempHP();
	}

	if (m_pMonsterObject->GetCurrentHP() != m_pMonsterObject->GetTempHP() && m_pMonsterObject->GetTempHP() > 0) {
		//m_pMonsterObject->SetColor(XMFLOAT4(1,0,0,0.00012));
		g_sound.NoLoopPlay("MonsterAttackedSound", m_pMonsterObject->CalculateDistanceSound());
		AddDamageFontToUiLayer(XMFLOAT3(m_pMonsterObject->GetPosition().x,
			m_pMonsterObject->GetPosition().y + 20,
			m_pMonsterObject->GetPosition().z), int(m_pMonsterObject->GetTempHP() - m_pMonsterObject->GetCurrentHP()));
		m_pMonsterObject->m_bAttacked = true;
	}
	if (m_pMonsterObject->m_bAttacked && m_pMonsterObject->m_fConditionTime < 0.23) {
		m_pMonsterObject->SetColor(XMFLOAT4(0.8, 0.3, 0.1, 0.00012));
		m_pMonsterObject->m_fConditionTime += fTimeElapsed;
	}
	else {
		if (m_pMonsterObject->GetTempHP() <= 0) {
			if (m_pMonsterObject->m_bActive) {
				//m_pMonsterObject->DieMonster(fTimeElapsed / 10);
			}
			if (m_pMonsterObject->m_xmf4Color.w > 0.95) {
				//m_pMonsterObject->m_bActive = false;
			}
		}
		else {
			m_pMonsterObject->m_bAttacked = false;
			m_pMonsterObject->SetColor(XMFLOAT4(0, 0, 0, 0.00));
			m_pMonsterObject->m_fConditionTime = 0;
		}
	}
	m_pMonsterObject->SetTempHp(m_pMonsterObject->GetCurrentHP());
}

void GameobjectManager::PlayerCurrentHpAnimate(float fTimeElapsed)
{
	m_pArcherObject->m_pHPBarUI->SetCurrentHP(m_pArcherObject->GetCurrentHP());
	m_pWarriorObject->m_pHPBarUI->SetCurrentHP(m_pWarriorObject->GetCurrentHP());
	m_pTankerObject->m_pHPBarUI->SetCurrentHP(m_pTankerObject->GetCurrentHP());
	m_pPriestObject->m_pHPBarUI->SetCurrentHP(m_pPriestObject->GetCurrentHP());
}

void GameobjectManager::OnPreRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pDepthShaderComponent->PrepareShadowMap(pd3dDevice, pd3dCommandList);
	//Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//렌더

	UpdateShaderVariables(pd3dCommandList);

	SkyboxRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pDepthShaderComponent) {
		m_pDepthShaderComponent->UpdateShaderVariables(pd3dCommandList);//오브젝트의 깊이값의 렌더입니다.
	}
	//g_Logic.m_MonsterSession.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	if (m_bDebugMode)
	{
		for (int i = 0; i < 5; i++)
			m_pBoundingBox[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		for (auto& p : m_ppObstacleBoundingBox)
		{
			p->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		}
		int normalMonsterIndex = 0;
		for (auto& p : m_ppNormalMonsterBoundingBox)
		{
			if (m_ppNormalMonsterObject[normalMonsterIndex++]->GetAliveState())
				p->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		}
	}

	//m_pUIGameSearchObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	//for (int i = 0; i < m_ppUIObjects.size(); i++) {
	//	m_ppUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}

	if (m_pShadowmapShaderComponent)
	{
		m_pShadowmapShaderComponent->Render(pd3dDevice, pd3dCommandList, 0, pd3dGraphicsRootSignature, m_fTimeElapsed, m_nStageType);

	}
	for (int i = 0; i < m_ppRockSpikeObjects.size(); ++i)
	{
		if (m_ppRockSpikeObjects[i] == nullptr) continue;
		if (m_ppRockSpikeObjects[i]->m_bActive == false) continue;
		m_ppRockSpikeObjects[i]->Animate(m_fTimeElapsed);
		m_ppRockSpikeObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		if(m_ppRockSpikeObjects[i]->m_pAttackedArea != nullptr)
			m_ppRockSpikeObjects[i]->m_pAttackedArea->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

	if (m_pNPCPressGObject) {
		m_pNPCPressGObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	//몬스터 체력바
	if (m_pMonsterHPBarObject) {
		m_pMonsterHPBarObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	for (int i = 0; i < m_pNormalMonsterHPBarObject.size(); i++) {
		if (m_ppNormalMonsterObject[i]->GetAliveState())
			m_pNormalMonsterHPBarObject[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	//몬스터 체력바
	if (m_pHealRange)
	{
		if (m_pHealRange->m_bActive)
		{
			XMFLOAT3 pos = m_pPriestObject->GetPosition();
			pos.y = 0.1f;
			m_pHealRange->SetPosition(pos);
			m_pHealRange->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		}
	}
	//오브젝트들 렌더 정리 필요
	for (int i = 0; i < m_ppProjectileObjects.size(); ++i) {
		if (m_ppProjectileObjects[i])
		{
			if (m_ppProjectileObjects[i]->m_bActive)
			{
				m_ppProjectileObjects[i]->Animate(m_fTimeElapsed);
				m_ppProjectileObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			}
			else
			{
				Character* possessObj = GetChracterInfo(m_ppProjectileObjects[i]->m_HostRole);
				m_ppProjectileObjects[i]->SetPosition(possessObj->GetPosition());
			}
		}
	}

	if (m_pBossSkillRange)
	{
		if (m_pBossSkillRange->m_bActive)
		{
			if (m_fTime - m_pBossSkillRange->m_fBossSkillTime > 6.0f)
				m_pBossSkillRange->m_bActive = false;
			m_pBossSkillRange->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		}
	}

	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
		if (m_ppParticleObjects[i]->m_bActive == true)
			m_ppParticleObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//파티클
	}

	TrailRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	EffectRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_fTime);

	AstarRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CrossHairRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pNaviMeshObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//네비 메쉬
	if (m_pTextureToViewportComponent)
	{
		//m_pTextureToViewportComponent->Render(pd3dCommandList, m_pCamera, 0, pd3dGraphicsRootSignature);//뎊스 렌더
	}

	if (GameEnd)
	{
		m_pVictoryUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pContinueUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		//m_pUIGameSearchObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	//sound
	//로비씬


}

void GameobjectManager::TrailRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//트레일 오브젝트 렌더입니다.
	//if (m_pTrailObject) {
		//m_pTrailObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
	if (m_pTrailComponent) {
		m_pTrailComponent->RenderTrail(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	for (int i = 0; i < m_pTrailArrowComponent.size(); i++) {
		if (m_pTrailArrowComponent[i]) {
			if (m_pArrowObjects[i]->m_bActive) {
				m_pTrailArrowObject[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_pTrailArrowComponent[i]->RenderTrail(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			}
		}
	}
	for (int i = 0; i < m_pTrailIceRanceObject.size(); i++) {
		if (m_pTrailIceRanceObject[i]) {
			if (m_pEnergyBallObjects[i]->m_bActive) {
				m_pTrailIceRanceObject[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_pTrailIceRanceComponent[i]->RenderTrail(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			}
		}
	}
}

void GameobjectManager::AstarRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//에이스타 오브젝트 렌더입니다.
	if (m_pAstarObject) {
		m_pAstarObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

	if (m_pAstarComponent) {
		m_nodeLock.lock();
		m_pAstarComponent->RenderAstar(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_VecNodeQueue);
		m_nodeLock.unlock();
	}
}

void GameobjectManager::UIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	UpdateShaderVariables(pd3dCommandList);
	for (int i = 0; i < m_ppUIObjects.size(); i++) {
		m_ppUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

}
#define ScreenSTORY 0
void GameobjectManager::CharacterUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	for (int i = 0; i < m_ppCharacterUIObjects.size(); i++) {
		//if(m_fStroyTime> m_ppStoryUIObjects.size()* ScreenSTORY)
		m_ppCharacterUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

}

void GameobjectManager::TalkUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	if (m_bNPCscreen) {

		m_pTalkUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pPressGUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	}
}

void GameobjectManager::StoryUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float ftimeElapsed)
{
	//m_fStroyTime += ftimeElapsed;
	//for (int i = 0; i < m_ppStoryUIObjects.size(); i++) {
	//	if (m_fStroyTime < 10 * (i + 1)&& m_fStroyTime > ScreenSTORY * (i ))
	//	m_ppStoryUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
}

void GameobjectManager::ReadObjectFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* fileName, CLoadedModelInfoCompnent* modelName, int type, int stagetype)
{
	ifstream objectFile(fileName);

	int objCount = -1;
	vector<XMFLOAT3> modelPosition;
	vector<XMFLOAT4> quaternion;

	vector<XMFLOAT3> modelScale;
	vector<XMFLOAT3> modelEulerRotate;

	vector<XMFLOAT3> colliderWorldPosition;
	vector<XMFLOAT3> colliderWorldBoundSize;
	vector<XMFLOAT3> colliderWorldExtentSize;

	string temp;
	float number[3] = {};
	float qnumber[4] = {};
	while (!objectFile.eof())
	{
		objectFile >> temp;
		if (temp == "<position>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> temp;
				number[i] = stof(temp);
			}
			modelPosition.emplace_back(number[0], number[1], number[2]);
		}
		else if (temp == "<quaternion>:")
		{
			for (int i = 0; i < 4; ++i)
			{
				objectFile >> temp;
				qnumber[i] = stof(temp);
			}
			quaternion.emplace_back(qnumber[0], qnumber[1], qnumber[2], qnumber[3]);
		}
		else if (temp == "<rotation>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> temp;
				number[i] = stof(temp);
			}
			modelEulerRotate.emplace_back(number[0], number[1], number[2]);
		}
		else if (temp == "<scale>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> temp;
				number[i] = stof(temp);
			}
			modelScale.emplace_back(number[0], number[1], number[2]);
		}
		else if (temp == "<BoxCollider>")
		{
			for (int j = 0; j < 7; ++j)
			{
				objectFile >> temp;
				if (temp == "<center>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					colliderWorldPosition.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<boundSize>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					colliderWorldBoundSize.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<extent>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					colliderWorldExtentSize.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<forward>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					///colliderForwardVec.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<right>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					//colliderRightVec.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<forwardDotRes>:")
				{
					objectFile >> temp;
					number[0] = stof(temp);
					//forwardDot.emplace_back(number[0]);
				}
				else if (temp == "<rightDotRes>:")
				{
					objectFile >> temp;
					number[0] = stof(temp);
					//rightDot.emplace_back(number[0]);
				}
			}
		}
		else
		{
			if (type == 0)
				objectFile >> temp;
			objCount++;
		}

	}
	GameObject** tempObject = new GameObject * [objCount];	// 멤버 변수로 교체 예정
	float scale = 1;
	if (type == 1)
		scale = 1.0f;

	for (int i = 0; i < objCount; ++i)
	{
		tempObject[i] = new GameObject(UNDEF_ENTITY);
		tempObject[i]->InsertComponent<RenderComponent>();
		tempObject[i]->InsertComponent<CLoadedModelInfoCompnent>();
		tempObject[i]->SetPosition(XMFLOAT3(modelPosition[i].x, modelPosition[i].y, modelPosition[i].z));
		tempObject[i]->SetModel(modelName);
		tempObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		//XMFLOAT3 Axis = XMFLOAT3(1, 0, 0);
		/*tempObject[i]->Rotate(&Axis, modelEulerRotate[i].x);
		Axis = tempObject[i]->GetUp();
		tempObject[i]->Rotate(&Axis, modelEulerRotate[i].y);
		Axis = tempObject[i]->GetRight();
		tempObject[i]->Rotate(&Axis, modelEulerRotate[i].z);*/
		tempObject[i]->Rotate(&quaternion[i]);
		tempObject[i]->m_nStageType = stagetype;
		tempObject[i]->SetScale(modelScale[i].x * scale, modelScale[i].y * scale, modelScale[i].z * scale);
		if (type == 1)
		{
			/*XMFLOAT3 extentPos = XMFLOAT3(tempScale[i].x * tempExtentScale[i].x,
				tempScale[i].y * tempExtentScale[i].y, tempScale[i].z * tempExtentScale[i].z);
			XMFLOAT3 centerPos = Vector3::Add(tempPos[i], tempLocalCenterPos[i]);
			extentPos = XMFLOAT3(extentPos.x * 0.5f, extentPos.y * 0.5f, extentPos.z * 0.5f);
			tempObject[i]->m_OBB = BoundingOrientedBox(tempCenterPos[i], extentPos, quaternion[i]);*/
			//m_ppObstacleObjects.emplace_back(tempObject[i]);
			//tempObject[i]->m_OBB = BoundingOrientedBox(colliderWorldPosition[i], colliderWorldExtentSize[i], quaternion[i]);
			//m_ppObstacleObjects.emplace_back(tempObject[i]);

			GameObject* tempBoundingBox = new GameObject(SQUARE_ENTITY);
			tempBoundingBox->InsertComponent<RenderComponent>();
			tempBoundingBox->InsertComponent<CubeMeshComponent>();
			tempBoundingBox->InsertComponent<BoundingBoxShaderComponent>();
			tempBoundingBox->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

			tempBoundingBox->SetPosition(colliderWorldPosition[i]);
			tempBoundingBox->Rotate(&quaternion[i]);
			tempBoundingBox->SetScale(colliderWorldBoundSize[i].x, colliderWorldBoundSize[i].y, colliderWorldBoundSize[i].z);
			m_ppObstacleBoundingBox.emplace_back(tempBoundingBox);
		}
		m_ppGameObjects.emplace_back(tempObject[i]);
	}
}

void GameobjectManager::ReadNormalMonsterFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* fileName, CLoadedModelInfoCompnent* modelName, int type, int stagetype)
{
	m_ppNormalMonsterObject = new NormalMonster * [15];
	ifstream objectFile(fileName);
	int objCount = -1;
	vector<XMFLOAT3> tempPos;
	vector<XMFLOAT4> quaternion;

	vector<XMFLOAT3> tempScale;
	vector<XMFLOAT3> tempRotate;

	string temp;
	float number[3] = {};
	float qnumber[4] = {};
	while (!objectFile.eof())
	{
		objectFile >> temp;
		if (temp == "<position>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> temp;
				number[i] = stof(temp);
			}
			tempPos.emplace_back(number[0], number[1], number[2]);
		}
		else if (temp == "<quaternion>:")
		{
			for (int i = 0; i < 4; ++i)
			{
				objectFile >> temp;
				qnumber[i] = stof(temp);
			}
			quaternion.emplace_back(qnumber[0], qnumber[1], qnumber[2], qnumber[3]);

		}
		else if (temp == "<rotation>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> temp;
				number[i] = stof(temp);
			}
			tempRotate.emplace_back(number[0], number[1], number[2]);
		}
		else if (temp == "<scale>:")
		{
			for (int i = 0; i < 3; ++i)
			{
				objectFile >> temp;
				number[i] = stof(temp);
			}
			tempScale.emplace_back(number[0], number[1], number[2]);
		}
		else
		{
			objectFile >> temp;
			objCount++;
		}
	}
	NormalMonster** tempObject = new NormalMonster * [objCount];
	m_ppNormalMonsterBoundingBox.reserve(sizeof(size_t) * objCount);
	vector<MonsterInitData>& monsterDatas = g_stage1MapData.GetMonsterData();
	float fScale = 1.0f;
	if (type == 1)
		fScale = 10.0f;
	for (int i = 0; i < objCount; ++i)
	{
		tempObject[i] = new NormalMonster();
		tempObject[i]->m_fObjectType = 1;
		tempObject[i]->m_nID = i;
		tempObject[i]->InsertComponent<RenderComponent>();
		tempObject[i]->InsertComponent<CLoadedModelInfoCompnent>();
		tempObject[i]->SetModel(modelName);
		//tempObject[i]->SetPosition(XMFLOAT3(tempPos[i].x * fScale, tempPos[i].y, tempPos[i].z * fScale));		
		tempObject[i]->SetPosition(monsterDatas[i].position);
		tempObject[i]->SetAnimationSets(6);
		tempObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		tempObject[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(6);
		tempObject[i]->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
		tempObject[i]->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_FIRSTSKILL]->m_nType = ANIMATION_TYPE_ONCE;
		XMFLOAT3 Axis = XMFLOAT3(1, 0, 0);
		//tempObject[i]->Rotate(&Axis, tempRotate[i].x);
		tempObject[i]->Rotate(&Axis, monsterDatas[i].eulerRotate.x);
		Axis = tempObject[i]->GetUp();
		//tempObject[i]->Rotate(&Axis, tempRotate[i].y);
		tempObject[i]->Rotate(&Axis, monsterDatas[i].eulerRotate.y);
		Axis = tempObject[i]->GetRight();
		//tempObject[i]->Rotate(&Axis, tempRotate[i].z);
		tempObject[i]->Rotate(&Axis, monsterDatas[i].eulerRotate.z);
		tempObject[i]->SetScale(tempScale[i].x, tempScale[i].y, tempScale[i].z);
		XMFLOAT3 t = tempObject[i]->GetPosition();
		tempObject[i]->m_nStageType = stagetype;
		// 바운딩 스피어 추가 필요
		GameObject* MonsterBoundingSphere = new GameObject(SQUARE_ENTITY);
		MonsterBoundingSphere->InsertComponent<RenderComponent>();
		MonsterBoundingSphere->InsertComponent<SphereMeshComponent>();
		MonsterBoundingSphere->InsertComponent<BoundingBoxShaderComponent>();
		MonsterBoundingSphere->SetBoundingSize(8.0f);
		MonsterBoundingSphere->SetBoundingOffset(XMFLOAT3(0.0f, 8.0f, 0.0f));
		MonsterBoundingSphere->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		MonsterBoundingSphere->SetScale(1.f);
		tempObject[i]->SetBoundingBox(MonsterBoundingSphere);

		//
		GameObject* mpMonsterHPBarObject = new GameObject(UNDEF_ENTITY);
		mpMonsterHPBarObject->InsertComponent<RenderComponent>();
		mpMonsterHPBarObject->InsertComponent<UIMeshComponent>();
		mpMonsterHPBarObject->InsertComponent <BlendShaderComponent>();
		mpMonsterHPBarObject->InsertComponent<TextureComponent>();
		mpMonsterHPBarObject->SetTexture(L"UI/HpBar.dds", RESOURCE_TEXTURE2D, 3);
		mpMonsterHPBarObject->SetPosition(XMFLOAT3(0, 40, 100));
		mpMonsterHPBarObject->SetScale(3);
		mpMonsterHPBarObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		tempObject[i]->m_pHPBarObject = mpMonsterHPBarObject;


		m_ppNormalMonsterBoundingBox.emplace_back(MonsterBoundingSphere);
		m_ppNormalMonsterObject[i] = tempObject[i];
		m_ppGameObjects.emplace_back(tempObject[i]);
	}
}

bool GameobjectManager::CheckCollideNPC()
{
	Character* myCharacter = GetChracterInfo(g_Logic.GetMyRole());
	if (myCharacter == nullptr) {
		exit(0);
	}
	if (m_pAngelNPCObject->m_SPBBNPC.Intersects(myCharacter->m_SPBB))
	{
		if (m_iTEXTiIndex != NPC_TEXT && m_bSendNpccollisionPK == false)
		{
			m_bNPCscreen = true;
			m_iTEXTiIndex = NPC_TEXT;
			m_bSendNPCPK = true;
			AddTextToUILayer(m_iTEXTiIndex);
			m_bNPCinteraction = true;
		}
	}
	return	m_pAngelNPCObject->m_SPBBNPC.Intersects(myCharacter->m_SPBB);
}

void GameobjectManager::CheckCollideNPCColorChange()//엔피씨 충돌시에 생기는 마법
{
	Character* myCharacter = GetChracterInfo(g_Logic.GetMyRole());
	if (m_pAngelNPCObject->m_SPBBNPC.Intersects(myCharacter->m_SPBB))
	{
		//	m_pAngelNPCObject->SetColor(XMFLOAT4(0.5, 0.5, 0.5, 0.5));
	}
}

void GameobjectManager::CheckCollidePortal()
{
	Character* myCharacter = GetChracterInfo(g_Logic.GetMyRole());
	if (myCharacter == nullptr) {
		exit(0);
	}
	if (m_SPBBPortal.Intersects(myCharacter->m_SPBB) && m_bPortalCheck == false)
	{
		m_bPortalCheck = true;
		//m_nStageType = 2;
		g_NetworkHelper.SendChangeStage_BOSS();
	}
}

Character* GameobjectManager::GetChracterInfo(ROLE r)
{
	if (r == ROLE::WARRIOR) return m_pWarriorObject;
	if (r == ROLE::PRIEST) return m_pPriestObject;
	if (r == ROLE::TANKER) return m_pTankerObject;
	if (r == ROLE::ARCHER) return m_pArcherObject;
	return nullptr;
}

void GameobjectManager::SetCharactersStage1Postion()
{
	m_pArcherObject->SetStage1Position();
	m_pTankerObject->SetStage1Position();
	m_pPriestObject->SetStage1Position();
	m_pWarriorObject->SetStage1Position();
}

void GameobjectManager::SetCharactersBossStagePostion()
{
	m_pArcherObject->SetBossStagePostion();
	m_pTankerObject->SetBossStagePostion();
	m_pPriestObject->SetBossStagePostion();
	m_pWarriorObject->SetBossStagePostion();
}

void GameobjectManager::SetCharactersLobbyPosition()
{
	m_pMonsterObject->SetPosition(XMFLOAT3(0, 0, 0));

	m_pPriestObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1520.f));

	m_pWarriorObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1460.f));
	m_pWarriorObject->Rotate(0.0f, -90.0f, 0.0f);

	m_pArcherObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1480.f));

	m_pTankerObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1500.0f));
	m_pTankerObject->Rotate(0.0f, -90.0f, 0.0f);

	m_pAngelNPCObject->SetPosition(XMFLOAT3(-1264.9f, 0.f, -1448.1f));//
	m_pAngelNPCObject->Rotate(0, -120.959, 0);
}

void GameobjectManager::EffectRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float ftimeElapsed)
{
	//for (auto& effect : m_ppEffectObjects) {
	//	SortEffect();
	//	//if (effect->m_bActive) {
	//		effect->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//	//}
	//}

	for (int i = 0; i < m_ppEffectObjects.size(); i++) {

		//if (effect->m_bActive) {
		m_ppEffectObjects[i]->CalculateDistance(m_pCamera->GetPosition());
		//}
	}
	SortEffect();
	for (int i = 0; i < m_ppEffectObjects.size(); i++) {
		if (m_ppEffectObjects[i] == nullptr) continue;
		if (m_ppEffectObjects[i]->m_bActive == false) continue;
		m_ppEffectObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	//for (auto& effect : m_ppShieldEffectObject)
	//{
	//	if (effect == nullptr) continue;
	//	if (effect->m_bActive == false) continue;
	//	effect->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
	//for (auto& effect : m_ppHealingEffectObject)
	//{
	//	if (effect == nullptr) continue;
	//	if (effect->m_bActive == false) continue;
	//	effect->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
	//if (m_pDebuffObject) {
	//	m_pDebuffObject->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
	//if (m_pLightEffectObject) {
	//	m_pLightEffectObject->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
	//if (m_pSheildEffectObject) {
	////	m_pSheildEffectObject->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
	//if (m_pPortalEffectObject) {
	//	m_pPortalEffectObject->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
}

void GameobjectManager::SkyboxRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	if (m_fStroyTime > 6) {
		m_pMonsterCubeObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	else {

		m_pSkyboxObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
}

void GameobjectManager::CrossHairRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	if (CheckCollision(m_ppGameObjects)) {
		m_pAttackUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	else {
		m_pBlueAttackUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
}

void GameobjectManager::BuildBossStageObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CLoadedModelInfoCompnent* Rock01Model = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock01.bin", NULL, true);
	CLoadedModelInfoCompnent* Rock02Model = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock02.bin", NULL, true);
	CLoadedModelInfoCompnent* Rock03Model = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock03.bin", NULL, true);
	CLoadedModelInfoCompnent* DeathModel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Death.bin", NULL, true);
	CLoadedModelInfoCompnent* RockSpikeModel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/RockSpike.bin", NULL, true);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock1.txt", Rock01Model, 1, STAGE2);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock2.txt", Rock02Model, 1, STAGE2);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock3.txt", Rock03Model, 1, STAGE2);

	for (int i = 0; i < 10; ++i)
	{
		RockSpike* pRockSpikeObject = new RockSpike();
		pRockSpikeObject->InsertComponent<RenderComponent>();
		pRockSpikeObject->InsertComponent<CLoadedModelInfoCompnent>();
		pRockSpikeObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		pRockSpikeObject->SetModel(RockSpikeModel);
		pRockSpikeObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		pRockSpikeObject->SetScale(50.0f, 50.0f, 50.0f);
		XMFLOAT3 right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		pRockSpikeObject->Rotate(&right, 90.0f);
		pRockSpikeObject->m_bActive = false;

		GameObject* pBossSkillRange = new GameObject(SQUARE_ENTITY);
		pBossSkillRange->InsertComponent<RenderComponent>();
		pBossSkillRange->InsertComponent<SquareMeshComponent>();
		pBossSkillRange->InsertComponent<BossSkillShaderComponent>();
		pBossSkillRange->SetSkillSize(10.0f);
		pBossSkillRange->m_iObjType = 1;
		pBossSkillRange->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		pBossSkillRange->SetPosition(XMFLOAT3(0, 0.1, 0));
		pBossSkillRange->m_fSkillTime = 0.0f;
		pRockSpikeObject->m_pAttackedArea = pBossSkillRange;

		m_ppRockSpikeObjects.emplace_back(pRockSpikeObject);
	}
}

void GameobjectManager::BuildProjectileObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CLoadedModelInfoCompnent* ArrowModel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Arrow.bin", NULL, true);
	CLoadedModelInfoCompnent* IceLanceModel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/IceLance.bin", NULL, true);


	Projectile** ppArrow = new Projectile * [MAX_ARROW];
	for (int i = 0; i < MAX_ARROW; ++i)
	{
		ppArrow[i] = new Arrow();
		ppArrow[i]->InsertComponent<RenderComponent>();
		ppArrow[i]->InsertComponent<CLoadedModelInfoCompnent>();
		ppArrow[i]->SetPosition(XMFLOAT3(0, 0, 0));
		ppArrow[i]->SetModel(ArrowModel);
		ppArrow[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		ppArrow[i]->SetScale(30.0f);
		ppArrow[i]->SetBoundingSize(4.0f);
		m_pArrowObjects[i] = ppArrow[i];
		m_ppProjectileObjects.emplace_back(ppArrow[i]);
	}
	m_pArcherObject->SetArrow(ppArrow);

	Arrow** m_ppArrowForQSkill = new Arrow * [3];
	for (int i = 0; i < 3; ++i)
	{
		m_ppArrowForQSkill[i] = new Arrow();
		m_ppArrowForQSkill[i]->InsertComponent<RenderComponent>();
		m_ppArrowForQSkill[i]->InsertComponent<CLoadedModelInfoCompnent>();
		m_ppArrowForQSkill[i]->SetPosition(XMFLOAT3(0, 0, 0));
		m_ppArrowForQSkill[i]->SetModel(ArrowModel);
		m_ppArrowForQSkill[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppArrowForQSkill[i]->SetScale(30.0f);
		m_ppArrowForQSkill[i]->SetBoundingSize(4.0f);
		m_pArrowObjects[i + 10] = m_ppArrowForQSkill[i];
		m_ppProjectileObjects.emplace_back(m_ppArrowForQSkill[i]);
	}
	m_pArcherObject->SetAdditionArrowForQSkill(m_ppArrowForQSkill);

	Arrow** m_ppArrowForESkill = new Arrow * [15];
	for (int i = 0; i < 15; ++i)
	{
		m_ppArrowForESkill[i] = new Arrow();
		m_ppArrowForESkill[i]->InsertComponent<RenderComponent>();
		m_ppArrowForESkill[i]->InsertComponent<CLoadedModelInfoCompnent>();
		m_ppArrowForESkill[i]->SetPosition(XMFLOAT3(0, 0, 0));
		m_ppArrowForESkill[i]->SetModel(ArrowModel);
		m_ppArrowForESkill[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppArrowForESkill[i]->SetScale(30.0f);
		m_ppArrowForESkill[i]->SetBoundingSize(4.0f);
		m_pArrowObjects[i + 13] = m_ppArrowForESkill[i];
		m_ppProjectileObjects.emplace_back(m_ppArrowForESkill[i]);
	}
	m_pArcherObject->SetAdditionArrowForESkill(m_ppArrowForESkill);

	Projectile** m_ppIceLanceObjects = new Projectile * [10];
	for (int i = 0; i < 10; ++i)
	{
		m_ppIceLanceObjects[i] = new IceLance();
		m_ppIceLanceObjects[i]->InsertComponent<RenderComponent>();
		m_ppIceLanceObjects[i]->InsertComponent<CLoadedModelInfoCompnent>();
		m_ppIceLanceObjects[i]->SetPosition(XMFLOAT3(25 * i, 0, 0));
		m_ppIceLanceObjects[i]->SetModel(IceLanceModel);
		//m_pEnergyBallObjects[i]->SetColor(XMFLOAT3(0,0.5,0);
		m_ppIceLanceObjects[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppIceLanceObjects[i]->SetScale(15.f, 20.f, 15.f);
		m_ppIceLanceObjects[i]->SetBoundingSize(4.0f);
		m_pEnergyBallObjects[i] = m_ppIceLanceObjects[i];
		m_ppProjectileObjects.emplace_back(m_ppIceLanceObjects[i]);
	}
	m_pPriestObject->SetProjectile(m_ppIceLanceObjects);
}



void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//빌드

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();

	m_pCamera->SetPosition(XMFLOAT3(-1450, 18, -1490));
	m_pCamera->Rotate(0, 90, 0);

	m_pPlaneObject = new GameObject(UNDEF_ENTITY);
	m_pPlaneObject->InsertComponent<RenderComponent>();
	m_pPlaneObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPlaneObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPlaneObject->SetModel("Model/Floor.bin");
	m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlaneObject->SetScale(1.0f, 1.0f, 1.0f);
	//m_pPlaneObject->SetColor(XMFLOAT4(0, 0, 0, 1));
	m_pPlaneObject->SetRimLight(false);
	m_ppGameObjects.emplace_back(m_pPlaneObject);
	BuildStage1(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildBossStageObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);




	for (int i = 0; i < 4; ++i)
	{
		m_pBoundingBox[i] = new GameObject(SQUARE_ENTITY);
		m_pBoundingBox[i]->InsertComponent<RenderComponent>();
		m_pBoundingBox[i]->InsertComponent<SphereMeshComponent>();
		m_pBoundingBox[i]->InsertComponent<BoundingBoxShaderComponent>();
		m_pBoundingBox[i]->SetBoundingSize(8.0f);
		m_pBoundingBox[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pBoundingBox[i]->SetScale(1.f);
	}

	m_pWarriorObject = new Warrior();
	m_pWarriorObject->InsertComponent<RenderComponent>();
	m_pWarriorObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pWarriorObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1460.f));
	// m_pWarriorObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pWarriorObject->SetModel("Model/Warrior.bin");
	m_pWarriorObject->SetAnimationSets(8);
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(8);
	m_pWarriorObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pWarriorObject->m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fSpeed = 1.5f;
	m_pWarriorObject->SetScale(30.0f);
	m_pWarriorObject->Rotate(0, -90, 0);
	m_pWarriorObject->SetBoundingBox(m_pBoundingBox[0]);
	m_ppGameObjects.emplace_back(m_pWarriorObject);

	m_pArcherObject = new Archer();
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pArcherObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1480.f));
	// m_pArcherObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pArcherObject->SetModel("Model/Archer.bin");
	m_pArcherObject->SetAnimationSets(5);
	m_pArcherObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackAnimationSet(5);
	m_pArcherObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pArcherObject->SetScale(30.0f);
	m_pArcherObject->Rotate(0, -90, 0);
	m_pArcherObject->SetBoundingBox(m_pBoundingBox[1]);
	m_ppGameObjects.emplace_back(m_pArcherObject);

	m_pTankerObject = new Tanker();
	m_pTankerObject->InsertComponent<RenderComponent>();
	m_pTankerObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pTankerObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1500.0f));
	// m_pTankerObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pTankerObject->SetModel("Model/Tanker.bin");
	m_pTankerObject->SetAnimationSets(7);
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(7);
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_SECONDSKILL]->m_nType = ANIMATION_TYPE_REVERSE;
	// m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_FIRSTSKILL]->m_nType = ANIMATION_TYPE_HALF;
	// m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fSpeed = 0.3f;
	m_pTankerObject->SetScale(30.0f);
	m_pTankerObject->Rotate(0, -90, 0);
	m_pTankerObject->SetBoundingBox(m_pBoundingBox[2]);
	m_ppGameObjects.emplace_back(m_pTankerObject);

	for (int i = 0; i < 4; ++i)
	{
		m_pTankerSkillEffects[i] = new EnergyBall();
		m_pTankerSkillEffects[i]->InsertComponent<RenderComponent>();
		m_pTankerSkillEffects[i]->InsertComponent<SphereMeshComponent>();
		m_pTankerSkillEffects[i]->InsertComponent<SphereShaderComponent>();
		m_pTankerSkillEffects[i]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pTankerSkillEffects[i]->SetBoundingSize(8);
		m_pTankerSkillEffects[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pTankerSkillEffects[i]->SetScale(1.0f);
		static_cast<Tanker*>(m_pTankerObject)->SetSkillBall(m_pTankerSkillEffects[i]);
		m_ppProjectileObjects.emplace_back(m_pTankerSkillEffects[i]);
	}

	m_pPriestObject = new Priest();
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPriestObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1520.f));
	// m_pPriestObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pPriestObject->SetModel("Model/Priests.bin");
	m_pPriestObject->SetAnimationSets(5);
	m_pPriestObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackAnimationSet(5);
	m_pPriestObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pPriestObject->SetScale(30.0f);
	m_pPriestObject->Rotate(0, -90, 0);
	m_pPriestObject->SetBoundingBox(m_pBoundingBox[3]);
	m_ppGameObjects.emplace_back(m_pPriestObject);

	m_pHealRange = new GameObject(SQUARE_ENTITY);
	m_pHealRange->InsertComponent<RenderComponent>();
	m_pHealRange->InsertComponent<SquareMeshComponent>();
	m_pHealRange->InsertComponent<SquareShaderComponent>();
	m_pHealRange->SetSkillSize(75.0f);
	m_pHealRange->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pHealRange->SetPosition(XMFLOAT3(0, 0.0f, 0));
	m_pHealRange->SetScale(1.0f);
	m_pHealRange->m_bActive = false;
	m_pPriestObject->SetSkillRangeObject(m_pHealRange);
	m_ppEffectObjects.emplace_back(m_pHealRange);

	m_pBoundingBox[4] = new GameObject(SQUARE_ENTITY);
	m_pBoundingBox[4]->InsertComponent<RenderComponent>();
	m_pBoundingBox[4]->InsertComponent<SphereMeshComponent>();
	m_pBoundingBox[4]->InsertComponent<BoundingBoxShaderComponent>();
	m_pBoundingBox[4]->SetBoundingSize(30.0f);
	m_pBoundingBox[4]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pBoundingBox[4]->SetScale(1.f);

	m_pMonsterObject = new Monster();
	m_pMonsterObject->InsertComponent<RenderComponent>();
	m_pMonsterObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pMonsterObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pMonsterObject->SetModel("Model/Boss.bin");
	m_pMonsterObject->SetAnimationSets(13);
	m_pMonsterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackAnimationSet(13);
	m_pMonsterObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[BOSS_ANIMATION::BA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pMonsterObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[BOSS_ANIMATION::BA_UNDERGROUND]->m_nType = ANIMATION_TYPE_ONCE;
	m_pMonsterObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[BOSS_ANIMATION::BA_REVERSE_SPAWN]->m_nType = ANIMATION_TYPE_REVERSE;
	m_pMonsterObject->SetScale(15.0f);
	m_pMonsterObject->m_fObjectType = 1;
	m_pMonsterObject->SetBoundingSize(30.0f);
	m_pMonsterObject->SetBoundingBox(m_pBoundingBox[4]);
	m_pMonsterObject->SetMoveState(false);
	m_ppGameObjects.emplace_back(m_pMonsterObject);

	m_pBossSkillRange = new GameObject(SQUARE_ENTITY);
	m_pBossSkillRange->InsertComponent<RenderComponent>();
	m_pBossSkillRange->InsertComponent<SquareMeshComponent>();
	m_pBossSkillRange->InsertComponent<BossSkillShaderComponent>();
	m_pBossSkillRange->SetSkillSize(100.0f);
	m_pBossSkillRange->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pBossSkillRange->SetPosition(XMFLOAT3(0, 0.2, 0));
	m_pBossSkillRange->SetColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pBossSkillRange->m_fSkillTime = 0.0f;
	m_pBossSkillRange->m_bActive = false;
	m_pMonsterObject->SetSkillRangeObject(m_pBossSkillRange);

	m_pSkyboxObject = new GameObject(SQUARE_ENTITY);
	m_pSkyboxObject->InsertComponent<RenderComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxMeshComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxShaderComponent>();
	m_pSkyboxObject->InsertComponent<TextureComponent>();
	m_pSkyboxObject->SetTexture(L"DreamWorld/SkyBox.dds", RESOURCE_TEXTURE_CUBE, 12);
	m_pSkyboxObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pSkyboxObject->SetScale(1, 1, 1);
	m_pSkyboxObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pMonsterCubeObject = new GameObject(SQUARE_ENTITY);
	m_pMonsterCubeObject->InsertComponent<RenderComponent>();
	m_pMonsterCubeObject->InsertComponent<SkyBoxMeshComponent>();
	m_pMonsterCubeObject->InsertComponent<SkyBoxShaderComponent>();
	m_pMonsterCubeObject->InsertComponent<TextureComponent>();
	m_pMonsterCubeObject->SetTexture(L"DreamWorld/DreamWorld.dds", RESOURCE_TEXTURE_CUBE, 12);
	m_pMonsterCubeObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pMonsterCubeObject->SetScale(1, 1, 1);
	m_pMonsterCubeObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pNaviMeshObject = new GameObject(SQUARE_ENTITY);
	m_pNaviMeshObject->InsertComponent<RenderComponent>();
	m_pNaviMeshObject->InsertComponent<CubeMeshComponent>();
	m_pNaviMeshObject->InsertComponent<NaviMeshShaderComponent>();
	m_pNaviMeshObject->SetPosition(XMFLOAT3(0, 1.0f, 0));
	m_pNaviMeshObject->SetScale(1, 1, 1);
	m_pNaviMeshObject->SetTexture(L"Model/Textures/1K_Mi24_TXTR.dds", RESOURCE_TEXTURE2D, 6);
	m_pNaviMeshObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

#if LOCAL_TASK
	// 플레이어가 캐릭터 선택하는 부분에 유사하게 넣을 예정
	// m_pWarriorObject m_pArcherObject m_pTankerObject m_pPriestObject
	// Archer Priest Tanker Warrior
	//m_pPlayerObject = new Priest();
	//m_pTankerObject->SetCamera(m_pCamera);
	//m_pPlayerObject = m_pTankerObject;
	//g_Logic.SetMyRole(ROLE::TANKER);
	// ARCHER PRIEST TANKER WARRIOR
#endif // LOCAL_TASK

	BuildProjectileObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildNPC(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildCharacterUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildShadow(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//무조건 마지막에 해줘야된다.
	// 서순을 잘챙기자 ㅋㅋ	
		//m_pTextureToViewportComponent = new TextureToViewportComponent();
		//m_pTextureToViewportComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
		//m_pTextureToViewportComponent->BuildObjects(pd3dDevice, pd3dCommandList, m_pDepthShaderComponent->GetDepthTexture());

	BuildParticle(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//BuildInstanceObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//BuildStoryUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildTrail(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildAstar(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}
void GameobjectManager::BuildParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pFireballSpriteObject = new GameObject(UNDEF_ENTITY);
	m_pFireballSpriteObject->InsertComponent<RenderComponent>();
	m_pFireballSpriteObject->InsertComponent<UIMeshComponent>();
	m_pFireballSpriteObject->InsertComponent<MultiSpriteShaderComponent>();
	m_pFireballSpriteObject->InsertComponent<TextureComponent>();
	m_pFireballSpriteObject->SetTexture(L"MagicEffect/BossFire.dds", RESOURCE_TEXTURE2D, 3);
	m_pFireballSpriteObject->SetPosition(XMFLOAT3(100, 40, 100));
	m_pFireballSpriteObject->SetScale(10);
	m_pFireballSpriteObject->SetRowColumn(4, 4, 0.03);
	m_pFireballSpriteObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppParticleObjects.emplace_back(m_pFireballSpriteObject);

	m_pLightningSpriteObject = new GameObject(UNDEF_ENTITY);
	m_pLightningSpriteObject->InsertComponent<RenderComponent>();
	m_pLightningSpriteObject->InsertComponent<UIMeshComponent>();
	m_pLightningSpriteObject->InsertComponent<MultiSpriteShaderComponent>();
	m_pLightningSpriteObject->InsertComponent<TextureComponent>();
	m_pLightningSpriteObject->SetTexture(L"MagicEffect/Lightning_2x2.dds", RESOURCE_TEXTURE2D, 3);
	m_pLightningSpriteObject->SetPosition(XMFLOAT3(0, 0, 100));
	m_pLightningSpriteObject->SetScale(7.0f, 20.0f, 7.0f);
	m_pLightningSpriteObject->SetRowColumn(2.0f, 2.0f, 0.06f);
	m_pLightningSpriteObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppParticleObjects.emplace_back(m_pLightningSpriteObject);


	//m_pMosterdebuffSpriteObject = new GameObject(UNDEF_ENTITY);
	//m_pMosterdebuffSpriteObject->InsertComponent<RenderComponent>();
	//m_pMosterdebuffSpriteObject->InsertComponent<UIMeshComponent>();
	//m_pMosterdebuffSpriteObject->InsertComponent<MultiSpriteShaderComponent>();
	//m_pMosterdebuffSpriteObject->InsertComponent<TextureComponent>();
	//m_pMosterdebuffSpriteObject->SetTexture(L"MagicEffect/Monsterdebuff_5x6.dds", RESOURCE_TEXTURE2D, 3);
	//m_pMosterdebuffSpriteObject->SetPosition(XMFLOAT3(0, 40, 130));
	//m_pMosterdebuffSpriteObject->SetScale(2);
	//m_pMosterdebuffSpriteObject->SetRowColumn(5, 6, 0.03);
	//m_pMosterdebuffSpriteObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppParticleObjects.emplace_back(m_pMosterdebuffSpriteObject);


	//m_pSwordFireObject = new GameObject(UNDEF_ENTITY);
	//m_pSwordFireObject->InsertComponent<RenderComponent>();
	//m_pSwordFireObject->InsertComponent<UIMeshComponent>();
	//m_pSwordFireObject->InsertComponent<MultiSpriteShaderComponent>();
	//m_pSwordFireObject->InsertComponent<TextureComponent>();
	//m_pSwordFireObject->SetTexture(L"MagicEffect/SwordEffectFire_5x5.dds", RESOURCE_TEXTURE2D, 3);
	//m_pSwordFireObject->SetPosition(XMFLOAT3(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().x,
	//	m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().y,
	//	m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().z));
	//m_pSwordFireObject->SetScale(10);
	//m_pSwordFireObject->SetRowColumn(5, 5, 0.005);
	//m_pSwordFireObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppParticleObjects.emplace_back(m_pSwordFireObject);


	//m_pFireballSpriteObjects.resize(20);
	//m_pFireballSpriteObjects[0] = new GameObject(UNDEF_ENTITY);
	//m_pFireballSpriteObjects[0]->InsertComponent<RenderComponent>();
	//m_pFireballSpriteObjects[0]->InsertComponent<CLoadedModelInfoCompnent>();
	//m_pFireballSpriteObjects[0]->SetPosition(XMFLOAT3(100, 0, 0));
	//m_pFireballSpriteObjects[0]->SetModel("Model/RockSpike.bin");
	//m_pFireballSpriteObjects[0]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pFireballSpriteObjects[0]->SetScale(30.0f, 30.0f, 30.0f);
	//for (int i = 1; i < 20; i++) {
	//	m_pFireballSpriteObjects[i] = new GameObject(UNDEF_ENTITY);
	//	m_pFireballSpriteObjects[i]->SetPosition(XMFLOAT3(0, i * 10, 0));
	//	m_pFireballSpriteObjects[i]->SetScale(30.0f, 30.0f, 30.0f);
	//	m_ppParticleObjects.emplace_back(m_pFireballSpriteObjects[i]);
	//}
}
void GameobjectManager::BuildTrail(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pTrailObject = new GameObject(UNDEF_ENTITY);
	m_pTrailObject->InsertComponent<RenderComponent>();
	m_pTrailObject->InsertComponent<TrailMeshComponent>();
	m_pTrailObject->InsertComponent<TrailShaderComponent>();
	m_pTrailObject->InsertComponent<TextureComponent>();
	m_pTrailObject->SetTexture(L"Trail/FireTrail4.dds", RESOURCE_TEXTURE2D, 3);
	m_pTrailObject->SetPosition(XMFLOAT3(0, 0, 0));
	//m_pTrailObject->SetColor(XMFLOAT4(1.0f, 0.3f, 0.0f, 0.0f));
	m_pTrailObject->SetScale(1);
	m_pTrailObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTrailComponent = new TrailComponent();
	m_pTrailComponent->ReadyComponent(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pTrailObject);
	m_pWarriorObject->SetTrailComponent(m_pTrailComponent);
	/////////////////////////////////////////Arrow///////////////////////////////
	//conflict
	for (int i = 0; i < m_pTrailArrowObject.size(); i++) {

		m_pTrailArrowObject[i] = new GameObject(UNDEF_ENTITY);
		m_pTrailArrowObject[i]->InsertComponent<RenderComponent>();
		m_pTrailArrowObject[i]->InsertComponent<TrailMeshComponent>();
		m_pTrailArrowObject[i]->InsertComponent<TrailShaderComponent>();
		m_pTrailArrowObject[i]->InsertComponent<TextureComponent>();
		m_pTrailArrowObject[i]->SetTexture(L"Trail/FireTrail.dds", RESOURCE_TEXTURE2D, 3);
		m_pTrailArrowObject[i]->SetPosition(XMFLOAT3(0, 0, 0));
		m_pTrailArrowObject[i]->SetColor(XMFLOAT4(1.0f, 0.3f, 0.0f, 0.0f));
		m_pTrailArrowObject[i]->SetScale(1);
		m_pTrailArrowObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pTrailArrowComponent[i] = new TrailComponent();
		m_pTrailArrowComponent[i]->ReadyComponent(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pTrailArrowObject[i]);
		m_pTrailArrowComponent[i]->SetRenderingTrail(true);
		m_pArrowObjects[i]->SetTrailComponent(m_pTrailArrowComponent[i]);
	}
	//m_pWarriorObject->m_pTrail = m_pTrailComponent;

	for (int i = 0; i < m_pTrailIceRanceObject.size(); i++) {

		m_pTrailIceRanceObject[i] = new GameObject(UNDEF_ENTITY);
		m_pTrailIceRanceObject[i]->InsertComponent<RenderComponent>();
		m_pTrailIceRanceObject[i]->InsertComponent<TrailMeshComponent>();
		m_pTrailIceRanceObject[i]->InsertComponent<TrailShaderComponent>();
		m_pTrailIceRanceObject[i]->InsertComponent<TextureComponent>();
		m_pTrailIceRanceObject[i]->SetTexture(L"Trail/FireTrail.dds", RESOURCE_TEXTURE2D, 3);
		m_pTrailIceRanceObject[i]->SetPosition(XMFLOAT3(0, 0, 0));
		m_pTrailIceRanceObject[i]->SetColor(XMFLOAT4(-1.0f, 0.0f, 1.0f, 0.0f));
		m_pTrailIceRanceObject[i]->SetScale(1);
		m_pTrailIceRanceObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pTrailIceRanceObject[i]->m_bActive = true;
		m_pTrailIceRanceComponent[i] = new TrailComponent();
		m_pTrailIceRanceComponent[i]->ReadyComponent(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pTrailIceRanceObject[i]);
	}

}
void GameobjectManager::BuildAstar(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pAstarObject = new GameObject(UNDEF_ENTITY);
	m_pAstarObject->InsertComponent<RenderComponent>();
	m_pAstarObject->InsertComponent<TrailMeshComponent>();
	m_pAstarObject->InsertComponent<TrailShaderComponent>();
	m_pAstarObject->InsertComponent<TextureComponent>();
	m_pAstarObject->SetTexture(L"Trail/Trail.dds", RESOURCE_TEXTURE2D, 3);
	m_pAstarObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pAstarObject->SetScale(1);
	m_pAstarObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pAstarComponent = new TrailComponent();
	m_pAstarComponent->ReadyComponent(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pAstarObject);
}
void GameobjectManager::BuildStage1(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CLoadedModelInfoCompnent* BigMushroom = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BigMushroom.bin", NULL, true);
	CLoadedModelInfoCompnent* Mushroom = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Mushroom.bin", NULL, true);
	CLoadedModelInfoCompnent* LongFence = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/LongFence.bin", NULL, true);
	CLoadedModelInfoCompnent* ShortFence01 = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence01.bin", NULL, true);
	CLoadedModelInfoCompnent* ShortFence02 = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence02.bin", NULL, true);
	CLoadedModelInfoCompnent* ShortFence03 = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence03.bin", NULL, true);
	CLoadedModelInfoCompnent* Tree = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Tree.bin", NULL, true);
	CLoadedModelInfoCompnent* Death = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Death.bin", NULL, true);
	CLoadedModelInfoCompnent* Cube = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Cube.bin", NULL, true);
	CLoadedModelInfoCompnent* BigRock = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BigRock.bin", NULL, true);
	CLoadedModelInfoCompnent* TripleBarel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/TripleBarel.bin", NULL, true);

	m_pStage1Objects[0] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[0]->InsertComponent<RenderComponent>();
	m_pStage1Objects[0]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[0]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[0]->SetModel("Model/New_Terrain.bin");
	m_pStage1Objects[0]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[0]->SetScale(10);
	m_pStage1Objects[0]->SetColor(XMFLOAT4(0, 1.0f, 102.7f, 0));
	m_pStage1Objects[0]->SetRimLight(false);
	m_pStage1Objects[0]->m_nStageType = STAGE1;
	m_ppGameObjects.emplace_back(m_pStage1Objects[0]);

	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BigMushroom.txt", BigMushroom, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Mushroom.txt", Mushroom, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/LongFence.txt", LongFence, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence1.txt", ShortFence01, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence2.txt", ShortFence02, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence3.txt", ShortFence03, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Tree.txt", Tree, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BigRock.txt", BigRock, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/TripleBarel.txt", TripleBarel, 0, STAGE1);
	//ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/OOBB.txt", Cube, 0, STAGE1);
	ReadNormalMonsterFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/NormalMonsterS1.txt", Death, 1, STAGE1);


	vector<MapCollide>& collides = g_stage1MapData.GetCollideData();
	for (auto& col : collides) {
		auto& boundingBox = col.GetObb();
		XMFLOAT4 q = boundingBox.Orientation;

		GameObject* BoundingBox = new GameObject(SQUARE_ENTITY);
		BoundingBox->InsertComponent<RenderComponent>();
		BoundingBox->InsertComponent<CubeMeshComponent>();
		BoundingBox->InsertComponent<BoundingBoxShaderComponent>();
		BoundingBox->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

		BoundingBox->SetPosition(boundingBox.Center);
		BoundingBox->Rotate(&q);
		BoundingBox->SetScale(boundingBox.Extents.x * 2.0f, boundingBox.Extents.y * 2.0f, boundingBox.Extents.z * 2.0f);
		m_ppObstacleBoundingBox.emplace_back(BoundingBox);
	}
	/*m_pStage1Objects[1] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[1]->InsertComponent<RenderComponent>();
	m_pStage1Objects[1]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[1]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[1]->SetModel("Model/New_Terrain.bin");
	m_pStage1Objects[1]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[1]->SetScale(30.0f, 30.0f, 30.0f);
	m_pStage1Objects[1]->SetRimLight(false);*/
	/*m_pStage1Objects[2] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[2]->InsertComponent<RenderComponent>();
	m_pStage1Objects[2]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[2]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[2]->SetModel("Model/Rock.bin");
	m_pStage1Objects[2]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[2]->SetScale(30.0f, 30.0f, 30.0f);
	m_pStage1Objects[2]->SetRimLight(false);
	m_pStage1Objects[3] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[3]->InsertComponent<RenderComponent>();
	m_pStage1Objects[3]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[3]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[3]->SetModel("Model/Rock2.bin");
	m_pStage1Objects[3]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[3]->SetScale(30.0f, 30.0f, 30.0f);
	m_pStage1Objects[3]->SetRimLight(false);
	m_pStage1Objects[4] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[4]->InsertComponent<RenderComponent>();
	m_pStage1Objects[4]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[4]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[4]->SetModel("Model/Mushroom.bin");
	m_pStage1Objects[4]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[4]->SetScale(30.0f, 30.0f, 30.0f);
	m_pStage1Objects[4]->SetRimLight(false);
	m_pStage1Objects[5] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[5]->InsertComponent<RenderComponent>();
	m_pStage1Objects[5]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[5]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[5]->SetModel("Model/Street.bin");
	m_pStage1Objects[5]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[5]->SetScale(30.0f, 30.0f, 30.0f);
	m_pStage1Objects[5]->SetRimLight(false);*/


	//m_pStage1TerrainObject = new GameObject(UNDEF_ENTITY);
	//m_pStage1TerrainObject->InsertComponent<RenderComponent>();
	//m_pStage1TerrainObject->InsertComponent<HeihtMapMeshComponent>();
	//m_pStage1TerrainObject->InsertComponent<ShaderComponent>();
	//m_pStage1TerrainObject->InsertComponent<TextureComponent>();
	//m_pStage1TerrainObject->SetTexture(L"MagicEffect/FireballEmission_7x7.dds", RESOURCE_TEXTURE2D, 3);
	//m_pStage1TerrainObject->SetFileName(_T("Terrain/terrain.raw"));
	//m_pStage1TerrainObject->SetPosition(XMFLOAT3(0, 0, 0));
	//m_pStage1TerrainObject->Rotate(0, 180, 0);
	//m_pStage1TerrainObject->SetScale(1, 1, 1);
	//m_pStage1TerrainObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}
void GameobjectManager::BuildLight()
{
	m_pLight->BuildLight();
}
void GameobjectManager::BuildShadow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pDepthShaderComponent = new DepthRenderShaderComponent();
	m_pDepthShaderComponent->BuildDepth(m_ppGameObjects, m_pLight->GetLight());
	m_pDepthShaderComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	m_pDepthShaderComponent->CreateCbvSrvDescriptorHeaps(pd3dDevice, 2, 2);
	//m_pDepthShaderComponent->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObjects, ncbElementBytes);
	//m_pDepthShaderComponent->CreateShaderResourceViews(pd3dDevice, m_pTextureComponent, 0, m_nRootParameter, pShadowMap);//texture입력
	//m_pDepthShaderComponent->SetCbvGPUDescriptorHandlePtr(m_pShaderComponent->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
	m_pDepthShaderComponent->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	m_pShadowmapShaderComponent = new ShadowMapShaderComponent();
	m_pShadowmapShaderComponent->BuildShadow(m_ppGameObjects);
	m_pShadowmapShaderComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	m_pShadowmapShaderComponent->BuildObjects(pd3dDevice, pd3dCommandList, m_pDepthShaderComponent->GetDepthTexture());
}
void GameobjectManager::Build2DUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();

	m_pUIGameSearchObject = new GameObject(UI_ENTITY);
	m_pUIGameSearchObject->InsertComponent<RenderComponent>();
	m_pUIGameSearchObject->InsertComponent<UIMeshComponent>();
	m_pUIGameSearchObject->InsertComponent<UiShaderComponent>();
	m_pUIGameSearchObject->InsertComponent<TextureComponent>();
	m_pUIGameSearchObject->SetTexture(L"UI/PressButtonPink.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameSearchObject->SetPosition(XMFLOAT3(-0.35, 0.45, 1.00));
	m_pUIGameSearchObject->SetScale(0.05, 0.02, 1);
	m_pUIGameSearchObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameSearchObject);

	m_pUIGameEndObject = new GameObject(UI_ENTITY);
	m_pUIGameEndObject->InsertComponent<RenderComponent>();
	m_pUIGameEndObject->InsertComponent<UIMeshComponent>();
	m_pUIGameEndObject->InsertComponent<UiShaderComponent>();
	m_pUIGameEndObject->InsertComponent<TextureComponent>();
	m_pUIGameEndObject->SetTexture(L"UI/PressButtonPinkEnd.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameEndObject->SetPosition(XMFLOAT3(0.35, 0.45, 1.00));
	m_pUIGameEndObject->SetScale(0.05, 0.02, 1);
	m_pUIGameEndObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameEndObject);


	m_pUIWarriorCharacterObject = new GameObject(UI_ENTITY);
	m_pUIWarriorCharacterObject->InsertComponent<RenderComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<TextureComponent>();
	m_pUIWarriorCharacterObject->SetTexture(L"UI/PressButtonGreen.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIWarriorCharacterObject->SetPosition(XMFLOAT3(-0.6, 0.12, 1.01));
	m_pUIWarriorCharacterObject->SetScale(0.05, 0.02, 1);
	m_pUIWarriorCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIWarriorCharacterObject);

	m_pUIArcherCharacterObject = new GameObject(UI_ENTITY);
	m_pUIArcherCharacterObject->InsertComponent<RenderComponent>();
	m_pUIArcherCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIArcherCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIArcherCharacterObject->InsertComponent<TextureComponent>();
	m_pUIArcherCharacterObject->SetTexture(L"UI/PressButtonGreen.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIArcherCharacterObject->SetPosition(XMFLOAT3(-0.2, 0.12, 1.01));
	m_pUIArcherCharacterObject->SetScale(0.05, 0.02, 1);
	m_pUIArcherCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIArcherCharacterObject);

	m_pUITankerCharacterObject = new GameObject(UI_ENTITY);
	m_pUITankerCharacterObject->InsertComponent<RenderComponent>();
	m_pUITankerCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUITankerCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUITankerCharacterObject->InsertComponent<TextureComponent>();
	m_pUITankerCharacterObject->SetTexture(L"UI/PressButtonGreen.dds", RESOURCE_TEXTURE2D, 3);
	m_pUITankerCharacterObject->SetPosition(XMFLOAT3(0.2, 0.12, 1.01));
	m_pUITankerCharacterObject->SetScale(0.05, 0.02, 1);
	m_pUITankerCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUITankerCharacterObject);

	m_pUIPriestCharacterObject = new GameObject(UI_ENTITY);
	m_pUIPriestCharacterObject->InsertComponent<RenderComponent>();
	m_pUIPriestCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIPriestCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIPriestCharacterObject->InsertComponent<TextureComponent>();
	m_pUIPriestCharacterObject->SetTexture(L"UI/PressButtonGreen.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIPriestCharacterObject->SetPosition(XMFLOAT3(0.6, 0.12, 1.01));
	m_pUIPriestCharacterObject->SetScale(0.05, 0.02, 1);
	m_pUIPriestCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIPriestCharacterObject);

	//SECTION2 
	/*m_pUIEnterRoomObject = new GameObject(UI_ENTITY);
	m_pUIEnterRoomObject->InsertComponent<RenderComponent>();
	m_pUIEnterRoomObject->InsertComponent<UIMeshComponent>();
	m_pUIEnterRoomObject->InsertComponent<UiShaderComponent>();
	m_pUIEnterRoomObject->InsertComponent<TextureComponent>();
	m_pUIEnterRoomObject->SetTexture(L"UI/PlayerApply.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIEnterRoomObject->SetPosition(XMFLOAT3(0.15, -0.5, 1.00));
	m_pUIEnterRoomObject->SetScale(0.05, 0.02, 1);
	m_pUIEnterRoomObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIEnterRoomObject);

	m_pUIGameCreateObject = new GameObject(UI_ENTITY);
	m_pUIGameCreateObject->InsertComponent<RenderComponent>();
	m_pUIGameCreateObject->InsertComponent<UIMeshComponent>();
	m_pUIGameCreateObject->InsertComponent<UiShaderComponent>();
	m_pUIGameCreateObject->InsertComponent<TextureComponent>();
	m_pUIGameCreateObject->SetTexture(L"UI/CreateRoom.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameCreateObject->SetPosition(XMFLOAT3(0.45, -0.5, 1.00));
	m_pUIGameCreateObject->SetScale(0.05, 0.02, 1);
	m_pUIGameCreateObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameCreateObject);*/


	//m_ppUIObjects.emplace_back(m_pUIGameCreateObject);

	//m_pPlayerObject->SetCamera(m_pCamera);
}

void GameobjectManager::BuildCharacterUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{


	m_pTalkUIObject = new GameObject(UI_ENTITY);
	m_pTalkUIObject->InsertComponent<RenderComponent>();
	m_pTalkUIObject->InsertComponent<UIMeshComponent>();
	m_pTalkUIObject->InsertComponent<UiShaderComponent>();
	m_pTalkUIObject->InsertComponent<TextureComponent>();
	m_pTalkUIObject->SetTexture(L"UI/TalkUI.dds", RESOURCE_TEXTURE2D, 3);
	m_pTalkUIObject->SetPosition(XMFLOAT3(0.0, -0.47, 1.01));
	m_pTalkUIObject->SetScale(0.34, 0.28, 1);
	m_pTalkUIObject->SetColor(XMFLOAT4(0, 0, 0, 0.0));
	m_pTalkUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pPressGUIObject = new GameObject(UI_ENTITY);
	m_pPressGUIObject->InsertComponent<RenderComponent>();
	m_pPressGUIObject->InsertComponent<UIMeshComponent>();
	m_pPressGUIObject->InsertComponent<UiShaderComponent>();
	m_pPressGUIObject->InsertComponent<TextureComponent>();
	m_pPressGUIObject->SetTexture(L"UI/PressGPink.dds", RESOURCE_TEXTURE2D, 3);
	m_pPressGUIObject->SetPosition(XMFLOAT3(0.63, -0.52, 1.005));
	m_pPressGUIObject->SetScale(0.02, 0.02, 1);
	m_pPressGUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	////////////////////////skill/////////////////////////////////////////////////
	/*m_pHealSkillUIObject = new GameObject(UI_ENTITY);
	m_pHealSkillUIObject->InsertComponent<RenderComponent>();
	m_pHealSkillUIObject->InsertComponent<UIMeshComponent>();
	m_pHealSkillUIObject->InsertComponent<UiShaderComponent>();
	m_pHealSkillUIObject->InsertComponent<TextureComponent>();
	m_pHealSkillUIObject->SetTexture(L"UI/HealSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pHealSkillUIObject->SetPosition(XMFLOAT3(-0.975, 0.2, 1.00));
	m_pHealSkillUIObject->SetScale(0.02, 0.02, 1);
	m_pHealSkillUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pHealSkillUIObject);

	m_pLightningSkillUIObject = new GameObject(UI_ENTITY);
	m_pLightningSkillUIObject->InsertComponent<RenderComponent>();
	m_pLightningSkillUIObject->InsertComponent<UIMeshComponent>();
	m_pLightningSkillUIObject->InsertComponent<UiShaderComponent>();
	m_pLightningSkillUIObject->InsertComponent<TextureComponent>();
	m_pLightningSkillUIObject->SetTexture(L"UI/HealSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pLightningSkillUIObject->SetPosition(XMFLOAT3(-0.975, 0.2, 1.00));
	m_pLightningSkillUIObject->SetScale(0.02, 0.02, 1);
	m_pLightningSkillUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pLightningSkillUIObject);

	m_pShieldSkillUIObject = new GameObject(UI_ENTITY);
	m_pShieldSkillUIObject->InsertComponent<RenderComponent>();
	m_pShieldSkillUIObject->InsertComponent<UIMeshComponent>();
	m_pShieldSkillUIObject->InsertComponent<UiShaderComponent>();
	m_pShieldSkillUIObject->InsertComponent<TextureComponent>();
	m_pShieldSkillUIObject->SetTexture(L"UI/HealSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pShieldSkillUIObject->SetPosition(XMFLOAT3(-0.975, 0.2, 1.00));
	m_pShieldSkillUIObject->SetScale(0.02, 0.02, 1);
	m_pShieldSkillUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pShieldSkillUIObject);

	m_pHealSkillUIObject = new GameObject(UI_ENTITY);
	m_pHealSkillUIObject->InsertComponent<RenderComponent>();
	m_pHealSkillUIObject->InsertComponent<UIMeshComponent>();
	m_pHealSkillUIObject->InsertComponent<UiShaderComponent>();
	m_pHealSkillUIObject->InsertComponent<TextureComponent>();
	m_pHealSkillUIObject->SetTexture(L"UI/HealSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pHealSkillUIObject->SetPosition(XMFLOAT3(-0.975, 0.2, 1.00));
	m_pHealSkillUIObject->SetScale(0.02, 0.02, 1);
	m_pHealSkillUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pHealSkillUIObject);

	m_pHealSkillUIObject = new GameObject(UI_ENTITY);
	m_pHealSkillUIObject->InsertComponent<RenderComponent>();
	m_pHealSkillUIObject->InsertComponent<UIMeshComponent>();
	m_pHealSkillUIObject->InsertComponent<UiShaderComponent>();
	m_pHealSkillUIObject->InsertComponent<TextureComponent>();
	m_pHealSkillUIObject->SetTexture(L"UI/HealSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pHealSkillUIObject->SetPosition(XMFLOAT3(-0.975, 0.2, 1.00));
	m_pHealSkillUIObject->SetScale(0.02, 0.02, 1);
	m_pHealSkillUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pHealSkillUIObject);*/
	/////////////////////////////////////////////////////////////////////////

	m_pAttackUIObject = new GameObject(UI_ENTITY);
	m_pAttackUIObject->InsertComponent<RenderComponent>();
	m_pAttackUIObject->InsertComponent<UIMeshComponent>();
	m_pAttackUIObject->InsertComponent<UiShaderComponent>();
	m_pAttackUIObject->InsertComponent<TextureComponent>();
	m_pAttackUIObject->SetTexture(L"UI/RedCrossHair.dds", RESOURCE_TEXTURE2D, 3);
	m_pAttackUIObject->SetPosition(XMFLOAT3(0.0, 0.0, 1.01));
	m_pAttackUIObject->SetScale(0.01, 0.01, 1);
	//m_pAttackUIObject->SetColor(XMFLOAT4(0, -0.7, -5, 0));
	//m_pAttackUIObject->SetColor(XMFLOAT4(0, 0, 0, 0.75));
	m_pAttackUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppCharacterUIObjects.emplace_back(m_pAttackUIObject);

	m_pBlueAttackUIObject = new GameObject(UI_ENTITY);
	m_pBlueAttackUIObject->InsertComponent<RenderComponent>();
	m_pBlueAttackUIObject->InsertComponent<UIMeshComponent>();
	m_pBlueAttackUIObject->InsertComponent<UiShaderComponent>();
	m_pBlueAttackUIObject->InsertComponent<TextureComponent>();
	m_pBlueAttackUIObject->SetTexture(L"UI/BlueCrossHair.dds", RESOURCE_TEXTURE2D, 3);
	m_pBlueAttackUIObject->SetPosition(XMFLOAT3(0.0, 0.0, 1.01));
	m_pBlueAttackUIObject->SetScale(0.01, 0.01, 1);
	//m_pAttackUIObject->SetColor(XMFLOAT4(0, -0.7, -5, 0));
	//m_pAttackUIObject->SetColor(XMFLOAT4(0, 0, 0, 0.75));
	m_pBlueAttackUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppCharacterUIObjects.emplace_back(m_pBlueAttackUIObject);

	//m_ppCharacterUIObjects.emplace_back(m_pTalkUIObject);
	//m_pMonsterHPBarObject = new GameObject(UNDEF_ENTITY);
	//m_pMonsterHPBarObject->InsertComponent<RenderComponent>();
	//m_pMonsterHPBarObject->InsertComponent<CLoadedModelInfoCompnent>();
	//m_pMonsterHPBarObject->SetPosition(XMFLOAT3(0, 0, 0));
	//m_pMonsterHPBarObject->SetModel("Model/BossHp.bin");
	//m_pMonsterHPBarObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pMonsterHPBarObject->Rotate(0.0f, 0.0f, 0.0f);
	//m_pMonsterHPBarObject->SetScale(3.0f, 3.0f, 3.0f);
	//m_ppCharacterUIObjects.emplace_back(m_pMonsterHPBarObject);
	m_pMonsterHPBarObject = new GameObject(UNDEF_ENTITY);
	m_pMonsterHPBarObject->InsertComponent<RenderComponent>();
	m_pMonsterHPBarObject->InsertComponent<UIMeshComponent>();
	m_pMonsterHPBarObject->InsertComponent <BlendShaderComponent>();
	m_pMonsterHPBarObject->InsertComponent<TextureComponent>();
	m_pMonsterHPBarObject->SetTexture(L"UI/HpBar.dds", RESOURCE_TEXTURE2D, 3);
	m_pMonsterHPBarObject->SetPosition(XMFLOAT3(0, 40, 100));
	m_pMonsterHPBarObject->SetScale(10);
	m_pMonsterHPBarObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppCharacterUIObjects.emplace_back(m_pMonsterHPBarObject);

	for (int i = 0; i < m_pNormalMonsterHPBarObject.size(); i++) {
		m_pNormalMonsterHPBarObject[i] = new GameObject(UNDEF_ENTITY);
		m_pNormalMonsterHPBarObject[i]->InsertComponent<RenderComponent>();
		m_pNormalMonsterHPBarObject[i]->InsertComponent<UIMeshComponent>();
		m_pNormalMonsterHPBarObject[i]->InsertComponent <BlendShaderComponent>();
		m_pNormalMonsterHPBarObject[i]->InsertComponent<TextureComponent>();
		m_pNormalMonsterHPBarObject[i]->SetTexture(L"UI/HpBar.dds", RESOURCE_TEXTURE2D, 3);
		m_pNormalMonsterHPBarObject[i]->SetPosition(XMFLOAT3(0, 40, 100));
		m_pNormalMonsterHPBarObject[i]->SetScale(4);
		m_pNormalMonsterHPBarObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

	///////////////////////////////////////////////////////
	m_pArcherObject->m_pHPBarUI = new GameObject(UI_ENTITY);
	m_pArcherObject->m_pHPBarUI->InsertComponent<RenderComponent>();
	m_pArcherObject->m_pHPBarUI->InsertComponent<UIMeshComponent>();
	m_pArcherObject->m_pHPBarUI->InsertComponent<UiShaderComponent>();
	m_pArcherObject->m_pHPBarUI->InsertComponent<TextureComponent>();
	m_pArcherObject->m_pHPBarUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.18, 1.00));
	m_pArcherObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pArcherObject->m_pHPBarUI->SetColor(XMFLOAT4(0.02, 0.08, 0, 0));
	m_pArcherObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pHPBarUI);

	m_pArcherObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pArcherObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pArcherObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pArcherObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pArcherObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pArcherObject->m_pProfileUI->SetTexture(L"UI/ArcherUI.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.2, 1.00));
	m_pArcherObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pArcherObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pProfileUI);

	m_pArcherObject->m_pSkillQUI = new GameObject(UI_ENTITY);
	m_pArcherObject->m_pSkillQUI->InsertComponent<RenderComponent>();
	m_pArcherObject->m_pSkillQUI->InsertComponent<UIMeshComponent>();
	m_pArcherObject->m_pSkillQUI->InsertComponent<UiShaderComponent>();
	m_pArcherObject->m_pSkillQUI->InsertComponent<TextureComponent>();
	m_pArcherObject->m_pSkillQUI->SetTexture(L"UI/AcherQSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherObject->m_pSkillQUI->SetPosition(XMFLOAT3(5, 5, 1.00));
	m_pArcherObject->m_pSkillQUI->SetScale(0.02, 0.02, 1);
	m_pArcherObject->m_pSkillQUI->SetCurrentHP(70);
	m_pArcherObject->m_pSkillQUI->SetColor(XMFLOAT4(0.06f, 0.05f, 0, 0.7));
	m_pArcherObject->m_pSkillQUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pSkillQUI);

	m_pArcherObject->m_pSkillEUI = new GameObject(UI_ENTITY);
	m_pArcherObject->m_pSkillEUI->InsertComponent<RenderComponent>();
	m_pArcherObject->m_pSkillEUI->InsertComponent<UIMeshComponent>();
	m_pArcherObject->m_pSkillEUI->InsertComponent<UiShaderComponent>();
	m_pArcherObject->m_pSkillEUI->InsertComponent<TextureComponent>();
	m_pArcherObject->m_pSkillEUI->SetTexture(L"UI/AcherESkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherObject->m_pSkillEUI->SetPosition(XMFLOAT3(5, 5, 1.00));
	m_pArcherObject->m_pSkillEUI->SetColor(XMFLOAT4(0.06f, 0.05f, 0, 0));
	m_pArcherObject->m_pSkillEUI->SetScale(0.02, 0.02, 1);

	m_pArcherObject->m_pSkillEUI->SetCurrentHP(70);
	m_pArcherObject->m_pSkillEUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pSkillEUI);

	///////////////////////////////////////////////////////
	m_pTankerObject->m_pHPBarUI = new GameObject(UI_ENTITY);
	m_pTankerObject->m_pHPBarUI->InsertComponent<RenderComponent>();
	m_pTankerObject->m_pHPBarUI->InsertComponent<UIMeshComponent>();
	m_pTankerObject->m_pHPBarUI->InsertComponent<UiShaderComponent>();
	m_pTankerObject->m_pHPBarUI->InsertComponent<TextureComponent>();
	m_pTankerObject->m_pHPBarUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.06, 1.00));
	m_pTankerObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pTankerObject->m_pHPBarUI->SetColor(XMFLOAT4(0.02, 0.08, 0, 0));
	m_pTankerObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pTankerObject->m_pHPBarUI);

	m_pTankerObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pTankerObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pTankerObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pTankerObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pTankerObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pTankerObject->m_pProfileUI->SetTexture(L"UI/TankerUI.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.08, 1.01));
	m_pTankerObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pTankerObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pTankerObject->m_pProfileUI);

	m_pTankerObject->m_pSkillQUI = new GameObject(UI_ENTITY);
	m_pTankerObject->m_pSkillQUI->InsertComponent<RenderComponent>();
	m_pTankerObject->m_pSkillQUI->InsertComponent<UIMeshComponent>();
	m_pTankerObject->m_pSkillQUI->InsertComponent<UiShaderComponent>();
	m_pTankerObject->m_pSkillQUI->InsertComponent<TextureComponent>();
	m_pTankerObject->m_pSkillQUI->SetTexture(L"UI/ShieldSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->m_pSkillQUI->SetPosition(XMFLOAT3(5, 5, 1.00));
	m_pTankerObject->m_pSkillQUI->SetScale(0.02, 0.02, 1);
	m_pTankerObject->m_pSkillQUI->SetColor(XMFLOAT4(0.06f, 0.05f, 0, 0.7));
	m_pTankerObject->m_pSkillQUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pTankerObject->m_pSkillQUI);

	m_pTankerObject->m_pSkillEUI = new GameObject(UI_ENTITY);
	m_pTankerObject->m_pSkillEUI->InsertComponent<RenderComponent>();
	m_pTankerObject->m_pSkillEUI->InsertComponent<UIMeshComponent>();
	m_pTankerObject->m_pSkillEUI->InsertComponent<UiShaderComponent>();
	m_pTankerObject->m_pSkillEUI->InsertComponent<TextureComponent>();
	m_pTankerObject->m_pSkillEUI->SetTexture(L"UI/SmashSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->m_pSkillEUI->SetPosition(XMFLOAT3(5, 5, 1.00));
	m_pTankerObject->m_pSkillEUI->SetScale(0.02, 0.02, 1);
	m_pTankerObject->m_pSkillEUI->SetColor(XMFLOAT4(0.06f, 0.05f, 0, 0.7));
	m_pTankerObject->m_pSkillEUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pTankerObject->m_pSkillEUI);
	////////////////////////////////////////////////////////////////
	m_pPriestObject->m_pHPBarUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pHPBarUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pHPBarUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pHPBarUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pHPBarUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pHPBarUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, -0.06, 1.01));
	m_pPriestObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pPriestObject->m_pHPBarUI->SetShield(0.2);
	m_pPriestObject->m_pHPBarUI->SetColor(XMFLOAT4(0.02, 0.08, 0, 0));
	m_pPriestObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pHPBarUI);

	m_pPriestObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pProfileUI->SetTexture(L"UI/PriestUI.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, -0.04, 1.01));
	m_pPriestObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pPriestObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pProfileUI);

	m_pPriestObject->m_pSkillQUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pSkillQUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pSkillQUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pSkillQUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pSkillQUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pSkillQUI->SetTexture(L"UI/HealSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pSkillQUI->SetPosition(XMFLOAT3(5, 5, 1.00));
	m_pPriestObject->m_pSkillQUI->SetScale(0.02, 0.02, 1);
	m_pPriestObject->m_pSkillQUI->SetColor(XMFLOAT4(0.06f, 0.05f, 0, 0));
	m_pPriestObject->m_pSkillQUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pSkillQUI);

	m_pPriestObject->m_pSkillEUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pSkillEUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pSkillEUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pSkillEUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pSkillEUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pSkillEUI->SetTexture(L"UI/LightningSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pSkillEUI->SetPosition(XMFLOAT3(5, 5, 1.00));
	m_pPriestObject->m_pSkillEUI->SetScale(0.02, 0.02, 1);
	m_pPriestObject->m_pSkillEUI->SetColor(XMFLOAT4(0.06f, 0.05f, 0, 0));
	m_pPriestObject->m_pSkillEUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pSkillEUI);
	////////////////////////////////////////////////////////////////
	m_pWarriorObject->m_pHPBarUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pHPBarUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pHPBarUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pHPBarUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pHPBarUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pHPBarUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, -0.18, 1.01));
	m_pWarriorObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pWarriorObject->m_pHPBarUI->SetColor(XMFLOAT4(0.02, 0.08, 0, 0));
	m_pWarriorObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pHPBarUI);

	m_pWarriorObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pProfileUI->SetTexture(L"UI/WarriorUI.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, -0.16, 1.01));
	m_pWarriorObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pWarriorObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pProfileUI);

	m_pWarriorObject->m_pSkillQUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pSkillQUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pSkillQUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pSkillQUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pSkillQUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pSkillQUI->SetTexture(L"UI/WarriorQSkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pSkillQUI->SetPosition(XMFLOAT3(5, 5, 1.00));
	m_pWarriorObject->m_pSkillQUI->SetScale(0.02, 0.02, 1);
	m_pWarriorObject->m_pSkillQUI->SetColor(XMFLOAT4(0.06f, 0.05f, 0, 0));
	m_pWarriorObject->m_pSkillQUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pSkillQUI);

	m_pWarriorObject->m_pSkillEUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pSkillEUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pSkillEUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pSkillEUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pSkillEUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pSkillEUI->SetTexture(L"UI/WarriorESkill.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pSkillEUI->SetPosition(XMFLOAT3(5, 5, 1.00));
	m_pWarriorObject->m_pSkillEUI->SetScale(0.02, 0.02, 1);
	m_pWarriorObject->m_pSkillEUI->SetColor(XMFLOAT4(0.06f, 0.05f, 0, 0));
	m_pWarriorObject->m_pSkillEUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pSkillEUI);

	m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pSkillQUI);
	m_pConditionUIObject = new GameObject(UI_ENTITY);
	m_pConditionUIObject->InsertComponent<RenderComponent>();
	m_pConditionUIObject->InsertComponent<UIMeshComponent>();
	m_pConditionUIObject->InsertComponent<UiShaderComponent>();
	m_pConditionUIObject->InsertComponent<TextureComponent>();
	m_pConditionUIObject->SetTexture(L"UI/Condition.dds", RESOURCE_TEXTURE2D, 3);
	m_pConditionUIObject->SetPosition(XMFLOAT3(0.0, 0.0, 1.00));
	m_pConditionUIObject->SetColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	m_pConditionUIObject->SetScale(0.44f, 0.24f, 1.0f);
	m_pConditionUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pConditionUIObject);


	m_pSceneChangeUIObject = new GameObject(UI_ENTITY);
	m_pSceneChangeUIObject->InsertComponent<RenderComponent>();
	m_pSceneChangeUIObject->InsertComponent<UIMeshComponent>();
	m_pSceneChangeUIObject->InsertComponent<UiShaderComponent>();
	m_pSceneChangeUIObject->InsertComponent<TextureComponent>();
	m_pSceneChangeUIObject->SetTexture(L"UI/Black.dds", RESOURCE_TEXTURE2D, 3);
	m_pSceneChangeUIObject->SetPosition(XMFLOAT3(0.0, 0.0, 1.00));
	m_pSceneChangeUIObject->SetColor(XMFLOAT4(0.4f, 0.0f, 0.0f, 0.0f));
	m_pSceneChangeUIObject->SetScale(0.44f, 0.24f, 1.0f);
	m_pSceneChangeUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//	m_ppCharacterUIObjects.emplace_back(m_pSceneChangeUIObject);

	m_pVictoryUIObject = new GameObject(UI_ENTITY);
	m_pVictoryUIObject->InsertComponent<RenderComponent>();
	m_pVictoryUIObject->InsertComponent<UIMeshComponent>();
	m_pVictoryUIObject->InsertComponent<BlendingUiShaderComponent>();
	m_pVictoryUIObject->InsertComponent<TextureComponent>();
	m_pVictoryUIObject->SetTexture(L"UI/Victory.dds", RESOURCE_TEXTURE2D, 3);
	m_pVictoryUIObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 1.01f));
	m_pVictoryUIObject->SetScale(0.3, 0.1, 1);
	m_pVictoryUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pContinueUIObject = new GameObject(UI_ENTITY);
	m_pContinueUIObject->InsertComponent<RenderComponent>();
	m_pContinueUIObject->InsertComponent<UIMeshComponent>();
	m_pContinueUIObject->InsertComponent<BlendingUiShaderComponent>();
	m_pContinueUIObject->InsertComponent<TextureComponent>();
	m_pContinueUIObject->SetTexture(L"UI/Continue.dds", RESOURCE_TEXTURE2D, 3);
	m_pContinueUIObject->SetPosition(XMFLOAT3(0.0f, -0.4f, 1.01f));
	m_pContinueUIObject->SetScale(0.1, 0.01, 1);
	m_pContinueUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	////////////////////////////////////////////////////////////////
}

void GameobjectManager::BuildInstanceObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pInstancingShaderComponent = new InstancingShaderComponent;
	m_pInstancingShaderComponent->BuildObject(pd3dDevice, pd3dCommandList, m_pFireballSpriteObjects);
}

void GameobjectManager::BuildStoryUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pStroy1Object = new GameObject(UI_ENTITY);
	m_pStroy1Object->InsertComponent<RenderComponent>();
	m_pStroy1Object->InsertComponent<UIMeshComponent>();
	m_pStroy1Object->InsertComponent<UiShaderComponent>();
	m_pStroy1Object->InsertComponent<TextureComponent>();
	m_pStroy1Object->SetTexture(L"UI/Story.dds", RESOURCE_TEXTURE2D, 3);
	m_pStroy1Object->SetPosition(XMFLOAT3(0.0, 0.0, 1.00));
	m_pStroy1Object->SetScale(0.44f, 0.24f, 1.0f);
	m_pStroy1Object->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppStoryUIObjects.emplace_back(m_pStroy1Object);
}

void GameobjectManager::BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	for (int i = 0; i < 4; ++i)
	{
		m_ppShieldEffectObject[i] = new SheildEffectObject;
		m_ppShieldEffectObject[i]->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, &m_ppEffectObjects);
		m_ppShieldEffectObject[i]->m_hostObject = static_cast<ROLE>(0x01 << i);
	}

	for (int i = 0; i < 4; ++i)
	{
		m_ppHealingEffectObject[i] = new EffectObject;
		m_ppHealingEffectObject[i]->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, &m_ppEffectObjects);
		m_ppHealingEffectObject[i]->m_hostObject = static_cast<ROLE>(0x01 << i);
	}

	m_pDebuffObject = new DebuffObject;
	m_pDebuffObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, &m_ppEffectObjects);

	m_pLightEffectObject = new LightningEffectObject;
	m_pLightEffectObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, &m_ppEffectObjects);

	m_pSheildEffectObject = new SheildEffectObject;
	m_pSheildEffectObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, &m_ppEffectObjects);

	m_pPortalEffectObject = new PortalEffectObject;
	m_pPortalEffectObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, &m_ppEffectObjects);

	m_pPreistAttackEffectObject = new PriestEffectObject;
	m_pPreistAttackEffectObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, &m_ppEffectObjects);

	m_pTankerAttackEffectObject = new TankerEffectObject;
	m_pTankerAttackEffectObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, &m_ppEffectObjects);
}

void GameobjectManager::BuildNPC(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pAngelNPCObject = new NpcCharacter();
	m_pAngelNPCObject->InsertComponent<RenderComponent>();
	m_pAngelNPCObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pAngelNPCObject->SetPosition(XMFLOAT3(-1264.9f, 0.f, -1448.1f));//
	m_pAngelNPCObject->SetModel("Model/Angel.bin");
	m_pAngelNPCObject->SetAnimationSets(1);
	m_pAngelNPCObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pAngelNPCObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1);
	m_pAngelNPCObject->SetScale(15.0f);
	m_pAngelNPCObject->Rotate(0, -120.959, 0);
	m_ppGameObjects.emplace_back(m_pAngelNPCObject);

	m_pNPCPressGObject = new GameObject(UNDEF_ENTITY);
	m_pNPCPressGObject->InsertComponent<RenderComponent>();
	m_pNPCPressGObject->InsertComponent<UIMeshComponent>();
	m_pNPCPressGObject->InsertComponent <BlendShaderComponent>();
	m_pNPCPressGObject->InsertComponent<TextureComponent>();
	m_pNPCPressGObject->SetTexture(L"UI/PressG.dds", RESOURCE_TEXTURE2D, 3);
	m_pNPCPressGObject->SetPosition(XMFLOAT3(0, 40, 100));
	m_pNPCPressGObject->SetScale(10);
	m_pNPCPressGObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

enum UI
{
	UI_GAMESEARCHING,
	UI_GAMEMATCHING,
	UI_WARRIORCHARACTER,
	UI_ARCHERCHARACTER,
	UI_TANKERCHARACTER,
	UI_PRIESTCHARACTER,
	UI_ENTERROOM,
	UI_CREATEROOM,
	UI_GAMESTART,
	UI_GAMEEND
};

void GameobjectManager::ResetLobbyUI()
{
	m_bInMatching = false;
	m_pUIGameEndObject->m_bUIActive = true;
	m_pUIWarriorCharacterObject->m_bUIActive = true;
	m_pUIArcherCharacterObject->m_bUIActive = true;
	m_pUITankerCharacterObject->m_bUIActive = true;
	m_pUIPriestCharacterObject->m_bUIActive = true;
}

void GameobjectManager::SetLightningEffect(XMFLOAT3& targetPos)
{
	g_sound.NoLoopPlay("LightningSound", m_pLightningSpriteObject->CalculateDistanceSound() + 0.3);
	m_LightningTargetPos = targetPos;
	m_pLightEffectObject->SetActive(m_pLightEffectObject->m_bActive);
	m_pLightEffectObject->AnimateEffect(m_pCamera, targetPos, m_fTimeElapsed, m_fTime * 5);
	targetPos.y += 50.0f;
	m_pLightningSpriteObject->SetPosition(targetPos);
	m_pLightEffectObject->m_fEffectLifeTime = 2.0f;
	m_pLightningSpriteObject->m_bActive = m_pLightEffectObject->m_bActive = true;
}


void GameobjectManager::PickObjectByRayIntersection(int xClient, int yClient)
{

	XMFLOAT4X4 xmf4x4View = m_pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = m_pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = m_pCamera->GetViewport();

	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;

	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	m_pSelectedObject = NULL;
	for (int j = 0; j < m_ppUIObjects.size(); j++)
	{
		nIntersected = m_ppUIObjects[j]->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
		{
			ProcessingUI(j);
			fNearestHitDistance = fHitDistance;
			m_pSelectedObject = m_ppUIObjects[j];
			break;
		}
	}
}

void GameobjectManager::ProcessingUI(int n)
{
	switch (n)
	{
	case UI::UI_GAMESEARCHING: {
		m_pUIGameEndObject->m_bUIActive = false;
		g_sound.NoLoopPlay("ClickSound", 1.0f);
		//g_sound.Pause("ClickSound");
		if (!m_bInMatching)
		{
			cout << "StartMatching" << endl;
			g_NetworkHelper.SendMatchRequestPacket();
			m_bInMatching = true;
		}
		break;
	}
	case UI::UI_WARRIORCHARACTER:
	{
		g_sound.NoLoopPlay("ClickSound", 1.0f);
		// 선택한 캐릭터 Warrior
		g_Logic.SetMyRole(ROLE::WARRIOR);
		m_pUIWarriorCharacterObject->m_bUIActive = true;
		m_pUIArcherCharacterObject->m_bUIActive = false;
		m_pUITankerCharacterObject->m_bUIActive = false;
		m_pUIPriestCharacterObject->m_bUIActive = false;
		cout << "Choose Warrior Character" << endl;
		break;
	}
	case UI::UI_ARCHERCHARACTER:
	{
		g_sound.NoLoopPlay("ClickSound", 1.0f);
		// 선택한 캐릭터 Archer
		g_Logic.SetMyRole(ROLE::ARCHER);
		m_pUIWarriorCharacterObject->m_bUIActive = false;
		m_pUIArcherCharacterObject->m_bUIActive = true;
		m_pUITankerCharacterObject->m_bUIActive = false;
		m_pUIPriestCharacterObject->m_bUIActive = false;
		cout << "Choose Archer Character" << endl;
		break;
	}
	case UI::UI_TANKERCHARACTER:
	{
		// 선택한 캐릭터 tanker
		g_sound.NoLoopPlay("ClickSound", 1.0f);
		g_Logic.SetMyRole(ROLE::TANKER);
		m_pUIWarriorCharacterObject->m_bUIActive = false;
		m_pUIArcherCharacterObject->m_bUIActive = false;
		m_pUITankerCharacterObject->m_bUIActive = true;
		m_pUIPriestCharacterObject->m_bUIActive = false;
		cout << "Choose Tanker Character" << endl;
		break;
	}
	case UI::UI_PRIESTCHARACTER:
	{
		// 선택한 캐릭터 priest
		g_sound.NoLoopPlay("ClickSound", 1.0f);
		g_Logic.SetMyRole(ROLE::PRIEST);
		m_pUIWarriorCharacterObject->m_bUIActive = false;
		m_pUIArcherCharacterObject->m_bUIActive = false;
		m_pUITankerCharacterObject->m_bUIActive = false;
		m_pUIPriestCharacterObject->m_bUIActive = true;
		cout << "Choose Priest Character" << endl;
		break;
	}
	case UI::UI_ENTERROOM:
	{
		g_sound.NoLoopPlay("ClickSound", 1.0f);
		// 방 입장 요청 Send
		cout << "Request Entering The Room" << endl;
		break;
	}
	case UI::UI_CREATEROOM:
	{
		g_sound.NoLoopPlay("ClickSound", 1.0f);
		cout << "Create Room" << endl;
		break;
	}
	default:
		break;
	}
}
void GameobjectManager::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEFRAMEWORK_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);

	if (m_pLight)
		m_pLight->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void GameobjectManager::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dcbGameObjects)
	{
		::memcpy(&m_pcbMappedGameObjects->m_xmfTime, &m_fTime, sizeof(float));
		::memcpy(&m_pcbMappedGameObjects->m_xmfMode, &m_xmfMode, sizeof(float));
		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbGameObjects->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(20, d3dGpuVirtualAddress);
	}

	if (m_pLight)
		m_pLight->UpdateShaderVariables(pd3dCommandList);
}
void GameobjectManager::ReleaseShaderVariables()
{
	if (m_pLight)
		m_pLight->ReleaseShaderVariables();
}
bool GameobjectManager::onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	static XMFLOAT3 upVec = XMFLOAT3(0, 1, 0);
	Character* myPlayCharacter = GetChracterInfo(g_Logic.GetMyRole());
	if (nMessageID == WM_KEYDOWN && wParam == VK_F4)
	{
		g_NetworkHelper.SendTestGameEndPacket();
		m_pMonsterObject->SetCurrentHP(0.0f);
		GameEnd = true;
	}
	if (nMessageID == WM_KEYDOWN && wParam == VK_F3)
	{
		m_bDebugMode = !m_bDebugMode;
	}
	if (nMessageID == WM_KEYDOWN && wParam == VK_TAB)
	{
		int curGameState = gGameFramework.GetCurrentGameState();
		curGameState = (curGameState + 1) % 3;
		gGameFramework.SetCurrentGameState(static_cast<GAME_STATE>(curGameState));
	}
	if (nMessageID == WM_KEYDOWN && wParam == VK_F5)
	{
		m_xmfMode = DEFAULT_MODE;
	}
	if (nMessageID == WM_KEYDOWN && wParam == VK_F6)
	{
		m_xmfMode = CARTOON_MODE;
	}
	if (nMessageID == WM_KEYDOWN && wParam == VK_F7)
	{
		m_xmfMode = OUTLINE_MODE;
	}
	if (nMessageID == WM_KEYDOWN && wParam == VK_F8)
	{
		m_xmfMode = CELLSHADING_MODE;
	}
	if (nMessageID == WM_KEYDOWN && wParam == VK_F9)
	{
		m_xmfMode = DISSOLVE_MODE;
	}
	if (myPlayCharacter->GetCurrentHP() < FLT_EPSILON)
		return false;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':
		{
			if (myPlayCharacter->CanMove() == true) {
				g_Logic.m_KeyInput->m_bWKey = true;
				myPlayCharacter->SetMoveState(true);
				myPlayCharacter->AddDirection(DIRECTION::FRONT);
				g_NetworkHelper.SendMovePacket(DIRECTION::FRONT);
			}
		}
		break;
		case 'A':
		{
			if (myPlayCharacter->CanMove() == true) {
				g_Logic.m_KeyInput->m_bAKey = true;
				myPlayCharacter->SetMoveState(true);
				myPlayCharacter->AddDirection(DIRECTION::LEFT);
				g_NetworkHelper.SendMovePacket(DIRECTION::LEFT);
			}
		}
		break;
		case 'S':
		{
			if (myPlayCharacter->CanMove() == true) {
				g_Logic.m_KeyInput->m_bSKey = true;
				myPlayCharacter->SetMoveState(true);
				myPlayCharacter->AddDirection(DIRECTION::BACK);
				g_NetworkHelper.SendMovePacket(DIRECTION::BACK);
			}
		}
		break;
		case 'D':
		{			
			if (myPlayCharacter->CanMove() == true) {
				g_Logic.m_KeyInput->m_bDKey = true;
				myPlayCharacter->SetMoveState(true);
				myPlayCharacter->AddDirection(DIRECTION::RIGHT);
				g_NetworkHelper.SendMovePacket(DIRECTION::RIGHT);
			}
		}
		break;
		case 'Q':
		{
			//g_Logic.m_KeyInput->m_bQKey = true;

			if (myPlayCharacter->GetQSkillState() == false && myPlayCharacter->GetOnAttack() == false)
			{
				myPlayCharacter->FirstSkillDown();
			}
			break;
		}
		case 'E':
		{
			if (myPlayCharacter->GetESkillState() == false && myPlayCharacter->GetOnAttack() == false)
			{
				if (g_Logic.GetMyRole() == ROLE::PRIEST || g_Logic.GetMyRole() == ROLE::ARCHER)
				{
					if (!m_bPickingenemy)
					{
						m_bPickingenemy = CheckCollision(m_ppGameObjects);
						if (m_bPickingenemy == true)
						{
							if (g_Logic.GetMyRole() == ROLE::ARCHER)
								static_cast<Archer*>(myPlayCharacter)->m_xmf3TargetPos = m_pSelectedObject->GetPosition();
							m_LightningTargetPos = m_pSelectedObject->GetPosition();
							myPlayCharacter->SecondSkillDown();
						}
					}
				}
				else
				{
					myPlayCharacter->SecondSkillDown();
				}

			}
			break;
		}
		case 'O':
		{
#ifdef  LOCAL_TASK
			m_pPlayerObject->m_pCamera->ReInitCamrea();
			m_pPlayerObject->SetCamera(m_pCamera);
#endif //  
#ifndef LOCAL_TASK
			myPlayCharacter->m_pCamera->ReInitCamrea();
			myPlayCharacter->SetCamera(m_pCamera);
#endif // !LOCAL_TASK

			break;
		}
		case VK_CONTROL:
			break;
		case VK_F1:
			break;
		case VK_F2:

		case VK_F3:

			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case 'W':
		{
			if (g_Logic.m_KeyInput->m_bWKey)
			{
				g_Logic.m_KeyInput->m_bWKey = false;
				if (g_Logic.m_KeyInput->IsAllMovekeyUp())
				{
					myPlayCharacter->SetMoveState(false);
					myPlayCharacter->SetStopDirection();
					g_NetworkHelper.SendStopPacket(myPlayCharacter->GetPosition()); //, g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate				
				}
				else {
					myPlayCharacter->RemoveDIrection(DIRECTION::FRONT);
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::FRONT);
				}
			}
		}
		break;
		case 'A':
		{
			if (g_Logic.m_KeyInput->m_bAKey)
			{
				g_Logic.m_KeyInput->m_bAKey = false;
				if (g_Logic.m_KeyInput->IsAllMovekeyUp())
				{
					myPlayCharacter->SetMoveState(false);
					myPlayCharacter->SetStopDirection();
					g_NetworkHelper.SendStopPacket(myPlayCharacter->GetPosition()); //, g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate				
				}
				else {
					myPlayCharacter->RemoveDIrection(DIRECTION::LEFT);
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::LEFT);
				}
			}
		}
		break;
		case 'S':
		{
			if (g_Logic.m_KeyInput->m_bSKey)
			{
				g_Logic.m_KeyInput->m_bSKey = false;
				if (g_Logic.m_KeyInput->IsAllMovekeyUp())
				{
					myPlayCharacter->SetMoveState(false);
					myPlayCharacter->SetStopDirection();
					g_NetworkHelper.SendStopPacket(myPlayCharacter->GetPosition()); //, g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate				
				}
				else {
					myPlayCharacter->RemoveDIrection(DIRECTION::BACK);
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::BACK);
				}
			}
		}
		break;
		case 'D':
		{
			if (g_Logic.m_KeyInput->m_bDKey)
			{
				g_Logic.m_KeyInput->m_bDKey = false;
				if (g_Logic.m_KeyInput->IsAllMovekeyUp())
				{
					myPlayCharacter->SetMoveState(false);
					myPlayCharacter->SetStopDirection();
					g_NetworkHelper.SendStopPacket(myPlayCharacter->GetPosition()); //, g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate				
				}
				else {
					myPlayCharacter->RemoveDIrection(DIRECTION::RIGHT);
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::RIGHT);
				}
			}
		}
		break;
		case 'Q':
		{
			//g_Logic.m_KeyInput->m_bQKey = false;
			myPlayCharacter->FirstSkillUp();
			break;
		}
		case 'E':
		{
			//g_Logic.m_KeyInput->m_bEKey = false;
			myPlayCharacter->SecondSkillUp(myPlayCharacter->GetRotateAxis());

			if (myPlayCharacter == GetChracterInfo(ROLE::ARCHER))
			{
				//g_NetworkHelper.SendSkillStatePacket(myPlayCharacter->GetQSkillState(), false);
			}
			break;
		}
		//NPC와 대화하거나 포털들어갈 때 상호작용 키
		case 'G':
		{
			bool isCollideNPC = CheckCollideNPC();
			if (isCollideNPC) {

				m_bSendNpccollisionPK = true;
				m_bNPCinteraction = true;
			}
			CheckCollidePortal();
			break;
		}
		case 'H':
		{
			m_bSkipText = true;
			break;
		}
		case 'V':
		{
			m_bNPCinteraction = true;


			break;
		}
		case 'P':
		{
			m_bTest = true;
			g_sound.NoLoopPlay("MonsterAttackedSound", 1.0f);
			g_sound.Play("ClickSound", 1.0f);
		}
		break;
		case 'O':
		{
			g_sound.Pause("testSound");
		}
		break;
		case 'U':
		{
			//AddDamageFontToUiLayer();
			break;
		}

		case 'T':
		{
#ifdef LOCAL_TASK
			if (m_nStageType == 1) {
				//m_nStageType = 2;
				m_pPlayerObject->SetPosition(XMFLOAT3(0, 0, 0));
			}
			else if (m_nStageType == 2) {
				//m_nStageType = 1;
				m_pPlayerObject->SetPosition(XMFLOAT3(-1400, 0, -1500));
			}
#endif
			if (m_nStageType == 1) {
				//m_nStageType = 2;
				GetChracterInfo(g_Logic.GetMyRole())->SetPosition(XMFLOAT3(0, 0, 0));
				g_NetworkHelper.SendChangeStage_BOSS();
			}
			break;
		}
		}
	}
	default:
		break;
	}
	return(false);
}

bool GameobjectManager::onProcessingKeyboardMessageLobby(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (nMessageID == WM_KEYDOWN && wParam == VK_F2)
	{
#ifdef LOCAL_TASK
		g_Logic.SetMyRole(ROLE::TANKER);//캐릭
		m_pCamera->Rotate(0, -90, 0);
		m_pPlayerObject = m_pTankerObject;
		m_pPlayerObject->SetCamera(m_pCamera);
		m_pTankerObject->SetRotateAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
#else
		g_Logic.SetMyRole(ROLE::ARCHER);
		m_pArcherObject->SetCamera(m_pCamera);
		m_pArcherObject->SetLook(XMFLOAT3(0, 0, 1));
		g_NetworkHelper.SendMatchRequestPacket();
#endif
		m_bSceneSwap = true;//페이드 인 아웃
	}
	if (nMessageID == WM_KEYDOWN && wParam == 'G')
	{
		m_bNPCinteraction = true;
		if (m_bLobbyTalkScreenSend) {
			m_bLobbyTalkScreenSend = false;
			m_bNPCscreen = true;
		}
	}
	if (nMessageID == WM_KEYDOWN && wParam == 'I')
	{
		g_Logic.SetMyRole(ROLE::TANKER);
		m_pTankerObject->SetCamera(m_pCamera);
		m_pTankerObject->SetLook(XMFLOAT3(0, 0, 1));
		g_NetworkHelper.SendMatchRequestPacket();
	}
	return false;
}

bool GameobjectManager::onProcessingKeyboardMessageUI(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		std::cout << "GameobjectManager::onProcessingKeyboardMessage() -key down: " << endl;
		switch (wParam)
		{
		case 'P':
		{
			if (m_pSelectedObject)
			{
				XMFLOAT3 TempPosition = m_pSelectedObject->GetPosition();
				TempPosition.x += 0.01f;
				m_pSelectedObject->SetPosition(TempPosition);

			}
		}
		break;
		case 'O':
		{
			if (m_pSelectedObject)
			{
				XMFLOAT3 TempPosition = m_pSelectedObject->GetPosition();
				TempPosition.x -= 0.01f;
				m_pSelectedObject->SetPosition(TempPosition);
			}
		}
		break;
		case 'K':
		{

			if (m_pSelectedObject)
			{
				XMFLOAT3 TempPosition = m_pSelectedObject->GetPosition();
				TempPosition.y += 0.01f;
				m_pSelectedObject->SetPosition(TempPosition);
			}
		}
		break;
		case 'L':
		{

			if (m_pSelectedObject)
			{
				XMFLOAT3 TempPosition = m_pSelectedObject->GetPosition();
				TempPosition.y -= 0.01f;
				m_pSelectedObject->SetPosition(TempPosition);
			}
		}
		break;
		case 'I':
		{
		}
		break;
		case 'q':
		case 'Q':
		{
		}
		break;
		case 'w':
		case 'W':
		{
		}
		break;
		case 'r':
		case 'R':
		{
			//g_NetworkHelper.SendMatchRequestPacket(ROLE::RAND);
		}
		break;
		case 'z':
		case 'Z': //탱커
		{
			//g_NetworkHelper.SendMatchRequestPacket(ROLE::TANKER);
		}
		break;
		case 'x':
		case 'X': // 전사
		{
			//g_NetworkHelper.SendMatchRequestPacket(ROLE::WARRIOR);
		}
		break;
		case 'c':
		case 'C': //프리스트
		{
			//g_NetworkHelper.SendMatchRequestPacket(ROLE::PRIEST);
		}
		break;
		case 'v':
		case 'V': // 궁수
		{
			//g_NetworkHelper.SendMatchRequestPacket(ROLE::ARCHER);
		}
		break;
		break;
		default:
			break;
		}
		break;
	}
	return(false);
}

void GameobjectManager::onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	bool SomethingChanging = false;
	Character* myPlayCharacter = GetChracterInfo(g_Logic.GetMyRole());

	if (GameEnd)
	{
		switch (nMessageID)
		{
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			g_NetworkHelper.SendTestGameEndOKPacket();
			g_Logic.ResetMyRole();
			ResetObject();
			break;
		}
		return;
	}

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		myPlayCharacter->m_LMouseInput = true;
		myPlayCharacter->SetLButtonClicked(true);
		SomethingChanging = true;

		break;
	}
	case WM_LBUTTONUP:
	{
		myPlayCharacter->m_LMouseInput = false;
		myPlayCharacter->SetLButtonClicked(false);
		SomethingChanging = true;

		/*	if (ROLE::PRIEST) {
				g_sound.NoLoopPlay("PriestAttackSound", 1.0f);
			}
			if (ROLE::ARCHER) {
				g_sound.NoLoopPlay("AcherAttackSound", 1.0f);
			}
			if (ROLE::WARRIOR) {
				g_sound.NoLoopPlay("WarriorAttackSound", 1.0f);
			}
			if (ROLE::TANKER) {
				g_sound.NoLoopPlay("TankerAttackSound", 1.0f);
			}*/
		break;
	}
	case WM_RBUTTONDOWN:
	{
		myPlayCharacter->m_RMouseInput = true;
		myPlayCharacter->SetRButtonClicked(true);
		myPlayCharacter->RbuttonClicked(true);
		SomethingChanging = true;
		break;
	}
	case WM_RBUTTONUP:
	{
		myPlayCharacter->m_RMouseInput = false;
		myPlayCharacter->SetRButtonClicked(false);
		myPlayCharacter->RbuttonUp(myPlayCharacter->GetRotateAxis());
		SomethingChanging = true;
		break;
	}
	default:
		break;
	}

#ifndef LOCAL_TASK
	if (SomethingChanging)
		g_NetworkHelper.SendMouseStatePacket(myPlayCharacter->m_LMouseInput
			, myPlayCharacter->m_RMouseInput);
#endif

}

void GameobjectManager::onProcessingMouseMessageUI(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		cout << "마우스 클릭 성공" << endl;
		PickObjectByRayIntersection(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}
}

void GameobjectManager::AddTextToUILayer(int& iIndex)
{
	//CGameObject* pObj = m_pUIObjectShader->GetObjectList(L"UI_Quest").front();
	//pObj->SetActiveState(true);

	queue<wstring> queueStr;
	if (iIndex == START_TEXT)
	{
		queueStr.emplace(L"z!");
		//queueStr.emplace(L"안녕하세요! 드림월드에 오신 것을 환영해요");
		//queueStr.emplace(L"먼저 플레이 방법에 대해서 알려드릴게요!");
		//queueStr.emplace(L"앞에 보이는 캐릭터들 중 원하는 캐릭터를");
		//queueStr.emplace(L"선택하신 후에 게임 시작을 누르시면");
		//queueStr.emplace(L"선택한 캐릭터를 플레이 하실 수 있어요!");
	}
	if (iIndex == NPC_TEXT)
	{
		queueStr.emplace(L"용사님들 꿈마을이 악몽에게 공격을 받았어요");
		queueStr.emplace(L"앞에 있는 악몽들을 처치해주세요!");
		//queueStr.emplace(L"앞에 있는 악몽들을 처치해주세요!");
	}
	if (iIndex == BOSS_TEXT)
	{
		queueStr.emplace(L"너희가 꿈마을을 지킬 수 있을거 같으냐!!!");
		queueStr.emplace(L"으하하하하하");
		queueStr.emplace(L"다 죽여주마!");
	}
	//if (iIndex == TEXT::WARRIOR_TEXT)
	//{
	//	queueStr.emplace(L"너희가 꿈마을을 지킬 수 있을거 같으냐!!!");
	//	queueStr.emplace(L"으하하하하하");
	//	queueStr.emplace(L"다 죽여주마!");
	//}
	//if (iIndex == TEXT::ACHER_TEXT)
	//{
	//	queueStr.emplace(L"너희가 꿈마을을 지킬 수 있을거 같으냐!!!");
	//	queueStr.emplace(L"으하하하하하");
	//	queueStr.emplace(L"다 죽여주마!");
	//}
	//if (iIndex == TEXT::TANKER_TEXT)
	//{
	//	queueStr.emplace(L"너희가 꿈마을을 지킬 수 있을거 같으냐!!!");
	//	queueStr.emplace(L"으하하하하하");
	//	queueStr.emplace(L"다 죽여주마!");
	//}
	//if (iIndex == TEXT::PRIEST_TEXT)
	//{
	//	queueStr.emplace(L"지금 선택하신 캐릭터는 메이지에요");
	//	queueStr.emplace(L"으하하하하하");
	//	queueStr.emplace(L"다 죽여주마!");
	//}
	if (iIndex != -1) {
		m_pUILayer->AddTextFont(queueStr);
	}
	iIndex = -1;

}

float GameobjectManager::CalculateDistance(const XMFLOAT3& firstPosition, const XMFLOAT3& lastPosition)
{
	XMVECTOR firstVec = XMLoadFloat3(&firstPosition);
	XMVECTOR lastVec = XMLoadFloat3(&lastPosition);

	XMVECTOR diffVec = XMVectorSubtract(lastVec, firstVec);
	XMVECTOR distanceVec = XMVector3Length(diffVec);

	float distance;
	XMStoreFloat(&distance, distanceVec);

	return distance;
}

void GameobjectManager::SortEffect()
{
	std::sort(m_ppEffectObjects.begin(), m_ppEffectObjects.end(), CompareGameObjects);
}

void GameobjectManager::SetPlayCharacter(Session* pSession) // 임시 함수
{
	//4명
	Session* cliSession = reinterpret_cast<Session*>(pSession);
	switch (cliSession->GetRole())
	{
	case ROLE::ARCHER:
	{
		cliSession->SetGameObject(m_pArcherObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(-200, 0, -40));
		cliSession->m_currentPlayGameObject->SetScale(30.0f);
	}
	break;
	case ROLE::PRIEST:
	{
		cliSession->SetGameObject(m_pPriestObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(-270, 0, 40));
		cliSession->m_currentPlayGameObject->SetScale(30.0f);
	}
	break;
	case ROLE::TANKER:
	{
		cliSession->SetGameObject(m_pTankerObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(150, 0, -60));
		cliSession->m_currentPlayGameObject->SetScale(30.0f);
	}
	break;
	case ROLE::WARRIOR:
	{
		cliSession->SetGameObject(m_pWarriorObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(260, 0, 50));
		cliSession->m_currentPlayGameObject->SetScale(30.0f);
	}
	break;
	default:
		break;
	}
}

void GameobjectManager::SetPlayerCamera(GameObject* obj)
{
	obj->SetCamera(m_pCamera);
	obj->m_pCamera->ReInitCamrea();
	obj->SetCamera(m_pCamera);
	// obj->SetLookAt(XMFLOAT3(0, 0, 0));
}

void GameobjectManager::ResetObject()
{
	for (int i = 0; i < m_ppGameObjects.size(); ++i)
	{
		m_ppGameObjects[i]->Reset();
	}

	for (int i = 0; i < m_ppEffectObjects.size(); ++i)
	{
		if (m_ppEffectObjects[i] == nullptr) continue;
		m_ppEffectObjects[i]->m_bActive = false;
	}

	for (int i = 0; i < m_ppParticleObjects.size(); ++i)
	{
		if (m_ppParticleObjects[i] == nullptr) continue;
		m_ppParticleObjects[i]->m_bActive = false;
	}

	for (int i = 0; i < m_ppProjectileObjects.size(); ++i)
	{
		if (m_ppProjectileObjects[i] == nullptr) continue;
		m_ppProjectileObjects[i]->Reset();
	}

	SetCharactersLobbyPosition();
	m_pCamera->ReInitCamrea();
	m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 0.98f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	m_pCamera->SetPosition(XMFLOAT3(-1450, 18, -1490));
	m_pCamera->Rotate(0, 90, 0);
}

void GameobjectManager::SetUIActive()
{
	m_pUIWarriorCharacterObject->m_bUIActive = true;
	m_pUIArcherCharacterObject->m_bUIActive = true;
	m_pUITankerCharacterObject->m_bUIActive = true;
	m_pUIPriestCharacterObject->m_bUIActive = true;
}

void GameobjectManager::ChangeStage1ToStage2(float fTimeelpased)
{
	if (m_bPortalCheck) {
		m_fStroyTime += fTimeelpased;
		if (m_fStroyTime < 6) {
			for (int i = 0; i < m_ppGameObjects.size(); ++i)
			{
				if (m_ppGameObjects[i]->m_nStageType == STAGE1)
					m_ppGameObjects[i]->Die(m_fStroyTime / 13);
				g_sound.Pause("LobbySound");
				g_sound.Pause("Stage1Sound");
				g_sound.Play("BossRespawnSound", 0.6);
			}
			//m_pSkyboxObject->Die(m_fStroyTime );
		}
		if (m_fStroyTime > 6) {
			m_nStageType = 2;
			for (int i = 0; i < m_ppGameObjects.size(); ++i)
			{
				if (m_ppGameObjects[i]->m_nStageType == STAGE2)
				{
					m_ppGameObjects[i]->Die(1 - (m_fStroyTime - 6) / 5);
				}
			}
			g_sound.Pause("LobbySound");
			g_sound.Pause("Stage1Sound");
			g_sound.Pause("BossRespawnSound");
			g_sound.Play("BossStage", 0.75);
		}
	}

}

void GameobjectManager::ChangeStage2ToStage1()
{
	m_bSendNpccollisionPK = !m_bSendNpccollisionPK;
	m_bGameStart = !m_bGameStart;
	m_bPortalCheck = !m_bPortalCheck;
	m_nStageType = 1;
	m_fStroyTime = 0;//스테이지 1에서 스테이지 2로 넘어가는 스토리 타임 초기화
	for (int i = 0; i < m_ppGameObjects.size(); ++i)
	{
		m_ppGameObjects[i]->m_xmf4Color.w = 0;//모든 오브젝트들 다시 블랜딩 초기화 
	}
	g_sound.Pause("BossStage");
	g_sound.Play("LobbySound", 0.42);
}

void GameobjectManager::SetTempHP()
{
	for (int i = 0; i < m_ppGameObjects.size(); i++) {
		m_ppGameObjects[i]->SetTempHp(m_ppGameObjects[i]->GetCurrentHP());
	}
	m_bGameStart = true;
}

bool GameobjectManager::CheckCollision(vector<GameObject*> m_ppObjects)
{
	XMVECTOR rayOrigin = XMLoadFloat3(&XMFLOAT3(m_pCamera->GetPosition().x, m_pCamera->GetPosition().y - 10, m_pCamera->GetPosition().z));
	XMVECTOR rayDirection = XMLoadFloat3(&m_pCamera->GetLookVector());
	float rayDistance;
	// 레이저와 BoundingSphere의 충돌 여부를 계산.
	Character* myPlayCharacter = GetChracterInfo(g_Logic.GetMyRole());
	if (myPlayCharacter == nullptr) return false;
	XMFLOAT3 PlayerPosition = myPlayCharacter->GetPosition();
	for (int i = 0; i < m_ppObjects.size(); i++)
	{
		if (m_ppObjects[i]->m_bActive == false) continue;
		if (m_ppObjects[i] == myPlayCharacter) continue;
		if (m_ppObjects[i]->m_SPBB.Intersects(rayOrigin, rayDirection, rayDistance) && m_ppObjects[i]->m_fObjectType == 1)
		{
			XMFLOAT3 TargetPosition = m_ppObjects[i]->GetPosition();
			float fDistance = Vector3::Length(Vector3::Subtract(PlayerPosition, TargetPosition));
			if (fDistance > 300.0f) continue;
			m_pSelectedObject = m_ppObjects[i];
			rayDistance = rayDistance;
			return true;
		}
	}
	return false;
}

void GameobjectManager::AddDamageFontToUiLayer(XMFLOAT3 xmf3Pos, float	fDamage)
{
	//XMFLOAT3 xmf3Pos = XMFLOAT3(-1400, 10, -1500);//충돌 포지션
	//float	fDamage = 300;
	m_pUILayer->AddDamageFont(xmf3Pos, to_wstring(fDamage));
}



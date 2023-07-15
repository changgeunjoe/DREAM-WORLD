#include "stdafx.h"
#include "GameobjectManager.h"
#include "Animation.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"
#include "DepthRenderShaderComponent.h"
#include "TextureToViewportComponent.h"
#include "UiShaderComponent.h"
#include "MultiSpriteShaderComponent.h"
#include "Character.h"
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

extern NetworkHelper g_NetworkHelper;
extern Logic g_Logic;
extern bool GameEnd;
extern MapData g_bossMapData;
extern CGameFramework gGameFramework;

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
	m_pSkyboxObject->SetPosition(m_pCamera->GetPosition());
	m_pLight->UpdatePosition(XMFLOAT3(m_pCamera->GetPosition().x,
		m_pCamera->GetPosition().y + 600, m_pCamera->GetPosition().z));
	if (m_pMonsterHPBarObject)//23.04.18 몬스터 체력바 -> 카메라를 바라 보도록 .ccg
	{
		m_pMonsterHPBarObject->SetLookAt(m_pCamera->GetPosition());
		m_pMonsterHPBarObject->SetPosition(XMFLOAT3(m_pMonsterObject->GetPosition().x,
			m_pMonsterObject->GetPosition().y + 70, m_pMonsterObject->GetPosition().z));
		m_pMonsterHPBarObject->Rotate(0, 180, 0);
		m_pMonsterHPBarObject->SetScale(10, 1, 1);
		m_pMonsterHPBarObject->SetCurrentHP(g_Logic.m_MonsterSession.m_currentPlayGameObject->GetCurrentHP());
	}
	XMFLOAT3 mfHittmp = g_Logic.m_MonsterSession.m_currentPlayGameObject->m_xmfHitPosition;
	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
		m_ppParticleObjects[i]->SetLookAt(m_pCamera->GetPosition());
		//m_ppParticleObjects[i]->SetPosition(XMFLOAT3(mfHittmp.x, mfHittmp.y+20, mfHittmp.z));
		m_ppParticleObjects[i]->SetScale(m_ppParticleObjects[i]->m_xmf3Scale.x,	
			m_ppParticleObjects[i]->m_xmf3Scale.y, m_ppParticleObjects[i]->m_xmf3Scale.z);
		m_ppParticleObjects[i]->Rotate(0, 180, 0);
		m_ppParticleObjects[i]->AnimateRowColumn(fTimeElapsed);
	}

	//Effect
	if (m_pSelectedObject) {
		m_pEffectObject->AnimateEffect(m_pCamera, m_pSelectedObject->GetPosition(), fTimeElapsed, m_fTime * 10);
		//m_pLightEffectObject->AnimateEffect(m_pCamera, m_pSelectedObject->GetPosition(), fTimeElapsed, m_fTime * 10);
	}
	//m_pDebuffObject->AnimateEffect(m_pCamera, g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), fTimeElapsed, m_fTime * 10);
	//m_pMonsterObject->Animate(fTimeElapsed);
	//sword effect
	//m_pSwordFireObject->SetPosition(XMFLOAT3(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().x,
	//	m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().y,
	//	m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().z));
	//g_Logic.m_MonsterSession.m_currentPlayGameObject->Animate(fTimeElapsed);
	//auto pos = g_Logic.m_MonsterSession.m_currentPlayGameObject->GetPosition();
	//cout << "GameobjectManager::Boss Position: " << pos.x << ", 0, " << pos.z << endl;
	for (int i = 0; i < m_ppGameObjects.size(); ++i)
	{
		if (m_xmfMode == DISSOLVE_MODE)
			m_ppGameObjects[i]->Die(fTimeElapsed);
	}


	for (auto& session : g_Logic.m_inGamePlayerSession) {
		if (-1 != session.m_id && session.m_isVisible) {
			switch (session.GetRole())
			{
			case ROLE::ARCHER:
				m_pArcherObject->m_pHPBarUI->SetCurrentHP(session.m_currentPlayGameObject->GetCurrentHP());
				break;
			case ROLE::WARRIOR:
				m_pWarriorObject->m_pHPBarUI->SetCurrentHP(session.m_currentPlayGameObject->GetCurrentHP());
				break;
			case ROLE::TANKER:
				m_pTankerObject->m_pHPBarUI->SetCurrentHP(session.m_currentPlayGameObject->GetCurrentHP());
				break;
			case ROLE::PRIEST:
				m_pPriestObject->m_pHPBarUI->SetCurrentHP(session.m_currentPlayGameObject->GetCurrentHP());
				break;
			}
			if (session.m_currentDirection != DIRECTION::IDLE) {

				//session.m_currentPlayGameObject->MoveForward(50.0f * fTimeElapsed);
				session.m_currentPlayGameObject->MoveObject(session.m_currentDirection, session.m_ownerRotateAngle);
				session.m_currentPlayGameObject->Move(session.m_currentDirection, 50 * fTimeElapsed);
#ifdef _DEBUG
				auto look = session.m_currentPlayGameObject->GetLook();
				auto up = session.m_currentPlayGameObject->GetUp();
				auto right = session.m_currentPlayGameObject->GetRight();
				auto pos = session.m_currentPlayGameObject->GetPosition();
				//cout << "GameobjectManager::Animate() SessionId: " << session.m_id << endl;
				// cout << "GameobjectManager::Animate() Position: " << pos.x << ", 0, " << pos.z << endl;
				//cout << "GameobjectManager::Animate() Look: " << look.x << ", " << look.y << ", " << look.z << endl;
				//cout << "GameobjectManager::Animate() up: " << up.x << ", " << up.y << ", " << up.z << endl;
				//cout << "GameobjectManager::Animate() right: " << right.x << ", " << right.y << ", " << right.z << endl;
				// std::cout << "GameobjectManager::Animate() rotation angle: " << session.m_rotateAngle.x << ", " << session.m_rotateAngle.y << ", " << session.m_rotateAngle.z << std::endl;
#endif

			}
			//if (session.m_currentPlayGameObject->GetRButtonClicked())
			//	session.m_currentPlayGameObject->RbuttonClicked(fTimeElapsed);
			//session.m_currentPlayGameObject->Animate(fTimeElapsed);
		}
	}

#ifdef LOCAL_TASK
	static int tempcount = 0;
	if (tempcount % 900 == 0)
	{
		float y = 0.0f;
		float z = 0.0f;
		float x = 0.0f;

		while (true)
		{
			x = 80.f * (2.f * (((float)rand() / (float)RAND_MAX)) - 1.f);
			y = 0.0f;
			z = 80.f * (2.f * (((float)rand() / (float)RAND_MAX)) - 1.f);
			if (Vector3::Length(XMFLOAT3(x, y, z)) < 350.0f)
				break;
		}
		g_Logic.m_MonsterSession.m_currentPlayGameObject->m_xmf3Destination = XMFLOAT3(x, y, z);
	}
	tempcount++;
#endif
	CharacterUIAnimate(fTimeElapsed);
	if (m_pTrailComponent) {
		TrailAnimate(fTimeElapsed);
	}
	//TextUI Update
	{//서순 중요 AddText
		AddTextToUILayer(m_iTEXTiIndex);
		m_pUILayer->Update(fTimeElapsed, m_bNPCinteraction, m_bNPCscreen);
	}

}

void GameobjectManager::CharacterUIAnimate(float fTimeElapsed)
{
	if (g_Logic.m_inGamePlayerSession[0].GetRole() == ROLE::ARCHER) {
		m_pArcherObject->m_pHPBarUI->SetinitScale(0.09, 0.0065, 1);
		m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.57, 0.46, 1.00));
		m_pArcherObject->m_pProfileUI->SetinitScale(0.04, 0.025, 1);
		m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.91, 0.5, 1.00));
		m_pTankerObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.36, 1.00));
		m_pTankerObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.38, 1.00));
		m_pPriestObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.24, 1.00));
		m_pPriestObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.26, 1.00));
		m_pWarriorObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.12, 1.00));
		m_pWarriorObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.14, 1.00));
	}
	else if (g_Logic.m_inGamePlayerSession[0].GetRole() == ROLE::TANKER) {
		m_pTankerObject->m_pHPBarUI->SetinitScale(0.09, 0.0065, 1);
		m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.57, 0.46, 1.00));
		m_pTankerObject->m_pProfileUI->SetinitScale(0.04, 0.025, 1);
		m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.91, 0.5, 1.00));
		m_pArcherObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.36, 1.00));
		m_pArcherObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.38, 1.00));
		m_pPriestObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.24, 1.00));
		m_pPriestObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.26, 1.00));
		m_pWarriorObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.12, 1.00));
		m_pWarriorObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.14, 1.00));
	}
	else if (g_Logic.m_inGamePlayerSession[0].GetRole() == ROLE::PRIEST) {
		m_pPriestObject->m_pHPBarUI->SetinitScale(0.09, 0.0065, 1);
		m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.57, 0.46, 1.00));
		m_pPriestObject->m_pProfileUI->SetinitScale(0.04, 0.025, 1);
		m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.91, 0.5, 1.00));
		m_pTankerObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.36, 1.00));
		m_pTankerObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.38, 1.00));
		m_pArcherObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.24, 1.00));
		m_pArcherObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.26, 1.00));
		m_pWarriorObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.12, 1.00));
		m_pWarriorObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.14, 1.00));
	}
	else if (g_Logic.m_inGamePlayerSession[0].GetRole() == ROLE::WARRIOR) {
		m_pWarriorObject->m_pHPBarUI->SetinitScale(0.09, 0.0065, 1);
		m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.57, 0.46, 1.00));
		m_pWarriorObject->m_pProfileUI->SetinitScale(0.04, 0.025, 1);
		m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.91, 0.5, 1.00));
		m_pTankerObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.36, 1.00));
		m_pTankerObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.38, 1.00));
		m_pPriestObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.24, 1.00));
		m_pPriestObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.26, 1.00));
		m_pArcherObject->m_pHPBarUI->SetinitScale(0.07, 0.005, 1);
		m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.12, 1.00));
		m_pArcherObject->m_pProfileUI->SetinitScale(0.03, 0.015, 1);
		m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.14, 1.00));
	}
}

void GameobjectManager::TrailAnimate(float fTimeElapsed)
{
	m_pTrailComponent->AddTrail(XMFLOAT3(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().x,
		m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().y,
		m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition().z),
		XMFLOAT3(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().x,
			m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().y,
			m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().z));

}

void GameobjectManager::StoryUIAnimate(float fTimeElapsed)
{

}

void GameobjectManager::OnPreRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pDepthShaderComponent->PrepareShadowMap(pd3dDevice, pd3dCommandList);
	//Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//렌더


	UpdateShaderVariables(pd3dCommandList);
	m_pSkyboxObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pDepthShaderComponent) {
		m_pDepthShaderComponent->UpdateShaderVariables(pd3dCommandList);//오브젝트의 깊이값의 렌더입니다.
	}

	//m_pFireballSpriteObjects[0]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//
	//g_Logic.m_MonsterSession.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	if (m_bDebugMode)
	{
		for (int i = 0; i < 5; i++)
			m_pBoundingBox[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		for (auto& p : m_ppObstacleBoundingBox)
		{
			p->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		}
		for(auto& p : m_ppNormalMonsterBoundingBox)
		{
			p->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		}
	}
	for (int i = 0; i < m_pArrowObjects.size(); i++) {
		if (m_pArrowObjects[i])
		{
			if (m_pArrowObjects[i]->m_bActive)
			{
				m_pArrowObjects[i]->Animate(m_fTimeElapsed);
				m_pArrowObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			}
		}
	}
	for (int i = 0; i < m_pEnergyBallObjects.size(); i++) {
		m_pEnergyBallObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

	//m_pMonsterCubeObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//for (auto& session : g_Logic.m_inGamePlayerSession) {
	//	if (-1 != session.m_id && session.m_isVisible) {
	//		session.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//		//if (session.m_currentPlayGameObject->m_SPBB.Intersects(m_pMonsterObject->m_SPBB))
	//		//{
	//		//	cout << "충돌 " << endl;
	//		//}
	//		//for (int i = 0; i < 10; ++i)
	//		//{
	//		//	if (session.m_currentPlayGameObject->m_pArrow[i]->m_bActive)
	//		//	{
	//		//		if (session.m_currentPlayGameObject->m_pArrow[i]->m_SPBB.Intersects(m_pMonsterObject->m_SPBB))
	//		//		{
	//		//			cout << i << "번째 화살 충돌 " << endl;
	//		//			session.m_currentPlayGameObject->m_pArrow[i]->m_bActive = false;
	//		//		}
	//		//	}
	//		//}
	//	}
	//}
	//m_pMonsterObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	//m_pUIGameSearchObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	//for (int i = 0; i < m_ppUIObjects.size(); i++) {
	//	m_ppUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}

	if (m_pShadowmapShaderComponent)
	{
	
		m_pShadowmapShaderComponent->Render(pd3dDevice, pd3dCommandList, 0, pd3dGraphicsRootSignature, m_fTimeElapsed,m_nStageType);
	
	}
	/*for (int i = 0; i < 6; i++) {
		if (m_pStage1Objects[i])
			m_pStage1Objects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}*/
	if (m_pMonsterHPBarObject) {
		//m_pMonsterHPBarObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

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

	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
		m_ppParticleObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//파티클
	}
	TrailRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	EffectRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_fTime);


	//m_pNaviMeshObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	if (m_pTextureToViewportComponent)
	{
		//m_pTextureToViewportComponent->Render(pd3dCommandList, m_pCamera, 0, pd3dGraphicsRootSignature);
	}

	if (GameEnd)
	{
		m_pVictoryUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pContinueUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		//m_pUIGameSearchObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

	if (m_pStage1TerrainObject) {
		//m_pStage1TerrainObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}


	AstarRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void GameobjectManager::TrailRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//트레일 오브젝트 렌더입니다.
	if (m_pTrailObject) {
		m_pTrailObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	if (m_pTrailComponent) {
		m_pTrailComponent->RenderTrail(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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

//XMFLOAT4 GetQuaternion(float x, float y, float z)
//{
//	float x_half = x / 2;
//	float y_half = y / 2;
//	float z_half = z / 2;
//
//	float sin_x = sin(x_half);
//	float cos_x = cos(x_half);
//
//	float sin_y = sin(y_half);
//	float cos_y = cos(y_half);
//
//	float sin_z = sin(z_half);
//	float cos_z = cos(z_half);
//
//	float revW = cos_x * cos_y * cos_z + sin_x * sin_y * sin_z;
//	float revX = sin_x * cos_y * cos_z - cos_x * sin_y * sin_z;
//	float revY = cos_x * sin_y * cos_z + sin_x * cos_y * sin_z;
//	float revZ = cos_x * cos_y * sin_z - sin_x * sin_y * cos_z;
//
//	float magnitude = sqrt(revW * revW + revX * revX + revY * revY + revZ * revZ);
//	revW /= magnitude;
//	revX /= magnitude;
//	revY /= magnitude;
//	revZ /= magnitude;
//
//	return XMFLOAT4(revW, revX, revY, revZ);
//}

void GameobjectManager::ReadObjectFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* fileName, CLoadedModelInfoCompnent* modelName, int type,int stagetype)
{
	ifstream objectFile(fileName);

	int objCount = -1;
	vector<XMFLOAT3> tempPos;
	vector<XMFLOAT4> quaternion;

	vector<XMFLOAT3> tempScale;
	vector<XMFLOAT3> tempRotate;

	vector<XMFLOAT3> tempCenterPos;
	vector<XMFLOAT3> tempLocalCenterPos;
	vector<XMFLOAT3> tempExtentScale;

	vector<XMFLOAT3> tempBoundingBoxSize;

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
		else if (temp == "<BoxCollider>")
		{
			for (int j = 0; j < 8; ++j)
			{
				objectFile >> temp;
				if (temp == "<center>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					tempCenterPos.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<size>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					tempBoundingBoxSize.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<localCenter>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					tempLocalCenterPos.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<localSize>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
						number[i] = stof(temp);
					}
					tempExtentScale.emplace_back(number[0], number[1], number[2]);
				}
				else if (temp == "<forward>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
					}
				}
				else if (temp == "<right>:")
				{
					for (int i = 0; i < 3; ++i)
					{
						objectFile >> temp;
					}
				}
				else if (temp == "<forwardDotRes>:")
				{
					objectFile >> temp;
				}
				else if (temp == "<rightDotRes>:")
				{
					objectFile >> temp;
				}
			}
		}
		else if (temp == "<Seq>:") { objectFile >> temp; }
		else
		{
			if (type == 0)
				objectFile >> temp;
			objCount++;
		}
	}
	GameObject** tempObject = new GameObject * [objCount];	// 멤버 변수로 교체 예정
	float scale = 10.0f;
	if (type == 1)
		scale = 1.0f;

	for (int i = 0; i < objCount; ++i)
	{
		tempObject[i] = new GameObject(UNDEF_ENTITY);
		tempObject[i]->InsertComponent<RenderComponent>();
		tempObject[i]->InsertComponent<CLoadedModelInfoCompnent>();
		tempObject[i]->SetPosition(XMFLOAT3(tempPos[i].x, tempPos[i].y, tempPos[i].z));
		tempObject[i]->SetModel(modelName);
		tempObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		tempObject[i]->SetScale(tempScale[i].x * scale, tempScale[i].y * scale, tempScale[i].z * scale);
		XMFLOAT3 Axis = XMFLOAT3(1, 0, 0);
		tempObject[i]->Rotate(&Axis, tempRotate[i].x);
		Axis = tempObject[i]->GetUp();
		tempObject[i]->Rotate(&Axis, tempRotate[i].y);
		Axis = tempObject[i]->GetUp();
		tempObject[i]->Rotate(&Axis, tempRotate[i].z);
		tempObject[i]->SetPosition(XMFLOAT3(tempPos[i].x * scale, tempPos[i].y * scale, tempPos[i].z * scale));
		tempObject[i]->m_nStageType = stagetype;
		if (type == 1)
		{
			XMFLOAT3 extentPos = XMFLOAT3(tempScale[i].x * tempExtentScale[i].x,
				tempScale[i].y * tempExtentScale[i].y, tempScale[i].z * tempExtentScale[i].z);
			XMFLOAT3 centerPos =  Vector3::Add(tempPos[i], tempLocalCenterPos[i]);
			extentPos = XMFLOAT3(extentPos.x * 0.5f, extentPos.y * 0.5f, extentPos.z * 0.5f);
			tempObject[i]->m_OBB = BoundingOrientedBox(tempCenterPos[i], extentPos, quaternion[i]);
			m_ppObstacleObjects.emplace_back(tempObject[i]);

			GameObject* tempBoundingBox = new GameObject(SQUARE_ENTITY);
			tempBoundingBox->InsertComponent<RenderComponent>();
			tempBoundingBox->InsertComponent<CubeMeshComponent>();
			tempBoundingBox->InsertComponent<BoundingBoxShaderComponent>();
			tempBoundingBox->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

			tempBoundingBox->SetPosition(tempCenterPos[i]);
			tempBoundingBox->Rotate(&quaternion[i]);
			tempBoundingBox->SetScale(extentPos.x * 2.0f, extentPos.y * 2.0f, extentPos.z * 2.0f);
			m_ppObstacleBoundingBox.emplace_back(tempBoundingBox);
		}
		m_ppGameObjects.emplace_back(tempObject[i]);
	}
}

void GameobjectManager::ReadNormalMonsterFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* fileName, CLoadedModelInfoCompnent* modelName, int type,int stagetype)
{
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
	float fScale = 1.0f;
	if (type == 1)
		fScale = 10.0f;
	for (int i = 0; i < objCount; ++i)
	{
		tempObject[i] = new NormalMonster();
		tempObject[i]->m_nID = i;
		tempObject[i]->InsertComponent<RenderComponent>();
		tempObject[i]->InsertComponent<CLoadedModelInfoCompnent>();
		tempObject[i]->SetModel(modelName);
		tempObject[i]->SetPosition(XMFLOAT3(tempPos[i].x * fScale, tempPos[i].y - 12.0f, tempPos[i].z * fScale));
		tempObject[i]->SetAnimationSets(6);
		tempObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		tempObject[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(6);
		tempObject[i]->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
		tempObject[i]->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_FIRSTSKILL]->m_nType = ANIMATION_TYPE_ONCE;

		pair<CharacterAnimation, CharacterAnimation> StartAnimations = { CharacterAnimation::CA_FIRSTSKILL, CharacterAnimation::CA_FIRSTSKILL };
		tempObject[i]->m_pSkinnedAnimationController->m_CurrentAnimations = StartAnimations;
		tempObject[i]->m_pSkinnedAnimationController->SetTrackEnable(StartAnimations);
		tempObject[i]->SetMoveState(true);
		XMFLOAT3 Axis = XMFLOAT3(1, 0, 0);
		tempObject[i]->Rotate(&Axis, tempRotate[i].x);
		Axis = tempObject[i]->GetUp();
		tempObject[i]->Rotate(&Axis, tempRotate[i].y);
		Axis = tempObject[i]->GetUp();
		tempObject[i]->Rotate(&Axis, tempRotate[i].z);
		tempObject[i]->SetScale(tempScale[i].x, tempScale[i].y, tempScale[i].z);
		XMFLOAT3 t = tempObject[i]->GetPosition();
		tempObject[i]->m_nStageType = stagetype;
		// 바운딩 스피어 추가 필요
		GameObject* MonsterBoundingSphere = new GameObject(SQUARE_ENTITY);
		MonsterBoundingSphere->InsertComponent<RenderComponent>();
		MonsterBoundingSphere->InsertComponent<SphereMeshComponent>();
		MonsterBoundingSphere->InsertComponent<BoundingBoxShaderComponent>();
		MonsterBoundingSphere->SetBoundingSize(15.0f);
		MonsterBoundingSphere->SetBoundingOffset(XMFLOAT3(0.0f, 18.0f, 0.0f));
		MonsterBoundingSphere->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		MonsterBoundingSphere->SetScale(1.f);
		tempObject[i]->SetBoundingBox(MonsterBoundingSphere);

		m_ppNormalMonsterBoundingBox.emplace_back(MonsterBoundingSphere);
		m_ppGameObjects.emplace_back(tempObject[i]);
	}
}

GameObject* GameobjectManager::GetChracterInfo(ROLE r)
{
	if (r == ROLE::WARRIOR) return m_pWarriorObject;
	if (r == ROLE::PRIEST) return m_pPriestObject;
	if (r == ROLE::TANKER) return m_pTankerObject;
	if (r == ROLE::ARCHER) return m_pArcherObject;
}

void GameobjectManager::EffectRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float ftimeElapsed)
{
	if (m_pEffectObject) {
		m_pEffectObject->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	if (m_pDebuffObject) {
		m_pDebuffObject->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	if (m_pLightEffectObject) {
		m_pLightEffectObject->RenderEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
}

void GameobjectManager::BuildBossStageObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CLoadedModelInfoCompnent* Rock01Model = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock01.bin", NULL, true);
	CLoadedModelInfoCompnent* Rock02Model = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock02.bin", NULL, true);
	CLoadedModelInfoCompnent* Rock03Model = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock03.bin", NULL, true);
	CLoadedModelInfoCompnent* DeathModel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Death.bin", NULL, true);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock1.txt", Rock01Model, 1, STAGE2);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock2.txt", Rock02Model, 1, STAGE2);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock3.txt", Rock03Model, 1, STAGE2);
	ReadNormalMonsterFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/NormalMonster.txt", DeathModel, 0, STAGE2);
}

void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//빌드
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();

	m_pCamera->SetPosition(XMFLOAT3(-1450, 18, -1490));
	m_pCamera->Rotate(0, 90, 0);
	CLoadedModelInfoCompnent* ArrowModel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Arrow.bin", NULL, true);

	BuildStage1(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildBossStageObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);


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

	/*for (int i = 0; i < 4; ++i)
	{
		m_pBoundingBox[i] = new GameObject(SQUARE_ENTITY);
		m_pBoundingBox[i]->InsertComponent<RenderComponent>();
		m_pBoundingBox[i]->InsertComponent<SphereMeshComponent>();
		m_pBoundingBox[i]->InsertComponent<BoundingBoxShaderComponent>();
		m_pBoundingBox[i]->SetBoundingSize(8.0f);
		m_pBoundingBox[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pBoundingBox[i]->SetScale(1.f);
	}*/
	m_pWarriorObject = new Warrior();
	m_pWarriorObject->InsertComponent<RenderComponent>();
	m_pWarriorObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pWarriorObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1500.f));
	// m_pWarriorObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pWarriorObject->SetModel("Model/Warrior.bin");
	m_pWarriorObject->SetAnimationSets(6);
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(6);
	m_pWarriorObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pWarriorObject->SetScale(30.0f);
	m_pWarriorObject->Rotate(0, -90, 0);
	m_pWarriorObject->SetBoundingBox(m_pBoundingBox[0]);
	m_ppGameObjects.emplace_back(m_pWarriorObject);

	m_pArcherObject = new Archer();
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pArcherObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1520.f));
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

	for (int i = 0; i < MAX_ARROW; ++i)
	{
		m_pArrowObjects[i] = new Arrow();
		m_pArrowObjects[i]->InsertComponent<RenderComponent>();
		m_pArrowObjects[i]->InsertComponent<CLoadedModelInfoCompnent>();
		m_pArrowObjects[i]->SetPosition(XMFLOAT3(25 * i, 0, 0));
		m_pArrowObjects[i]->SetModel(ArrowModel);
		m_pArrowObjects[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pArrowObjects[i]->SetScale(30.0f);
		m_pArrowObjects[i]->SetBoundingSize(0.2f);
		static_cast<Archer*>(m_pArcherObject)->SetArrow(m_pArrowObjects[i]);
	}

	m_pTankerObject = new Tanker();
	m_pTankerObject->InsertComponent<RenderComponent>();
	m_pTankerObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pTankerObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1480.f));
	// m_pTankerObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pTankerObject->SetModel("Model/Tanker.bin");
	m_pTankerObject->SetAnimationSets(8);
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(8);
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_FIRSTSKILL]->m_nType = ANIMATION_TYPE_HALF;
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fSpeed = 0.3f;
	m_pTankerObject->SetScale(30.0f);
	m_pTankerObject->Rotate(0, -90, 0);
	m_pTankerObject->SetBoundingBox(m_pBoundingBox[2]);
	m_ppGameObjects.emplace_back(m_pTankerObject);

	m_pPriestObject = new Priest();
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPriestObject->SetPosition(XMFLOAT3(-1400.f, 0.f, -1460.f));
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
	m_pHealRange->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pHealRange->SetPosition(XMFLOAT3(0, 0.0f, 0));
	m_pHealRange->SetScale(1.0f);
	m_pHealRange->m_bActive = false;
	static_cast<Priest*>(m_pPriestObject)->m_pHealRange = m_pHealRange;

	for (int i = 0; i < 10; ++i)
	{
		m_pEnergyBallObjects[i] = new EnergyBall();
		m_pEnergyBallObjects[i]->InsertComponent<RenderComponent>();
		m_pEnergyBallObjects[i]->InsertComponent<SphereMeshComponent>();
		m_pEnergyBallObjects[i]->InsertComponent<SphereShaderComponent>();
		m_pEnergyBallObjects[i]->SetPosition(XMFLOAT3(0, 0, 100));
		m_pEnergyBallObjects[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pEnergyBallObjects[i]->SetScale(0.5f);
		m_pEnergyBallObjects[i]->SetBoundingSize(4);
		static_cast<Priest*>(m_pPriestObject)->SetEnergyBall(m_pEnergyBallObjects[i]);
	}

	m_pEnergyBallObject = new EnergyBall();
	m_pEnergyBallObject->InsertComponent<RenderComponent>();
	m_pEnergyBallObject->InsertComponent<SphereMeshComponent>();
	m_pEnergyBallObject->InsertComponent<SphereShaderComponent>();
	m_pEnergyBallObject->SetPosition(XMFLOAT3(0, 0, 100));
	m_pEnergyBallObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pEnergyBallObject->SetScale(0.5f);
	m_pEnergyBallObject->SetBoundingSize(4);

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
	m_pMonsterObject->SetPosition(XMFLOAT3(500, 0, 0));
	m_pMonsterObject->SetModel("Model/Boss.bin");
	m_pMonsterObject->SetAnimationSets(13);
	m_pMonsterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackAnimationSet(13);
	m_pMonsterObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[BOSS_ANIMATION::BA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pMonsterObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[BOSS_ANIMATION::BA_UNDERGROUND]->m_nType = ANIMATION_TYPE_ONCE;
	m_pMonsterObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[BOSS_ANIMATION::BA_REVERSE_SPAWN]->m_nType = ANIMATION_TYPE_REVERSE;
	m_pMonsterObject->m_pSkinnedAnimationController->m_pAnimationTracks[BOSS_ANIMATION::BA_PUNCHING_SKILL].m_fSpeed = 2.0f;
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_PUNCHING_SKILL, 2);
	m_pMonsterObject->SetScale(15.0f);
	m_pMonsterObject->SetBoundingSize(30.0f);
	m_pMonsterObject->SetBoundingBox(m_pBoundingBox[4]);
	m_pMonsterObject->SetMoveState(false);
	m_ppGameObjects.emplace_back(m_pMonsterObject);
	g_Logic.m_MonsterSession.SetGameObject(m_pMonsterObject);

	m_pMonsterCubeObject = new GameObject(SQUARE_ENTITY);
	m_pMonsterCubeObject->InsertComponent<RenderComponent>();
	m_pMonsterCubeObject->InsertComponent<SkyBoxMeshComponent>();
	m_pMonsterCubeObject->InsertComponent<SkyBoxShaderComponent>();
	m_pMonsterCubeObject->InsertComponent<TextureComponent>();
	m_pMonsterCubeObject->SetTexture(L"DreamWorld/DreamWorld.dds", RESOURCE_TEXTURE_CUBE, 12);
	m_pMonsterCubeObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pMonsterCubeObject->SetScale(10);
	m_pMonsterCubeObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pSkyboxObject = new GameObject(SQUARE_ENTITY);
	m_pSkyboxObject->InsertComponent<RenderComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxMeshComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxShaderComponent>();
	m_pSkyboxObject->InsertComponent<TextureComponent>();
	m_pSkyboxObject->SetTexture(L"DreamWorld/SkyBox.dds", RESOURCE_TEXTURE_CUBE, 12);
	m_pSkyboxObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pSkyboxObject->SetScale(1, 1, 1);
	m_pSkyboxObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

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
	m_pPlayerObject = new GameObject(UNDEF_ENTITY);
	m_pCamera->Rotate(0, -90, 0);
	m_pArcherObject->SetCamera(m_pCamera);
	m_pPlayerObject = m_pArcherObject;
	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject = m_pPlayerObject;
	g_Logic.m_inGamePlayerSession[0].m_isVisible = true;
	g_Logic.m_inGamePlayerSession[0].m_id = 0;
#endif // LOCAL_TASK

	BuildNPC(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildShadow(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//무조건 마지막에 해줘야된다.
// 서순을 잘챙기자 ㅋㅋ	
	//m_pTextureToViewportComponent = new TextureToViewportComponent();
	//m_pTextureToViewportComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	//m_pTextureToViewportComponent->BuildObjects(pd3dDevice, pd3dCommandList, m_pDepthShaderComponent->GetDepthTexture());

	BuildCharacterUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
	m_pFireballSpriteObject->SetTexture(L"MagicEffect/CandleFlame.dds", RESOURCE_TEXTURE2D, 3);
	m_pFireballSpriteObject->SetPosition(XMFLOAT3(100, 40, 100));
	m_pFireballSpriteObject->SetScale(10);
	m_pFireballSpriteObject->SetRowColumn(16, 8, 0);
	m_pFireballSpriteObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppParticleObjects.emplace_back(m_pFireballSpriteObject);

	m_pLightningSpriteObject = new GameObject(UNDEF_ENTITY);
	m_pLightningSpriteObject->InsertComponent<RenderComponent>();
	m_pLightningSpriteObject->InsertComponent<UIMeshComponent>();
	m_pLightningSpriteObject->InsertComponent<MultiSpriteShaderComponent>();
	m_pLightningSpriteObject->InsertComponent<TextureComponent>();
	m_pLightningSpriteObject->SetTexture(L"MagicEffect/Lightning_2x2.dds", RESOURCE_TEXTURE2D, 3);
	m_pLightningSpriteObject->SetPosition(XMFLOAT3(0, 40, 100));
	m_pLightningSpriteObject->SetScale(7.0f, 12.0f, 7.0f);
	m_pLightningSpriteObject->SetRowColumn(2.0f, 2.0f, 0.06f);
	m_pLightningSpriteObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppParticleObjects.emplace_back(m_pLightningSpriteObject);

	m_pMosterdebuffSpriteObject = new GameObject(UNDEF_ENTITY);
	m_pMosterdebuffSpriteObject->InsertComponent<RenderComponent>();
	m_pMosterdebuffSpriteObject->InsertComponent<UIMeshComponent>();
	m_pMosterdebuffSpriteObject->InsertComponent<MultiSpriteShaderComponent>();
	m_pMosterdebuffSpriteObject->InsertComponent<TextureComponent>();
	m_pMosterdebuffSpriteObject->SetTexture(L"MagicEffect/Monsterdebuff_5x6.dds", RESOURCE_TEXTURE2D, 3);
	m_pMosterdebuffSpriteObject->SetPosition(XMFLOAT3(0, 40, 130));
	m_pMosterdebuffSpriteObject->SetScale(2);
	m_pMosterdebuffSpriteObject->SetRowColumn(5, 6, 0.03);
	m_pMosterdebuffSpriteObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppParticleObjects.emplace_back(m_pMosterdebuffSpriteObject);


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
	m_pTrailObject->SetTexture(L"Trail/Trail.dds", RESOURCE_TEXTURE2D, 3);
	m_pTrailObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pTrailObject->SetColor(XMFLOAT4(1.0f, 0.3f, 0.0f, 0.0f));
	m_pTrailObject->SetScale(1);
	m_pTrailObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTrailComponent = new TrailComponent();
	m_pTrailComponent->ReadyComponent(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pTrailObject);
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

	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BigMushroom.txt", BigMushroom, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Mushroom.txt", Mushroom, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/LongFence.txt", LongFence, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence1.txt", ShortFence01, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence2.txt", ShortFence02, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ShortFence3.txt", ShortFence03, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Tree.txt", Tree, 0, STAGE1);
	ReadObjectFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/OOBB.txt", Cube, 0, STAGE1);
	ReadNormalMonsterFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/NormalMonsterS1.txt", Death, 1, STAGE1);
	m_pStage1Objects[0] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[0]->InsertComponent<RenderComponent>();
	m_pStage1Objects[0]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[0]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[0]->SetModel("Model/New_Terrain.bin");
	m_pStage1Objects[0]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[0]->SetScale(10);
	m_pStage1Objects[0]->SetColor(XMFLOAT4(0, 1.0f, 1.0f, 1));
	m_pStage1Objects[0]->SetRimLight(false);
	m_pStage1Objects[0]->m_nStageType = STAGE1;
	m_ppGameObjects.emplace_back(m_pStage1Objects[0]);
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

	/*m_pUIWarriorCharacterObject = new GameObject(UI_ENTITY);
	m_pUIWarriorCharacterObject->InsertComponent<RenderComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<TextureComponent>();
	m_pUIWarriorCharacterObject->SetTexture(L"UI/Warrior.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIWarriorCharacterObject->SetPosition(XMFLOAT3(-0.3, 0.2, 1.00));
	m_pUIWarriorCharacterObject->SetScale(0.08, 0.08, 1);
	m_pUIWarriorCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIWarriorCharacterObject);

	m_pUIArcherCharacterObject = new GameObject(UI_ENTITY);
	m_pUIArcherCharacterObject->InsertComponent<RenderComponent>();
	m_pUIArcherCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIArcherCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIArcherCharacterObject->InsertComponent<TextureComponent>();
	m_pUIArcherCharacterObject->SetTexture(L"UI/Archer.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIArcherCharacterObject->SetPosition(XMFLOAT3(0.3, 0.2, 1.00));
	m_pUIArcherCharacterObject->SetScale(0.08, 0.08, 1);
	m_pUIArcherCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIArcherCharacterObject);

	m_pUITankerCharacterObject = new GameObject(UI_ENTITY);
	m_pUITankerCharacterObject->InsertComponent<RenderComponent>();
	m_pUITankerCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUITankerCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUITankerCharacterObject->InsertComponent<TextureComponent>();
	m_pUITankerCharacterObject->SetTexture(L"UI/Tanker.dds", RESOURCE_TEXTURE2D, 3);
	m_pUITankerCharacterObject->SetPosition(XMFLOAT3(-0.3, -0.2, 1.00));
	m_pUITankerCharacterObject->SetScale(0.08, 0.08, 1);
	m_pUITankerCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUITankerCharacterObject);

	m_pUIPriestCharacterObject = new GameObject(UI_ENTITY);
	m_pUIPriestCharacterObject->InsertComponent<RenderComponent>();
	m_pUIPriestCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIPriestCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIPriestCharacterObject->InsertComponent<TextureComponent>();
	m_pUIPriestCharacterObject->SetTexture(L"UI/Priest.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIPriestCharacterObject->SetPosition(XMFLOAT3(0.3, -0.2, 1.00));
	m_pUIPriestCharacterObject->SetScale(0.08, 0.08, 1);
	m_pUIPriestCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIPriestCharacterObject);*/

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
	m_pTalkUIObject->SetTexture(L"UI/Panel10Blue.dds", RESOURCE_TEXTURE2D, 3);
	m_pTalkUIObject->SetPosition(XMFLOAT3(0.0, -0.45, 1.01));
	m_pTalkUIObject->SetScale(0.23, 0.035, 1);
	m_pTalkUIObject->SetColor(XMFLOAT4(0, 0, 0, 0.0));
	m_pTalkUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pPressGUIObject = new GameObject(UI_ENTITY);
	m_pPressGUIObject->InsertComponent<RenderComponent>();
	m_pPressGUIObject->InsertComponent<UIMeshComponent>();
	m_pPressGUIObject->InsertComponent<UiShaderComponent>();
	m_pPressGUIObject->InsertComponent<TextureComponent>();
	m_pPressGUIObject->SetTexture(L"UI/PressGPink.dds", RESOURCE_TEXTURE2D, 3);
	m_pPressGUIObject->SetPosition(XMFLOAT3(0.56, -0.5, 1.00));
	m_pPressGUIObject->SetScale(0.02, 0.02, 1);
	m_pPressGUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	////////////////////////skill/////////////////////////////////////////////////
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
	/////////////////////////////////////////////////////////////////////////

	m_pAttackUIObject = new GameObject(UI_ENTITY);
	m_pAttackUIObject->InsertComponent<RenderComponent>();
	m_pAttackUIObject->InsertComponent<UIMeshComponent>();
	m_pAttackUIObject->InsertComponent<UiShaderComponent>();
	m_pAttackUIObject->InsertComponent<TextureComponent>();
	m_pAttackUIObject->SetTexture(L"UI/Attack.dds", RESOURCE_TEXTURE2D, 3);
	m_pAttackUIObject->SetPosition(XMFLOAT3(0.0, 0.0, 1.01));
	m_pAttackUIObject->SetScale(0.014, 0.005, 1);
	m_pAttackUIObject->SetColor(XMFLOAT4(0, -0.7, -5, 0));
	//m_pAttackUIObject->SetColor(XMFLOAT4(0, 0, 0, 0.75));
	m_pAttackUIObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pAttackUIObject);
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


///////////////////////////////////////////////////////
	m_pArcherObject->m_pHPBarUI = new GameObject(UI_ENTITY);
	m_pArcherObject->m_pHPBarUI->InsertComponent<RenderComponent>();
	m_pArcherObject->m_pHPBarUI->InsertComponent<UIMeshComponent>();
	m_pArcherObject->m_pHPBarUI->InsertComponent<UiShaderComponent>();
	m_pArcherObject->m_pHPBarUI->InsertComponent<TextureComponent>();
	m_pArcherObject->m_pHPBarUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.18, 1.00));
	m_pArcherObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pArcherObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pHPBarUI);

	m_pArcherObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pArcherObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pArcherObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pArcherObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pArcherObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pArcherObject->m_pProfileUI->SetTexture(L"UI/Archer.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.2, 1.00));
	m_pArcherObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pArcherObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pProfileUI);

	m_pArcherObject->m_pSkillUI = new GameObject(UI_ENTITY);
	m_pArcherObject->m_pSkillUI->InsertComponent<RenderComponent>();
	m_pArcherObject->m_pSkillUI->InsertComponent<UIMeshComponent>();
	m_pArcherObject->m_pSkillUI->InsertComponent<UiShaderComponent>();
	m_pArcherObject->m_pSkillUI->InsertComponent<TextureComponent>();
	m_pArcherObject->m_pSkillUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherObject->m_pSkillUI->SetPosition(XMFLOAT3(0.25, 0.5, 1.00));
	m_pArcherObject->m_pSkillUI->SetScale(0.05, 0.025, 1);
	m_pArcherObject->m_pSkillUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pSkillUI);
	////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////
	m_pTankerObject->m_pHPBarUI = new GameObject(UI_ENTITY);
	m_pTankerObject->m_pHPBarUI->InsertComponent<RenderComponent>();
	m_pTankerObject->m_pHPBarUI->InsertComponent<UIMeshComponent>();
	m_pTankerObject->m_pHPBarUI->InsertComponent<UiShaderComponent>();
	m_pTankerObject->m_pHPBarUI->InsertComponent<TextureComponent>();
	m_pTankerObject->m_pHPBarUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, 0.06, 1.00));
	m_pTankerObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pTankerObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pTankerObject->m_pHPBarUI);

	m_pTankerObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pTankerObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pTankerObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pTankerObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pTankerObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pTankerObject->m_pProfileUI->SetTexture(L"UI/Tanker.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.08, 1.01));
	m_pTankerObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pTankerObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pTankerObject->m_pProfileUI);

	m_pTankerObject->m_pSkillUI = new GameObject(UI_ENTITY);
	m_pTankerObject->m_pSkillUI->InsertComponent<RenderComponent>();
	m_pTankerObject->m_pSkillUI->InsertComponent<UIMeshComponent>();
	m_pTankerObject->m_pSkillUI->InsertComponent<UiShaderComponent>();
	m_pTankerObject->m_pSkillUI->InsertComponent<TextureComponent>();
	m_pTankerObject->m_pSkillUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->m_pSkillUI->SetPosition(XMFLOAT3(0.25, 0.5, 1.01));
	m_pTankerObject->m_pSkillUI->SetScale(0.05, 0.025, 1);
	m_pTankerObject->m_pSkillUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppCharacterUIObjects.emplace_back(m_pTankerObject->m_pSkillUI);
	////////////////////////////////////////////////////////////////
	m_pPriestObject->m_pHPBarUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pHPBarUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pHPBarUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pHPBarUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pHPBarUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pHPBarUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, -0.06, 1.01));
	m_pPriestObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pPriestObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pHPBarUI);

	m_pPriestObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pProfileUI->SetTexture(L"UI/Priest.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, -0.04, 1.01));
	m_pPriestObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pPriestObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pProfileUI);

	m_pPriestObject->m_pSkillUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pSkillUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pSkillUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pSkillUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pSkillUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pSkillUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pSkillUI->SetPosition(XMFLOAT3(0.25, 0.5, 1.01));
	m_pPriestObject->m_pSkillUI->SetScale(0.05, 0.025, 1);
	m_pPriestObject->m_pSkillUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pSkillUI);
	////////////////////////////////////////////////////////////////
	m_pWarriorObject->m_pHPBarUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pHPBarUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pHPBarUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pHPBarUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pHPBarUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pHPBarUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, -0.18, 1.01));
	m_pWarriorObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pWarriorObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pHPBarUI);

	m_pWarriorObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pProfileUI->SetTexture(L"UI/Warrior.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, -0.16, 1.01));
	m_pWarriorObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pWarriorObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pProfileUI);

	m_pWarriorObject->m_pSkillUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pSkillUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pSkillUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pSkillUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pSkillUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pSkillUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pSkillUI->SetPosition(XMFLOAT3(0.25, 0.5, 1.01));
	m_pWarriorObject->m_pSkillUI->SetScale(0.05, 0.025, 1);
	m_pWarriorObject->m_pSkillUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pSkillUI);
	//m_ppCharacterUIObjects.emplace_back(m_pArcherObject->m_pSkillUI);

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
	m_pEffectObject = new EffectObject;
	m_pEffectObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pDebuffObject = new DebuffObject;
	m_pDebuffObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pLightEffectObject = new LightningEffectObject;
	m_pLightEffectObject->BuildEffect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

}

void GameobjectManager::BuildNPC(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pAngelNPCObject = new Warrior();//사각형 오브젝트를 만들겠다
	m_pAngelNPCObject->InsertComponent<RenderComponent>();
	m_pAngelNPCObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pAngelNPCObject->SetPosition(XMFLOAT3(-1350.f, 0.f, -1500.f));
	m_pAngelNPCObject->SetModel("Model/Angel.bin");
	m_pAngelNPCObject->SetAnimationSets(1);
	m_pAngelNPCObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pAngelNPCObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1);
	m_pAngelNPCObject->SetScale(30.0f);
	m_ppGameObjects.emplace_back(m_pAngelNPCObject);
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
	case UI::UI_GAMESEARCHING:
	{
#ifdef LOCAL_TASK
		m_nSection = 1;

#endif
		m_pUIGameSearchObject->m_bUIActive = false;
		cout << "request Room List" << endl;
		g_NetworkHelper.SendRequestRoomList();
		break;
	}
	case UI::UI_GAMEMATCHING:
	{
		m_pUIGameEndObject->m_bUIActive = false;
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
		// 선택한 캐릭터 Warrior
		g_NetworkHelper.SetRole(ROLE::WARRIOR);
		m_pUIWarriorCharacterObject->m_bUIActive = true;
		m_pUIArcherCharacterObject->m_bUIActive = false;
		m_pUITankerCharacterObject->m_bUIActive = false;
		m_pUIPriestCharacterObject->m_bUIActive = false;
		cout << "Choose Warrior Character" << endl;
		break;
	}
	case UI::UI_ARCHERCHARACTER:
	{
		// 선택한 캐릭터 Archer
		g_NetworkHelper.SetRole(ROLE::ARCHER);
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
		g_NetworkHelper.SetRole(ROLE::TANKER);
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
		g_NetworkHelper.SetRole(ROLE::PRIEST);
		m_pUIWarriorCharacterObject->m_bUIActive = false;
		m_pUIArcherCharacterObject->m_bUIActive = false;
		m_pUITankerCharacterObject->m_bUIActive = false;
		m_pUIPriestCharacterObject->m_bUIActive = true;
		cout << "Choose Priest Character" << endl;
		break;
	}
	case UI::UI_ENTERROOM:
	{
		// 방 입장 요청 Send
		cout << "Request Entering The Room" << endl;
		break;
	}
	case UI::UI_CREATEROOM:
	{
		cout << "Create Room" << endl;
		uniform_int_distribution<int> uid(0, 1000);
		random_device rd;
		default_random_engine dre(rd());
		g_NetworkHelper.SendCreateRoomPacket(ROLE::ARCHER, L"testRoomCreate" + to_wstring(uid(dre)));
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
	if (nMessageID == WM_KEYDOWN && wParam == VK_F4)
	{
		g_NetworkHelper.SendTestGameEndPacket();
		g_Logic.m_MonsterSession.m_currentPlayGameObject->SetCurrentHP(0.0f);
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
	if (g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetCurrentHP() < FLT_EPSILON)
		return false;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':
		{
			g_Logic.m_KeyInput->m_bWKey = true;
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetMoveState(true);
			g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::FRONT);
			g_NetworkHelper.SendMovePacket(DIRECTION::FRONT);
			cout << "x" << g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition().x << "z" << g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition().z << endl;

		}
		break;
		case 'A':
		{
			g_Logic.m_KeyInput->m_bAKey = true;
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetMoveState(true);
			g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::LEFT);
			g_NetworkHelper.SendMovePacket(DIRECTION::LEFT);
		}
		break;
		case 'S':
		{
			g_Logic.m_KeyInput->m_bSKey = true;
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetMoveState(true);
			g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::BACK);
			g_NetworkHelper.SendMovePacket(DIRECTION::BACK);
		}
		break;
		case 'D':
		{
			g_Logic.m_KeyInput->m_bDKey = true;
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetMoveState(true);
			g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::RIGHT);
			g_NetworkHelper.SendMovePacket(DIRECTION::RIGHT);
		}
		break;
		case 'Q':
		{
			//g_Logic.m_KeyInput->m_bQKey = true;
			Character* pcharacter = static_cast<Character*>(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject);
			if (pcharacter->GetQSkillState() == false && pcharacter->GetOnAttack() == false)
			{
				g_NetworkHelper.SendSkillStatePacket(pcharacter->GetQSkillState(), pcharacter->GetESkillState());
				pcharacter->FirstSkillDown();
			}
			break;
		}
		case 'E':
		{
			Character* pcharacter = static_cast<Character*>(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject);
			if (pcharacter->GetESkillState() == false && pcharacter->GetOnAttack() == false)
			{
				g_NetworkHelper.SendSkillStatePacket(pcharacter->GetQSkillState(), pcharacter->GetESkillState());
				pcharacter->SecondSkillDown();
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
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pCamera->ReInitCamrea();
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetCamera(m_pCamera);
#endif // !LOCAL_TASK

			break;
		}
		case VK_CONTROL:
			break;
		case VK_F1:
			break;
		case VK_F2:
			break;
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
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::FRONT);
				if (g_Logic.m_KeyInput->IsAllMovekeyUp())
				{
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetMoveState(false);
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition()); //, g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate				
				}
				else
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::FRONT);
			}
		}
		break;
		case 'A':
		{
			if (g_Logic.m_KeyInput->m_bAKey)
			{
				g_Logic.m_KeyInput->m_bAKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::LEFT);
				if (g_Logic.m_KeyInput->IsAllMovekeyUp())
				{
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetMoveState(false);
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition()); //, g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate
				}
				else
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::LEFT);
			}
		}
		break;
		case 'S':
		{
			if (g_Logic.m_KeyInput->m_bSKey)
			{
				g_Logic.m_KeyInput->m_bSKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::BACK);
				if (g_Logic.m_KeyInput->IsAllMovekeyUp())
				{
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetMoveState(false);
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition()); // , g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate
				}
				else
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::BACK);
			}
		}
		break;
		case 'D':
		{
			if (g_Logic.m_KeyInput->m_bDKey)
			{
				g_Logic.m_KeyInput->m_bDKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::RIGHT);
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetMoveState(false);
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition()); //, g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate
				}
				else
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::RIGHT);
			}
		}
		break;
		case 'Q':
		{
			//g_Logic.m_KeyInput->m_bQKey = false;
			static_cast<Character*>(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject)->FirstSkillUp();
			break;
		}
		case 'E':
		{
			//g_Logic.m_KeyInput->m_bEKey = false;
			static_cast<Character*>(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject)->SecondSkillUp(g_Logic.m_inGamePlayerSession[0].m_ownerRotateAngle);
			break;
		}
		//NPC와 대화하거나 포털들어갈 때 상호작용 키  
		case 'G':
		{
			m_bNPCinteraction = true;
			m_bNPCscreen = true;
			break;
		}
		case 'V':
		{
			m_bNPCinteraction = true;
			m_iTEXTiIndex = 2;
			break;
		}
		case 'T':
		{
			m_nStageType= 2;
			break;
		}
		}
	}
	default:
		break;
		}
	return(false);
	}

bool GameobjectManager::onProcessingKeyboardMessageUI(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		std::cout << "GameobjectManager::onProcessingKeyboardMessage() -key down: ";
		switch (wParam)
		{
		case 'P':
		{

			if (m_pSelectedObject)
			{
				XMFLOAT3 TempPosition = m_pSelectedObject->GetPosition();
				TempPosition.x += 0.01;
				m_pSelectedObject->SetPosition(TempPosition);

			}
		}
		break;
		case 'O':
		{
			if (m_pSelectedObject)
			{
				XMFLOAT3 TempPosition = m_pSelectedObject->GetPosition();
				TempPosition.x -= 0.01;
				m_pSelectedObject->SetPosition(TempPosition);
			}
		}
		break;
		case 'K':
		{

			if (m_pSelectedObject)
			{
				XMFLOAT3 TempPosition = m_pSelectedObject->GetPosition();
				TempPosition.y += 0.01;
				m_pSelectedObject->SetPosition(TempPosition);
			}
		}
		break;
		case 'L':
		{

			if (m_pSelectedObject)
			{
				XMFLOAT3 TempPosition = m_pSelectedObject->GetPosition();
				TempPosition.y -= 0.01;
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
			uniform_int_distribution<int> uid(0, 1000);
			random_device rd;
			default_random_engine dre(rd());
			g_NetworkHelper.SendCreateRoomPacket(ROLE::ARCHER, L"testRoomCreate" + to_wstring(uid(dre)));
			cout << "create Room" << endl;
		}
		break;
		case 'w':
		case 'W':
		{
			g_NetworkHelper.SendRequestRoomList();
			cout << "request Room List" << endl;
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

	if (GameEnd)
	{
		switch (nMessageID)
		{
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			g_NetworkHelper.SendTestGameEndOKPacket();
			g_NetworkHelper.SetRole(ROLE::NONE_SELECT);
			for (int i = 0; i < 4; ++i)
				g_Logic.m_inGamePlayerSession[i].Reset();
			ResetObject();
			break;
		}
		return;
	}

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_LMouseInput = true;
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetLButtonClicked(true);
		SomethingChanging = true;
		CheckCollision(m_ppGameObjects);
		break;
	}
	case WM_LBUTTONUP:
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_LMouseInput = false;
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetLButtonClicked(false);
		SomethingChanging = true;
		break;
	}
	case WM_RBUTTONDOWN:
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_RMouseInput = true;
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetRButtonClicked(true);
		SomethingChanging = true;
		break;
	}
	case WM_RBUTTONUP:
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_RMouseInput = false;
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetRButtonClicked(false);
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->RbuttonUp(g_Logic.m_inGamePlayerSession[0].m_ownerRotateAngle);
		SomethingChanging = true;
		break;
	}
	default:
		break;
	}

#ifndef LOCAL_TASK
	if (SomethingChanging)
		g_NetworkHelper.SendMouseStatePacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_LMouseInput
			, g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_RMouseInput);
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
	if (iIndex == START_TEXT )
	{
		queueStr.emplace(L"안녕하세요! 드림월드에 오신 것을 환영해요");
		queueStr.emplace(L"먼저 플레이 방법에 대해서 알려드릴게요!");
		queueStr.emplace(L"앞에 보이는 캐릭터들 중 원하는 캐릭터를 선택하신 후에");
		queueStr.emplace(L"게임 시작을 누르시면 선택한 캐릭터를 플레이 하실 수 있어요!");
	}
	if (iIndex == NPC_TEXT)
	{
		queueStr.emplace(L"용사님들 드디어 오셧군요");
		queueStr.emplace(L"저희 꿈마을을 지켜주세요!");
		queueStr.emplace(L"앞에 있는 악몽들을 처치해주세요!");
	}
	if (iIndex == BOSS_TEXT)
	{
		queueStr.emplace(L"너희가 꿈마을을 지킬 수 있을거 같으냐!!!");
		queueStr.emplace(L"으하하하하하");
		queueStr.emplace(L"다 죽여주마!");
	}
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
	}
	break;
	case ROLE::PRIEST:
	{
		cliSession->SetGameObject(m_pPriestObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(-270, 0, 40));
	}
	break;
	case ROLE::TANKER:
	{
		cliSession->SetGameObject(m_pTankerObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(150, 0, -60));
	}
	break;
	case ROLE::WARRIOR:
	{
		cliSession->SetGameObject(m_pWarriorObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(260, 0, 50));
	}
	break;
	default:
		break;
	}
}

void GameobjectManager::SetPlayerCamera(Session& mySession)
{
	mySession.m_currentPlayGameObject->SetCamera(m_pCamera);
	mySession.m_currentPlayGameObject->m_pCamera->ReInitCamrea();
	mySession.m_currentPlayGameObject->SetCamera(m_pCamera);
	mySession.m_currentPlayGameObject->SetLookAt(XMFLOAT3(0, 0, 0));
	auto mPos = mySession.m_currentPlayGameObject->GetPosition();
	auto cPos = mySession.m_currentPlayGameObject->m_pCamera->GetPosition();
	boolalpha(cout);
	cout << "visbie: " << mySession.m_isVisible << endl;
	cout << "MyPos: " << mPos.x << ", " << mPos.y << ", " << mPos.z << endl;
	cout << "CameraPos: " << cPos.x << ", " << cPos.y << ", " << cPos.z << endl;
}

void GameobjectManager::ResetObject()
{
	for (int i = 0; i < m_ppGameObjects.size(); ++i)
	{
		m_ppGameObjects[i]->Reset();
	}
}

void GameobjectManager::SetUIActive()
{
	m_pUIWarriorCharacterObject->m_bUIActive = true;
	m_pUIArcherCharacterObject->m_bUIActive = true;
	m_pUITankerCharacterObject->m_bUIActive = true;
	m_pUIPriestCharacterObject->m_bUIActive = true;
}

bool GameobjectManager::CheckCollision(vector<GameObject*> m_ppObjects)
{
	XMVECTOR rayOrigin = XMLoadFloat3(&XMFLOAT3(m_pCamera->GetPosition().x, m_pCamera->GetPosition().y - 10, m_pCamera->GetPosition().z));
	cout << "x" << m_pCamera->GetPosition().x << "y " << m_pCamera->GetPosition().y;
	XMVECTOR rayDirection = XMLoadFloat3(&m_pCamera->GetLookVector());
	float rayDistance;
	// 레이저와 BoundingSphere의 충돌 여부를 계산
	for (int i = 0; i < m_ppObjects.size(); i++)
	{
		if (m_ppObjects[i]->m_SPBB.Intersects(rayOrigin, rayDirection, rayDistance))
		{
			m_pSelectedObject = m_ppObjects[i];
			rayDistance = rayDistance;
			return true;
		}
	}
	return false;
}


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
#include "./Network/MapData/MapData.h"

extern NetworkHelper g_NetworkHelper;
extern Logic g_Logic;
extern bool GameEnd;
extern MapData g_bossMapData;


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
	m_pSkyboxObject->SetPosition(m_pCamera->GetPosition());
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
		m_ppParticleObjects[i]->SetScale(8);
		m_ppParticleObjects[i]->Rotate(0, 180, 0);
	}

	//m_pMonsterObject->Animate(fTimeElapsed);

	g_Logic.m_MonsterSession.m_currentPlayGameObject->Animate(fTimeElapsed);
	//auto pos = g_Logic.m_MonsterSession.m_currentPlayGameObject->GetPosition();
	//cout << "GameobjectManager::Boss Position: " << pos.x << ", 0, " << pos.z << endl;
	if (!g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject) return;
	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->UpdateCameraPosition();

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
			if (session.m_currentPlayGameObject->GetRButtonClicked())
				session.m_currentPlayGameObject->RbuttonClicked(fTimeElapsed);
			session.m_currentPlayGameObject->Animate(fTimeElapsed);
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
		cout << "x : " << x << ", y : " << y << ", z : " << z << endl;
		g_Logic.m_MonsterSession.m_currentPlayGameObject->m_xmf3Destination = XMFLOAT3(x, y, z);
	}
	tempcount++;
#endif
	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
		m_ppParticleObjects[i]->AnimateRowColumn(fTimeElapsed);
	}
	CharacterUIAnimate(fTimeElapsed);
	TrailAnimate(fTimeElapsed);
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
	m_pTrailComponent->AddTrail(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponStart->GetPosition(), XMFLOAT3(m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().x,
		m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().y, m_pWarriorObject->m_pLoadedModelComponent->m_pWeaponEnd->GetPosition().z));

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
	//인스턴싱 렌더 
	/*if (m_pInstancingShaderComponent) {
		m_pInstancingShaderComponent->Render(pd3dDevice, pd3dCommandList, 0, pd3dGraphicsRootSignature);
	}*/

	//m_pFireballSpriteObjects[0]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//
	//g_Logic.m_MonsterSession.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	if (m_bDebugMode)
	{
		for (int i = 0; i < 5; i++)
			m_pBoundingBox[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	for (int i = 0; i < m_pArrowObjects.size(); i++) {
		m_pArrowObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
	//m_pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pMonsterObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	//m_pUIGameSearchObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	//for (int i = 0; i < m_ppUIObjects.size(); i++) {
	//	m_ppUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}

	if (m_pMonsterHPBarObject) {
		//m_pMonsterHPBarObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	if (m_pShadowmapShaderComponent)
	{
		m_pShadowmapShaderComponent->Render(pd3dDevice, pd3dCommandList, 0, pd3dGraphicsRootSignature);
	}

	m_pNaviMeshObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//if (m_pTextureToViewportComponent)
	//{
	//	m_pTextureToViewportComponent->Render(pd3dCommandList, m_pCamera, 0, pd3dGraphicsRootSignature);
	//}

	/*for (int i = 0; i < m_ppCharacterUIObjects.size(); i++) {
		m_ppCharacterUIObjects[i]->Render(pd3dDevice, pd3dCommandList, 0, pd3dGraphicsRootSignature);
	}*/
	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
		//m_ppParticleObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);파티클
	}
	if (GameEnd)
	{
		m_pVictoryUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pContinueUIObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	for (int i = 0; i < 6; i++) {
		if(m_pStage1Objects[i])
		m_pStage1Objects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	if (m_pStage1TerrainObject) {
		m_pStage1TerrainObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	TrailRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
		m_pAstarComponent->RenderAstar(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_qrecvNodeQueue);
		m_nodeLock.unlock();
	}
}

void GameobjectManager::UIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	UpdateShaderVariables(pd3dCommandList);
	for (int i = Section[m_nSection]; i < Section[m_nSection + 1]; i++) {
		m_ppUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

}
#define ScreenSTORY 0
void GameobjectManager::CharacterUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	//for (int i = 0; i < m_ppCharacterUIObjects.size(); i++) {
	//	if(m_fStroyTime> m_ppStoryUIObjects.size()* ScreenSTORY)
	//	m_ppCharacterUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
}

void GameobjectManager::StoryUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float ftimeElapsed)
{
	//m_fStroyTime += ftimeElapsed;
	//for (int i = 0; i < m_ppStoryUIObjects.size(); i++) {
	//	if (m_fStroyTime < 10 * (i + 1)&& m_fStroyTime > ScreenSTORY * (i ))
	//	m_ppStoryUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//}
}

void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//빌드
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();

	CLoadedModelInfoCompnent* WarriorModel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Warrior.bin", NULL, true);

	//m_pPlaneObject = new GameObject(UNDEF_ENTITY);
	//m_pPlaneObject->InsertComponent<RenderComponent>();
	//m_pPlaneObject->InsertComponent<CLoadedModelInfoCompnent>();
	//m_pPlaneObject->SetPosition(XMFLOAT3(0, 0, 0));
	//m_pPlaneObject->SetModel("Model/Floor.bin");
	//m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pPlaneObject->SetScale(1.0f, 1.0f, 1.0f);
	//m_pPlaneObject->SetRimLight(false);
	//m_ppGameObjects.emplace_back(m_pPlaneObject);

	//m_pRockObject = new GameObject(UNDEF_ENTITY);
	//m_pRockObject->InsertComponent<RenderComponent>();
	//m_pRockObject->InsertComponent<CLoadedModelInfoCompnent>();
	//m_pRockObject->SetPosition(XMFLOAT3(0, 0, 0));
	//m_pRockObject->SetModel("Model/OutLineRock.bin");
	//m_pRockObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pRockObject->SetScale(1.0f, 1.0f, 1.0f);
	//m_ppGameObjects.emplace_back(m_pRockObject);

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
	m_pWarriorObject = new Warrior();//사각형 오브젝트를 만들겠다
	m_pWarriorObject->InsertComponent<RenderComponent>();
	m_pWarriorObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pWarriorObject->SetPosition(XMFLOAT3(0.f, 0.f, 0.f));
	m_pWarriorObject->SetModel("Model/Warrior.bin");
	m_pWarriorObject->SetAnimationSets(5);
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(5);
	m_pWarriorObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pWarriorObject->SetScale(30.0f);
	m_pWarriorObject->SetBoundingBox(m_pBoundingBox[0]);
	m_ppGameObjects.emplace_back(m_pWarriorObject);

	m_pArcherObject = new Archer();
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pArcherObject->SetPosition(XMFLOAT3(20, 0, 0));
	m_pArcherObject->SetModel("Model/Archer.bin");
	m_pArcherObject->SetAnimationSets(5);
	m_pArcherObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackAnimationSet(5);
	m_pArcherObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pArcherObject->SetScale(30.0f);
	m_pArcherObject->SetBoundingBox(m_pBoundingBox[1]);
	m_ppGameObjects.emplace_back(m_pArcherObject);

	for (int i = 0; i < 10; ++i)
	{
		m_pArrowObjects[i] = new Arrow();
		m_pArrowObjects[i]->InsertComponent<RenderComponent>();
		m_pArrowObjects[i]->InsertComponent<CLoadedModelInfoCompnent>();
		m_pArrowObjects[i]->SetPosition(XMFLOAT3(0, 0, 0));
		m_pArrowObjects[i]->SetModel("Model/Arrow.bin");
		m_pArrowObjects[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pArrowObjects[i]->SetScale(30.0f);
		m_pArrowObjects[i]->SetBoundingSize(0.2f);
		static_cast<Archer*>(m_pArcherObject)->SetArrow(m_pArrowObjects[i]);
	}

	m_pTankerObject = new Tanker();
	m_pTankerObject->InsertComponent<RenderComponent>();
	m_pTankerObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pTankerObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pTankerObject->SetModel("Model/Tanker.bin");
	m_pTankerObject->SetAnimationSets(6);
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(6);
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DEFENCE]->m_nType = ANIMATION_TYPE_HALF;
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_DEFENCE].m_fSpeed = 0.3f;
	m_pTankerObject->SetScale(30.0f);
	m_pTankerObject->SetBoundingBox(m_pBoundingBox[2]);
	m_ppGameObjects.emplace_back(m_pTankerObject);

	m_pPriestObject = new Priest();
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPriestObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPriestObject->SetModel("Model/Priests.bin");
	m_pPriestObject->SetAnimationSets(5);
	m_pPriestObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackAnimationSet(5);
	m_pPriestObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DIE]->m_nType = ANIMATION_TYPE_ONCE;
	m_pPriestObject->SetScale(30.0f);
	m_pPriestObject->SetBoundingBox(m_pBoundingBox[3]);
	m_ppGameObjects.emplace_back(m_pPriestObject);

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

	m_pEnergyBallObject=new EnergyBall();
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
	m_pMonsterObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pMonsterObject->SetModel("Model/Boss.bin");
	m_pMonsterObject->SetAnimationSets(11);
	m_pMonsterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackAnimationSet(11);
	m_pMonsterObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[10]->m_nType = ANIMATION_TYPE_ONCE;
	m_pMonsterObject->SetScale(15.0f);
	m_pMonsterObject->SetBoundingSize(30.0f);
	m_pMonsterObject->SetBoundingBox(m_pBoundingBox[4]);
	m_pMonsterObject->SetMoveState(false);
	m_ppGameObjects.emplace_back(m_pMonsterObject);
	g_Logic.m_MonsterSession.SetGameObject(m_pMonsterObject);

	m_pEnergyBallObject = new GameObject(SQUARE_ENTITY);
	m_pEnergyBallObject->InsertComponent<RenderComponent>();
	m_pEnergyBallObject->InsertComponent<CubeMeshComponent>();
	m_pEnergyBallObject->InsertComponent<SphereShaderComponent>();
	m_pEnergyBallObject->SetPosition(XMFLOAT3(0, 0, 100));
	m_pEnergyBallObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pEnergyBallObject->SetScale(0.5f);
	

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
	m_pSkyboxObject->SetTexture(L"DreamWorld/DreamWorld.dds", RESOURCE_TEXTURE_CUBE, 12);
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
	m_pPlayerObject = new GameObject(UNDEF_ENTITY);	//수정필요
	memcpy(m_pPlayerObject, m_pWarriorObject, sizeof(GameObject));
	m_pPlayerObject->SetCamera(m_pCamera);
	m_pPlayerObject->SetCharacterType(CT_ARCHER);
	//delete m_pArcherObject;->delete하면서 뎊스렌더 문제 발생

	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject = m_pPlayerObject;
	g_Logic.m_inGamePlayerSession[0].m_isVisible = true;
	g_Logic.m_inGamePlayerSession[0].m_id = 0;
#endif // LOCAL_TASK


	BuildShadow(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//무조건 마지막에 해줘야된다.
//	Build2DUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildCharacterUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//BuildParticle(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//BuildInstanceObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildStoryUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildTrail(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildStage1(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildAstar(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
	m_pFireballSpriteObject->SetRowColumn(16, 8, 0.05);
	m_pFireballSpriteObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppParticleObjects.emplace_back(m_pFireballSpriteObject);

	m_pFireballEmissionSpriteObject = new GameObject(UNDEF_ENTITY);
	m_pFireballEmissionSpriteObject->InsertComponent<RenderComponent>();
	m_pFireballEmissionSpriteObject->InsertComponent<UIMeshComponent>();
	m_pFireballEmissionSpriteObject->InsertComponent<MultiSpriteShaderComponent>();
	m_pFireballEmissionSpriteObject->InsertComponent<TextureComponent>();
	m_pFireballEmissionSpriteObject->SetTexture(L"MagicEffect/FireballEmission_7x7.dds", RESOURCE_TEXTURE2D, 3);
	m_pFireballEmissionSpriteObject->SetPosition(XMFLOAT3(0, 40, 100));
	m_pFireballEmissionSpriteObject->SetScale(10);
	m_pFireballEmissionSpriteObject->SetRowColumn(7, 7, 0.05);
	m_pFireballEmissionSpriteObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppParticleObjects.emplace_back(m_pFireballEmissionSpriteObject);

	m_pFireballSpriteObjects.resize(20);
	m_pFireballSpriteObjects[0] = new GameObject(UNDEF_ENTITY);
	m_pFireballSpriteObjects[0]->InsertComponent<RenderComponent>();
	m_pFireballSpriteObjects[0]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pFireballSpriteObjects[0]->SetPosition(XMFLOAT3(100, 0, 0));
	m_pFireballSpriteObjects[0]->SetModel("Model/RockSpike.bin");
	m_pFireballSpriteObjects[0]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pFireballSpriteObjects[0]->SetScale(30.0f, 30.0f, 30.0f);
	for (int i = 1; i < 20; i++) {
		m_pFireballSpriteObjects[i] = new GameObject(UNDEF_ENTITY);
		m_pFireballSpriteObjects[i]->SetPosition(XMFLOAT3(0, i * 10, 0));
		m_pFireballSpriteObjects[i]->SetScale(30.0f, 30.0f, 30.0f);
		m_ppParticleObjects.emplace_back(m_pFireballSpriteObjects[i]);
	}
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
	/*m_pStage1Objects[0] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[0]->InsertComponent<RenderComponent>();
	m_pStage1Objects[0]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[0]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[0]->SetModel("Model/New_Terrain.bin");
	m_pStage1Objects[0]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[0]->SetScale(1.0f, 1.0f, 1.0f);
	m_pStage1Objects[0]->SetRimLight(false);
	m_pStage1Objects[1] = new GameObject(UNDEF_ENTITY);
	m_pStage1Objects[1]->InsertComponent<RenderComponent>();
	m_pStage1Objects[1]->InsertComponent<CLoadedModelInfoCompnent>();
	m_pStage1Objects[1]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pStage1Objects[1]->SetModel("Model/Fence.bin");
	m_pStage1Objects[1]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStage1Objects[1]->SetScale(1.0f, 1.0f, 1.0f);
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
	m_pUIGameSearchObject->SetTexture(L"UI/SearchingRoom.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameSearchObject->SetPosition(XMFLOAT3(0.25, 0.5, 1.00));
	m_pUIGameSearchObject->SetScale(0.05, 0.025, 1);
	m_pUIGameSearchObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameSearchObject);

	m_pUIGameMathchingObject = new GameObject(UI_ENTITY);
	m_pUIGameMathchingObject->InsertComponent<RenderComponent>();
	m_pUIGameMathchingObject->InsertComponent<UIMeshComponent>();
	m_pUIGameMathchingObject->InsertComponent<UiShaderComponent>();
	m_pUIGameMathchingObject->InsertComponent<TextureComponent>();
	m_pUIGameMathchingObject->SetTexture(L"UI/Matching.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameMathchingObject->SetPosition(XMFLOAT3(-0.25, 0.5, 1.00));
	m_pUIGameMathchingObject->SetScale(0.05, 0.02, 1);
	m_pUIGameMathchingObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameMathchingObject);

	m_pUIWarriorCharacterObject = new GameObject(UI_ENTITY);
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
	m_ppUIObjects.emplace_back(m_pUIPriestCharacterObject);

	//SECTION2 
	m_pUIEnterRoomObject = new GameObject(UI_ENTITY);
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
	m_ppUIObjects.emplace_back(m_pUIGameCreateObject);

	//m_pPlayerObject->SetCamera(m_pCamera);
}

void GameobjectManager::BuildCharacterUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
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
	m_pMonsterHPBarObject->InsertComponent<ShaderComponent>();
	m_pMonsterHPBarObject->InsertComponent<TextureComponent>();
	m_pMonsterHPBarObject->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
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
	m_pTankerObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, 0.08, 1.00));
	m_pTankerObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pTankerObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pTankerObject->m_pProfileUI);

	m_pTankerObject->m_pSkillUI = new GameObject(UI_ENTITY);
	m_pTankerObject->m_pSkillUI->InsertComponent<RenderComponent>();
	m_pTankerObject->m_pSkillUI->InsertComponent<UIMeshComponent>();
	m_pTankerObject->m_pSkillUI->InsertComponent<UiShaderComponent>();
	m_pTankerObject->m_pSkillUI->InsertComponent<TextureComponent>();
	m_pTankerObject->m_pSkillUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->m_pSkillUI->SetPosition(XMFLOAT3(0.25, 0.5, 1.00));
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
	m_pPriestObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, -0.06, 1.00));
	m_pPriestObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pPriestObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pHPBarUI);

	m_pPriestObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pProfileUI->SetTexture(L"UI/Priest.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, -0.04, 1.00));
	m_pPriestObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pPriestObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pPriestObject->m_pProfileUI);

	m_pPriestObject->m_pSkillUI = new GameObject(UI_ENTITY);
	m_pPriestObject->m_pSkillUI->InsertComponent<RenderComponent>();
	m_pPriestObject->m_pSkillUI->InsertComponent<UIMeshComponent>();
	m_pPriestObject->m_pSkillUI->InsertComponent<UiShaderComponent>();
	m_pPriestObject->m_pSkillUI->InsertComponent<TextureComponent>();
	m_pPriestObject->m_pSkillUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->m_pSkillUI->SetPosition(XMFLOAT3(0.25, 0.5, 1.00));
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
	m_pWarriorObject->m_pHPBarUI->SetPosition(XMFLOAT3(-0.67, -0.18, 1.00));
	m_pWarriorObject->m_pHPBarUI->SetScale(0.07, 0.005, 1);
	m_pWarriorObject->m_pHPBarUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pHPBarUI);

	m_pWarriorObject->m_pProfileUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pProfileUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pProfileUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pProfileUI->SetTexture(L"UI/Warrior.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pProfileUI->SetPosition(XMFLOAT3(-0.94, -0.16, 1.00));
	m_pWarriorObject->m_pProfileUI->SetScale(0.03, 0.015, 1);
	m_pWarriorObject->m_pProfileUI->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pWarriorObject->m_pProfileUI);

	m_pWarriorObject->m_pSkillUI = new GameObject(UI_ENTITY);
	m_pWarriorObject->m_pSkillUI->InsertComponent<RenderComponent>();
	m_pWarriorObject->m_pSkillUI->InsertComponent<UIMeshComponent>();
	m_pWarriorObject->m_pSkillUI->InsertComponent<UiShaderComponent>();
	m_pWarriorObject->m_pSkillUI->InsertComponent<TextureComponent>();
	m_pWarriorObject->m_pSkillUI->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->m_pSkillUI->SetPosition(XMFLOAT3(0.25, 0.5, 1.00));
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
		cout << "request Room List" << endl;
		g_NetworkHelper.SendRequestRoomList();
		break;
	}
	case UI::UI_GAMEMATCHING:
	{
		if (!m_bInMatching)
		{
			cout << "StartMatching" << endl;
			g_NetworkHelper.SendMatchRequestPacket();
			m_bInMatching = true;
		
			break;
		}
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
	if (nMessageID == WM_KEYDOWN && wParam == VK_F12)
	{
		m_bDebugMode = !m_bDebugMode;
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
			break;
		}
		case 'E':
		{
			//g_Logic.m_KeyInput->m_bEKey = true;
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
			g_NetworkHelper.SendTestGameEndPacket();
			break;
		}
		case 'E':
		{
			g_Logic.m_KeyInput->m_bEKey = false;
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
			for(int i = 0; i < 4; ++i)
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

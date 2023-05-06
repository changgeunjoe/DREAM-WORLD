#include "stdafx.h"
#include "GameobjectManager.h"
#include "Animation.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"
#include "DepthRenderShaderComponent.h"
#include "TextureToViewportComponent.h"
#include "UiShaderComponent.h"
#include"MultiSpriteShaderComponent.h"
#include "Character.h"
#include"InstancingShaderComponent.h"


extern NetworkHelper g_NetworkHelper;
extern Logic g_Logic;


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
	m_fTime = fTimeElapsed;
	m_pSkyboxObject->SetPosition(m_pCamera->GetPosition());
	if (m_pMonsterHPBarObject)//23.04.18 몬스터 체력바 -> 카메라를 바라 보도록 .ccg
	{
		m_pMonsterHPBarObject->SetLookAt(m_pCamera->GetPosition());
		m_pMonsterHPBarObject->SetPosition(XMFLOAT3(m_pMonsterObject->GetPosition().x,
			m_pMonsterObject->GetPosition().y + 70, m_pMonsterObject->GetPosition().z));
		m_pMonsterHPBarObject->Rotate(0, 180, 0);
		m_pMonsterHPBarObject->SetScale(10,1,1);
		m_pMonsterHPBarObject->SetCurrentHP(g_Logic.m_MonsterSession.m_currentPlayGameObject->GetCurrentHP());
	}
	XMFLOAT3 mfHittmp= g_Logic.m_MonsterSession.m_currentPlayGameObject->m_xmfHitPosition;
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

	m_pDepthShaderComponent->UpdateShaderVariables(pd3dCommandList);
	//인스턴싱 렌더 
	//m_pInstancingShaderComponent->Render(pd3dDevice, pd3dCommandList,0, pd3dGraphicsRootSignature);

	//
	g_Logic.m_MonsterSession.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	for (int i = 0; i < 5; i++) 
		m_pBoundingBox[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	for (int i = 0; i < m_pArrowObjects.size(); i++) {
		m_pArrowObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
	for (int i = 0; i < m_pEnergyBallObjects.size(); i++) {
		m_pEnergyBallObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
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
		m_pMonsterHPBarObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

	if (m_pShadowmapShaderComponent)
	{
		m_pShadowmapShaderComponent->Render(pd3dDevice, pd3dCommandList, 0, pd3dGraphicsRootSignature);
	}

	//if (m_pTextureToViewportComponent)
	//{
	//	m_pTextureToViewportComponent->Render(pd3dCommandList, m_pCamera, 0, pd3dGraphicsRootSignature);
	//}

	/*for (int i = 0; i < m_ppCharacterUIObjects.size(); i++) {
		m_ppCharacterUIObjects[i]->Render(pd3dDevice, pd3dCommandList, 0, pd3dGraphicsRootSignature);
	}*/
	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
		m_ppParticleObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
}

void GameobjectManager::UIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	UpdateShaderVariables(pd3dCommandList);
	for (int i = Section[m_nSection]; i < Section[m_nSection + 1]; i++) {
		m_ppUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

}

void GameobjectManager::CharacterUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	for (int i = 0; i < m_ppCharacterUIObjects.size(); i++) {
		m_ppCharacterUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
}

void GameobjectManager::StoryUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	for (int i = 0; i < m_ppStoryUIObjects.size(); i++) {
		m_ppStoryUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
}

void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{//빌드
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();

	m_pPlaneObject = new GameObject(UNDEF_ENTITY);
	m_pPlaneObject->InsertComponent<RenderComponent>();
	m_pPlaneObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPlaneObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPlaneObject->SetModel("Model/Floor.bin");
	m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlaneObject->SetScale(30.0f, 30.0f, 30.0f);
	m_pPlaneObject->SetRimLight(false);
	m_ppGameObjects.emplace_back(m_pPlaneObject);

	m_pRockObject = new GameObject(UNDEF_ENTITY);
	m_pRockObject->InsertComponent<RenderComponent>();
	m_pRockObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pRockObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pRockObject->SetModel("Model/OutLineRock.bin");
	m_pRockObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pRockObject->SetScale(30.0f, 30.0f, 30.0f);
	m_ppGameObjects.emplace_back(m_pRockObject);

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
	m_pWarriorObject->SetAnimationSets(6);
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(6);
	m_pWarriorObject->SetScale(30.0f);
	m_pWarriorObject->SetBoundingBox(m_pBoundingBox[0]);
	m_ppGameObjects.emplace_back(m_pWarriorObject);

	m_pArcherObject = new Archer();
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pArcherObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pArcherObject->SetModel("Model/Archer.bin");
	m_pArcherObject->SetAnimationSets(6);
	m_pArcherObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackAnimationSet(6);
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
	m_pTankerObject->SetAnimationSets(7);
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(7);
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_DEFENCE]->m_nType = ANIMATION_TYPE_HALF;
	m_pTankerObject->m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_DEFENCE].m_fSpeed = 0.3f;
	m_pTankerObject->SetScale(30.0f);
	m_pTankerObject->SetBoundingBox(m_pBoundingBox[2]);
	m_ppGameObjects.emplace_back(m_pTankerObject);

	m_pPriestObject = new Priest();
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPriestObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPriestObject->SetModel("Model/Priests.bin");
	m_pPriestObject->SetAnimationSets(4);
	m_pPriestObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackAnimationSet(4);
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
	m_pMonsterObject->SetAnimationSets(10);
	m_pMonsterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackAnimationSet(10);
	m_pMonsterObject->SetScale(15.0f);
	m_pMonsterObject->SetBoundingSize(30.0f);
	m_pMonsterObject->SetBoundingBox(m_pBoundingBox[4]);
	m_pMonsterObject->SetMoveState(false);
	m_ppGameObjects.emplace_back(m_pMonsterObject);
	g_Logic.m_MonsterSession.SetGameObject(m_pMonsterObject);

	m_pSkyboxObject = new GameObject(SQUARE_ENTITY);
	m_pSkyboxObject->InsertComponent<RenderComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxMeshComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxShaderComponent>();
	m_pSkyboxObject->InsertComponent<TextureComponent>();
	m_pSkyboxObject->SetTexture(L"DreamWorld/DreamWorld.dds", RESOURCE_TEXTURE_CUBE, 12);
	m_pSkyboxObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pSkyboxObject->SetScale(1, 1, 1);
	m_pSkyboxObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

#if LOCAL_TASK
	// 플레이어가 캐릭터 선택하는 부분에 유사하게 넣을 예정
	m_pPlayerObject = new GameObject(UNDEF_ENTITY);	//수정필요
	memcpy(m_pPlayerObject, m_pPriestObject, sizeof(GameObject));
	m_pPlayerObject->SetCamera(m_pCamera);
	m_pPlayerObject->SetCharacterType(CT_ARCHER);
	//delete m_pArcherObject;->delete하면서 뎊스렌더 문제 발생

	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject = m_pPlayerObject;
	g_Logic.m_inGamePlayerSession[0].m_isVisible = true;
	g_Logic.m_inGamePlayerSession[0].m_id = 0;
#endif // LOCAL_TASK


	BuildShadow(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//무조건 마지막에 해줘야된다.
	Build2DUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildCharacterUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildParticle(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildInstanceObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildStoryUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
	m_pFireballSpriteObject->SetRowColumn(16,8, 0.05);
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
	m_pFireballSpriteObjects[0]->SetPosition(XMFLOAT3(0, 0, 0));
	m_pFireballSpriteObjects[0]->SetModel("Model/RockSpike.bin");
	m_pFireballSpriteObjects[0]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pFireballSpriteObjects[0]->SetScale(30.0f, 30.0f, 30.0f);
	for (int i = 1; i < 20; i++) {
		m_pFireballSpriteObjects[i] = new GameObject(UNDEF_ENTITY);
		m_pFireballSpriteObjects[i]->SetPosition(XMFLOAT3(0, i*10, 0));
		m_pFireballSpriteObjects[i]->SetScale(30.0f, 30.0f, 30.0f);
		m_ppParticleObjects.emplace_back(m_pFireballSpriteObjects[i]);
	}
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
	m_pUIGameSearchObject->SetPosition(XMFLOAT3(0.25, 0.5, 1.03));
	m_pUIGameSearchObject->SetScale(0.05, 0.025, 1);
	m_pUIGameSearchObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameSearchObject);

	m_pUIGameMathchingObject = new GameObject(UI_ENTITY);
	m_pUIGameMathchingObject->InsertComponent<RenderComponent>();
	m_pUIGameMathchingObject->InsertComponent<UIMeshComponent>();
	m_pUIGameMathchingObject->InsertComponent<UiShaderComponent>();
	m_pUIGameMathchingObject->InsertComponent<TextureComponent>();
	m_pUIGameMathchingObject->SetTexture(L"UI/Matching.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameMathchingObject->SetPosition(XMFLOAT3(-0.25, 0.5, 1.03));
	m_pUIGameMathchingObject->SetScale(0.05, 0.02, 1);
	m_pUIGameMathchingObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameMathchingObject);

	m_pUIWarriorCharacterObject = new GameObject(UI_ENTITY);
	m_pUIWarriorCharacterObject->InsertComponent<RenderComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIWarriorCharacterObject->InsertComponent<TextureComponent>();
	m_pUIWarriorCharacterObject->SetTexture(L"UI/Warrior.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIWarriorCharacterObject->SetPosition(XMFLOAT3(-0.3, 0.2, 1.03));
	m_pUIWarriorCharacterObject->SetScale(0.08, 0.08, 1);
	m_pUIWarriorCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIWarriorCharacterObject);

	m_pUIArcherCharacterObject = new GameObject(UI_ENTITY);
	m_pUIArcherCharacterObject->InsertComponent<RenderComponent>();
	m_pUIArcherCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIArcherCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIArcherCharacterObject->InsertComponent<TextureComponent>();
	m_pUIArcherCharacterObject->SetTexture(L"UI/Archer.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIArcherCharacterObject->SetPosition(XMFLOAT3(0.3, 0.2, 1.03));
	m_pUIArcherCharacterObject->SetScale(0.08, 0.08, 1);
	m_pUIArcherCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIArcherCharacterObject);

	m_pUITankerCharacterObject = new GameObject(UI_ENTITY);
	m_pUITankerCharacterObject->InsertComponent<RenderComponent>();
	m_pUITankerCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUITankerCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUITankerCharacterObject->InsertComponent<TextureComponent>();
	m_pUITankerCharacterObject->SetTexture(L"UI/Tanker.dds", RESOURCE_TEXTURE2D, 3);
	m_pUITankerCharacterObject->SetPosition(XMFLOAT3(-0.3, -0.2, 1.03));
	m_pUITankerCharacterObject->SetScale(0.08, 0.08, 1);
	m_pUITankerCharacterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUITankerCharacterObject);

	m_pUIPriestCharacterObject = new GameObject(UI_ENTITY);
	m_pUIPriestCharacterObject->InsertComponent<RenderComponent>();
	m_pUIPriestCharacterObject->InsertComponent<UIMeshComponent>();
	m_pUIPriestCharacterObject->InsertComponent<UiShaderComponent>();
	m_pUIPriestCharacterObject->InsertComponent<TextureComponent>();
	m_pUIPriestCharacterObject->SetTexture(L"UI/Priest.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIPriestCharacterObject->SetPosition(XMFLOAT3(0.3, -0.2, 1.03));
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
	m_pUIEnterRoomObject->SetPosition(XMFLOAT3(0.15, -0.5, 1.03));
	m_pUIEnterRoomObject->SetScale(0.05, 0.02, 1);
	m_pUIEnterRoomObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIEnterRoomObject);

	m_pUIGameCreateObject = new GameObject(UI_ENTITY);
	m_pUIGameCreateObject->InsertComponent<RenderComponent>();
	m_pUIGameCreateObject->InsertComponent<UIMeshComponent>();
	m_pUIGameCreateObject->InsertComponent<UiShaderComponent>();
	m_pUIGameCreateObject->InsertComponent<TextureComponent>();
	m_pUIGameCreateObject->SetTexture(L"UI/CreateRoom.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameCreateObject->SetPosition(XMFLOAT3(0.45, -0.5, 1.03));
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
	m_pArcherHPBarObject = new GameObject(UI_ENTITY);
	m_pArcherHPBarObject->InsertComponent<RenderComponent>();
	m_pArcherHPBarObject->InsertComponent<UIMeshComponent>();
	m_pArcherHPBarObject->InsertComponent<UiShaderComponent>();
	m_pArcherHPBarObject->InsertComponent<TextureComponent>();
	m_pArcherHPBarObject->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherHPBarObject->SetPosition(XMFLOAT3(0.09, -0.53, 1.03));
	m_pArcherHPBarObject->SetScale(0.13, 0.010, 1);
	m_pArcherHPBarObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherHPBarObject);

	m_pArcherProfileObject = new GameObject(UI_ENTITY);
	m_pArcherProfileObject->InsertComponent<RenderComponent>();
	m_pArcherProfileObject->InsertComponent<UIMeshComponent>();
	m_pArcherProfileObject->InsertComponent<UiShaderComponent>();
	m_pArcherProfileObject->InsertComponent<TextureComponent>();
	m_pArcherProfileObject->SetTexture(L"UI/Archer.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherProfileObject->SetPosition(XMFLOAT3(-0.4, -0.5, 1.03));
	m_pArcherProfileObject->SetScale(0.05, 0.025, 1);
	m_pArcherProfileObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherProfileObject);

	m_pArcherSkillBarObject = new GameObject(UI_ENTITY);
	m_pArcherSkillBarObject->InsertComponent<RenderComponent>();
	m_pArcherSkillBarObject->InsertComponent<UIMeshComponent>();
	m_pArcherSkillBarObject->InsertComponent<UiShaderComponent>();
	m_pArcherSkillBarObject->InsertComponent<TextureComponent>();
	m_pArcherSkillBarObject->SetTexture(L"UI/HP.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherSkillBarObject->SetPosition(XMFLOAT3(0.25, 0.5, 1.03));
	m_pArcherSkillBarObject->SetScale(0.05, 0.025, 1);
	m_pArcherSkillBarObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppCharacterUIObjects.emplace_back(m_pArcherSkillBarObject);
////////////////////////////////////////////////////////////////
}

void GameobjectManager::BuildInstanceObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
//	m_pInstancingShaderComponent = new InstancingShaderComponent;
//	m_pInstancingShaderComponent->BuildObject(pd3dDevice, pd3dCommandList, m_pFireballSpriteObjects);
}

void GameobjectManager::BuildStoryUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pStroy1Object = new GameObject(UI_ENTITY);
	m_pStroy1Object->InsertComponent<RenderComponent>();
	m_pStroy1Object->InsertComponent<UIMeshComponent>();
	m_pStroy1Object->InsertComponent<UiShaderComponent>();
	m_pStroy1Object->InsertComponent<TextureComponent>();
	m_pStroy1Object->SetTexture(L"UI/Story.dds", RESOURCE_TEXTURE2D, 3);
	m_pStroy1Object->SetPosition(XMFLOAT3(0.0, 0.0, 1.03));
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
		cout << "StartMatching" << endl;
		break;
	}
	case UI::UI_WARRIORCHARACTER:
	{
		// 선택한 캐릭터 Warrior
		g_NetworkHelper.SendMatchRequestPacket(ROLE::WARRIOR);
		cout << "Choose Warrior Character" << endl;
		break;
	}
	case UI::UI_ARCHERCHARACTER:
	{
		// 선택한 캐릭터 Archer
		g_NetworkHelper.SendMatchRequestPacket(ROLE::ARCHER);
		cout << "Choose Archer Character" << endl;
		break;
	}
	case UI::UI_TANKERCHARACTER:
	{
		// 선택한 캐릭터 tanker
		g_NetworkHelper.SendMatchRequestPacket(ROLE::TANKER);
		cout << "Choose Tanker Character" << endl;
		break;
	}
	case UI::UI_PRIESTCHARACTER:
	{
		// 선택한 캐릭터 priest
		g_NetworkHelper.SendMatchRequestPacket(ROLE::PRIEST);
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
			g_Logic.m_KeyInput->m_bQKey = false;
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
			g_NetworkHelper.SendMatchRequestPacket(ROLE::RAND);
		}
		break;
		case 'z':
		case 'Z': //탱커
		{
			g_NetworkHelper.SendMatchRequestPacket(ROLE::TANKER);
		}
		break;
		case 'x':
		case 'X': // 전사
		{
			g_NetworkHelper.SendMatchRequestPacket(ROLE::WARRIOR);
		}
		break;
		case 'c':
		case 'C': //프리스트
		{
			g_NetworkHelper.SendMatchRequestPacket(ROLE::PRIEST);
		}
		break;
		case 'v':
		case 'V': // 궁수
		{
			g_NetworkHelper.SendMatchRequestPacket(ROLE::ARCHER);
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
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(200, 0, 40));
	}
	break;
	case ROLE::PRIEST:
	{
		cliSession->SetGameObject(m_pPriestObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(270, 0, 80));
	}
	break;
	case ROLE::TANKER:
	{
		cliSession->SetGameObject(m_pTankerObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(230, 0, 60));
	}
	break;
	case ROLE::WARRIOR:
	{
		cliSession->SetGameObject(m_pWarriorObject);
		cliSession->m_currentPlayGameObject->SetPosition(XMFLOAT3(300, 0, 100));
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
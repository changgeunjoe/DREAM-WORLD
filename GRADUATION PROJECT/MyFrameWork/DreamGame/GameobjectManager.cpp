#include "stdafx.h"
#include "GameobjectManager.h"
#include "Animation.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"
#include "DepthRenderShaderComponent.h"
#include "TextureToViewportComponent.h"
#include "UiShaderComponent.h"

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
	//m_pMonsterObject->Animate(fTimeElapsed);
	AnimateObjects();
	if (g_Logic.m_KeyInput->m_bQKey)
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetUp(), -30.0f * fTimeElapsed);
		g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y -= 30.0f * fTimeElapsed;
		g_NetworkHelper.SendRotatePacket(ROTATE_AXIS::Y, -30.0f * fTimeElapsed);
	}
	if (g_Logic.m_KeyInput->m_bEKey)
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetUp(), 30.0f * fTimeElapsed);
		g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 30.0f * fTimeElapsed;
		g_NetworkHelper.SendRotatePacket(ROTATE_AXIS::Y, 30.0f * fTimeElapsed);
	}

	for (auto& session : g_Logic.m_inGamePlayerSession) {
		if (-1 != session.m_id && session.m_isVisible) {
			if (DIRECTION::FRONT == (session.m_currentDirection & DIRECTION::FRONT)) {
				if (DIRECTION::LEFT == (session.m_currentDirection & DIRECTION::LEFT)) {
					session.m_currentPlayGameObject->MoveDiagonal(1, -1, 50.0f * fTimeElapsed);
				}
				else if (DIRECTION::RIGHT == (session.m_currentDirection & DIRECTION::RIGHT)) {
					session.m_currentPlayGameObject->MoveDiagonal(1, 1, 50.0f * fTimeElapsed);
				}
				else {
					session.m_currentPlayGameObject->MoveForward(50.0f * fTimeElapsed);
				}
#ifdef _DEBUG
				auto look = session.m_currentPlayGameObject->GetLook();
				auto up = session.m_currentPlayGameObject->GetUp();
				auto right = session.m_currentPlayGameObject->GetRight();
				cout << "GameobjectManager::Animate() SessionId: " << session.m_id << endl;
				cout << "GameobjectManager::Animate() Look: " << look.x << ", " << look.y << ", " << look.z << endl;
				cout << "GameobjectManager::Animate() up: " << up.x << ", " << up.y << ", " << up.z << endl;
				cout << "GameobjectManager::Animate() right: " << right.x << ", " << right.y << ", " << right.z << endl;
				std::cout << "GameobjectManager::Animate() rotation angle: " << session.m_rotateAngle.x << ", " << session.m_rotateAngle.y << ", " << session.m_rotateAngle.z << std::endl;
#endif
			}
			else if (DIRECTION::BACK == (session.m_currentDirection & DIRECTION::BACK)) {
				if (DIRECTION::LEFT == (session.m_currentDirection & DIRECTION::LEFT)) {
					session.m_currentPlayGameObject->MoveDiagonal(-1, -1, 50.0f * fTimeElapsed);
				}
				else if (DIRECTION::RIGHT == (session.m_currentDirection & DIRECTION::RIGHT)) {
					session.m_currentPlayGameObject->MoveDiagonal(-1, 1, 50.0f * fTimeElapsed);
				}
				else {
					session.m_currentPlayGameObject->MoveForward(-50.0f * fTimeElapsed);
				}
#ifdef _DEBUG
				auto look = session.m_currentPlayGameObject->GetLook();
				auto up = session.m_currentPlayGameObject->GetUp();
				auto right = session.m_currentPlayGameObject->GetRight();
				cout << "GameobjectManager::Animate() SessionId: " << session.m_id << endl;
				cout << "GameobjectManager::Animate() Look: " << look.x << ", " << look.y << ", " << look.z << endl;
				cout << "GameobjectManager::Animate() up: " << up.x << ", " << up.y << ", " << up.z << endl;
				cout << "GameobjectManager::Animate() right: " << right.x << ", " << right.y << ", " << right.z << endl;
				std::cout << "GameobjectManager::Animate() rotation angle: " << session.m_rotateAngle.x << ", " << session.m_rotateAngle.y << ", " << session.m_rotateAngle.z << std::endl;
#endif
			}
			else {
				if (DIRECTION::LEFT == (session.m_currentDirection & DIRECTION::LEFT)) {
					session.m_currentPlayGameObject->MoveStrafe(-50.0f * fTimeElapsed);
#ifdef _DEBUG
					auto look = session.m_currentPlayGameObject->GetLook();
					auto up = session.m_currentPlayGameObject->GetUp();
					auto right = session.m_currentPlayGameObject->GetRight();
					cout << "GameobjectManager::Animate() SessionId: " << session.m_id << endl;
					cout << "GameobjectManager::Animate() Look: " << look.x << ", " << look.y << ", " << look.z << endl;
					cout << "GameobjectManager::Animate() up: " << up.x << ", " << up.y << ", " << up.z << endl;
					cout << "GameobjectManager::Animate() right: " << right.x << ", " << right.y << ", " << right.z << endl;
					std::cout << "GameobjectManager::Animate() rotation angle: " << session.m_rotateAngle.x << ", " << session.m_rotateAngle.y << ", " << session.m_rotateAngle.z << std::endl;
#endif
				}
				else if (DIRECTION::RIGHT == (session.m_currentDirection & DIRECTION::RIGHT)) {
					session.m_currentPlayGameObject->MoveStrafe(50.0f * fTimeElapsed);
#ifdef _DEBUG
					auto look = session.m_currentPlayGameObject->GetLook();
					auto up = session.m_currentPlayGameObject->GetUp();
					auto right = session.m_currentPlayGameObject->GetRight();
					cout << "GameobjectManager::Animate() SessionId: " << session.m_id << endl;
					cout << "GameobjectManager::Animate() Look: " << look.x << ", " << look.y << ", " << look.z << endl;
					cout << "GameobjectManager::Animate() up: " << up.x << ", " << up.y << ", " << up.z << endl;
					cout << "GameobjectManager::Animate() right: " << right.x << ", " << right.y << ", " << right.z << endl;
					std::cout << "GameobjectManager::Animate() rotation angle: " << session.m_rotateAngle.x << ", " << session.m_rotateAngle.y << ", " << session.m_rotateAngle.z << std::endl;
#endif
				}
			}
			session.m_currentPlayGameObject->Animate(fTimeElapsed);
		}
	}
}

void GameobjectManager::OnPreRender( ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList ,ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pDepthShaderComponent->PrepareShadowMap(pd3dDevice,pd3dCommandList);
	//Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}


void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	

	UpdateShaderVariables(pd3dCommandList);
//	m_pSkyboxObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pDepthShaderComponent->UpdateShaderVariables(pd3dCommandList);

	//for (auto& session : g_Logic.m_inGamePlayerSession) {
	//	if (-1 != session.m_id && session.m_isVisible) {
	//		session.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//	}
	//}
	//m_pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pMonsterObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pUIGameSearchObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pShadowmapShaderComponent)
	{
		m_pShadowmapShaderComponent->Render(pd3dDevice,pd3dCommandList, 0,pd3dGraphicsRootSignature);
	}

	//if (m_pTextureToViewportComponent)
	//{
	//	m_pTextureToViewportComponent->Render(pd3dCommandList, m_pCamera, 0, pd3dGraphicsRootSignature);
	//}
}

void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();
	
	m_pPlaneObject = new GameObject(UNDEF_ENTITY);
	m_pPlaneObject->InsertComponent<RenderComponent>();
	m_pPlaneObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPlaneObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPlaneObject->SetModel("Model/Floor.bin");
	m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlaneObject->SetScale(10.0f, 1.0f, 10.0f);
	m_ppGameObjects.emplace_back(m_pPlaneObject);

	m_pWarriorObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pWarriorObject->InsertComponent<RenderComponent>();
	m_pWarriorObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pWarriorObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pWarriorObject->SetModel("Model/Warrior.bin");
	m_pWarriorObject->SetAnimationSets(3);
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackEnable(2, true);
	m_pWarriorObject->m_pSkinnedAnimationController->SetRootMotion(false);
	m_pWarriorObject->SetScale(30.0f, 30.0f, 30.0f);
	m_ppGameObjects.emplace_back( m_pWarriorObject);
	//m_ppGameObjects[m_nObjects++]= m_pWarriorObject;

	m_pArcherObject = new GameObject(UNDEF_ENTITY);
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pArcherObject->SetPosition(XMFLOAT3(-20, 0, 0));
	m_pArcherObject->SetModel("Model/Archer.bin");
	m_pArcherObject->SetAnimationSets(3);
	m_pArcherObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackEnable(2, true);
	m_pArcherObject->m_pSkinnedAnimationController->SetRootMotion(false);
	m_pArcherObject->SetScale(30.0f, 30.0f, 30.0f);
	m_ppGameObjects.emplace_back(m_pArcherObject);
	//m_ppGameObjects[m_nObjects++] = m_pArcherObject;

	m_pTankerObject = new GameObject(UNDEF_ENTITY);
	m_pTankerObject->InsertComponent<RenderComponent>();
	m_pTankerObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pTankerObject->SetPosition(XMFLOAT3(20, 0, 0));
	m_pTankerObject->SetModel("Model/Tanker.bin");
	m_pTankerObject->SetAnimationSets(3);
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackEnable(2, true);
	m_pTankerObject->m_pSkinnedAnimationController->SetRootMotion(false);
	m_pTankerObject->SetScale(30.0f, 30.0f, 30.0f);
	m_ppGameObjects.emplace_back(m_pTankerObject);
	//m_ppGameObjects[m_nObjects++] = m_pTankerObject;

	m_pPriestObject = new GameObject(UNDEF_ENTITY);
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPriestObject->SetPosition(XMFLOAT3(40, 0, 0));
	m_pPriestObject->SetModel("Model/Priests.bin");
	m_pPriestObject->SetAnimationSets(3);
	m_pPriestObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackEnable(2, true);
	m_pPriestObject->m_pSkinnedAnimationController->SetRootMotion(false);
	m_pPriestObject->SetScale(30.0f, 30.0f, 30.0f);
	m_ppGameObjects.emplace_back(m_pPriestObject);

	m_pMonsterObject = new GameObject(UNDEF_ENTITY);
	m_pMonsterObject->InsertComponent<RenderComponent>();
	m_pMonsterObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pMonsterObject->SetPosition(XMFLOAT3(0, 0, 100));
	m_pMonsterObject->SetModel("Model/Boss.bin");
	m_pMonsterObject->SetAnimationSets(3);
	m_pMonsterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackEnable(2, true);
	m_pMonsterObject->m_pSkinnedAnimationController->SetRootMotion(false);
	m_pMonsterObject->SetScale(10.0f, 10.0f, 10.0f);
	m_ppGameObjects.emplace_back(m_pMonsterObject);

	//m_pPlaneObject = new GameObject(SQUARE_ENTITY);
	//m_pPlaneObject->InsertComponent<RenderComponent>();
	//m_pPlaneObject->InsertComponent<SkyBoxMeshComponent>();
	//m_pPlaneObject->InsertComponent<SkyBoxShaderComponent>();
	//m_pPlaneObject->InsertComponent<TextureComponent>();
	//m_pPlaneObject->SetTexture(L"DreamWorld/DreamWorld.dds", RESOURCE_TEXTURE_CUBE, 12);
	//m_pPlaneObject->SetPosition(XMFLOAT3(0, 0, 0));
	//m_pPlaneObject->SetScale(1, 1, 1);
	//m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_ppGameObjects.emplace_back(m_pPlaneObject);

	m_pSkyboxObject = new GameObject(SQUARE_ENTITY);
	m_pSkyboxObject->InsertComponent<RenderComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxMeshComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxShaderComponent>();
	m_pSkyboxObject->InsertComponent<TextureComponent>();
	m_pSkyboxObject->SetTexture(L"DreamWorld/DreamWorld.dds", RESOURCE_TEXTURE_CUBE, 12);
	m_pSkyboxObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pSkyboxObject->SetScale(1, 1, 1);
	m_pSkyboxObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
//	m_ppGameObjects.emplace_back(m_pSkyboxObject);

	m_pUIGameSearchObject = new GameObject(UI_ENTITY);
	m_pUIGameSearchObject->InsertComponent<RenderComponent>();
	m_pUIGameSearchObject->InsertComponent<UIMeshComponent>();
	m_pUIGameSearchObject->InsertComponent<UiShaderComponent>();
	m_pUIGameSearchObject->InsertComponent<TextureComponent>();
	m_pUIGameSearchObject->SetTexture(L"Image/GameSearching.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameSearchObject->SetPosition(XMFLOAT3(0.2, 0.8, 1));
	m_pUIGameSearchObject->SetScale(0.1, 0.03, 1);
	m_pUIGameSearchObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);




#ifdef LOCAL_TASK
	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject = m_pWarriorObject;
	g_Logic.m_inGamePlayerSession[0].m_isVisible = true;
	g_Logic.m_inGamePlayerSession[0].m_id = 0;
#endif // LOCAL_TASK

	
	BuildShadow(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//무조건 마지막에 해줘야된다.
	Build2DUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
	m_pTextureToViewportComponent = new TextureToViewportComponent();
	m_pTextureToViewportComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	m_pTextureToViewportComponent->BuildObjects(pd3dDevice, pd3dCommandList, m_pDepthShaderComponent->GetDepthTexture());

}
void GameobjectManager::AnimateObjects()
{
	m_pSkyboxObject->SetPosition(m_pCamera->GetPosition());
	m_pLight->m_pLights[1].m_xmf3Position = m_pCamera->GetPosition();
	m_pLight->m_pLights[1].m_xmf3Direction = m_pCamera->GetLookVector();
}
void GameobjectManager::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pLight->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void GameobjectManager::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pLight->UpdateShaderVariables(pd3dCommandList);
}
void GameobjectManager::ReleaseShaderVariables()
{
	m_pLight->ReleaseShaderVariables();
}
bool GameobjectManager::onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':
		{
			if (!g_Logic.m_KeyInput->m_bWKey) {
				g_Logic.m_KeyInput->m_bWKey = true;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::FRONT);
				g_NetworkHelper.SendMovePacket(DIRECTION::FRONT);
			}
		}
		break;
		case 'A':
		{
			if (!g_Logic.m_KeyInput->m_bAKey) {
				g_Logic.m_KeyInput->m_bAKey = true;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::LEFT);
				g_NetworkHelper.SendMovePacket(DIRECTION::LEFT);
			}
		}
		break;
		case 'S':
		{
			if (!g_Logic.m_KeyInput->m_bSKey) {
				g_Logic.m_KeyInput->m_bSKey = true;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::BACK);
				g_NetworkHelper.SendMovePacket(DIRECTION::BACK);
			}
		}
		break;
		case 'D':
		{
			if (!g_Logic.m_KeyInput->m_bDKey) {
				g_Logic.m_KeyInput->m_bDKey = true;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::RIGHT);
				g_NetworkHelper.SendMovePacket(DIRECTION::RIGHT);
			}
		}
		break;
		case 'Q':
		{
			g_Logic.m_KeyInput->m_bQKey = true;
			break;
		}
		case 'E':
		{
			g_Logic.m_KeyInput->m_bEKey = true;
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

			if (g_Logic.m_KeyInput->m_bWKey) {
				g_Logic.m_KeyInput->m_bWKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::FRONT);
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					cout << "send Stop Packet" << endl;
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate				
				}
				else g_NetworkHelper.SendKeyUpPacket(DIRECTION::FRONT);
			}
		}
		break;
		case 'A':
		{
			if (g_Logic.m_KeyInput->m_bAKey) {
				g_Logic.m_KeyInput->m_bAKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::LEFT);
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					cout << "send Stop Packet" << endl;
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate
				}
				else g_NetworkHelper.SendKeyUpPacket(DIRECTION::LEFT);
			}
		}
		break;
		case 'S':
		{
			if (g_Logic.m_KeyInput->m_bSKey) {
				g_Logic.m_KeyInput->m_bSKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::BACK);
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					cout << "send Stop Packet" << endl;
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate			
				}
				else g_NetworkHelper.SendKeyUpPacket(DIRECTION::BACK);
			}
		}
		break;
		case 'D':
		{
			if (g_Logic.m_KeyInput->m_bDKey) {
				g_Logic.m_KeyInput->m_bDKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::RIGHT);
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					cout << "send Stop Packet" << endl;
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate
				}
				else g_NetworkHelper.SendKeyUpPacket(DIRECTION::RIGHT);
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


void GameobjectManager::SetPlayCharacter(Session* pSession) // 임시 함수
{
	//4명
	Session* cliSession = reinterpret_cast<Session*>(pSession);
	if (0 == cliSession->m_id) {
		cliSession->SetGameObject(m_pWarriorObject);
	}
	else if (1 == cliSession->m_id) {
		cliSession->SetGameObject(m_pTankerObject);
	}
	else if (2 == cliSession->m_id) {
		cliSession->SetGameObject(m_pArcherObject);
	}
	else {
		cliSession->SetGameObject(m_pPriestObject);
	}
}

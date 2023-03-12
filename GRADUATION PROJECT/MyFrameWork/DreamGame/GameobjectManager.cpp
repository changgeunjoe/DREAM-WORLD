#include "stdafx.h"
#include "GameobjectManager.h"
#include "Animation.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"
#include "DepthRenderShaderComponent.h"
#include "TextureToViewportComponent.h"
#include "UiShaderComponent.h"
#include "Character.h"

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
	m_pSkyboxObject->SetPosition(m_pCamera->GetPosition());
	m_pMonsterObject->Animate(fTimeElapsed);
	if (!g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject) return;
	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetLookAt();
	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->UpdateCameraPosition();
	for (auto& session : g_Logic.m_inGamePlayerSession) {
		if (-1 != session.m_id && session.m_isVisible) {
			if (session.m_currentDirection != DIRECTION::IDLE) {
				//session.m_currentPlayGameObject->MoveForward(50.0f * fTimeElapsed);
				session.m_currentPlayGameObject->Move(session.m_currentDirection, 50 * fTimeElapsed);
				if (session.m_currentPlayGameObject->GetRButtonClicked())
					session.m_currentPlayGameObject->RbuttonClicked(fTimeElapsed);
#ifdef _DEBUG
				auto look = session.m_currentPlayGameObject->GetLook();
				auto up = session.m_currentPlayGameObject->GetUp();
				auto right = session.m_currentPlayGameObject->GetRight();
				auto pos = session.m_currentPlayGameObject->GetPosition();

				//cout << "GameobjectManager::Animate() SessionId: " << session.m_id << endl;
				cout << "GameobjectManager::Animate() Position: " << pos.x << ", 0, " << pos.z << endl;
				//cout << "GameobjectManager::Animate() Look: " << look.x << ", " << look.y << ", " << look.z << endl;
				//cout << "GameobjectManager::Animate() up: " << up.x << ", " << up.y << ", " << up.z << endl;
				//cout << "GameobjectManager::Animate() right: " << right.x << ", " << right.y << ", " << right.z << endl;
				std::cout << "GameobjectManager::Animate() rotation angle: " << session.m_rotateAngle.x << ", " << session.m_rotateAngle.y << ", " << session.m_rotateAngle.z << std::endl;
#endif

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
{//렌더

	UpdateShaderVariables(pd3dCommandList);
	m_pSkyboxObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pDepthShaderComponent->UpdateShaderVariables(pd3dCommandList);

	for (auto& session : g_Logic.m_inGamePlayerSession) {
		if (-1 != session.m_id && session.m_isVisible) {
			session.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		}
	}
	//m_pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pMonsterObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);


	for (int i = 0; i < m_ppUIObjects.size(); i++) {
		m_ppUIObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

	

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
{//빌드
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();
	
	m_pPlaneObject = new GameObject(UNDEF_ENTITY);
	m_pPlaneObject->InsertComponent<RenderComponent>();
	m_pPlaneObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPlaneObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPlaneObject->SetModel("Model/Floor.bin");
	m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlaneObject->SetScale(30.0f, 1.0f, 30.0f);
	m_ppGameObjects.emplace_back(m_pPlaneObject);

	m_pWarriorObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pWarriorObject->InsertComponent<RenderComponent>();
	m_pWarriorObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pWarriorObject->SetPosition(XMFLOAT3(30.f, 0, 30.f));
	m_pWarriorObject->SetModel("Model/Warrior.bin");
	m_pWarriorObject->SetAnimationSets(6);
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(6);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_ATTACK, true);
	m_pWarriorObject->SetScale(30.0f);
	m_ppGameObjects.emplace_back(m_pWarriorObject);

	m_pArcherObject = new Archer();
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pArcherObject->SetPosition(XMFLOAT3(-20, 0, 0));
	m_pArcherObject->SetModel("Model/Archer.bin");
	m_pArcherObject->SetAnimationSets(6);
	m_pArcherObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackAnimationSet(6);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, true);
	m_pArcherObject->SetScale(30.0f);
	m_ppGameObjects.emplace_back(m_pArcherObject);

	m_pTankerObject = new Tanker();
	m_pTankerObject->InsertComponent<RenderComponent>();
	m_pTankerObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pTankerObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pTankerObject->SetModel("Model/Tanker.bin");
	m_pTankerObject->SetAnimationSets(7);
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(7);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackEnable(6, true);
	m_pTankerObject->SetScale(30.0f);
	m_ppGameObjects.emplace_back(m_pTankerObject);

	m_pPriestObject = new Priest();
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPriestObject->SetPosition(XMFLOAT3(40, 0, 0));
	m_pPriestObject->SetModel("Model/Priests.bin");
	m_pPriestObject->SetAnimationSets(4);
	m_pPriestObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackAnimationSet(4);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, true);
	m_pPriestObject->SetScale(30.0f);
	m_ppGameObjects.emplace_back(m_pPriestObject);

	m_pMonsterObject = new GameObject(UNDEF_ENTITY);
	m_pMonsterObject->InsertComponent<RenderComponent>();
	m_pMonsterObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pMonsterObject->SetPosition(XMFLOAT3(0, 0, 100));
	m_pMonsterObject->SetModel("Model/Boss.bin");
	m_pMonsterObject->SetAnimationSets(3);
	m_pMonsterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackAnimationSet(3);
	m_pMonsterObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, true);
	m_pMonsterObject->SetScale(30.0f);
	m_ppGameObjects.emplace_back(m_pMonsterObject);

	m_pSkyboxObject = new GameObject(SQUARE_ENTITY);
	m_pSkyboxObject->InsertComponent<RenderComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxMeshComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxShaderComponent>();
	m_pSkyboxObject->InsertComponent<TextureComponent>();
	m_pSkyboxObject->SetTexture(L"DreamWorld/DreamWorld.dds", RESOURCE_TEXTURE_CUBE, 12);
	m_pSkyboxObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pSkyboxObject->SetScale(1, 1, 1);
	m_pSkyboxObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);


	m_pUIGameSearchObject = new GameObject(UI_ENTITY);
	m_pUIGameSearchObject->InsertComponent<RenderComponent>();
	m_pUIGameSearchObject->InsertComponent<UIMeshComponent>();
	m_pUIGameSearchObject->InsertComponent<UiShaderComponent>();
	m_pUIGameSearchObject->InsertComponent<TextureComponent>();
	m_pUIGameSearchObject->SetTexture(L"Image/GameSearching.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameSearchObject->SetPosition(XMFLOAT3(0, 0, 1.01f));
	m_pUIGameSearchObject->SetScale(0.05, 0.025, 1);
	m_pUIGameSearchObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameSearchObject);

	m_pUIGameChoiceObject = new GameObject(UI_ENTITY);
	m_pUIGameChoiceObject->InsertComponent<RenderComponent>();
	m_pUIGameChoiceObject->InsertComponent<UIMeshComponent>();
	m_pUIGameChoiceObject->InsertComponent<UiShaderComponent>();
	m_pUIGameChoiceObject->InsertComponent<TextureComponent>();
	m_pUIGameChoiceObject->SetTexture(L"Image/GameSearching.dds", RESOURCE_TEXTURE2D, 3);
	m_pUIGameChoiceObject->SetPosition(XMFLOAT3(0.5, 0.1, 1.01));
	m_pUIGameChoiceObject->SetScale(0.05, 0.02, 1);
	m_pUIGameChoiceObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppUIObjects.emplace_back(m_pUIGameChoiceObject);





#if LOCAL_TASK
	// 플레이어가 캐릭터 선택하는 부분에 유사하게 넣을 예정
	m_pPlayerObject = new GameObject(UNDEF_ENTITY);	//수정필요
	memcpy(m_pPlayerObject, m_pArcherObject, sizeof(GameObject));
	m_pPlayerObject->SetCamera(m_pCamera);
	//delete m_pArcherObject;->delete하면서 뎊스렌더 문제 발생

	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject = m_pPlayerObject;
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
void GameobjectManager::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance)
{
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	GameObject* pSelectedObject = NULL;
	for (int j = 0; j < m_nObjects; j++)
	{
		nIntersected = m_ppUIObjects[j]->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = m_ppUIObjects[j];
		}
	}
	//return(pSelectedObject);
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
	static XMFLOAT3 upVec = XMFLOAT3(0, 1, 0);
	std::cout << "GameobjectManager::onProcessingKeyboardMessage() - before move current Direction: ";
	std::cout << (int)g_Logic.m_inGamePlayerSession[0].m_currentDirection << " ";
	if (!g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::IDLE)
		std::cout << "IDLE ";
	if (g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::LEFT)
		std::cout << "LEFT ";
	if (g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::RIGHT)
		std::cout << "RIGHT ";
	if (g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::FRONT)
		std::cout << "FRONT ";
	if (g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::BACK)
		std::cout << "BACK ";
	std::cout << std::endl;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		std::cout << "GameobjectManager::onProcessingKeyboardMessage() -key down: ";
		switch (wParam)
		{
		case 'W':
		{
			if (!g_Logic.m_KeyInput->m_bWKey) {
				g_Logic.m_KeyInput->m_bWKey = true;
				if (g_Logic.m_inGamePlayerSession[0].m_currentDirection == DIRECTION::IDLE) {
					switch (g_Logic.m_inGamePlayerSession[0].m_prevDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						break;
					case DIRECTION::BACK:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 180.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 180.0f;
						break;
					case DIRECTION::RIGHT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -90.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -90.0f;
						break;
					case DIRECTION::LEFT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 90.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 90.0f;
						break;
					default:
						break;
					}
				}
				else {
					switch (g_Logic.m_inGamePlayerSession[0].m_currentDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						break;
					case DIRECTION::BACK:
						break;
					case DIRECTION::RIGHT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::LEFT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 45.0f;
						break;
					default:
						break;
					}
				}
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::FRONT);
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin |= DIRECTION::FRONT;
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetMove(true);
				g_NetworkHelper.SendMovePacket(DIRECTION::FRONT);
			}
		}
		break;
		case 'A':
		{
			if (!g_Logic.m_KeyInput->m_bAKey) {
				g_Logic.m_KeyInput->m_bAKey = true;
				if (g_Logic.m_inGamePlayerSession[0].m_currentDirection == DIRECTION::IDLE) {
					switch (g_Logic.m_inGamePlayerSession[0].m_prevDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -90.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -90.0f;
						break;
					case DIRECTION::BACK:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 90.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 180.0f;
						break;
					case DIRECTION::RIGHT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 180.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -90.0f;
						break;
					case DIRECTION::LEFT:
						break;
					default:
						break;
					}
				}
				else {
					switch (g_Logic.m_inGamePlayerSession[0].m_currentDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::BACK:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::RIGHT:
						break;
					case DIRECTION::LEFT:
						break;
					default:
						break;
					}
				}
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::LEFT);
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin |= DIRECTION::LEFT;
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetMove(true);
				g_NetworkHelper.SendMovePacket(DIRECTION::LEFT);
			}
		}
		break;
		case 'S':
		{
			if (!g_Logic.m_KeyInput->m_bSKey) {
				g_Logic.m_KeyInput->m_bSKey = true;
				if (g_Logic.m_inGamePlayerSession[0].m_currentDirection == DIRECTION::IDLE) {
					switch (g_Logic.m_inGamePlayerSession[0].m_prevDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 180.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 180.0f;
						break;
					case DIRECTION::BACK:
						break;
					case DIRECTION::RIGHT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 90.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 90.0f;
						break;
					case DIRECTION::LEFT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -90.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -90.0f;
						break;
					default:
						break;
					}
				}
				else {
					switch (g_Logic.m_inGamePlayerSession[0].m_currentDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						break;
					case DIRECTION::BACK:
						break;
					case DIRECTION::RIGHT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 45.0f;
						break;
					case DIRECTION::LEFT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					default:
						break;
					}
				}
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::BACK);
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin |= DIRECTION::BACK;
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetMove(true);
				g_NetworkHelper.SendMovePacket(DIRECTION::BACK);
			}
		}
		break;
		case 'D':
		{
			if (!g_Logic.m_KeyInput->m_bDKey) {
				g_Logic.m_KeyInput->m_bDKey = true;
				if (g_Logic.m_inGamePlayerSession[0].m_currentDirection == DIRECTION::IDLE) {
					switch (g_Logic.m_inGamePlayerSession[0].m_prevDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 90.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 90.0f;
						break;
					case DIRECTION::BACK:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -90.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -90.0f;
						break;
					case DIRECTION::RIGHT:
						break;
					case DIRECTION::LEFT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 180.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 180.0f;
						break;
					default:
						break;
					}
				}
				else {
					switch (g_Logic.m_inGamePlayerSession[0].m_currentDirection)
					{
					case DIRECTION::IDLE:
					case DIRECTION::FRONT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 45.0f;
						break;
					case DIRECTION::BACK:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::RIGHT:
						break;
					case DIRECTION::LEFT:

						break;
					default:
						break;
					}
				}
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection | DIRECTION::RIGHT);
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin |= DIRECTION::RIGHT;
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetMove(true);
				g_NetworkHelper.SendMovePacket(DIRECTION::RIGHT);
			}
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
			m_pPlayerObject->m_pCamera->ReInitCamrea();
			m_pPlayerObject->SetCamera(m_pCamera);
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
			std::cout << "GameobjectManager::onProcessingKeyboardMessage() -key down: ";
			if (g_Logic.m_KeyInput->m_bWKey) {
				g_Logic.m_KeyInput->m_bWKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::FRONT);
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin ^= DIRECTION::FRONT;
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					PrintCurrentTime();
					cout << "send Stop Packet" << endl;
					auto look = g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetLook();
					cout << "GameobjectManager::onProcessingKeyboardMessage() - Look Dir: " << look.x << ", " << look.y << ", " << look.z << endl;
					g_Logic.m_inGamePlayerSession[0].m_prevDirection = DIRECTION::FRONT;
					g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin = DIRECTION::IDLE;
					std::cout << "GameobjectManager::onProcessingKeyboardMessage() - stop move m_prevDirection: ";
					std::cout << (int)g_Logic.m_inGamePlayerSession[0].m_prevDirection << " ";
					if (!g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::IDLE)
						std::cout << "IDLE ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::LEFT)
						std::cout << "LEFT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::RIGHT)
						std::cout << "RIGHT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::FRONT)
						std::cout << "FRONT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::BACK)
						std::cout << "BACK ";
					std::cout << std::endl;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate				
				}
				else {
					switch (g_Logic.m_inGamePlayerSession[0].m_currentDirection)
					{
					case DIRECTION::FRONT:
						break;
					case DIRECTION::LEFT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::BACK:
						break;
					case DIRECTION::RIGHT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 45.0f;
						break;
					}
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::FRONT);
				}
			}
		}
		break;
		case 'A':
		{
			if (g_Logic.m_KeyInput->m_bAKey) {
				g_Logic.m_KeyInput->m_bAKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::LEFT);
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin ^= DIRECTION::LEFT;
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					PrintCurrentTime();
					cout << "send Stop Packet" << endl;
					auto look = g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetLook();
					cout << "GameobjectManager::onProcessingKeyboardMessage() - Look Dir: " << look.x << ", " << look.y << ", " << look.z << endl;
					g_Logic.m_inGamePlayerSession[0].m_prevDirection = DIRECTION::LEFT;
					//g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin = DIRECTION::IDLE;
					std::cout << "GameobjectManager::onProcessingKeyboardMessage() - stop move m_prevDirection: ";
					std::cout << (int)g_Logic.m_inGamePlayerSession[0].m_prevDirection << " ";
					if (!g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::IDLE)
						std::cout << "IDLE ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::LEFT)
						std::cout << "LEFT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::RIGHT)
						std::cout << "RIGHT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::FRONT)
						std::cout << "FRONT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::BACK)
						std::cout << "BACK ";
					std::cout << std::endl;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetMove(false);
				}
				else {
					switch (g_Logic.m_inGamePlayerSession[0].m_currentDirection)
					{
					case DIRECTION::FRONT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 45.0f;
						break;
					case DIRECTION::LEFT:
						break;
					case DIRECTION::BACK:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::RIGHT:
						break;
					}
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::LEFT);
				}
			}
		}
		break;
		case 'S':
		{
			if (g_Logic.m_KeyInput->m_bSKey) {
				g_Logic.m_KeyInput->m_bSKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::BACK);
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin ^= DIRECTION::BACK;
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					PrintCurrentTime();
					cout << "send Stop Packet" << endl;
					auto look = g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetLook();
					cout << "GameobjectManager::onProcessingKeyboardMessage() - Look Dir: " << look.x << ", " << look.y << ", " << look.z << endl;
					g_Logic.m_inGamePlayerSession[0].m_prevDirection = DIRECTION::BACK;
					//g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin = DIRECTION::IDLE;
					std::cout << "GameobjectManager::onProcessingKeyboardMessage() - stop move m_prevDirection: ";
					std::cout << (int)g_Logic.m_inGamePlayerSession[0].m_prevDirection << " ";
					if (!g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::IDLE)
						std::cout << "IDLE ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::LEFT)
						std::cout << "LEFT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::RIGHT)
						std::cout << "RIGHT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::FRONT)
						std::cout << "FRONT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::BACK)
						std::cout << "BACK ";
					std::cout << std::endl;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate			
				}
				else {
					switch (g_Logic.m_inGamePlayerSession[0].m_currentDirection)
					{
					case DIRECTION::FRONT:
						break;
					case DIRECTION::LEFT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 45.0f;
						break;
					case DIRECTION::BACK:
						break;
					case DIRECTION::RIGHT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					}
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::BACK);
				}
			}
		}
		break;
		case 'D':
		{
			if (g_Logic.m_KeyInput->m_bDKey) {
				g_Logic.m_KeyInput->m_bDKey = false;
				g_Logic.m_inGamePlayerSession[0].m_currentDirection = (DIRECTION)(g_Logic.m_inGamePlayerSession[0].m_currentDirection ^ DIRECTION::RIGHT);
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin ^= DIRECTION::RIGHT;
				if (g_Logic.m_KeyInput->IsAllMovekeyUp()) {
					PrintCurrentTime();
					cout << "send Stop Packet" << endl;
					auto look = g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetLook();
					cout << "GameobjectManager::onProcessingKeyboardMessage() - Look Dir: " << look.x << ", " << look.y << ", " << look.z << endl;
					g_Logic.m_inGamePlayerSession[0].m_prevDirection = DIRECTION::RIGHT;
					//g_Logic.m_inGamePlayerSession[0].m_currentDirection = DIRECTION::IDLE;
					//g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_iLookDirectoin = DIRECTION::IDLE;
					std::cout << "GameobjectManager::onProcessingKeyboardMessage() - stop move m_prevDirection: ";
					std::cout << (int)g_Logic.m_inGamePlayerSession[0].m_prevDirection << " ";
					if (!g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::IDLE)
						std::cout << "IDLE ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::LEFT)
						std::cout << "LEFT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::RIGHT)
						std::cout << "RIGHT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::FRONT)
						std::cout << "FRONT ";
					if (g_Logic.m_inGamePlayerSession[0].m_prevDirection & DIRECTION::BACK)
						std::cout << "BACK ";
					std::cout << std::endl;
					g_NetworkHelper.SendStopPacket(g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetPosition(), g_Logic.m_inGamePlayerSession[0].m_rotateAngle); // XMFLOAT3 postion, XMFOAT3 Rotate
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetMove(false);
				}
				else {
					switch (g_Logic.m_inGamePlayerSession[0].m_currentDirection)
					{
					case DIRECTION::FRONT:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, -45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += -45.0f;
						break;
					case DIRECTION::LEFT:
						break;
					case DIRECTION::BACK:
						g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&upVec, 45.0f);
						g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 45.0f;
						break;
					case DIRECTION::RIGHT:
						break;
					}
					g_NetworkHelper.SendKeyUpPacket(DIRECTION::RIGHT);
				}
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
	std::cout << "GameobjectManager::onProcessingKeyboardMessage() - after move current Direction: ";
	std::cout << (int)g_Logic.m_inGamePlayerSession[0].m_currentDirection << " ";
	if (!g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::IDLE)
		std::cout << "IDLE ";
	if (g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::LEFT)
		std::cout << "LEFT ";
	if (g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::RIGHT)
		std::cout << "RIGHT ";
	if (g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::FRONT)
		std::cout << "FRONT ";
	if (g_Logic.m_inGamePlayerSession[0].m_currentDirection & DIRECTION::BACK)
		std::cout << "BACK ";
	std::cout << std::endl;
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
			XMFLOAT3 TempPosition = m_ppUIObjects[m_nUIObjects]->GetPosition();
			TempPosition.x += 0.01;
			m_ppUIObjects[m_nUIObjects]->SetPosition(TempPosition);
		}
		break;
		case 'O':
		{
			XMFLOAT3 TempPosition = m_ppUIObjects[m_nUIObjects]->GetPosition();
			TempPosition.x -= 0.01;
			m_ppUIObjects[m_nUIObjects]->SetPosition(TempPosition);
		}
		break;
		case 'K':
		{
			XMFLOAT3 TempPosition = m_ppUIObjects[m_nUIObjects]->GetPosition();
			TempPosition.y += 0.01;
			m_ppUIObjects[m_nUIObjects]->SetPosition(TempPosition);
		}
		break;
		case 'L':
		{
			XMFLOAT3 TempPosition = m_ppUIObjects[m_nUIObjects]->GetPosition();
			TempPosition.y -= 0.01;
			m_ppUIObjects[m_nUIObjects]->SetPosition(TempPosition);
		}
		break;
		case 'I':
		{
			m_nUIObjects++;
		//	if (m_nUIObjects > m_ppUIObjects.size())
			//m_nUIObjects = 0;
		}
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
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		if (g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->GetMove())
		{
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAnimationBlending(true);
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAction(true);
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->m_nUpperBodyAnimation = CharacterAnimation::CA_ATTACK;
		}
		else
		{
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAction(true);
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAllTrackdisable();
			g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_ATTACK, true);
		}
		break;
	}
	case WM_LBUTTONUP:
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAnimationBlending(false);
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAction(false);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		//m_pPlayerObject->SetRButtonClicked(true);
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetRButtonClicked(true);
		break;
	}
	case WM_RBUTTONUP:
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetRButtonClicked(false);
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->RbuttonUp();
		break;
	}
	default:
		break;
	}

}

void GameobjectManager::SetPlayCharacter(Session* pSession) // 임시 함수
{
	//4명
	Session* cliSession = reinterpret_cast<Session*>(pSession);
	if (0 == cliSession->m_id) {
		cliSession->SetGameObject(m_pTankerObject);
	}
	else if (1 == cliSession->m_id) {
		cliSession->SetGameObject(m_pWarriorObject);
	}
	else if (2 == cliSession->m_id) {
		cliSession->SetGameObject(m_pArcherObject);
	}
	else {
		cliSession->SetGameObject(m_pPriestObject);
	}
	if (g_Logic.myId == cliSession->m_id)
		cliSession->m_currentPlayGameObject->SetCamera(m_pCamera);
}

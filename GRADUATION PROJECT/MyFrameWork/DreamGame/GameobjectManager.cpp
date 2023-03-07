#include "stdafx.h"
#include "GameobjectManager.h"
#include "Animation.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"

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
	//g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->SetLookAt();
	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->UpdateCameraPosition();
	for (auto& session : g_Logic.m_inGamePlayerSession) {
		if (-1 != session.m_id && session.m_isVisible) {
			if (session.m_currentDirection != DIRECTION::IDLE) {
				session.m_currentPlayGameObject->MoveForward(50.0f * fTimeElapsed);
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

void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	UpdateShaderVariables(pd3dCommandList);
	m_pSkyboxObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);


	//m_pWarriorObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	for (auto& session : g_Logic.m_inGamePlayerSession) {
		if (-1 != session.m_id && session.m_isVisible) {
			session.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			session.m_currentPlayGameObject->CheckIntersect(m_pMonsterObject);	//수정필요
		}
	}
	m_pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();

	m_pWarriorObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pWarriorObject->InsertComponent<RenderComponent>();
	m_pWarriorObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pWarriorObject->SetPosition(XMFLOAT3(30.f, 0, 30.f));
	m_pWarriorObject->SetModel("Model/Warrior.bin");
	m_pWarriorObject->SetAnimationSets(3);
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackAnimationSet(3);
	m_pWarriorObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, true);
	m_pWarriorObject->SetScale(30.0f);

	m_pArcherObject = new GameObject(UNDEF_ENTITY);
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pArcherObject->SetPosition(XMFLOAT3(-20, 0, 0));
	m_pArcherObject->SetModel("Model/Archer.bin");
	m_pArcherObject->SetAnimationSets(3);
	m_pArcherObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackAnimationSet(3);
	m_pArcherObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, true);
	m_pArcherObject->SetScale(30.0f);

	m_pTankerObject = new GameObject(UNDEF_ENTITY);
	m_pTankerObject->InsertComponent<RenderComponent>();
	m_pTankerObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pTankerObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pTankerObject->SetModel("Model/Tanker.bin");
	m_pTankerObject->SetAnimationSets(3);
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(3);
	m_pTankerObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, true);
	m_pTankerObject->SetScale(30.0f);

	m_pPriestObject = new GameObject(UNDEF_ENTITY);
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPriestObject->SetPosition(XMFLOAT3(40, 0, 0));
	m_pPriestObject->SetModel("Model/Priests.bin");
	m_pPriestObject->SetAnimationSets(3);
	m_pPriestObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackAnimationSet(3);
	m_pPriestObject->m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, true);
	m_pPriestObject->SetScale(30.0f);

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

	m_pPlaneObject = new GameObject(UNDEF_ENTITY);
	m_pPlaneObject->InsertComponent<RenderComponent>();
	m_pPlaneObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pPlaneObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPlaneObject->SetModel("Model/Floor.bin");
	//m_pPlaneObject->SetAnimationSets(3);
	m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//m_pPlaneObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	//m_pPlaneObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	//m_pPlaneObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	//m_pPlaneObject->m_pSkinnedAnimationController->SetTrackEnable(2, true);
	//m_pPlaneObject->m_pSkinnedAnimationController->SetRootMotion(false);
	m_pPlaneObject->SetScale(30.0f, 30.0f, 30.0f);

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
	memcpy(m_pPlayerObject, m_pArcherObject, sizeof(GameObject));
	m_pPlayerObject->SetCamera(m_pCamera);
	delete m_pArcherObject;

	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject = m_pPlayerObject;
	g_Logic.m_inGamePlayerSession[0].m_isVisible = true;
	g_Logic.m_inGamePlayerSession[0].m_id = 0;
#endif // LOCAL_TASK



}
void GameobjectManager::BuildLight()
{
	m_pLight->BuildLight();
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

void GameobjectManager::onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAttackAnimation(true);
		break;
	}
	case WM_LBUTTONUP:
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAnimationBlending(false);
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->m_pSkinnedAnimationController->SetAttackAnimation(false);
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

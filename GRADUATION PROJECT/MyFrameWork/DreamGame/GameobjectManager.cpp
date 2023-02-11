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

	if (g_Logic.m_KeyInput->m_bQKey)
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetUp(), -12.0f * fTimeElapsed);
		g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y -= 12.0f * fTimeElapsed;
		g_NetworkHelper.SendRotatePacket(ROTATE_AXIS::Y, -12.0f * fTimeElapsed);
	}
	if (g_Logic.m_KeyInput->m_bEKey)
	{
		g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetUp(), 12.0f * fTimeElapsed);
		g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += 12.0f * fTimeElapsed;
		g_NetworkHelper.SendRotatePacket(ROTATE_AXIS::Y, 12.0f * fTimeElapsed);
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
		}

	}
}

void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	UpdateShaderVariables(pd3dCommandList);
	m_pSkyboxObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);


	for (auto& session : g_Logic.m_inGamePlayerSession) {
		if (-1 != session.m_id && session.m_isVisible) {
			session.m_currentPlayGameObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		}
	}
	m_pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void GameobjectManager::Animate(float fTimeElapsed)
{
	m_pMonsterObject->Animate(fTimeElapsed);
	m_pMonsterObject2->Animate(fTimeElapsed);
}

void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();

	m_pWarriorObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pWarriorObject->InsertComponent<RenderComponent>();
	m_pWarriorObject->InsertComponent<CubeMeshComponent>();
	m_pWarriorObject->InsertComponent<ShaderComponent>();
	m_pWarriorObject->InsertComponent<TextureComponent>();
	m_pWarriorObject->SetTexture(L"Image/stones.dds", RESOURCE_TEXTURE2D, 3);
	m_pWarriorObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pArcherObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CubeMeshComponent>();
	m_pArcherObject->InsertComponent<ShaderComponent>();
	m_pArcherObject->InsertComponent<TextureComponent>();
	m_pArcherObject->SetTexture(L"Image/stones.dds", RESOURCE_TEXTURE2D, 3);
	m_pArcherObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pArcherObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pTankerObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pTankerObject->InsertComponent<RenderComponent>();
	m_pTankerObject->InsertComponent<CubeMeshComponent>();
	m_pTankerObject->InsertComponent<ShaderComponent>();
	m_pTankerObject->InsertComponent<TextureComponent>();
	m_pTankerObject->SetTexture(L"Image/stones.dds", RESOURCE_TEXTURE2D, 3);
	m_pTankerObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pPriestObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CubeMeshComponent>();
	m_pPriestObject->InsertComponent<ShaderComponent>();
	m_pPriestObject->InsertComponent<TextureComponent>();
	m_pPriestObject->SetTexture(L"Image/stones.dds", RESOURCE_TEXTURE2D, 3);
	m_pPriestObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPriestObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);


	m_pPlaneObject = new GameObject(PlANE_ENTITY);
	m_pPlaneObject->InsertComponent<RenderComponent>();
	m_pPlaneObject->InsertComponent<CubeMeshComponent>();
	m_pPlaneObject->InsertComponent<ShaderComponent>();
	m_pPlaneObject->InsertComponent<TextureComponent>();
	m_pPlaneObject->SetTexture(L"Image/Base_Texture.dds", RESOURCE_TEXTURE2D, 3);
	m_pPlaneObject->SetPosition(XMFLOAT3(0, -10, 50));
	m_pPlaneObject->SetScale(100, 0.1, 100);
	//m_pPlaneObject->SetScale(10.f, 0.2f, 10.f);
	m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject = new GameObject(UNDEF_ENTITY);
	m_pMonsterObject->InsertComponent<RenderComponent>();
	m_pMonsterObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pMonsterObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pMonsterObject->SetModel("Model/Boss.bin");
	m_pMonsterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pSkyboxObject = new GameObject(SQUARE_ENTITY);
	m_pSkyboxObject->InsertComponent<RenderComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxMeshComponent>();
	m_pSkyboxObject->InsertComponent<SkyBoxShaderComponent>();
	m_pSkyboxObject->InsertComponent<TextureComponent>();
	m_pSkyboxObject->SetTexture(L"DreamWorld/DreamWorld.dds", RESOURCE_TEXTURE_CUBE, 12);
	m_pSkyboxObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pSkyboxObject->SetScale(1, 1, 1);
	m_pSkyboxObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

#ifdef LOCAL_TASK
	g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject = m_pWarriorObject;
	g_Logic.m_inGamePlayerSession[0].m_isVisible = true;
	g_Logic.m_inGamePlayerSession[0].m_id = 0;
#endif // LOCAL_TASK



}
void GameobjectManager::BuildLight()
{
	m_pLight->BuildLight();
}
void GameobjectManager::AnimateObjects()
{
	m_pSkyboxObject->SetPosition(m_pCamera->GetPosition());
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

#include "stdafx.h"
#include "GameobjectManager.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"

extern NetworkHelper g_NetworkHelper;
extern clientNet::Logic g_Logic;


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

GameobjectManager::GameobjectManager()
{
	m_pLight = new CLight();
}

GameobjectManager::~GameobjectManager()
{
}

void GameobjectManager::Animate(float fTimeElapsed)
{
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
			}
			else {
				if (DIRECTION::LEFT == (session.m_currentDirection & DIRECTION::LEFT)) {
					session.m_currentPlayGameObject->MoveStrafe(-50.0f * fTimeElapsed);
				}
				else if (DIRECTION::RIGHT == (session.m_currentDirection & DIRECTION::RIGHT)) {
					session.m_currentPlayGameObject->MoveStrafe(50.0f * fTimeElapsed);
				}
			}
		}
	}
	cout << "elapsed Time: " << fTimeElapsed << endl;

}

void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	UpdateShaderVariables(pd3dCommandList);


	m_pWarriorObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pArcherObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTankerObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPriestObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

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
	m_pWarriorObject->SetTexture(L"Image/stones.dds");
	m_pWarriorObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pWarriorObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pArcherObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pArcherObject->InsertComponent<RenderComponent>();
	m_pArcherObject->InsertComponent<CubeMeshComponent>();
	m_pArcherObject->InsertComponent<ShaderComponent>();
	m_pArcherObject->InsertComponent<TextureComponent>();
	m_pArcherObject->SetTexture(L"Image/stones.dds");
	m_pArcherObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pArcherObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pTankerObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pTankerObject->InsertComponent<RenderComponent>();
	m_pTankerObject->InsertComponent<CubeMeshComponent>();
	m_pTankerObject->InsertComponent<ShaderComponent>();
	m_pTankerObject->InsertComponent<TextureComponent>();
	m_pTankerObject->SetTexture(L"Image/stones.dds");
	m_pTankerObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pTankerObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pPriestObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pPriestObject->InsertComponent<RenderComponent>();
	m_pPriestObject->InsertComponent<CubeMeshComponent>();
	m_pPriestObject->InsertComponent<ShaderComponent>();
	m_pPriestObject->InsertComponent<TextureComponent>();
	m_pPriestObject->SetTexture(L"Image/stones.dds");
	m_pPriestObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pPriestObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);


	m_pPlaneObject = new GameObject(PlANE_ENTITY);
	m_pPlaneObject->InsertComponent<RenderComponent>();
	m_pPlaneObject->InsertComponent<CubeMeshComponent>();
	m_pPlaneObject->InsertComponent<ShaderComponent>();
	m_pPlaneObject->InsertComponent<TextureComponent>();
	m_pPlaneObject->SetTexture(L"Image/stones.dds");
	m_pPlaneObject->SetPosition(XMFLOAT3(0, -10, 50));
	m_pPlaneObject->SetScale(10.f, 0.2f, 10.f);
	m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
					//g_NetworkHelper.SendStopPacket(m_pSqureObject->GetPosition(), m_pSqureObject->GetLook()); // XMFLOAT3 postion, XMFOAT3 Rotate				
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
					//g_NetworkHelper.SendStopPacket(m_pSqureObject->GetPosition(), m_pSqureObject->GetLook()); // XMFLOAT3 postion, XMFOAT3 Rotate
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
					//g_NetworkHelper.SendStopPacket(m_pSqureObject->GetPosition(), m_pSqureObject->GetLook()); // XMFLOAT3 postion, XMFOAT3 Rotate			
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
					//g_NetworkHelper.SendStopPacket(m_pSqureObject->GetPosition(), m_pSqureObject->GetLook()); // XMFLOAT3 postion, XMFOAT3 Rotate
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
	clientNet::Session* cliSession = reinterpret_cast<clientNet::Session*>(pSession);
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
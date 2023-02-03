#include "GameobjectManager.h"
#include "Network/NetworkHelper.h"

extern NetworkHelper g_NetworkHelper;

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
	CKeyInput* KeyInput = m_pSqureObject->m_KeyInput;
	if (KeyInput->m_bWKey || KeyInput->m_bAKey || KeyInput->m_bSKey || KeyInput->m_bDKey)
	{
		// State 만들어서 움직이는 중이라고 알려줘도 괜찮을듯합니다.
		cout << "elapsed Time: " << fTimeElapsed << endl;
		if (KeyInput->m_bWKey)
		{
			if (KeyInput->m_bAKey)
			{
				m_pSqureObject->MoveDiagonal(1, -1, 50.0f * fTimeElapsed);
			}
			if (KeyInput->m_bDKey)
			{
				m_pSqureObject->MoveDiagonal(1, 1, 50.0f * fTimeElapsed);
			}
			else m_pSqureObject->MoveForward(50.0f * fTimeElapsed);
		}
		if (KeyInput->m_bSKey)
		{
			if (KeyInput->m_bAKey)
			{
				m_pSqureObject->MoveDiagonal(-1, -1, 50.0f * fTimeElapsed);
			}
			if (KeyInput->m_bDKey)
			{
				m_pSqureObject->MoveDiagonal(-1, 1, 50.0f * fTimeElapsed);
			}
			m_pSqureObject->MoveForward(-50.0f * fTimeElapsed);
		}
		if (KeyInput->m_bAKey)
		{
			m_pSqureObject->MoveStrafe(-50.0f * fTimeElapsed);
		}
		if (KeyInput->m_bDKey)
		{
			m_pSqureObject->MoveStrafe(50.0f * fTimeElapsed);
		}
		auto pos = m_pSqureObject->GetPosition();
		std::cout << "currentPos: " << pos.x << ", " << pos.y << ", " << pos.z << endl;
	}
	if (KeyInput->m_bQKey || KeyInput->m_bEKey)
	{
		if (KeyInput->m_bQKey)
		{
			m_pSqureObject->Rotate(&m_pSqureObject->GetUp(), -30.0f * fTimeElapsed);
		}
		if (KeyInput->m_bEKey)
		{
			m_pSqureObject->Rotate(&m_pSqureObject->GetUp(), 30.0f * fTimeElapsed);
		}
		auto look = m_pSqureObject->GetLook();
		std::cout << "currentLook: " << look.x << ", " << look.y << ", " << look.z << endl;
	}
}

void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	UpdateShaderVariables(pd3dCommandList);


	m_pSqureObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pSqure2Object->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

}

void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	BuildLight();
	m_pSqureObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pSqureObject->InsertComponent<RenderComponent>();
	m_pSqureObject->InsertComponent<CubeMeshComponent>();
	m_pSqureObject->InsertComponent<ShaderComponent>();
	m_pSqureObject->InsertComponent<TextureComponent>();
	m_pSqureObject->SetTexture(L"Image/stones.dds");
	m_pSqureObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pSqureObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pSqure2Object = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pSqure2Object->InsertComponent<RenderComponent>();
	m_pSqure2Object->InsertComponent<CubeMeshComponent>();
	m_pSqure2Object->InsertComponent<ShaderComponent>();
	m_pSqure2Object->InsertComponent<TextureComponent>();
	m_pSqure2Object->SetTexture(L"Image/stones.dds");
	m_pSqure2Object->SetPosition(XMFLOAT3(100, 0, 50));
	m_pSqure2Object->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
			if (!m_pSqureObject->m_KeyInput->m_bWKey) {
				m_pSqureObject->m_KeyInput->m_bWKey = true;
				g_NetworkHelper.SendMovePacket(DIRECTION::FRONT);
			}
		}
		break;
		case 'A':
		{
			if (!m_pSqureObject->m_KeyInput->m_bAKey) {
				m_pSqureObject->m_KeyInput->m_bAKey = true;
				g_NetworkHelper.SendMovePacket(DIRECTION::LEFT);
			}
		}
		break;
		case 'S':
		{
			if (!m_pSqureObject->m_KeyInput->m_bSKey) {
				m_pSqureObject->m_KeyInput->m_bSKey = true;
				g_NetworkHelper.SendMovePacket(DIRECTION::BACK);
			}
		}
		break;
		case 'D':
		{
			if (!m_pSqureObject->m_KeyInput->m_bDKey) {
				m_pSqureObject->m_KeyInput->m_bDKey = true;
				g_NetworkHelper.SendMovePacket(DIRECTION::RIGHT);
			}
		}
		break;
		case 'Q':
		{
			m_pSqureObject->m_KeyInput->m_bQKey = true;
			break;
		}
		case 'E':
		{
			m_pSqureObject->m_KeyInput->m_bEKey = true;
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

			if (m_pSqureObject->m_KeyInput->m_bWKey) {
				m_pSqureObject->m_KeyInput->m_bWKey = false;
				if (m_pSqureObject->m_KeyInput->IsAllMovekeyUp()) {
					cout << "send Stop Packet" << endl;
					g_NetworkHelper.SendStopPacket(m_pSqureObject->GetPosition(), m_pSqureObject->GetLook()); // XMFLOAT3 postion, XMFOAT3 Rotate				
				}
				else g_NetworkHelper.SendKeyUpPacket(DIRECTION::FRONT);
			}
		}
		break;
		case 'A':
		{
			if (m_pSqureObject->m_KeyInput->m_bAKey) {
				m_pSqureObject->m_KeyInput->m_bAKey = false;
				if (m_pSqureObject->m_KeyInput->IsAllMovekeyUp()) {
					cout << "send Stop Packet" << endl;
					g_NetworkHelper.SendStopPacket(m_pSqureObject->GetPosition(), m_pSqureObject->GetLook()); // XMFLOAT3 postion, XMFOAT3 Rotate
				}
				else g_NetworkHelper.SendKeyUpPacket(DIRECTION::LEFT);
			}
		}
		break;
		case 'S':
		{
			if (m_pSqureObject->m_KeyInput->m_bSKey) {
				m_pSqureObject->m_KeyInput->m_bSKey = false;
				if (m_pSqureObject->m_KeyInput->IsAllMovekeyUp()) {
					cout << "send Stop Packet" << endl;
					g_NetworkHelper.SendStopPacket(m_pSqureObject->GetPosition(), m_pSqureObject->GetLook()); // XMFLOAT3 postion, XMFOAT3 Rotate			
				}
				else g_NetworkHelper.SendKeyUpPacket(DIRECTION::BACK);
			}
		}
		break;
		case 'D':
		{
			if (m_pSqureObject->m_KeyInput->m_bDKey) {
				m_pSqureObject->m_KeyInput->m_bDKey = false;
				if (m_pSqureObject->m_KeyInput->IsAllMovekeyUp()) {
					cout << "send Stop Packet" << endl;
					g_NetworkHelper.SendStopPacket(m_pSqureObject->GetPosition(), m_pSqureObject->GetLook()); // XMFLOAT3 postion, XMFOAT3 Rotate
				}
				else g_NetworkHelper.SendKeyUpPacket(DIRECTION::RIGHT);
			}
		}
		break;
		case 'Q':
		{
			m_pSqureObject->m_KeyInput->m_bQKey = false;
			break;
		}
		case 'E':
		{
			m_pSqureObject->m_KeyInput->m_bEKey = false;
			break;
		}
		}
	}
	default:
		break;
	}
	return(false);

}

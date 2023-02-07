#include "GameobjectManager.h"

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

void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	UpdateShaderVariables(pd3dCommandList);

	m_pSqureObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//player1 //화면에 보이는건데
	m_pSqure2Object->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);//player2 
	//m_pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
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
	m_pSqureObject->SetPosition(XMFLOAT3(0, 0, 50));
	m_pSqureObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pSqure2Object = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	m_pSqure2Object->InsertComponent<RenderComponent>();
	m_pSqure2Object->InsertComponent<CubeMeshComponent>();
	m_pSqure2Object->InsertComponent<ShaderComponent>();
	m_pSqure2Object->InsertComponent<TextureComponent>();
	m_pSqure2Object->SetTexture(L"Image/stones.dds");
	m_pSqure2Object->SetPosition(XMFLOAT3(100, 0, 50));
	m_pSqure2Object->SetScale(0.5,0.5,1);
	m_pSqure2Object->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlaneObject=new GameObject(PlANE_ENTITY);
	m_pPlaneObject->InsertComponent<RenderComponent>();
	m_pPlaneObject->InsertComponent<CubeMeshComponent>();
	m_pPlaneObject->InsertComponent<ShaderComponent>();
	m_pPlaneObject->InsertComponent<TextureComponent>();
	m_pPlaneObject->SetTexture(L"Image/stones.dds");
	m_pPlaneObject->SetPosition(XMFLOAT3(0, -10, 50));
	m_pPlaneObject->SetScale(0.1, 0.1, 1);
	//m_pPlaneObject->SetScale(10.f, 0.2f, 10.f);
	m_pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pMonsterObject = new GameObject(UNDEF_ENTITY);
	//m_pMonsterObject->InsertComponent<ShaderComponent>();
	m_pMonsterObject->InsertComponent<RenderComponent>();
	m_pMonsterObject->InsertComponent<TextureComponent>();
	m_pMonsterObject->SetTexture(L"Image/stones.dds");
	m_pMonsterObject->InsertComponent<CLoadedModelInfoCompnent>();
	m_pMonsterObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pMonsterObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//CLoadedModelInfoCompnent* pMonsterModel = GameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Monster.bin", shader,NULL);
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
		case 'W': m_pMonsterObject->MoveForward(+1.0f); break;//틱단위로 바꿔줘라
		case 'S': m_pMonsterObject->MoveForward(-1.0f); break;
		case 'A': m_pMonsterObject->MoveStrafe(-1.0f); break;
		case 'D': m_pMonsterObject->MoveStrafe(+1.0f); break;
		case 'Q': m_pMonsterObject->MoveUp(+1.0f); break;
		case 'E': m_pMonsterObject->MoveUp(-1.0f); break;
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
	default:
		break;
	}
	return(false);

}

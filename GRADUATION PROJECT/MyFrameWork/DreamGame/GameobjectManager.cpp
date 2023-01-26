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
	//std::cout<<typeid(T).name();
	//std::cout<<typeid(RenderComponent).name();
	return 0;
}

GameobjectManager::GameobjectManager()
{
	pLight = new CLight;
}

GameobjectManager::~GameobjectManager()
{
}

void GameobjectManager::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	//GameObject* pGameObject = new GameObject(SQUARE_ENTITY);
	//ModelRenderComponent* pModelRenderComponent = new ModelRenderComponent();
	//pGameObject->InsertComponent(pModelRenderComponent);
	//for (auto iter = pGameObject->Getcomponents().begin(); iter != pGameObject->Getcomponents().end(); iter++)
	//{
	//	pGameObject->Getcomponents().find(component_id::RENDER_COMPONENT);
	//}
	//auto it = .cbegin(); 
 //   while (it != pGameObject->Getcomponents().cend())
 //   {
 //       if (keys.find(it->first) != keys.cend())
 //       {
 //           // supported in C++11
 //           it = m.erase(it);
 //       }
 //       else {
 //           ++it;
 //       }
 //   }
 //

	//ComponentBase* pComponent = pGameObject->GetComponent(component_id::RENDER_COMPONENT);
	//if (pComponent != NULL)
	//{
	//	RenderComponent* pRenderComponent = static_cast<RenderComponent*>(pComponent);
	////	pRenderComponent->Render();
	//}

	pSqureObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	pPlaneObject->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void GameobjectManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	pSqureObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	pSqureObject->InsertComponent<RenderComponent>();
	pSqureObject->InsertComponent<CubeMeshComponent>();
	pSqureObject->InsertComponent<ShaderComponent>();
	pSqureObject->SetPosition(XMFLOAT3(0, 0, 50));
	pSqureObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pPlaneObject = new GameObject(PlANE_ENTITY);
	pPlaneObject->InsertComponent<RenderComponent>();
	pPlaneObject->InsertComponent<CubeMeshComponent>();
	pPlaneObject->InsertComponent<ShaderComponent>();
	pPlaneObject->SetPosition(XMFLOAT3(0, -10, 50));
	pPlaneObject->SetScale(10.f, 0.2f, 10.f);
	pPlaneObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void GameobjectManager::Rotate()
{
}

bool GameobjectManager::onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 'W':
		{
			pSqureObject->MoveForward(+1.0f);
			g_NetworkHelper.SendMovePacket(DIRECTION::FRONT);
		}
		break;
		case 'S':
		{
			pSqureObject->MoveForward(-1.0f);
			g_NetworkHelper.SendMovePacket(DIRECTION::BACK);
		}
		break;
		case 'A':
		{
			pSqureObject->MoveStrafe(-1.0f);
			g_NetworkHelper.SendMovePacket(DIRECTION::LEFT);
		}
		break;
		case 'D':
		{
			pSqureObject->MoveStrafe(+1.0f);
			g_NetworkHelper.SendMovePacket(DIRECTION::RIGHT);
		}
		break;
		case 'Q':
		{
			pSqureObject->MoveUp(+1.0f);
		}
		break;
		case 'R':
		{
			pSqureObject->MoveUp(-1.0f);
		}
		break;
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
	}
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case 'W':
		case 'S':
		case 'A':
		case 'D':
		{
			//창근 - 위치랑 로테이트 값 삽입 해주세요
			g_NetworkHelper.SendStopPacket(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1));
		}
		break;
		break;
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
	}
	break;

	default:
		break;
	}
	return(false);

}

void GameobjectManager::Move()
{
}


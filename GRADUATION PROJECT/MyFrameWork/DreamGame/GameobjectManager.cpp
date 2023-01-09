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
	//std::cout<<typeid(T).name();
	//std::cout<<typeid(RenderComponent).name();
	return 0;
}

void GameobjectManager::render()
{
	GameObject* pGameObject = new GameObject(SQUARE_ENTITY);
	ModelRenderComponent* pModelRenderComponent = new ModelRenderComponent();
	pGameObject->InsertComponent(pModelRenderComponent);
	for (auto iter = pGameObject->Getcomponents().begin(); iter != pGameObject->Getcomponents().end(); iter++)
	{
		
		pGameObject->Getcomponents().find(component_id::RENDER_COMPONENT);
	}
	auto it = .cbegin(); 
    while (it != pGameObject->Getcomponents().cend())
    {
        if (keys.find(it->first) != keys.cend())
        {
            // supported in C++11
            it = m.erase(it);
        }
        else {
            ++it;
        }
    }
 
	
	ComponentBase* pComponent = pGameObject->GetComponent(component_id::RENDER_COMPONENT);
	if (pComponent != NULL)
	{
		RenderComponent* pRenderComponent = static_cast<RenderComponent*>(pComponent);
		pRenderComponent->Render();
	}
}

void GameobjectManager::BuildObject()
{
	GameObject* pSqureObject = new GameObject(SQUARE_ENTITY);//사각형 오브젝트를 만들겠다
	pSqureObject->InsertComponent<RenderComponent>();
	pSqureObject->InsertComponent<CubeMeshComponent>();
	pSqureObject->InsertComponent<ShaderComponent>();

}

void GameobjectManager::Rotate()
{
}

void GameobjectManager::onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}


#pragma once
#include"GameObject.h"
#include"ComponentBase.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
#include"MeshComponent.h"
class GameobjectManager
{
public:
	virtual void render();
	virtual void BuildObject();
	virtual void Rotate();
	virtual void onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void Move();

private:
	GameObject* pGameObject=NULL;
	ComponentBase* pComponent = NULL;
	GameObject* pSqureObject = NULL;

};


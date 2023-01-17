#pragma once
#include"GameObject.h"
#include"ComponentBase.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
#include"MeshComponent.h"
#include"Light.h"
class GameobjectManager
{
public:
	GameobjectManager();
	~GameobjectManager();
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Rotate();
	virtual void onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void Move();

private:
	GameObject* pGameObject=NULL;
	ComponentBase* pComponent = NULL;
	GameObject* pSqureObject = NULL;
	GameObject* pPlaneObject = NULL;
	CLight* pLight = NULL;

};


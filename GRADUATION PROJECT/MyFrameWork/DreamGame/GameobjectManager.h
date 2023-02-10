#pragma once
#include"GameObject.h"
#include"ComponentBase.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
#include"MeshComponent.h"
#include"Light.h"
#include"CLoadModelinfo.h"
class GameobjectManager
{
public:
	GameobjectManager(CCamera* pCamera);
	~GameobjectManager();
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildLight();
	virtual void AnimateObjects();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


private: //active object 
	GameObject**	pGameObjects = NULL;
	GameObject*		m_pGameObject = NULL;
	GameObject*		m_pSqureObject = NULL;
	GameObject*     m_pSqure2Object = NULL;
	GameObject*		m_pPlaneObject = NULL;
	GameObject* m_pSkyboxObject = NULL;
	GameObject* m_pWarriorObject = NULL;
	GameObject* m_pAnimationObject = NULL;
	GameObject* m_pMonsterObject = NULL;
	CLight*			m_pLight = NULL;
	CCamera* m_pCamera = NULL;

};


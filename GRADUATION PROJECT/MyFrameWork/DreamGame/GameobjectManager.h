#pragma once
#include"GameObject.h"
#include"ComponentBase.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
#include"MeshComponent.h"
#include"Light.h"

class Session;
class ShadowMapShaderComponent;
class DepthRenderShaderComponent;
class TextureToViewportComponent;
#include"CLoadModelinfo.h"
class GameobjectManager
{
public:
	GameobjectManager(CCamera* pCamera);
	~GameobjectManager();
	virtual void Animate(float fTimeElapsed);
	virtual void OnPreRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildLight();
	virtual void BuildShadow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Build2DUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void AnimateObjects();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private: //active object 
	vector<GameObject*> m_ppGameObjects;
	vector<Arrow*> m_pArrowObjects;

	int				m_nObjects{};
	GameObject* m_pGameObject{ NULL };

	GameObject* m_pWarriorObject{ NULL };
	GameObject* m_pArcherObject{ NULL };
	GameObject* m_pTankerObject{ NULL }; // ∂À≈ ≈© √¢±Ÿ¿Ã
	GameObject* m_pPriestObject{ NULL };
	DepthRenderShaderComponent* m_pDepthShaderComponent{ NULL };
	ShadowMapShaderComponent* m_pShadowmapShaderComponent{NULL};
	TextureToViewportComponent* m_pTextureToViewportComponent{ NULL };
	GameObject* m_pPlaneObject{ NULL };
	GameObject* m_pSkyboxObject{ NULL };
	GameObject* m_pAnimationObject{ NULL };
	GameObject* m_pMonsterObject{ NULL };
	CLight* m_pLight{ NULL };
	CCamera* m_pCamera{ NULL };

	GameObject* m_pPlayerObject = NULL;

	GameObject* m_pUIGameSearchObject{ NULL };

	POINT						m_ptOldCursorPos;
public:
	void SetPlayCharacter(Session* pSession);
};


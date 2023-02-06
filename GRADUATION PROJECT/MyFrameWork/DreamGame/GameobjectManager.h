#pragma once
#include"GameObject.h"
#include"ComponentBase.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
#include"MeshComponent.h"
#include"Light.h"

class Session;
class GameobjectManager
{
public:
	GameobjectManager();
	~GameobjectManager();
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildLight();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
private: //active object 
	GameObject**	pGameObjects = NULL;
	GameObject*		m_pGameObject = NULL;

	GameObject*		m_pWarriorObject = NULL;
	GameObject*		m_pArcherObject = NULL;
	GameObject*		m_pTankerObject = NULL; // ∂À≈ ≈© √¢±Ÿ¿Ã
	GameObject*		m_pPriestObject = NULL;
	

	GameObject*		m_pPlaneObject = NULL;

	CLight*			m_pLight = NULL;

public:
	void SetPlayCharacter(Session* pSession);
};


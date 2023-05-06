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
class InstancingShaderComponent;
#include"CLoadModelinfo.h"
class GameobjectManager
{
public:
	GameobjectManager(CCamera* pCamera);
	~GameobjectManager();
	virtual void Animate(float fTimeElapsed);
	virtual void OnPreRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void UIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CharacterUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void StoryUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildLight();
	virtual void BuildShadow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Build2DUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildCharacterUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildInstanceObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildStoryUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void PickObjectByRayIntersection(int xClient, int yClient);
	virtual void ProcessingUI(int n);
	

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool onProcessingKeyboardMessageUI(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void onProcessingMouseMessageUI(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private: //active object 
	vector<GameObject*> m_ppGameObjects;
	array<Projectile*, 10> m_pArrowObjects;
	array<Projectile*, 10> m_pEnergyBallObjects;
	array<GameObject*, 10> m_pBoundingBox;
	int				m_nObjects{};
	GameObject* m_pGameObject{ NULL };

	GameObject* m_pWarriorObject{ NULL };
	GameObject* m_pArcherObject{ NULL };
	GameObject* m_pTankerObject{ NULL };
	GameObject* m_pPriestObject{ NULL };
	GameObject* m_pPlaneObject{ NULL };
	GameObject* m_pRockObject{ NULL };
	GameObject* m_pSkyboxObject{ NULL };
	GameObject* m_pAnimationObject{ NULL };
	GameObject* m_pMonsterObject{ NULL };
	GameObject* m_pPlayerObject{ NULL };


	DepthRenderShaderComponent* m_pDepthShaderComponent{ NULL };
	ShadowMapShaderComponent* m_pShadowmapShaderComponent{ NULL };
	TextureToViewportComponent* m_pTextureToViewportComponent{ NULL };
	InstancingShaderComponent* m_pInstancingShaderComponent{ NULL };


	CLight* m_pLight{ NULL };
	CCamera* m_pCamera{ NULL };



	////////////UI Object////////////// 
	vector<GameObject*> m_ppUIObjects;
	int				m_nUIObjects{};
	int				m_nSection = 0;
	array<int, 3> Section{ 0, 6, 8};
	//SECTION 1
	GameObject* m_pUIGameSearchObject{ NULL };
	GameObject* m_pUIGameChoiceObject{ NULL };
	GameObject* m_pUIGameMathchingObject{ NULL };
	GameObject* m_pUIWarriorCharacterObject{ NULL };
	GameObject* m_pUIArcherCharacterObject{ NULL };
	GameObject* m_pUITankerCharacterObject{ NULL };
	GameObject* m_pUIPriestCharacterObject{ NULL };

	//SECTION 2
	GameObject* m_pUIGameCreateObject{ NULL };
	GameObject* m_pUIEnterRoomObject{ NULL };
	GameObject* m_pSelectedObject{ NULL };

	//GmaeUI-HPBAR,CharacterUI - 조창근 23.04.13
	GameObject* m_pMonsterHPBarObject{ NULL };
	GameObject* m_pArcherHPBarObject{ NULL };//캐릭터 HPBAR
	GameObject* m_pArcherProfileObject{ NULL };//캐릭터 사진이 들어갈 UI
	GameObject* m_pArcherSkillBarObject{ NULL };
	vector<GameObject*> m_ppCharacterUIObjects;

	//ParticleObject-Particle -  23.04.13 .ccg

	vector<GameObject*>  m_pFireballSpriteObjects;
	GameObject* m_pFireballSpriteObject{ NULL };
	GameObject* m_pFireballEmissionSpriteObject{ NULL };
	GameObject* m_pFireball2EmissionSpriteObject{ NULL };
	vector<GameObject*> m_ppParticleObjects;

	//StoryUIObject -23.
	GameObject* m_pStroy1Object{ NULL };
	GameObject* m_pStory2Object{ NULL };
	GameObject* m_pStroy3Object{ NULL };
	GameObject* m_pStory4Object{ NULL };
	vector<GameObject*> m_ppStoryUIObjects;

	POINT						m_ptOldCursorPos;

	float m_fTime = 0;
	bool	m_bUIScene = true;

	ID3D12Resource* m_pd3dcbGameObjects = nullptr;
	//CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
	CB_GAMEFRAMEWORK_INFO* m_pcbMappedGameObjects = nullptr;
public:
	void SetPlayCharacter(Session* pSession);
	void SetSection(int n) { m_nSection = n; }
	void SetPlayerCamera(Session& mySession);
};


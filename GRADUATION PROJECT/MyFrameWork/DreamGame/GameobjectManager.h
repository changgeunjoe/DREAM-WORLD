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
class TrailComponent;
class EffectObject;
class DebuffObject;
class UILayer;
#include"CLoadModelinfo.h"
class GameobjectManager
{
public:
	GameobjectManager(CCamera* pCamera);
	~GameobjectManager();
	//Animate
	virtual void Animate(float fTimeElapsed);
	virtual void CharacterUIAnimate(float fTimeElapsed);
	virtual void TrailAnimate(float fTimeElapsed);
	virtual void StoryUIAnimate(float fTimeElapsed);
	//Render
	virtual void OnPreRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void TrailRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void AstarRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void UIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CharacterUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void TalkUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void StoryUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float ftimeElapsed);
	virtual void EffectRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float ftimeElapsed);
	virtual void SkyboxRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	//Build
	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildTrail(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildAstar(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildStage1(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildLight();
	virtual void BuildShadow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Build2DUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildCharacterUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildInstanceObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildStoryUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildNPC(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	
	virtual void BuildBossStageObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void PickObjectByRayIntersection(int xClient, int yClient);
	virtual void ProcessingUI(int n);
	

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool onProcessingKeyboardMessageLobby(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool onProcessingKeyboardMessageUI(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void onProcessingMouseMessageUI(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	bool CheckCollision(vector<GameObject*> m_ppObjects);


	void AddTextToUILayer(int &index);
	float CalculateDistance(const XMFLOAT3& firstPosition, const XMFLOAT3& lastPosition);
	

private: //active object 
	vector<GameObject*> m_ppGameObjects;
	vector<GameObject*> m_ppObstacleObjects;
	array<Projectile*, 10> m_pArrowObjects;
	array<Projectile*, 10> m_pEnergyBallObjects;
	array<GameObject*, 10> m_pBoundingBox;
	vector<GameObject*> m_ppObstacleBoundingBox;
	vector<GameObject*> m_ppNormalMonsterBoundingBox;
	int				m_nObjects{};
	GameObject* m_pGameObject{ NULL };

	GameObject* m_pWarriorObject{ NULL };
	GameObject* m_pArcherObject{ NULL };
	GameObject* m_pTankerObject{ NULL };
	GameObject* m_pPriestObject{ NULL };
	GameObject* m_pPlaneObject{ NULL };
	GameObject* m_pRockObject{ NULL };
	GameObject* m_pSkyboxObject{ NULL };
	GameObject* m_pNaviMeshObject{ NULL };
	GameObject* m_pAnimationObject{ NULL };
	GameObject* m_pMonsterObject{ NULL };
	GameObject* m_pMonsterCubeObject{ NULL };
	GameObject* m_pPlayerObject{ NULL };
	GameObject* m_pEnergyBallObject{ NULL };

	GameObject* m_pHealRange{ NULL };
	GameObject* m_pBarrierObject{ NULL };



	DepthRenderShaderComponent* m_pDepthShaderComponent{ NULL };
	ShadowMapShaderComponent*   m_pShadowmapShaderComponent{ NULL };
	TextureToViewportComponent* m_pTextureToViewportComponent{ NULL };
	InstancingShaderComponent*  m_pInstancingShaderComponent{ NULL };


	CLight* m_pLight{ NULL };
	CCamera* m_pCamera{ NULL };
	int m_nSetCharacter{0};



	////////////UI Object////////////// 
	vector<GameObject*> m_ppUIObjects;
	int				m_nUIObjects{};
	int				m_nSection = 0;
	array<int, 3> Section{ 0, 6, 8};
	//SECTION 1
	GameObject* m_pUIGameSearchObject{ NULL };
	GameObject* m_pUIGameChoiceObject{ NULL };
	GameObject* m_pUIGameEndObject{ NULL };
	GameObject* m_pUICharacterPickObjects[4]{ NULL };
	GameObject* m_pUIWarriorCharacterObject{ NULL };
	GameObject* m_pUIArcherCharacterObject{ NULL };
	GameObject* m_pUITankerCharacterObject{ NULL };
	GameObject* m_pUIPriestCharacterObject{ NULL };
	GameObject* m_pTalkUIObject{ NULL };
	GameObject* m_pPressGUIObject{ NULL };
	GameObject* m_pAttackUIObject{ NULL };
	GameObject* m_pConditionUIObject{ NULL };

	//Skill	
	GameObject* m_pHealSkillUIObject{ NULL };
	GameObject* m_pLightningSkillUIObject{ NULL };
	GameObject* m_pShieldSkillUIObject{ NULL };
	GameObject* m_pSwordSkillUIObject{ NULL };
	GameObject* m_pArrowSkillUIObject{ NULL };

	//SECTION 2
	GameObject* m_pUIGameCreateObject{ NULL };
	GameObject* m_pUIEnterRoomObject{ NULL };
	GameObject* m_pSelectedObject{ NULL };

	//GmaeUI-HPBAR,CharacterUI - Á¶Ã¢±Ù 23.04.13
	GameObject* m_pMonsterHPBarObject{ NULL };
	GameObject* m_pVictoryUIObject{ NULL };
	GameObject* m_pContinueUIObject{ NULL };
	vector<GameObject*> m_ppCharacterUIObjects;
	string m_sChooseCharcater{};
	//ParticleObject-Particle -  23.04.13 .ccg
	
	vector<GameObject*>  m_pFireballSpriteObjects;
	GameObject* m_pFireballSpriteObject{ NULL };
	GameObject* m_pFireballEmissionSpriteObject{ NULL };
	GameObject* m_pLightningSpriteObject{ NULL };
	GameObject* m_pMosterdebuffSpriteObject{ NULL };
	GameObject* m_pFireball2EmissionSpriteObject{ NULL };
	GameObject* m_pSwordFireObject{ NULL };
	vector<GameObject*> m_ppParticleObjects;

	//StoryUIObject -23.
	GameObject* m_pStroy1Object{ NULL };
	GameObject* m_pStory2Object{ NULL };
	GameObject* m_pStroy3Object{ NULL };
	GameObject* m_pStory4Object{ NULL };
	vector<GameObject*> m_ppStoryUIObjects;
	
	//TrailObject
	GameObject* m_pTrailObject{ NULL };
	TrailComponent* m_pTrailComponent{ NULL };
	//
	//AstarObject
	GameObject* m_pAstarObject{ NULL };
	TrailComponent* m_pAstarComponent{ NULL };
	//Effect
	array<GameObject*, 10> m_pStage1Objects{ NULL };
	GameObject* m_pStage1TerrainObject{ NULL };
	
	EffectObject* m_pEffectObject{NULL};
	EffectObject* m_pDebuffObject{ NULL };
	EffectObject* m_pLightEffectObject{ NULL };
	vector<EffectObject*> m_ppEffectObjects{};

	//NPC Object 
	GameObject* m_pAngelNPCObject{ NULL };
	GameObject* m_pAngelMageNPCObject{ NULL };

	POINT						m_ptOldCursorPos;

	float	m_fTime = 0;
	float	m_fTimeElapsed = 0;
	float	m_fStroyTime = 0;
	float	m_xmfMode = 3;
	bool	m_bUIScene = true;
	bool	m_bDebugMode = false;
	bool	m_bInMatching{ false };
	ID3D12Resource* m_pd3dcbGameObjects = nullptr;
	//CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
	CB_GAMEFRAMEWORK_INFO* m_pcbMappedGameObjects = nullptr;

	
public:
	std::vector<int> m_VecNodeQueue;
	std::mutex m_nodeLock;
	UILayer* m_pUILayer{ NULL };
	bool m_bNPCinteraction{true};
	bool m_bNPCscreen{ true };
	int m_iTEXTiIndex{ 2 };
	int m_nStageType=1;
public:
	void SetPlayCharacter(Session* pSession);
	void SetSection(int n) { m_nSection = n; }
	void SetPlayerCamera(Session& mySession);
	void ResetObject();
	void SetInMatching(bool inMatching) { m_bInMatching = inMatching; }
	void SetUIActive();
	void SetStoryTime() { m_fStroyTime = 0; };
	void ReadObjectFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* fileName, CLoadedModelInfoCompnent* modelName, int type, int stagetype);
	void ReadNormalMonsterFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* fileName, CLoadedModelInfoCompnent* modelName, int type, int stagetype);
	vector<GameObject*>& GetObstacle() { return m_ppObstacleObjects; }
	GameObject* GetChracterInfo(ROLE r);
};


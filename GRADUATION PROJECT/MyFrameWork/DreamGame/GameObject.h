#pragma once
//#include"stdafx.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
#include"MeshComponent.h"
#include"TextureComponent.h"
#include"ShadowMapShaderComponent.h"
#include"SkinnedMeshComponent.h"
#include"CLoadModelinfo.h"
#include"MaterialComponent.h"
#include"UiShaderComponent.h"
#include"MultiSpriteShaderComponent.h"
#include"InstanceRenderComponent.h"
#include"TerrainShaderComponent.h"
#include"TrailShaderComponent.h"
#include"EffectShaderComponent.h"
//include"CLoadModelinfo.h"
class DepthRenderShaderComponent;
class CLoadedModelInfoCompnent;
class SkinnedMeshComponent;
class ComponentBase;
class CAnimationController;
class Projectile;
class InstanceRenderComponent;
class TrailShaderComponent;
class TerrainShaderComponent;
class TrailComponent;


#define MATERIAL_ALBEDO_MAP				0x01
#define MATERIAL_SPECULAR_MAP			0x02
#define MATERIAL_NORMAL_MAP				0x04
#define MATERIAL_METALLIC_MAP			0x08
#define MATERIAL_EMISSION_MAP			0x10
#define MATERIAL_DETAIL_ALBEDO_MAP		0x20
#define MATERIAL_DETAIL_NORMAL_MAP		0x40

class GameObject
{
public:
	GameObject(entity_id entityID = UNDEF_ENTITY);
	~GameObject();

	void Update(float elapsedTime);

	entity_id GetEntityID() const;

	void SetPosition(const XMFLOAT3& position);
	void SetAddPosition(XMFLOAT3 position) { m_AddPosition = position; }
	XMFLOAT3 GetAddPosition() { return m_AddPosition; }
	void UpdateCameraPosition();
	const XMFLOAT3& GetPosition() const;

	void SetScale(float x, float y, float z);
	void SetinitScale(float x, float y, float z);
	void SetScale(float fScale);
	void SetTexture(wchar_t* pszFileName, int nSamplers, int nRootParameter);
	void SetModel(char* pszModelName);
	void SetModel(CLoadedModelInfoCompnent* pModel) { m_pLoadedModelComponent = pModel; }
	void SetAnimationSets(int nAnimationSets);
	void SetMesh(MeshComponent* pMesh);
	void SetTrailComponent(TrailComponent* pTrail) { m_pTrailComponent = pTrail; }
	TrailComponent* GetTrailComponent() { return m_pTrailComponent; }
	void SetCamera(CCamera* pCamera);
	void SetCamera(CCamera* pCamera, XMFLOAT3 xmf3CameraAxis);
	void SetRowColumn(float x, float y, float fSpeed = 0.1);

	virtual void MoveForward(int forwardDirection = 1, float ftimeElapsed = 0.01768f);
	virtual void MoveStrafe(int rightDirection = 1, float ftimeElapsed = 0.01768f);
	virtual void MoveDiagonal(int fowardDirection, int rightDirection, float ftimeElapsed = 0.01768f);
	void MoveUp(float fDistance = 1.0f);

	virtual void MoveVelocity(XMFLOAT3 direction, float ftimeelapsed, float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);

	void SetMaterial(int nMaterial, MaterialComponent* pMaterial);
	XMFLOAT3 GetLook();
	XMFLOAT3 GetObjectLook();
	void SetLook(const XMFLOAT3& xmfLook);
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	float CalculateDistanceSound();
	void SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f));

	unordered_map<component_id, ComponentBase*> Getcomponents();

	// void SetOrientation(const Quaternion& orientation);
	 //const Quaternion& GetOrientation() const;
	void SetComponent(const component_id& componentID);
	ComponentBase* GetComponent(const component_id& componentID);

	void HandleMessage(string message);

	virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void BuildShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender = false);
	virtual void InstanceRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nObjects, bool bPrerender = false);
	virtual void ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent);
	virtual void Animate(float fTimeElapsed);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World, MaterialComponent* ppMaterialsComponent);
	virtual void UpdateObjectVarialbes(XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();
	virtual void Reset() {};

	virtual void Die(float ftimeelapsed);
	virtual void DieMonster(float ftimeelapsed);

	void SetChild(GameObject* pChild, bool bReferenceUpdate = false);
	GameObject* FindFrame(char* pstrFrameName);
	void FindAndSetSkinnedMesh(SkinnedMeshComponent** ppSkinnedMeshes, int* pnSkinnedMesh);
	TextureComponent* FindReplicatedTexture(_TCHAR* pstrTextureName);
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);

	CAnimationController* m_pSkinnedAnimationController = NULL;

	UINT GetMeshType() { return((m_pMeshComponent) ? m_pMeshComponent->GetType() : 0x00); }
	void SetMaterialType(UINT nType) { m_nType |= nType; }

	void AnimateRowColumn(float fTimeElapsed);

	void SetRimLight(bool bRimLight);
	void SetCurrentHP(float fHP);
	void SetMaxHP(float fHP) { m_fMaxHp = fHP; }

	virtual void SetShield(float fShield);
	void SetShieldPercent(float fShieldPercent) { m_xmf4Color.w = fShieldPercent; }
	void SetSkillTime(float fSkillTime);
	void SetTempHp(float fHp) { m_fTempHp = fHp; };
	float GetCurrentHP() { return m_fHp; }
	float GetTempHP() { return m_fTempHp; }
	float GetMaxHP() { return m_fMaxHp; }
	MeshComponent* GetMesh() { return m_pMeshComponent; }

	float GetDistance() const { return m_fDistance; } //카메라와 오브젝트의 거리
public:
	static CLoadedModelInfoCompnent* LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, ShaderComponent* pShader, bool isBinary);

	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, GameObject* pParent, FILE* pInFile, ShaderComponent* pShader, ID3D12RootSignature* pd3dGraphicsRootSignature);
	static void LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfoCompnent* pLoadedModel);
	static GameObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, GameObject* pParent, FILE* pInFile, ShaderComponent* pShader, int* pnSkinnedMeshes);


	template<typename T>
	T* InsertComponent();
	template<typename T>
	T* ComponentType(component_id& componentID);

	virtual void OnPrepareRender() { }

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4* pxmf4x4World, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
public:

	int								m_nMaterials = 0;
	int                             m_nAnimationSets = 0;

	XMFLOAT4X4						m_xmf4x4ToParent;
	XMFLOAT4X4						m_xmf4x4Transform;//변환 행렬
	XMFLOAT4X4						m_xmf4x4World; //월드 행렬

	GameObject* m_pParent = nullptr;
	GameObject* m_pChild = nullptr;
	GameObject* m_pSibling = nullptr;

	int 							m_nRow = 0;
	int 							m_nCol = 0;

	int 							m_nRows = 1;
	int 							m_nCols = 1;

	XMFLOAT4X4						m_xmf4x4Texture;//멀티스프라이트를 활용하기위한 4x4행렬
	bool                            m_bMultiSprite{ false };
	bool							m_bBossSkillActive{ false };

	UINT							m_nType = 0x00;
	char							m_pstrFrameName[64]{};


	CCamera* m_pCamera{ nullptr };
	float                           m_fBoundingSize{ 8.0f };
	XMFLOAT3                        m_xmf3BoundingSphereOffset{ 0.0f, 0.0f, 0.0f };
	BoundingSphere					m_SPBB = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), m_fBoundingSize);
	BoundingSphere					m_SPBBNPC = BoundingSphere(XMFLOAT3(30.0f, 30.0f, 30.0f), 30);
	
	BoundingOrientedBox				m_OBB;
	GameObject* m_VisualizeSPBB{ nullptr };


	float                           m_fTime{};

	int                             m_iRButtionCount = 0;

	DIRECTION                       m_prevDirection = DIRECTION::IDLE;
	bool                            m_LMouseInput{ false };
	bool                            m_RMouseInput{ false };

	bool                            m_bRimLight = true;
	bool							m_bDie = false;

	XMFLOAT4						m_xmf4Color{};

	bool							m_bActive{ true };
	float                           m_fScale = 0.0f;
	float							m_fSkillTime = 7.0f;
	float							m_fBossSkillTime = 0.0f;

	int								m_nStageType = 0;
	float							m_fSkillSize = 0.0f;

	float							m_fObjectType = 0.0f;
	float							m_fConditionTime = 0.0f;
	int								m_nCondition = false;
	bool							m_bAttacked=false;
protected:

protected:
	int								m_nReferences = 0;
	UINT							m_nTextureType;
	int                             m_nSamplers = 1;
	int                             m_nRootParameter = 1;
public:

	XMFLOAT3						m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
protected:
	int								m_iVertexCount = 0;
	entity_id m_entityID{};//object id 
	XMFLOAT3 m_position{};
	//Quaternion m_orientation;
	float                           m_nRotateAngle = 0.0f;
	float							m_fDistance = 0.0f;
	wchar_t* pszFileNames{};
	char* pszModelNames{};
	LPCTSTR m_pFileName{};

	//////////////////////Component/////////////////////////////////
	unordered_map<component_id, ComponentBase*> m_components;
	MeshComponent* m_pMeshComponent{ nullptr };
	TextureComponent* m_pTextureComponent{ nullptr };
	CubeMeshComponent* m_pCubeComponent{ nullptr };
	SkyBoxMeshComponent* m_pSkyboxComponent{ nullptr };
	UIMeshComponent* m_pUiComponent{ nullptr };
	ShaderComponent* m_pShaderComponent{ nullptr };
	RenderComponent* m_pRenderComponent{ nullptr };
	SphereMeshComponent* m_pSphereComponent{ nullptr };
	TrailMeshComponent* m_pTrailMeshComponent{ nullptr };
	CylinderMeshComponent* m_pCylinderMeshComponent{ nullptr };
	SquareMeshComponent* m_pSquareMeshComponent{ nullptr };
	HeihtMapMeshComponent* m_pHeihtMapMeshComponent{ nullptr };
	InstanceRenderComponent* m_pInstanceRenderComponent{ nullptr };//인스턴스 렌더 추가 23.04.26 .ccg
	TrailComponent* m_pTrailComponent{ nullptr };

	MaterialComponent** m_ppMaterialsComponent{ nullptr };
	DepthRenderShaderComponent* m_pDepthShaderComponent{ nullptr };
	ShadowMapShaderComponent* m_pShadowMapShaderComponent{ nullptr };
public:
	CLoadedModelInfoCompnent* m_pLoadedModelComponent{ nullptr };
protected:

	ID3D12RootSignature* m_pd3dGraphicsRootSignature;

	ID3D12Resource* m_pd3dcbGameObjects = nullptr;
	//CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
	CB_GAMEOBJECT_STAT* m_pcbMappedGameObjects = nullptr;

	ID3D12Resource* m_pd3dcbUIGameObjects = nullptr;
	//CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
	CB_UIOBJECT_INFO* m_pcbMappedUIGameObjects = nullptr;

	ID3D12Resource* m_pd3dcbMultiSpriteGameObjects = NULL;
	ID3D12Resource* m_pd3dcbGameObjectsWorld = NULL;
	CB_GAMEOBJECTWORLD_INFO* m_pcbMappedGameObjectsWorld = NULL;
	CB_GAMEOBJECT_MULTISPRITE* m_pcbMappedMultiSpriteGameObjects = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	ID3D12Resource* m_pd3dcbGameObjectColor = NULL;
	CB_GAMEOBJECTCOLOR_INFO* m_pcbMappedGameObjectsColor = NULL;

	ID3D12Resource* m_pd3dcbGameObjectSkill= NULL;
	CB_GAMEOBJECTCOLOR_INFO* m_pcbMappedGameObjectsSkill = NULL;

	ID3D12Resource* m_pd3dcbGameObjectBossSkill = NULL;
	CB_GAMEOBJECTSKILL_INFO* m_pcbMappedGameObjectsBossSkill = NULL;

	ID3D12Resource* pShadowMap = nullptr;
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);// 삭제 예정(변경)
	int nObjects = 0;//삭제 예정(변경)

	//인스턴스 데이터를 포함하는 버퍼와 포인터이다.23.04.23 ccg
   /* ID3D12Resource *m_pd3dcbGameObjects = NULL;
	VS_VB_INSTANCE* m_pcbMappedGameObjects = NULL;*/

public:
	void SetBoundingBox(GameObject* boundingbox) { m_VisualizeSPBB = boundingbox; }

	void SetRButtonClicked(bool bRButtonClicked) { m_bRButtonClicked = bRButtonClicked; }
	bool GetRButtonClicked() { return m_bRButtonClicked; }

	virtual void SetLButtonClicked(bool bLButtonClicked) { m_bLButtonClicked = bLButtonClicked; }
	bool GetLButtonClicked() { return m_bLButtonClicked; }

	void SetMoveState(bool bMoveState) { m_bMoveState = bMoveState; }
	bool GetMoveState() { return m_bMoveState; }

	virtual void RbuttonClicked(float fTimeElapsed) {};
	virtual void RbuttonUp(const XMFLOAT3& CameraAxis = XMFLOAT3{ 0.0f, 0.0f, 0.0f }) {};
	virtual void SetSkillRangeObject(GameObject* obj) {};

	void SetFileName(LPCTSTR pFileName);

	void SetColor(XMFLOAT4 xmf4Color) { m_xmf4Color = xmf4Color; }

	void CalculateDistance(const XMFLOAT3& xmf3CameramPosition);

protected:
	bool                            m_bLButtonClicked = false;
	bool                            m_bRButtonClicked = false;
	atomic_bool                     m_bMoveState = false;

protected:
	float                           m_fHp{ 100 };//캐릭터 현재 체력
	float                           m_fMaxHp{ 100 };//캐릭터 최대 체력
	float                           m_fTempHp{ 100 };//캐릭터 최대 체력
	float							m_fShield{ 0};
	float                           m_fSpeed{};
	float                           m_fDamage{};
	XMFLOAT3						m_AddPosition{};


	float                           m_fTimeElapsed{};
	int                             m_nProjectiles{};

	CHeightMapImage* m_pHeightMapImage;
public:
	array<Projectile*, MAX_ARROW>          m_ppProjectiles;

	void SetBoundingSize(float size);
	void SetBoundingOffset(XMFLOAT3& boundingOffset);

	void SetSkillSize(float size);

	float GetBoundingSize() { return m_fBoundingSize; }
public:
	XMFLOAT3						m_xmfHitPosition;
	XMFLOAT3                        m_xmf3Destination = XMFLOAT3{ 0,0,0 };
	ROLE							m_roleDesPlayer = ROLE::NONE_SELECT;
	float                           m_UIScale = 10.0f;
	bool                            m_bUIActive{ true };

	int								m_iObjType = 0;
public:
	XMFLOAT3 m_xmf3RamdomDirection{};
public:
	GameObject* m_pHPBarUI{ NULL };
	GameObject* m_pProfileUI{ NULL };
	GameObject* m_pSkillQUI{ NULL };
	GameObject* m_pSkillEUI{ NULL };
	GameObject* m_pTrailStart{ NULL };
	GameObject* m_pTrailEnd{ NULL };
	GameObject* m_pWeapon{ NULL };
	
};

template<typename T>//템플릿을 활용하는 이유-> 
inline T* GameObject::InsertComponent()//컴포넌트를 게임 오브젝트에 넣는 함수
{
	component_id ComponentID{};
	ComponentType<T>(ComponentID);//
	if (m_components.find(ComponentID) != m_components.end())
	{
		return NULL;//같은 컴포넌트 요소는 들어갈 수 없다.
	}
	T* pComponent = new T();
	m_components.insert(std::make_pair(ComponentID, pComponent));
	pComponent->SetOwner(this);
	return pComponent;
}


template<typename T>
inline T* GameObject::ComponentType(component_id& componentID)
{
	if (typeid(T).name() == typeid(RenderComponent).name())
	{
		componentID = component_id::RENDER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(InstanceRenderComponent).name())
	{
		componentID = component_id::INSRENDER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(MeshComponent).name())
	{
		componentID = component_id::MESH_COMPONENT;
	}
	else if (typeid(T).name() == typeid(CubeMeshComponent).name())
	{
		componentID = component_id::CUBEMESH_COMPONENT;
	}
	else if (typeid(T).name() == typeid(SkyBoxMeshComponent).name())
	{
		componentID = component_id::SKYBOXMESH_COMPONENT;
	}
	else if (typeid(T).name() == typeid(UIMeshComponent).name())
	{
		componentID = component_id::UIMESH_COMPONENT;
	}
	else if (typeid(T).name() == typeid(SphereMeshComponent).name())
	{
		componentID = component_id::SPHEREMESH_COMPONENT;
	}
	else if (typeid(T).name() == typeid(ShaderComponent).name())
	{
		componentID = component_id::SHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(SkyBoxShaderComponent).name())
	{
		componentID = component_id::SKYSHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(UiShaderComponent).name())
	{
		componentID = component_id::UISHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(BlendingUiShaderComponent).name())
	{
		componentID = component_id::BLENDINGUISHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(MultiSpriteShaderComponent).name())
	{
		componentID = component_id::SPRITESHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(BoundingBoxShaderComponent).name())
	{
		componentID = component_id::BOUNDINGBOX_COMPONENT;
	}
	else if (typeid(T).name() == typeid(SphereShaderComponent).name())
	{
		componentID = component_id::SPHERE_COMPONENT;
	}
	else if (typeid(T).name() == typeid(TrailShaderComponent).name())
	{
		componentID = component_id::TRAILSHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(TerrainShaderComponent).name())
	{
		componentID = component_id::TERRAINSHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(TrailMeshComponent).name())
	{
		componentID = component_id::TRAILMESH_COMPONENT;
	}
	else if (typeid(T).name() == typeid(HeihtMapMeshComponent).name())
	{
		componentID = component_id::HEIGHTMESH_COMPONENT;
	}
	else if (typeid(T).name() == typeid(TextureComponent).name())
	{
		componentID = component_id::TEXTURE_COMPONENT;
	}
	else if (typeid(T).name() == typeid(CLoadedModelInfoCompnent).name())
	{
		componentID = component_id::LOADEDMODEL_COMPONET;
	}
	else if (typeid(T).name() == typeid(NaviMeshShaderComponent).name())
	{
		componentID = component_id::NAVIMESHSHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(EffectShaderComponent).name())
	{
		componentID = component_id::EFFECTSHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(BlendShaderComponent).name())
	{
		componentID = component_id::BLENDSHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(CylinderMeshComponent).name())
	{
		componentID = component_id::CYLINDER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(CylinderShaderComponent).name())
	{
		componentID = component_id::CYLINDERSHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(SquareMeshComponent).name())
	{
		componentID = component_id::SQUAREMESH_COMPONENT;
	}
	else if (typeid(T).name() == typeid(SquareShaderComponent).name())
	{
		componentID = component_id::SQUARESHADER_COMPONENT;
	}
	else if (typeid(T).name() == typeid(BossSkillShaderComponent).name())
	{
		componentID = component_id::BOSSSKILLSHADER_COMPONENT;
	}
	else
	{
		componentID = component_id::UNDEF_COMPONENT;
	}
	return 0;
}

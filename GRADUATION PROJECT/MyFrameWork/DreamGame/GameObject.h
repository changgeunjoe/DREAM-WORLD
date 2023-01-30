#pragma once
#include"stdafx.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
#include"MeshComponent.h"
#include"TextureComponent.h"
class ComponentBase;
class GameObject
{
public:
    GameObject(entity_id entityID);
    ~GameObject();

    void Update(float elapsedTime);

    entity_id GetEntityID() const;

    void SetPosition(const XMFLOAT3& position);
    const XMFLOAT3& GetPosition() const;

    void SetScale(float x, float y, float z);


    void MoveStrafe(float fDistance = 1.0f);
    void MoveUp(float fDistance = 1.0f);
    void MoveForward(float fDistance = 1.0f);

    XMFLOAT3 GetLook();
    XMFLOAT3 GetUp();
    XMFLOAT3 GetRight();

    unordered_map<component_id, ComponentBase*> Getcomponents();

   // void SetOrientation(const Quaternion& orientation);
    //const Quaternion& GetOrientation() const;
    void SetComponent(const component_id& componentID);
    ComponentBase* GetComponent(const component_id& componentID);

    void HandleMessage(string message);

    void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseShaderVariables();

    template<typename T>
    T* InsertComponent();
    template<typename T>
    T* ComponentType(component_id &componentID);

    virtual void OnPrepareRender() { }

    void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
    void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }
public:
    XMFLOAT4X4						m_xmf4x4Transform;//변환 행렬
    XMFLOAT4X4						m_xmf4x4World; //월드 행렬

protected:
    entity_id m_entityID;//object id 

    XMFLOAT3 m_position;
    //Quaternion m_orientation;

    unordered_map<component_id, ComponentBase*> m_components;

    CTexture* m_pMissileTexture{ NULL };
protected:
    ID3D12Resource* m_pd3dcbGameObjects = NULL;
    CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;

    D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
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
    return 0;
}

template<typename T>
inline T* GameObject::ComponentType(component_id &componentID)
{
    if (typeid(T).name() == typeid(RenderComponent).name())
    {
        componentID = component_id::RENDER_COMPONENT;
    }
    else if (typeid(T).name() == typeid(MeshComponent).name())
    {
        componentID = component_id::MESH_COMPONENT;
    }
    else if (typeid(T).name() == typeid(CubeMeshComponent).name())
    {
        componentID = component_id::CUBEMESH_COMPONENT;
    }
    else if (typeid(T).name() == typeid(ShaderComponent).name())
    {
        componentID = component_id::SHADER_COMPONENT;
    }
    else
    {
        componentID = component_id::UNDEF_COMPONENT;
    }
    return 0;
}


class CCharacterObject : public GameObject
{
private:
    int								m_nReferences = 0;

public:
    void AddRef();
    void Release();

public:
    CCharacterObject();
    CCharacterObject(int nMaterials);
    virtual ~CCharacterObject();

public:
    char							m_pstrFrameName[64];

    MeshComponent*                  m_pMesh = NULL;

    int								m_nMaterials = 0;
   // CMaterial** m_ppMaterials = NULL;

    CCharacterObject* m_pParent = NULL;
    CCharacterObject* m_pChild = NULL;
    CCharacterObject* m_pSibling = NULL;

    CAnimationController* m_pSkinnedAnimationController = NULL;

    int								m_iCurrentAnimationSet = 0;

    void SetMesh(CMesh* pMesh);
    void SetShader(CShader* pShader);
    void SetShader(int nMaterial, CShader* pShader);
    void SetMaterial(int nMaterial, CMaterial* pMaterial);

    void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);

    virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }

    virtual void OnPrepareAnimate() { }
    virtual void Animate(float fTimeElapsed);

    virtual void OnPrepareRender() { }
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

    virtual void OnLateUpdate() { }

    virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseShaderVariables();

    virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
    virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial);

    virtual void ReleaseUploadBuffers();

    XMFLOAT3 GetPosition();
    XMFLOAT3 GetLook();
    XMFLOAT3 GetUp();
    XMFLOAT3 GetRight();

    XMFLOAT3 GetToParentPosition();
    void Move(XMFLOAT3 xmf3Offset);

    void SetPosition(float x, float y, float z);
    void SetPosition(XMFLOAT3 xmf3Position);
    void SetScale(float x, float y, float z);

    void MoveStrafe(float fDistance = 1.0f);
    void MoveUp(float fDistance = 1.0f);
    void MoveForward(float fDistance = 1.0f);

    void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
    void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
    void Rotate(XMFLOAT4* pxmf4Quaternion);

    CGameObject* GetParent() { return(m_pParent); }
    void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
    CGameObject* FindFrame(char* pstrFrameName);

    CTexture* FindReplicatedTexture(_TCHAR* pstrTextureName);

    UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0x00); }

public:
    CSkinnedMesh* FindSkinnedMesh(char* pstrSkinnedMeshName);
    void FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh);

    void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
    void SetTrackAnimationPosition(int nAnimationTrack, float fPosition);

    void SetRootMotion(bool bRootMotion) { if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetRootMotion(bRootMotion); }

    void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader);

    static void LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfo* pLoadedModel);
    static CGameObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader, int* pnSkinnedMeshes);

    static CLoadedModelInfo* LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader);

    static void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);
};
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
//include"CLoadModelinfo.h"
class DepthRenderShaderComponent;
class CLoadedModelInfoCompnent;
class SkinnedMeshComponent;
class ComponentBase;
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
    GameObject(entity_id entityID);
    ~GameObject();

    void Update(float elapsedTime);

    entity_id GetEntityID() const;

    void SetPosition(const XMFLOAT3& position);
    const XMFLOAT3& GetPosition() const;

    void SetScale(float x, float y, float z);
    void SetTexture(wchar_t* pszFileName, int nSamplers, int nRootParameter);
    void SetModel(char* pszModelName);
    void SetMesh(MeshComponent* pMesh);

    void MoveStrafe(float fDistance = 1.0f);
    void MoveUp(float fDistance = 1.0f);
    void MoveForward(float fDistance = 1.0f);


    void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
    void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
    void Rotate(XMFLOAT4* pxmf4Quaternion);

    void SetMaterial(int nMaterial, MaterialComponent* pMaterial);
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


    void SetChild(GameObject* pChild, bool bReferenceUpdate = false);
    GameObject* FindFrame(char* pstrFrameName);
    void FindAndSetSkinnedMesh(SkinnedMeshComponent** ppSkinnedMeshes, int* pnSkinnedMesh);
    TextureComponent* FindReplicatedTexture(_TCHAR* pstrTextureName);
    void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);
    
    
    UINT GetMeshType() { return((m_pMeshComponent) ? m_pMeshComponent->GetType() : 0x00); };
    void SetMaterialType(UINT nType) { m_nType |= nType; }

public:
    static CLoadedModelInfoCompnent* LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, ShaderComponent* pShader, bool isBinary );

    void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, GameObject* pParent, FILE* pInFile, ShaderComponent* pShader);
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
public:

    int								m_nMaterials = 0;
    

    XMFLOAT4X4						m_xmf4x4ToParent;
    XMFLOAT4X4						m_xmf4x4Transform;//��ȯ ���
    XMFLOAT4X4						m_xmf4x4World; //���� ���

    GameObject* m_pParent = NULL;
    GameObject* m_pChild = NULL;
    GameObject* m_pSibling = NULL;


    UINT							m_nType = 0x00;
    char							m_pstrFrameName[64];


protected:

    int								m_nReferences = 0;
    UINT							m_nTextureType;
    int                             m_nSamplers=1;
    int                             m_nRootParameter = 1;


    entity_id m_entityID{};//object id 
    XMFLOAT3 m_position{};
    //Quaternion m_orientation;

    wchar_t* pszFileNames{};
    char* pszModelNames{};
    
    //////////////////////Component/////////////////////////////////
    unordered_map<component_id, ComponentBase*> m_components;
    MeshComponent* m_pMeshComponent{ NULL };
    TextureComponent* m_pTextureComponent{ NULL };
    CubeMeshComponent* m_pCubeComponent{ NULL };
    SkyBoxMeshComponent* m_pSkyboxComponent{ NULL };
    ShaderComponent* m_pShaderComponent{ NULL };
    RenderComponent* m_pRenderComponent{ NULL };
    CLoadedModelInfoCompnent* m_pLoadedModelComponent{ NULL };
    MaterialComponent** m_ppMaterialsComponent{ NULL };
    DepthRenderShaderComponent* m_pDepthShaderComponent{ NULL };
    ShadowMapShaderComponent* m_pShadowMapShaderComponent{NULL};

protected:
    ID3D12Resource* m_pd3dcbGameObjects = NULL;
    CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;

    D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

    ID3D12Resource* pShadowMap = NULL;
    UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);// ���� ����(����)
    int nObjects = 0;//���� ����(����)
};

template<typename T>//���ø��� Ȱ���ϴ� ����-> 
inline T* GameObject::InsertComponent()//������Ʈ�� ���� ������Ʈ�� �ִ� �Լ�
{
    component_id ComponentID{};
    ComponentType<T>(ComponentID);//
    if (m_components.find(ComponentID) != m_components.end())
    {
        return NULL;//���� ������Ʈ ��Ҵ� �� �� ����.
    }
    T* pComponent = new T();
    m_components.insert(std::make_pair(ComponentID, pComponent));
    pComponent->SetOwner(this);
    return pComponent;
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
    else if (typeid(T).name() == typeid(SkyBoxMeshComponent).name())
    {
        componentID = component_id::SKYBOXMESH_COMPONENT;
    }
    else if (typeid(T).name() == typeid(ShaderComponent).name())
    {
        componentID = component_id::SHADER_COMPONENT;
    }
    else if (typeid(T).name() == typeid(SkyBoxShaderComponent).name())
    {
        componentID = component_id::SKYSHADER_COMPONENT;
    }
    else if (typeid(T).name() == typeid(TextureComponent).name())
    {
        componentID = component_id::TEXTURE_COMPONENT;
    }
    else if (typeid(T).name() == typeid(CLoadedModelInfoCompnent).name())
    {
        componentID = component_id::LOADEDMODEL_COMPONET;
    }
    else
    {
        componentID = component_id::UNDEF_COMPONENT;
    }
    return 0;
}

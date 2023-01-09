#include "GameObject.h"


GameObject::GameObject(entity_id entityID)
{
    m_entityID = entityID;
}

GameObject::~GameObject()
{
}

void GameObject::Update(float elapsedTime)
{
}


void GameObject::SetPosition(const XMFLOAT3& position)
{
}

const XMFLOAT3& GameObject::GetPosition() const
{
    return m_position;
	// // O: 여기에 return 문을 삽입합니다.
}

unordered_map<component_id, ComponentBase*> GameObject::Getcomponents()
{
    return m_components;
}

template<typename T>
T* InsertComponent()
{
    unsigned int componentID = RTTI::GetTypeID<T>();
    if (m_components.find(componentID) != m_components.end())
    {
        return NULL;
    }
    T* pComponent = new T();
    m_components.insert(std::make_pair(componentID, pComponent));
    pComponent->SetOwner(this);
    return pComponent;
}
//void GameObject::SetComponent(const component_id& componentID)
//{
//    switch (componentID) 
//    {
//        case RENDERCOMPONENT:
//            break;
//
//    }
//   
//}



void GameObject::SetComponent(const component_id& componentID)
{
}
ComponentBase* GameObject::GetComponent(const component_id& componentID) 
{
	auto it = m_components.find(componentID);
	ComponentBase* Component= it->second;
	return Component;
}


void GameObject::HandleMessage(string message)
{
}

void GameObject::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	ComponentBase* pRenderComponent=GetComponent(component_id::RENDER_COMPONENT);
	if (pRenderComponent != NULL)
	{
		CubeMeshComponent* pCubeComponent = static_cast<CubeMeshComponent*>(pRenderComponent);
		pCubeComponent->BuildObject(pd3dDevice,pd3dCommandList, pd3dGraphicsRootSignature);
	}
	//m_pMissileTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	//m_pMissileTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/stones.dds", RESOURCE_TEXTURE2D, 0); ->텍스쳐 컴포넌트로 분리

	ComponentBase* pMeshComponent = GetComponent(component_id::MESH_COMPONENT);
	if (pComponent != NULL)
	{
		CubeMeshComponent* pRenderComponent = static_cast<CubeMeshComponent*>(pComponent);
		pRenderComponent->Render(pd3dCommandList);
	}
	//->메테리얼 생성 텍스쳐와 쉐이더를 넣어야되는데 쉐이더이므로 안 넣어도 됨
	m_pMissileMaterial = new CMaterial();
	m_pMissileMaterial->SetTexture(m_pMissileTexture);

	if()
	m_pMissileTexturedMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 10.f, 10.5f, 5.f);
	CMesh* pMeshIlluminated = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, 100.0f, 20, 20);
	m_nObjects = 10;
	m_ppObjects = new CGameObject * [m_nObjects];
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	
	ComponentBase* pShaderComponent = GetComponent(component_id::SHADER_COMPONENT);
	if (pComponent != NULL)
	{
		ShaderComponent* pShaderComponent = static_cast<CubeMeshComponent*>(pComponent);
		pShaderComponent->CreateCbvSrvDescriptorHeaps(pd3dDevice, m_nObjects, 10);
		pShaderComponent->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		pShaderComponent->CreateConstantBufferViews(pd3dDevice, m_nObjects, m_pd3dcbGameObjects, ncbElementBytes);
		pShaderComponent->CreateShaderResourceViews(pd3dDevice, m_pMissileTexture, 0, 11);
	}
	//	CreateShader(pd3dDevice,  pd3dCommandList,pd3dGraphicsRootSignature);
	int nObjects = 0;
	for (int i = 0; i < m_nObjects; i++) {
		pMissleObject = new CMissleObject();
		pMissleObject->SetMesh(0, m_pMissileTexturedMesh);
		pMissleObject->SetMaterial(0, m_pMissileMaterial);
		pMissleObject->SetActive(false);
		pMissleObject->SetPosition(0.f, 0.f, i);
		pMissleObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
		m_ppObjects[nObjects++] = pMissleObject;
	}
	//m_BulletList.push_back(pBullet);

};

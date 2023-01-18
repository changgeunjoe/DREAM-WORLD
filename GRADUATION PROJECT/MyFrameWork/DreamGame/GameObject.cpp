#include "GameObject.h"


GameObject::GameObject(entity_id entityID)
{
    m_entityID = entityID;
	m_xmf4x4World = Matrix4x4::Identity();
}

GameObject::~GameObject()
{
}

void GameObject::Update(float elapsedTime)
{
}

entity_id GameObject::GetEntityID() const
{
	return entity_id();
}


void GameObject::SetPosition(const XMFLOAT3& position)
{
	m_xmf4x4World._41 = position.x;
	m_xmf4x4World._42 = position.y;
	m_xmf4x4World._43 = position.z;

	//UpdateTransform(NULL);
}

const XMFLOAT3& GameObject::GetPosition() const
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
	// // O: 여기에 return 문을 삽입합니다.
}


XMFLOAT3 GameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 GameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 GameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}


void GameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World);
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

	//ComponentBase* pComponent=GetComponent(component_id::RENDER_COMPONENT);
	//if (pComponent != NULL)
	//{
	//	RenderComponent* pRenderComponent = static_cast<RenderComponent*>(pComponent);
	//	pRenderComponent->Render(pd3dDevice);
	//}
	m_pMissileTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	m_pMissileTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/stones.dds", RESOURCE_TEXTURE2D, 0);// ->텍스쳐 컴포넌트로 분리

	ComponentBase* pMeshComponent = GetComponent(component_id::CUBEMESH_COMPONENT);
	if (pMeshComponent != NULL)
	{
		CubeMeshComponent* pCubeComponent = static_cast<CubeMeshComponent*>(pMeshComponent);
		pCubeComponent->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 10, 10, 10);
		//m_pMissileTexturedMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 10.f, 10.5f, 5.f);
		//CMesh* pMeshIlluminated = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, 100.0f, 20, 20);
	}
	//->메테리얼 생성 텍스쳐와 쉐이더를 넣어야되는데 쉐이더이므로 안 넣어도 됨
	//m_pMissileMaterial = new CMaterial();
	//m_pMissileMaterial->SetTexture(m_pMissileTexture);
	//m_nObjects = 10;
	//m_ppObjects = new CGameObject * [m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);// 삭제 예정(변경)
	int nObjects = 0;//삭제 예정(변경)
	ComponentBase* pShaderComponent = GetComponent(component_id::SHADER_COMPONENT);	
	if (pShaderComponent != NULL)
	{
		ShaderComponent* PShaderComponent = static_cast<ShaderComponent*>(pShaderComponent);
		PShaderComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
		PShaderComponent->CreateCbvSrvDescriptorHeaps(pd3dDevice, 1, 1);
		PShaderComponent->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		PShaderComponent->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObjects, ncbElementBytes);
		//PShaderComponent->CreateShaderResourceViews(pd3dDevice, m_pMissileTexture, 0, 11);//texture입력
		//SetCbvGPUDescriptorHandle(PShaderComponent->GetCbvGPUDescriptorHandle());
		SetCbvGPUDescriptorHandlePtr(PShaderComponent->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
	}

	//CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void GameObject::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	//OnPrepareRender();
	//UpdateShaderVariables(pd3dCommandList);

	ComponentBase* pShaderComponent = GetComponent(component_id::SHADER_COMPONENT);
	if (pShaderComponent != NULL)
	{
		ShaderComponent* PShaderComponent = static_cast<ShaderComponent*>(pShaderComponent);
		PShaderComponent->Render(pd3dCommandList,0, pd3dGraphicsRootSignature);
		PShaderComponent->UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World);
		pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_d3dCbvGPUDescriptorHandle);
	}
	ComponentBase* pRenderComponent = GetComponent(component_id::RENDER_COMPONENT);
	ComponentBase* pMeshComponent = GetComponent(component_id::CUBEMESH_COMPONENT);
	if (pRenderComponent != NULL)
	{
		RenderComponent* pRenderComponents = static_cast<RenderComponent*>(pRenderComponent);
		CubeMeshComponent* pMeshComponents = static_cast<CubeMeshComponent*>(pMeshComponent);
		pRenderComponents->Render(pd3dCommandList, pMeshComponents);
	}

	//if (m_nMaterials > 0)
	//{
	//	for (int i = 0; i < m_nMaterials; i++)
	//	{
	//		if (m_ppMaterials[i])
	//		{
	//			if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
	//			m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
	//		}
	//		if (m_ppMaterials[i]->m_pTexture)
	//		{
	//			m_ppMaterials[i]->m_pTexture->UpdateShaderVariables(pd3dCommandList);
	//			if (m_pcbMappedGameObject) XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4Texture, XMMatrixTranspose(XMLoadFloat4x4(&m_ppMaterials[i]->m_pTexture->m_xmf4x4Texture)));
	//		}
	//		//pd3dCommandList->SetGraphicsRootDescriptorTable(13, m_d3dCbvGPUDescriptorHandle);
	//		if (m_pMesh) m_pMesh->Render(pd3dCommandList, i);
	//	}
	//}
}
void GameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}
void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObjects->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
}
void GameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
}


void GameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	GameObject::SetPosition(xmf3Position);
}

void GameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	GameObject::SetPosition(xmf3Position);
}

void GameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	GameObject::SetPosition(xmf3Position);
}

;

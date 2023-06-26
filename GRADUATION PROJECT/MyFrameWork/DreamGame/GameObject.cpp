#include "stdafx.h"
#include "GameObject.h"
#include "CAnimationSets.h"
#include "Animation.h"
#include "DepthRenderShaderComponent.h"
#include "InstanceRenderComponent.h"
#include "TrailShaderComponent.h"
#include"TerrainShaderComponent.h"
#include "Network/MapData/MapData.h"
#include "GameFramework.h"
#include "GameobjectManager.h"

extern MapData g_bossMapData;
extern CGameFramework gGameFramework;


BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;

	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';
	return(nStrLength);
}

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

GameObject::GameObject(entity_id entityID)
{
	m_entityID = entityID;
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4ToParent = Matrix4x4::Identity();
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

	m_xmf4x4ToParent._41 = position.x;
	m_xmf4x4ToParent._42 = position.y;
	m_xmf4x4ToParent._43 = position.z;

	m_SPBB = BoundingSphere(XMFLOAT3(position.x, position.y, position.z), m_fBoundingSize);
	//if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(position, m_pCamera->GetOffset()));

	UpdateTransform(NULL);
}

void GameObject::UpdateCameraPosition()
{
	if (m_pCamera)
		m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
}

const XMFLOAT3& GameObject::GetPosition() const
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
	// // O: 여기에 return 문을 삽입합니다.
}


void GameObject::SetMaterial(int nMaterial, MaterialComponent* pMaterial)
{
	if (m_ppMaterialsComponent[nMaterial]) m_ppMaterialsComponent[nMaterial]->Release();
	m_ppMaterialsComponent[nMaterial] = pMaterial;
	if (m_ppMaterialsComponent[nMaterial]) m_ppMaterialsComponent[nMaterial]->AddRef();

}

XMFLOAT3 GameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 GameObject::GetObjectLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4ToParent._31, m_xmf4x4ToParent._32, m_xmf4x4ToParent._33)));
}

void GameObject::SetLook(const XMFLOAT3& xmfLook)
{
	XMFLOAT3 xmftLook = Vector3::Normalize(xmfLook);
	XMFLOAT3 xmftUp = GetUp();
	XMFLOAT3 xmftRight = GetRight();

	xmftRight = Vector3::CrossProduct(xmftUp, xmftLook, true);
	xmftUp = Vector3::CrossProduct(xmftLook, xmftRight, true);

	xmftLook = Vector3::ScalarProduct(xmftLook, m_fScale, false);
	xmftRight = Vector3::ScalarProduct(xmftRight, m_fScale, false);
	xmftUp = Vector3::ScalarProduct(xmftUp, m_fScale, false);

	m_xmf4x4ToParent._11 = xmftRight.x;	m_xmf4x4ToParent._12 = xmftRight.y;	m_xmf4x4ToParent._13 = xmftRight.z;
	m_xmf4x4ToParent._21 = xmftUp.x;	m_xmf4x4ToParent._22 = xmftUp.y;	m_xmf4x4ToParent._23 = xmftUp.z;
	m_xmf4x4ToParent._31 = xmftLook.x;	m_xmf4x4ToParent._32 = xmftLook.y;	m_xmf4x4ToParent._33 = xmftLook.z;
}

XMFLOAT3 GameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 GameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void GameObject::SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up)
{
	XMFLOAT3 xmf3Position(m_xmf4x4ToParent._41, m_xmf4x4ToParent._42, m_xmf4x4ToParent._43);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	m_xmf4x4ToParent._11 = mtxLookAt._11; m_xmf4x4ToParent._12 = mtxLookAt._21; m_xmf4x4ToParent._13 = mtxLookAt._31;
	m_xmf4x4ToParent._21 = mtxLookAt._12; m_xmf4x4ToParent._22 = mtxLookAt._22; m_xmf4x4ToParent._23 = mtxLookAt._32;
	m_xmf4x4ToParent._31 = mtxLookAt._13; m_xmf4x4ToParent._32 = mtxLookAt._23; m_xmf4x4ToParent._33 = mtxLookAt._33;

	UpdateTransform(NULL);
}


void GameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}
void GameObject::SetinitScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void GameObject::SetScale(float fScale)
{
	m_fScale = fScale;
	XMMATRIX mtxScale = XMMatrixScaling(fScale, fScale, fScale);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void GameObject::SetTexture(wchar_t* pszFileName, int nSamplers, int nRootParameter)
{
	m_nRootParameter = nRootParameter;//텍스쳐가 시작하는 t(n)위치 
	pszFileNames = pszFileName;//파일이름
	m_nSamplers = nSamplers;//샘플러 번호
}

void GameObject::SetModel(char* pszModelName)
{
	pszModelNames = pszModelName;
}

void GameObject::SetAnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
}

void GameObject::SetMesh(MeshComponent* pMesh)
{
	if (m_pMeshComponent) m_pMeshComponent->Release();
	m_pMeshComponent = pMesh;
	if (m_pMeshComponent) m_pMeshComponent->AddRef();
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

void GameObject::SetComponent(const component_id& componentID)
{
}
ComponentBase* GameObject::GetComponent(const component_id& componentID)
{
	if (m_components.find(componentID) == m_components.end())
	{
		return NULL;//컴포넌트 없으면 그냥 나가게 해주는거 
	}
	auto it = m_components.find(componentID);
	ComponentBase* Component = it->second;
	return Component;
}


void GameObject::HandleMessage(string message)
{
}



void GameObject::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pd3dGraphicsRootSignature = pd3dGraphicsRootSignature;
	ComponentBase* pComponent = GetComponent(component_id::RENDER_COMPONENT);
	ComponentBase* pInsComponent = GetComponent(component_id::INSRENDER_COMPONENT);
	if (pComponent != NULL|| pInsComponent!=NULL)
	{
		if (pComponent)
		{
			m_pRenderComponent = static_cast<RenderComponent*>(pComponent);
		}
		else if (pInsComponent)
		{
			m_pRenderComponent = static_cast<InstanceRenderComponent*>(pComponent);
		}
	}
	ComponentBase* pTextureComponent = GetComponent(component_id::TEXTURE_COMPONENT);
	if (pTextureComponent != NULL)
	{
		m_pTextureComponent = static_cast<TextureComponent*>(pTextureComponent);
		m_pTextureComponent->BuildTexture(1, m_nSamplers, 0, 1);
		m_pTextureComponent->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pszFileNames, m_nSamplers, 0);
	}
	BuildMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	BuildShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	ComponentBase* pLoadedmodelComponent = GetComponent(component_id::LOADEDMODEL_COMPONET);
	if (pLoadedmodelComponent != NULL)
	{
		////MaterialComponent::PrepareShaders(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pd3dcbGameObjects);
		//if (m_pLoadedModelComponent == nullptr)
		//{
		//	m_pLoadedModelComponent = static_cast<CLoadedModelInfoCompnent*>(pLoadedmodelComponent);
		//	m_pLoadedModelComponent = LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		//		pd3dGraphicsRootSignature, pszModelNames, NULL, true);//NULL ->Shader
		//	SetChild(m_pLoadedModelComponent->m_pModelRootObject, true);
		//	m_pTrailStart = FindFrame("WeaponPositionStart");
		//	m_pTrailEnd = FindFrame("WeaponPositionEnd");
		//}
		//MaterialComponent::PrepareShaders(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pd3dcbGameObjects);
		if (m_pLoadedModelComponent == nullptr)
		{
			m_pLoadedModelComponent = static_cast<CLoadedModelInfoCompnent*>(pLoadedmodelComponent);
			m_pLoadedModelComponent = LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
				pd3dGraphicsRootSignature, pszModelNames, NULL, true);//NULL ->Shader
			m_pTrailStart = FindFrame("WeaponPositionStart");
			m_pTrailEnd = FindFrame("WeaponPositionEnd");
		}
		SetChild(m_pLoadedModelComponent->m_pModelRootObject, true);
		if (m_nAnimationSets != 0)
		{
			m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, m_nAnimationSets, m_pLoadedModelComponent);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void GameObject::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	ComponentBase* pCubeMeshComponent = GetComponent(component_id::CUBEMESH_COMPONENT);
	if (pCubeMeshComponent != NULL)
	{
		m_pCubeComponent = static_cast<CubeMeshComponent*>(pCubeMeshComponent);
		m_pCubeComponent->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 1, 1, 1);
		//m_pCubeComponent->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_bossMapData.GetTriangleMesh());
		m_pMeshComponent = m_pCubeComponent;
	}
	ComponentBase* pSkyMeshComponent = GetComponent(component_id::SKYBOXMESH_COMPONENT);
	if (pSkyMeshComponent != NULL)
	{
		m_pSkyboxComponent = static_cast<SkyBoxMeshComponent*>(pSkyMeshComponent);
		m_pSkyboxComponent->BuildObject(pd3dDevice, pd3dCommandList, 200.0f, 200.0f, 200.0f);
		m_pMeshComponent = m_pSkyboxComponent;
	}
	ComponentBase* pUIMeshComponent = GetComponent(component_id::UIMESH_COMPONENT);
	if (pUIMeshComponent != NULL)
	{
		m_pUiComponent = static_cast<UIMeshComponent*>(pUIMeshComponent);
		m_pUiComponent->BuildObject(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 2.0f, 2.0f);
		m_pMeshComponent = m_pUiComponent;
	}
	ComponentBase* pSphereMeshComponent = GetComponent(component_id::SPHEREMESH_COMPONENT);
	if (pSphereMeshComponent != NULL)
	{
		m_pSphereComponent = static_cast<SphereMeshComponent*>(pSphereMeshComponent);
		m_pSphereComponent->BuildObject(pd3dDevice, pd3dCommandList, m_fBoundingSize, 20, 20);
		m_pMeshComponent = m_pSphereComponent;
	}
	ComponentBase* pTrailMeshComponent = GetComponent(component_id::TRAILMESH_COMPONENT);
	if (pTrailMeshComponent != NULL)
	{
		m_pTrailMeshComponent = static_cast<TrailMeshComponent*>(pTrailMeshComponent);
		m_pTrailMeshComponent->BuildObject(pd3dDevice, pd3dCommandList, 24*8*6);
		m_pMeshComponent = m_pTrailMeshComponent;
	}
	ComponentBase* pHeightMapMeshComponent = GetComponent(component_id::HEIGHTMESH_COMPONENT);
	if (pHeightMapMeshComponent != NULL)
	{
		m_pHeihtMapMeshComponent = static_cast<HeihtMapMeshComponent*>(pHeightMapMeshComponent);
		XMFLOAT3 xmf3Scale(1.0f, 1.0f, 1.0f);
		XMFLOAT4 xmf4Color(0.3f, 1.0f, 0.0f, 0.0f);
		m_pHeightMapImage = new CHeightMapImage(m_pFileName, 257, 257, xmf3Scale);
		m_pHeihtMapMeshComponent->BuildObject(pd3dDevice, pd3dCommandList,0,0, 257, 257, xmf3Scale, xmf4Color, m_pHeightMapImage);
		m_pMeshComponent = m_pHeihtMapMeshComponent;
	}
}

void GameObject::BuildShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	//->메테리얼 생성 텍스쳐와 쉐이더를 넣어야되는데 쉐이더이므로 안 넣어도 됨
	ComponentBase* pShaderComponent = GetComponent(component_id::SHADER_COMPONENT);
	ComponentBase* pSkyShaderComponent = GetComponent(component_id::SKYSHADER_COMPONENT);
	ComponentBase* pUiShaderComponent = GetComponent(component_id::UISHADER_COMPONENT);
	ComponentBase* pBlendingUiShaderComponent = GetComponent(component_id::BLENDINGUISHADER_COMPONENT);
	ComponentBase* pSpriteShaderComponent = GetComponent(component_id::SPRITESHADER_COMPONENT);
	ComponentBase* pBoundingBoxShaderComponent = GetComponent(component_id::BOUNDINGBOX_COMPONENT);
	ComponentBase* pNaviMeshShaderComponent = GetComponent(component_id::NAVIMESHSHADER_COMPONENT);
	ComponentBase* pTrailShaderComponent = GetComponent(component_id::TRAILSHADER_COMPONENT);
	ComponentBase* pTerrainShaderComponent = GetComponent(component_id::TERRAINSHADER_COMPONENT);
	if (pShaderComponent != NULL || pSkyShaderComponent != NULL || pUiShaderComponent != NULL || pSpriteShaderComponent != NULL 
		|| pBoundingBoxShaderComponent != NULL || pBlendingUiShaderComponent != NULL|| pTrailShaderComponent!=NULL
		|| pTerrainShaderComponent!=NULL)
	{
		if (pShaderComponent != NULL)
		{
			m_pShaderComponent = static_cast<ShaderComponent*>(pShaderComponent);
		}
		else if (pSkyShaderComponent != NULL) {
			m_pShaderComponent = static_cast<SkyBoxShaderComponent*>(pSkyShaderComponent);
		}
		else if (pUiShaderComponent != NULL) {
			m_pShaderComponent = static_cast<UiShaderComponent*>(pUiShaderComponent);
		}
		else if (pBlendingUiShaderComponent != NULL) {
			m_pShaderComponent = static_cast<BlendingUiShaderComponent*>(pBlendingUiShaderComponent);
		}
		else if (pSpriteShaderComponent != NULL) {
			m_pShaderComponent = static_cast<MultiSpriteShaderComponent*>(pSpriteShaderComponent);
		}
		else if (pBoundingBoxShaderComponent != NULL) {
			m_pShaderComponent = static_cast<BoundingBoxShaderComponent*>(pBoundingBoxShaderComponent);
		}
		else if (pTrailShaderComponent != NULL) {
			m_pShaderComponent = static_cast<TrailShaderComponent*>(pTrailShaderComponent);
		}
		else if (pTerrainShaderComponent != NULL) {
			m_pShaderComponent = static_cast<TrailShaderComponent*>(pTerrainShaderComponent);
		}
		else if (pNaviMeshShaderComponent != NULL)
		{
			m_pShaderComponent = static_cast<SphereShaderComponent*>(pNaviMeshShaderComponent);
			m_pShaderComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
			m_pShaderComponent->CreateCbvSrvDescriptorHeaps(pd3dDevice, 1, 0);
			m_pShaderComponent->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			m_pShaderComponent->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObjects, ncbElementBytes);
			m_pShaderComponent->SetCbvGPUDescriptorHandlePtr(m_pShaderComponent->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
		}
		m_pShaderComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
		m_pShaderComponent->CreateCbvSrvDescriptorHeaps(pd3dDevice, 2, 2);
		m_pShaderComponent->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_pShaderComponent->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObjects, ncbElementBytes);
		if (m_pTextureComponent)
		{
			m_pShaderComponent->CreateShaderResourceViews(pd3dDevice, m_pTextureComponent, 0, m_nRootParameter, pShadowMap);//texture입력
		}
		m_pShaderComponent->SetCbvGPUDescriptorHandlePtr(m_pShaderComponent->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
	}
}

void GameObject::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	UpdateShaderVariables(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World, NULL);

	if (m_pSkinnedAnimationController)
		m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pShaderComponent != NULL)
	{
		m_pShaderComponent->Render(pd3dCommandList, 0, pd3dGraphicsRootSignature, bPrerender);
		m_pShaderComponent->UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World, NULL);
		pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_pShaderComponent->GetCbvGPUDescriptorHandle());
		if (m_pTextureComponent != NULL)
		{
			m_pTextureComponent->UpdateShaderVariables(pd3dCommandList);
		}
	}
	if (m_pRenderComponent != NULL && m_pMeshComponent != NULL && m_ppMaterialsComponent == NULL && m_pLoadedModelComponent == NULL)
	{
		m_pRenderComponent->Render(pd3dCommandList, m_pMeshComponent, 0);//수정필요
	}
	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterialsComponent[i])
			{
				if (m_ppMaterialsComponent[i]->m_pShader)
				{
					m_ppMaterialsComponent[i]->m_pShader->Render(pd3dCommandList, 0, pd3dGraphicsRootSignature, bPrerender);
					pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_ppMaterialsComponent[i]->m_pShader->GetCbvGPUDescriptorHandle());
					m_ppMaterialsComponent[i]->m_pShader->UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World, m_ppMaterialsComponent[i]);
					//m_ppMaterialsComponent[i]->UpdateShaderVariable(pd3dCommandList);
				}
			}
			m_pRenderComponent->Render(pd3dCommandList, m_pMeshComponent, i);
		}
	}
	if (m_pSibling) m_pSibling->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
	if (m_pChild) m_pChild->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void GameObject::InstanceRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nObjects,bool bPrerender)
{

	UpdateShaderVariables(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World, NULL);
	if (m_pSkinnedAnimationController)
		m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pShaderComponent != NULL)
	{
		m_pShaderComponent->Render(pd3dCommandList, 0, pd3dGraphicsRootSignature, bPrerender);
		m_pShaderComponent->UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World, NULL);
		pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_pShaderComponent->GetCbvGPUDescriptorHandle());
		if (m_pTextureComponent != NULL)
		{
			m_pTextureComponent->UpdateShaderVariables(pd3dCommandList);
		}
	}
	if (m_pInstanceRenderComponent != NULL && m_pMeshComponent != NULL && m_ppMaterialsComponent == NULL && m_pLoadedModelComponent == NULL)
	{
		m_pInstanceRenderComponent->Render(pd3dCommandList, m_pMeshComponent, 0,nObjects);//수정필요
	}
	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterialsComponent[i])
			{
				if (m_ppMaterialsComponent[i]->m_pShader)
				{
					m_ppMaterialsComponent[i]->m_pShader->Render(pd3dCommandList, 0, pd3dGraphicsRootSignature, bPrerender);
					pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_ppMaterialsComponent[i]->m_pShader->GetCbvGPUDescriptorHandle());
					m_ppMaterialsComponent[i]->m_pShader->UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World, m_ppMaterialsComponent[i]);
					//m_ppMaterialsComponent[i]->UpdateShaderVariable(pd3dCommandList);
				}
			}
			m_pRenderComponent->InstanceRender(pd3dCommandList, m_pMeshComponent, i, nObjects);
		}
	}
	if (m_pSibling) m_pSibling->InstanceRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, nObjects,bPrerender);
	if (m_pChild) m_pChild->InstanceRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, nObjects,bPrerender);
}

void GameObject::ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent)
{
	UpdateShaderVariables(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World, NULL);
	if (m_pSkinnedAnimationController)
		m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);


	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterialsComponent[i])
			{
				if (m_ppMaterialsComponent[i]->m_pShader)
				{
					if (m_ppMaterialsComponent[i]->m_isBoundingBoxShader) {
						m_ppMaterialsComponent[i]->m_pShader->Render(pd3dCommandList, 0, pd3dGraphicsRootSignature, false);
					}
					else
					{
						m_ppMaterialsComponent[i]->m_pShader->Render(pd3dCommandList, 0, pd3dGraphicsRootSignature, false);
						pShaderComponent->SetPipelineState(pd3dCommandList, 0);
					}
					pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_ppMaterialsComponent[i]->m_pShader->GetCbvGPUDescriptorHandle());
					m_ppMaterialsComponent[i]->m_pShader->UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World, m_ppMaterialsComponent[i]);
					//m_ppMaterialsComponent[i]->UpdateShaderVariable(pd3dCommandList);
				}
			}
			m_pRenderComponent->Render(pd3dCommandList, m_pMeshComponent, i);
		}
	}
	if (m_pSibling) m_pSibling->ShadowRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender, pShaderComponent);
	if (m_pChild) m_pChild->ShadowRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender, pShaderComponent);

}

void GameObject::Animate(float fTimeElapsed)
{
	if(m_pCamera) UpdateCameraPosition();
	if (m_pSkinnedAnimationController)
	{
		m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);
		if(m_VisualizeSPBB) m_VisualizeSPBB->SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y + m_fBoundingSize, GetPosition().z));
	}
	
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}

void GameObject::AnimateRowColumn(float fTimeElapsed)
{
	m_fTime += fTimeElapsed * 0.5f;
	if (m_fTime >= m_fSpeed) m_fTime = 0.0f;

	m_xmf4x4Texture._11 = 1.0f / float(m_nRows);
	m_xmf4x4Texture._22 = 1.0f / float(m_nCols);
	m_xmf4x4Texture._31 = float(m_nRow) / float(m_nRows);
	m_xmf4x4Texture._32 = float(m_nCol) / float(m_nCols);
	if (m_fTime == 0.0f)
	{
		if (++m_nCol == m_nCols) { m_nRow++; m_nCol = 0; }
		if (m_nRow == m_nRows) m_nRow = 0;
	}
}

void GameObject::SetRimLight(bool bRimLight)
{
	m_bRimLight = bRimLight;
}

void GameObject::SetCurrentHP(float fHP)
{
	m_fHp = fHP;
}

void GameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_STAT) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);

	UINT ncbElementBytes3 = ((sizeof(CB_UIOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbUIGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes3, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbUIGameObjects->Map(0, NULL, (void**)&m_pcbMappedUIGameObjects);

	UINT ncbElementBytes2 = ((sizeof(CB_GAMEOBJECT_MULTISPRITE) + 255) & ~255); //256의 배수
	m_pd3dcbMultiSpriteGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes2, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMultiSpriteGameObjects->Map(0, NULL, (void**)&m_pcbMappedMultiSpriteGameObjects);
	

	UINT ncbElementBytes4 = ((sizeof(CB_GAMEOBJECTWORLD_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjectsWorld = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes4, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjectsWorld->Map(0, NULL, (void**)&m_pcbMappedGameObjectsWorld);
}
void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dcbGameObjects)
	{
		float mfhp=0;
		mfhp = (m_fHp / m_fMaxHp);//현재 체력값을 최대체력 비례로 나타낸식 23.04.18 .ccg
		::memcpy(&m_pcbMappedGameObjects->m_xmfHP, &mfhp, sizeof(float));
		::memcpy(&m_pcbMappedGameObjects->m_bRimLight, &m_bRimLight, sizeof(bool));
		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbGameObjects->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(17, d3dGpuVirtualAddress);
	}
	if (m_pd3dcbUIGameObjects)
	{
		::memcpy(&m_pcbMappedUIGameObjects->m_bUIActive, &m_bUIActive, sizeof(bool));
		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbUIGameObjects->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(21, d3dGpuVirtualAddress);
	}
	if (m_bMultiSprite)
	{
		XMStoreFloat4x4(&m_pcbMappedMultiSpriteGameObjects->m_xmf4x4Texture, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Texture)));
		::memcpy(&m_pcbMappedMultiSpriteGameObjects->m_bMultiSprite, &m_bMultiSprite, sizeof(bool));//멀티 스프라이트 활성화를 나타내는 코드 23.04.19 .ccg
		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress2 = m_pd3dcbMultiSpriteGameObjects->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(18, d3dGpuVirtualAddress2);
	}
}
void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World, MaterialComponent* ppMaterialsComponent)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(22, 16, &xmf4x4World, 0);
}
void GameObject::UpdateObjectVarialbes(XMFLOAT4X4* pxmf4x4World)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
}
void GameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
}

void GameObject::SetChild(GameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

GameObject* GameObject::FindFrame(char* pstrFrameName)
{
	GameObject* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void GameObject::FindAndSetSkinnedMesh(SkinnedMeshComponent** ppSkinnedMeshes, int* pnSkinnedMesh)
{
	if (m_pMeshComponent && (m_pMeshComponent->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) ppSkinnedMeshes[(*pnSkinnedMesh)++] = (SkinnedMeshComponent*)m_pMeshComponent;

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
}

TextureComponent* GameObject::FindReplicatedTexture(_TCHAR* pstrTextureName)
{
	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterialsComponent[i])
		{
			for (int j = 0; j < m_ppMaterialsComponent[i]->m_nTextures; j++)
			{
				if (m_ppMaterialsComponent[i]->m_ppTextures[j])
				{
					if (!_tcsncmp(m_ppMaterialsComponent[i]->m_ppstrTextureNames[j], pstrTextureName, _tcslen(pstrTextureName))) return(m_ppMaterialsComponent[i]->m_ppTextures[j]);
				}
			}
		}
	}
	TextureComponent* pTexture = NULL;
	if (m_pSibling) if (pTexture = m_pSibling->FindReplicatedTexture(pstrTextureName)) return(pTexture);
	if (m_pChild) if (pTexture = m_pChild->FindReplicatedTexture(pstrTextureName)) return(pTexture);

	return(NULL);
}

CLoadedModelInfoCompnent* GameObject::LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, ShaderComponent* pShader, bool isBinary)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CLoadedModelInfoCompnent* pLoadedModel = new CLoadedModelInfoCompnent();

	char pstrToken[64] = { '\0' };

	for (; ; )
	{

		if (::ReadStringFromFile(pInFile, pstrToken))
		{
			if (!strcmp(pstrToken, "<Hierarchy>:"))
			{
				pLoadedModel->m_pModelRootObject = GameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes);
				::ReadStringFromFile(pInFile, pstrToken); //"</Hierarchy>"

				
			}
			else if (!strcmp(pstrToken, "<Animation>:"))
			{
				GameObject::LoadAnimationFromFile(pInFile, pLoadedModel);
				pLoadedModel->PrepareSkinning();

			}
			else if (!strcmp(pstrToken, "</Animation>:"))
			{
				break;
			}
		}
		else
		{
			break;
		}

	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, "Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pLoadedModel);
}

void GameObject::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, GameObject* pParent, FILE* pInFile, ShaderComponent* pShader, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	char pstrToken[64] = { '\0' };
	int nMaterial = 0;
	UINT nReads = 0;

	m_nMaterials = ReadIntegerFromFile(pInFile);

	m_ppMaterialsComponent = new MaterialComponent * [m_nMaterials];
	m_pRenderComponent = new RenderComponent;
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterialsComponent[i] = NULL;

	MaterialComponent* pMaterial = NULL;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMaterial = ReadIntegerFromFile(pInFile);

			pMaterial = new MaterialComponent(7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

			if (!pShader)
			{
				UINT nMeshType = GetMeshType();
				if (nMeshType & VERTEXT_NORMAL_TANGENT_TEXTURE)
				{
					if (nMeshType & VERTEXT_BONE_INDEX_WEIGHT)
					{
						pMaterial->PrepareSkinnedShaders(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pd3dcbGameObjects);
						pMaterial->SetSkinnedAnimationShader();
					}
					else
					{
						/*		if (!strncmp(m_pstrFrameName, "Bounding", 8))
									pMaterial->SetBoundingBoxShader();
								else*/
						pMaterial->PrepareStandardShaders(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pd3dcbGameObjects);
						pMaterial->SetStandardShader();
					}
				}
			}
			SetMaterial(nMaterial, pMaterial);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			pMaterial->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_ALBEDO_MAP, 5, pMaterial->m_ppstrTextureNames[0], &(pMaterial->m_ppTextures[0]), pParent, pInFile, pMaterial->m_pShader);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			m_ppMaterialsComponent[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_SPECULAR_MAP, 6, pMaterial->m_ppstrTextureNames[1], &(pMaterial->m_ppTextures[1]), pParent, pInFile, pMaterial->m_pShader);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			m_ppMaterialsComponent[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_NORMAL_MAP, 7, pMaterial->m_ppstrTextureNames[2], &(pMaterial->m_ppTextures[2]), pParent, pInFile, pMaterial->m_pShader);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			m_ppMaterialsComponent[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_METALLIC_MAP, 8, pMaterial->m_ppstrTextureNames[3], &(pMaterial->m_ppTextures[3]), pParent, pInFile, pMaterial->m_pShader);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			m_ppMaterialsComponent[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_EMISSION_MAP, 9, pMaterial->m_ppstrTextureNames[4], &(pMaterial->m_ppTextures[4]), pParent, pInFile, pMaterial->m_pShader);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			m_ppMaterialsComponent[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_ALBEDO_MAP, 10, pMaterial->m_ppstrTextureNames[5], &(pMaterial->m_ppTextures[5]), pParent, pInFile, pMaterial->m_pShader);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			m_ppMaterialsComponent[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_NORMAL_MAP, 11, pMaterial->m_ppstrTextureNames[6], &(pMaterial->m_ppTextures[6]), pParent, pInFile, pMaterial->m_pShader);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

void GameObject::LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfoCompnent* pLoadedModel)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;
	
	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nAnimationSets = ::ReadIntegerFromFile(pInFile);
			pLoadedModel->m_pAnimationSets = new CAnimationSets(nAnimationSets);
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames = ::ReadIntegerFromFile(pInFile);
			pLoadedModel->m_pAnimationSets->m_ppAnimatedBoneFrameCaches = new GameObject * [pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames];
			pLoadedModel->m_pWeaponStart = pLoadedModel->m_pModelRootObject->FindFrame("WeaponPositionStart");
			pLoadedModel->m_pWeaponEnd = pLoadedModel->m_pModelRootObject->FindFrame("WeaponPositionEnd");
			for (int j = 0; j < pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames; j++)
			{
				::ReadStringFromFile(pInFile, pstrToken);
				pLoadedModel->m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j] = pLoadedModel->m_pModelRootObject->FindFrame(pstrToken);
				
#ifdef _WITH_DEBUG_SKINNING_BONE
				TCHAR pstrDebug[256] = { 0 };
				TCHAR pwstrAnimationBoneName[64] = { 0 };
				TCHAR pwstrBoneCacheName[64] = { 0 };
				size_t nConverted = 0;
				mbstowcs_s(&nConverted, pwstrAnimationBoneName, 64, pstrToken, _TRUNCATE);
				mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, pLoadedModel->m_ppAnimatedBoneFrameCaches[j]->m_pstrFrameName, _TRUNCATE);
				_stprintf_s(pstrDebug, 256, _T("AnimationBoneFrame:: Cache(%s) AnimationBone(%s)\n"), pwstrBoneCacheName, pwstrAnimationBoneName);
				OutputDebugString(pstrDebug);
#endif
			}
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			int nAnimationSet = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //Animation Set Name

			float fLength = ::ReadFloatFromFile(pInFile);
			int nFramesPerSecond = ::ReadIntegerFromFile(pInFile);
			int nKeyFrames = ::ReadIntegerFromFile(pInFile);

			pLoadedModel->m_pAnimationSets->m_pAnimationSets[nAnimationSet] = new CAnimationSet(fLength, nFramesPerSecond, nKeyFrames, pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pstrToken);

			for (int i = 0; i < nKeyFrames; i++)
			{
				::ReadStringFromFile(pInFile, pstrToken);
				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					CAnimationSet* pAnimationSet = pLoadedModel->m_pAnimationSets->m_pAnimationSets[nAnimationSet];

					int nKey = ::ReadIntegerFromFile(pInFile); //i
					float fKeyTime = ::ReadFloatFromFile(pInFile);

#ifdef _WITH_ANIMATION_SRT
					m_pfKeyFrameScaleTimes[i] = fKeyTime;
					m_pfKeyFrameRotationTimes[i] = fKeyTime;
					m_pfKeyFrameTranslationTimes[i] = fKeyTime;
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf3KeyFrameScales[i], sizeof(XMFLOAT3), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4KeyFrameRotations[i], sizeof(XMFLOAT4), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf3KeyFrameTranslations[i], sizeof(XMFLOAT3), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
#else
					pAnimationSet->m_pfKeyFrameTimes[i] = fKeyTime;
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i], sizeof(XMFLOAT4X4), pLoadedModel->m_pAnimationSets->m_nAnimatedBoneFrames, pInFile);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
		}
	}
}

GameObject* GameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, GameObject* pParent, FILE* pInFile, ShaderComponent* pShader, int* pnSkinnedMeshes)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	GameObject* pGameObject = new GameObject(UNDEF_ENTITY);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ::ReadIntegerFromFile(pInFile);
			nTextures = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pGameObject->m_pstrFrameName);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			StandardMeshComponent* pMesh = new StandardMeshComponent();
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			SkinnedMeshComponent* pSkinnedMesh = new SkinnedMeshComponent();
			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pGameObject->SetMesh(pSkinnedMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, pd3dGraphicsRootSignature);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					GameObject* pChild = GameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader, pnSkinnedMeshes);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

#define _WITH_RAY_BY_TRANSFORM
void GameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4* pxmf4x4World, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	//XMFLOAT4X4 xmf4x4WorldView = (pxmf4x4World) ? Matrix4x4::Multiply(*pxmf4x4World, xmf4x4View) : xmf4x4View;
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(*pxmf4x4World);

#ifdef _WITH_RAY_BY_TRANSFORM
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
#else
	pxmf3PickRayDirection->x = xmf3PickPosition.x * xmf4x4Inverse._11 + xmf3PickPosition.y * xmf4x4Inverse._21 + xmf3PickPosition.z * xmf4x4Inverse._31;
	pxmf3PickRayDirection->y = xmf3PickPosition.x * xmf4x4Inverse._12 + xmf3PickPosition.y * xmf4x4Inverse._22 + xmf3PickPosition.z * xmf4x4Inverse._32;
	pxmf3PickRayDirection->z = xmf3PickPosition.x * xmf4x4Inverse._13 + xmf3PickPosition.y * xmf4x4Inverse._23 + xmf3PickPosition.z * xmf4x4Inverse._33;
	pxmf3PickRayOrigin->x = xmf4x4Inverse._41;
	pxmf3PickRayOrigin->y = xmf4x4Inverse._42;
	pxmf3PickRayOrigin->z = xmf4x4Inverse._43;
	*pxmf3PickRayDirection = Vector3::Normalize(*pxmf3PickRayDirection);
#endif

}

int GameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMeshComponent)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		GenerateRayForPicking(xmf3PickPosition, &m_xmf4x4World, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);

		nIntersected = m_pMeshComponent->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

void GameObject::SetFileName(LPCTSTR pFileName)
{
	m_pFileName = pFileName;
}




void GameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	if (Vector3::Length(xmf3Position) < PLAYER_MAX_RANGE)	GameObject::SetPosition(xmf3Position);

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
	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, m_interpolationDistance * fDistance / 50.0f);
	//if (Vector3::Length(xmf3Position) < PLAYER_MAX_RANGE)	GameObject::SetPosition(xmf3Position);
	vector<GameObject*> tempVector = gGameFramework.GetScene()->GetObjectManager()->GetObstacle();
	XMVECTOR tempPoint = XMVectorSet(xmf3Position.x, xmf3Position.y, xmf3Position.z, 0.0f);
	for (int i = 0; i < tempVector.size(); ++i)
	{
		if (tempVector[i]->m_OBB.Contains(tempPoint))
		{
			// cout << "충돌 발생 : " << i << "번째 바위와 충돌하였습니다." << endl;
			// cout << "바위 Center Position : " << tempVector[i]->GetPosition().x <<", " << tempVector[i]->GetPosition().y << ", " << tempVector[i]->GetPosition().z << " )" << endl;
			// cout << "충돌 포지션 : ( "<< xmf3Position.x <<", " << xmf3Position.y << ", " << xmf3Position.z << " )" << endl;
			return;
		}
	}
	GameObject::SetPosition(xmf3Position);
}

void GameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void GameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void GameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}


void GameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	// m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void GameObject::Move(DIRECTION direction, float fDistance)
{
}

void GameObject::Move(XMFLOAT3 direction, float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, direction, fDistance);
	if (Vector3::Length(xmf3Position) < PLAYER_MAX_RANGE)	GameObject::SetPosition(xmf3Position);
}

void GameObject::MoveDiagonal(int fowardDirection, int rightDirection, float distance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Look = Vector3::ScalarProduct(xmf3Look, fowardDirection, false);
	xmf3Right = Vector3::ScalarProduct(xmf3Right, rightDirection, false);

	XMFLOAT3 resDirection = Vector3::Add(xmf3Right, xmf3Look);
	resDirection = Vector3::Normalize(resDirection);
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(resDirection, distance));
	xmf3Position = Vector3::Add(xmf3Position, m_interpolationVector, m_interpolationDistance * distance / 50.0f);
	if (Vector3::Length(xmf3Position) < PLAYER_MAX_RANGE)
	{
		GameObject::SetPosition(xmf3Position);
		if (m_pCamera) m_pCamera->SetPosition(Vector3::Add(xmf3Position, m_pCamera->GetOffset()));
	}
}

bool GameObject::CheckIntersect(const GameObject* GameObject)	//수정필요
{
	if (m_SPBB.Intersects(GameObject->m_SPBB))
	{
#ifdef _DEBUG
		cout << m_pChild->m_pstrFrameName << "와 " << GameObject->m_pChild->m_pstrFrameName << "가 충돌했습니다." << endl;
#endif
		return true;
	}
	return false;
}

void GameObject::SetCamera(CCamera* pCamera)
{
	m_pCamera = pCamera;
	m_pCamera->SetTimeLag(0.25f);
	m_pCamera->SetOffset(XMFLOAT3(0.0f, 25.0f, -50.0f));
	m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	m_pCamera->SetPosition(Vector3::Add(GetPosition(), m_pCamera->GetOffset()));
}

void GameObject::SetRowColumn(float nRows, float nCols, float fSpeed)
{
	m_bMultiSprite = true;
	m_nRows = nRows;
	m_nCols = nCols;
	m_fSpeed = fSpeed;
}

#define PI 3.14159265359

void GameObject::MoveObject(DIRECTION& currentDirection, const XMFLOAT3& CameraAxis) // currentDirection : 향한 방향 
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(CameraAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);

	XMFLOAT3 xmf3Rev = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fRotateAngle = -1.0f;

	DIRECTION tempDir = currentDirection;

	if (m_bRButtonClicked)
	{
		SetLook(xmf3Look);
		return;
	}

	if (tempDir != DIRECTION::IDLE)
	{
		if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
			((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
		{
			tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
			tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
		}
		if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
			((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
		{
			tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
			tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
		}
		switch (tempDir)
		{
		case DIRECTION::FRONT:						fRotateAngle = 0.0f;	break;
		case DIRECTION::LEFT | DIRECTION::FRONT:	fRotateAngle = 45.0f;	break;
		case DIRECTION::LEFT:						fRotateAngle = 90.0f;	break;
		case DIRECTION::BACK | DIRECTION::LEFT:		fRotateAngle = 135.0f;	break;
		case DIRECTION::BACK:						fRotateAngle = 180.0f;	break;
		case DIRECTION::RIGHT | DIRECTION::BACK:	fRotateAngle = 225.0f;	break;
		case DIRECTION::RIGHT:						fRotateAngle = 270.0f;	break;
		case DIRECTION::FRONT | DIRECTION::RIGHT:	fRotateAngle = 315.0f;	break;
		default:
			tempDir = DIRECTION::IDLE;
			MoveObject(tempDir, CameraAxis);
			return;
		}

		fRotateAngle = fRotateAngle * (PI / 180.0f);
		xmf3Rev.x = xmf3Look.x * cos(fRotateAngle) - xmf3Look.z * sin(fRotateAngle);
		xmf3Rev.z = xmf3Look.x * sin(fRotateAngle) + xmf3Look.z * cos(fRotateAngle);
		xmf3Rev = Vector3::Normalize(xmf3Rev);
	}

	if ((xmf3Rev.x || xmf3Rev.y || xmf3Rev.z))
	{
		SetLook(xmf3Rev);
	}
}

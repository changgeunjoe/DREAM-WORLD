#pragma once
#include"stdafx.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
#include"MeshComponent.h"
#include"TextureComponent.h"
#include"ShadowShaderComponent.h"
class ComponentBase;

class CKeyInput {
public:
	CKeyInput() {};
	~CKeyInput() {};
	bool m_bWKey = false;
	bool m_bAKey = false;
	bool m_bSKey = false;
	bool m_bDKey = false;
	bool m_bEKey = false;
	bool m_bQKey = false;
	bool m_bLeftMouse_Button = false;
public:
	bool IsAllMovekeyUp() {
		return!(m_bWKey || m_bAKey || m_bSKey || m_bDKey);
	}
};

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
	void SetTexture(wchar_t* pszFileName);


	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void MoveDiagonal(int fowardDirection, int rightDirection, float distance = 1.0f);

	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
    void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

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
	T* ComponentType(component_id& componentID);

	virtual void OnPrepareRender() { }

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }
public:
	XMFLOAT4X4						m_xmf4x4Transform;//변환 행렬
	XMFLOAT4X4						m_xmf4x4World; //월드 행렬

	CKeyInput* m_KeyInput = new CKeyInput();
protected:
	entity_id m_entityID{};//object id 
	XMFLOAT3 m_position{};
	//Quaternion m_orientation;

	wchar_t* pszFileNames{};

	//////////////////////Component/////////////////////////////////
	unordered_map<component_id, ComponentBase*> m_components;

	TextureComponent* m_pTextureComponent{ NULL };
	CubeMeshComponent* m_pCubeComponent{ NULL };
	ShaderComponent* m_pShaderComponent{ NULL };
	RenderComponent* m_pRenderComponent{ NULL };
protected:
	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	ID3D12Resource* pShadowMap = NULL;
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);// 삭제 예정(변경)
	int nObjects = 0;//삭제 예정(변경)
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
inline T* GameObject::ComponentType(component_id& componentID)
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
	else if (typeid(T).name() == typeid(TextureComponent).name())
	{
		componentID = component_id::TEXTURE_COMPONENT;
	}
	else
	{
		componentID = component_id::UNDEF_COMPONENT;
	}
	return 0;
}

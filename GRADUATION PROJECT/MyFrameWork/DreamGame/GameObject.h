#pragma once
#include"stdafx.h"
#include"RenderComponent.h"
#include"ShaderComponent.h"
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

    unordered_map<component_id, ComponentBase*> Getcomponents();

   // void SetOrientation(const Quaternion& orientation);
    //const Quaternion& GetOrientation() const;
    void SetComponent(const component_id& componentID);
    ComponentBase* GetComponent(const component_id& componentID);

    void HandleMessage(string message);

    void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    template<typename T>
    T* InsertComponent();
    template<typename T>
    T* ComponentType(component_id componentID);

protected:
    entity_id m_entityID;//object id 

    XMFLOAT3 m_position;
    //Quaternion m_orientation;

    unordered_map<component_id, ComponentBase*> m_components;
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
inline T* GameObject::ComponentType(component_id componentID)
{
    if (typeid(T).name() == typeid(RenderComponent).name())
    {
        componentID = component_id::RENDER_COMPONENT;
    }
    else if (typeid(T).name() == typeid(MeshComponent).name())
    {
        componentID = component_id::MESH_COMPONENT;
    }
    else if (typeid(T).name() == typeid(PhysicsComponent).name())
    {
        componentID = component_id::PHYSICS_COMPONENT;
    }
    else
    {
        componentID = component_id::UNDEF_COMPONENT;
    }
    return 0;
}

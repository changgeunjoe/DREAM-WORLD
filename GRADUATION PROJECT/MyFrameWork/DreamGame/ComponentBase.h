#pragma once
#include"stdafx.h"
#include"Camera.h"
class GameObject;
class ComponentBase
{
public:
    ComponentBase() : m_pOwner(NULL) {}
    virtual ~ComponentBase() {}

    virtual const component_id& GetComponentID();
    virtual const component_id& GetFamilyID();

    virtual void Update(float elapsedTime) {}

    virtual void SetOwner(GameObject* pOwner);
    GameObject* GetOwner() const;

    virtual void HandleMessage(Message message);

protected:
    GameObject* m_pOwner;
    component_id m_pComponentID;
};

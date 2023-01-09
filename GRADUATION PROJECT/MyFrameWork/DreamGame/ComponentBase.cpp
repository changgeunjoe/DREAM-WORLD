#include "ComponentBase.h"



const component_id& ComponentBase::GetComponentID()
{
	return;
}

const component_id& ComponentBase::GetFamilyID()
{
	return;
}

void ComponentBase::SetOwner(GameObject* pOwner)
{
	m_pOwner = pOwner;
}

GameObject* ComponentBase::GetOwner() const
{
	return m_pOwner;
}

void ComponentBase::HandleMessage(Message message)
{
}
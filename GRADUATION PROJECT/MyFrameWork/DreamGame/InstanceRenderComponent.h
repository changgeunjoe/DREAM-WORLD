#pragma once
#include "RenderComponent.h"
class InstanceRenderComponent : public RenderComponent
{
public:
	InstanceRenderComponent();
	~InstanceRenderComponent();

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, MeshComponent* meshcomponent, int nSubSet, UINT nInstances);


};


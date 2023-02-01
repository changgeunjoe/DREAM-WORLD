#include "stdafx.h"
#include "RenderComponent.h"

RenderComponent::RenderComponent()
{
	
}
RenderComponent::~RenderComponent()
{

}

void RenderComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList,MeshComponent* meshcomponent)
{
	pd3dCommandList->IASetPrimitiveTopology(meshcomponent->GetPrimitveTopology());
	pd3dCommandList->IASetVertexBuffers(meshcomponent->GetSlot(), 1, &meshcomponent->GetVertexBufferView());
	if (meshcomponent->GetIndexBufferResource())
	{
		pd3dCommandList->IASetIndexBuffer(&meshcomponent->GetIndexBufferView());
		pd3dCommandList->DrawIndexedInstanced(meshcomponent->GetIndices(), 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(meshcomponent->GetVertices(), 1, meshcomponent->GetOffset(), 0);
	}
}

void RenderComponent::HandleMessage(Message message)
{
	int i = 0;
	switch (message)
	{
	default:
		break;
	}
}

;

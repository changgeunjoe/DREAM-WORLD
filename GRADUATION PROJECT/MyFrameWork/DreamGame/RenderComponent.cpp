#include "RenderComponent.h"

RenderComponent::RenderComponent()
{

}
RenderComponent::~RenderComponent()
{

}

void RenderComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList, MeshComponent* meshcomponent, int nSubSet)
{
	meshcomponent->UpdateShaderVariables(pd3dCommandList);
	meshcomponent->OnPreRender(pd3dCommandList, NULL);
	//pd3dCommandList->IASetVertexBuffers(meshcomponent->GetSlot(), 1, &meshcomponent->GetVertexBufferView());
	pd3dCommandList->IASetPrimitiveTopology(meshcomponent->GetPrimitveTopology());
	if (meshcomponent->GetIndexBufferResource())
	{
		pd3dCommandList->IASetIndexBuffer(&meshcomponent->GetIndexBufferView());
		pd3dCommandList->DrawIndexedInstanced(meshcomponent->GetIndices(), 1, 0, 0, 0);
	}
	else if ((meshcomponent->GetSubMeshes() > 0) && (nSubSet < meshcomponent->GetSubMeshes()))
	{
		pd3dCommandList->IASetIndexBuffer(&meshcomponent->GetSubsetIndexBufferView(nSubSet));
		pd3dCommandList->DrawIndexedInstanced(meshcomponent->GetSubsetIndices(nSubSet), 1, 0, 0, 0);
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

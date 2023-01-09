#include "RenderComponent.h"

RenderComponent::RenderComponent()
{
	
}
RenderComponent::~RenderComponent()
{

}

void RenderComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
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

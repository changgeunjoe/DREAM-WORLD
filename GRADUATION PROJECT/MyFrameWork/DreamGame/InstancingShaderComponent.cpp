#include "stdafx.h"
#include "InstancingShaderComponent.h"
#include "GameObject.h"
InstancingShaderComponent::InstancingShaderComponent()
{

}
InstancingShaderComponent::~InstancingShaderComponent()
{
}

D3D12_INPUT_LAYOUT_DESC InstancingShaderComponent::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}
//ss
D3D12_SHADER_BYTECODE InstancingShaderComponent::CreateVertexShader(int nPipelineState)
{
	return(CompileShaderFromFile(L"Shaders.hlsl", "VSInstancing", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE InstancingShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(CompileShaderFromFile(L"Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}
void InstancingShaderComponent::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
sizeof(VS_VB_INSTANCE) * m_ppObjects.size(), D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedInsGameObjects);
}
void InstancingShaderComponent::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(19, m_pd3dcbGameObjects->GetGPUVirtualAddress());
	for (int j = 0; j < m_ppObjects.size(); j++)
	{
		XMStoreFloat4x4(&m_pcbMappedInsGameObjects[j].m_xmf4x4Transform, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}
void InstancingShaderComponent::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, vector<GameObject*>& pObjects)
{
	m_ppObjects = pObjects;
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
}
void InstancingShaderComponent::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	ShaderComponent::Render(pd3dCommandList, nPipelineState, pd3dGraphicsRootSignature, false);

	//±íÀÌ¹öÆÛ update
	UpdateShaderVariables(pd3dCommandList);

	//m_ppObjects[0]->
	//for (int i = 0; i < m_ppObjects.size(); i++) {
	//	m_ppObjects[i]->ShadowRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, true, this);
	//}

}
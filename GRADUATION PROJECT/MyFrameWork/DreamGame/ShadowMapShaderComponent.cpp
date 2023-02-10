#include "stdafx.h"
#include "ShadowMapShaderComponent.h"

ShadowMapShaderComponent::ShadowMapShaderComponent()
{
    
}

ShadowMapShaderComponent::~ShadowMapShaderComponent()
{
}

D3D12_DEPTH_STENCIL_DESC ShadowMapShaderComponent::CreateDepthStencilState(int nPipelineState)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE ShadowMapShaderComponent::CreateVertexShader( int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shadow.hlsl", "VSShadowMapShadow", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE ShadowMapShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shadow.hlsl", "PSShadowMapShadow", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void ShadowMapShaderComponent::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void ShadowMapShaderComponent::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pDepthTexture) m_pDepthTexture->UpdateShaderVariables(pd3dCommandList);
}

void ShadowMapShaderComponent::ReleaseShaderVariables()
{
}

void ShadowMapShaderComponent::BuildShadow(ShaderComponent* pObjectsShader)
{
	m_pObjectsShader = pObjectsShader;
}

void ShadowMapShaderComponent::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	m_pDepthTexture = (TextureComponent*)pContext;
	m_pDepthTexture->AddRef();

	//CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, m_pDepthTexture->GetTextures()); //씬에서?
	//CScene::CreateShaderResourceViews(pd3dDevice, m_pDepthTexture, RP_DEPTH_BUFFER, false);
	CreateShaderResourceViews(pd3dDevice, m_pDepthTexture, RP_DEPTH_BUFFER, false);//중요

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void ShadowMapShaderComponent::ReleaseObjects()
{
	if (m_pDepthTexture) m_pDepthTexture->Release();
}

void ShadowMapShaderComponent::ReleaseUploadBuffers()
{
}

void ShadowMapShaderComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	ShaderComponent::Render(pd3dCommandList,nPipelineState, pd3dGraphicsRootSignature);

	//깊이버퍼 update
	UpdateShaderVariables(pd3dCommandList);

	//for (int i = 0; i < m_pObjectsShader->m_
	// ; i++)//중요
	//{
	//	if (m_pObjectsShader->m_ppObjects[i])
	//	{
	//		m_pObjectsShader->m_ppObjects[i]->UpdateShaderVariables(pd3dCommandList);
	//		m_pObjectsShader->m_ppObjects[i]->Render(pd3dCommandList, pCamera);
	//	}
	//}

	//m_pPlayer->UpdateShaderVariables(pd3dCommandList); // ?????
	//m_pPlayer->UpdateShaderVariable(pd3dCommandList, &m_pPlayer->m_xmf4x4World);
	//m_pPlayer->Render(pd3dCommandList, pCamera); //쉐이더 렌더에서 파이프라인상태 바꾸지 않기위함
	//m_pPlayer->MeshRender(pd3dCommandList, pCamera);

}

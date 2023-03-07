#include "stdafx.h"
#include "TextureToViewportComponent.h"

TextureToViewportComponent::TextureToViewportComponent()
{
}

TextureToViewportComponent::~TextureToViewportComponent()
{
}

D3D12_DEPTH_STENCIL_DESC TextureToViewportComponent::CreateDepthStencilState(int nPipelineState)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
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

D3D12_SHADER_BYTECODE TextureToViewportComponent::CreateVertexShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "VSTextureToViewport", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE TextureToViewportComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "PSTextureToViewport", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void TextureToViewportComponent::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState)
{
}

void TextureToViewportComponent::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void TextureToViewportComponent::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
}

void TextureToViewportComponent::ReleaseObjects()
{
}

void TextureToViewportComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	float fSize = FRAME_BUFFER_WIDTH / 4;
	D3D12_VIEWPORT d3dViewport = { 0.0f, 0.0f, fSize, fSize, 0.0f, 1.0f };
	D3D12_RECT d3dScissorRect = { 0, 0, fSize, fSize };
	pd3dCommandList->RSSetViewports(1, &d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &d3dScissorRect);

	ShaderComponent::Render(pd3dCommandList, 0, pd3dGraphicsRootSignature,false);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
}

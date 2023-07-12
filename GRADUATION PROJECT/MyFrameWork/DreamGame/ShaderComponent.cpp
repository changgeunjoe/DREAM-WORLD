#include "stdafx.h"
#include "ShaderComponent.h"
#include "MaterialComponent.h"


ShaderComponent::ShaderComponent()
{
}

ShaderComponent::~ShaderComponent()
{
}

D3D12_INPUT_LAYOUT_DESC ShaderComponent::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC ShaderComponent::CreateRasterizerState(int nPipelineState)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	// d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC ShaderComponent::CreateBlendState(int nPipelineState)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);

}

D3D12_DEPTH_STENCIL_DESC ShaderComponent::CreateDepthStencilState(int nPipelineState)
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

D3D12_STREAM_OUTPUT_DESC ShaderComponent::CreateStreamOuputState(int nPipelineState)
{
	return D3D12_STREAM_OUTPUT_DESC();
}

D3D12_SHADER_BYTECODE ShaderComponent::CreateVertexShader(int nPipelineState)
{
	if (nPipelineState == 0)
		return(CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", &m_pd3dVertexShaderBlob));
	else
		return(CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", &m_pd3dVertexShaderBlob));
}
D3D12_SHADER_BYTECODE ShaderComponent::CreatePixelShader(int nPipelineState)
{
	if (nPipelineState == 0)
		return(CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", &m_pd3dPixelShaderBlob));
	else
		return(CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_SHADER_BYTECODE ShaderComponent::CreateGeometryShader(int nPipelineState)
{
	return D3D12_SHADER_BYTECODE();
}

D3D12_SHADER_BYTECODE ShaderComponent::CreateHullShader(int nPipelineState)
{
	return D3D12_SHADER_BYTECODE();
}

void ShaderComponent::SetName(string name)
{
	ShaderName = name;
}

D3D12_SHADER_BYTECODE ShaderComponent::CreateDomainShader(int nPipelineState)
{
	return D3D12_SHADER_BYTECODE();
}

D3D12_SHADER_BYTECODE ShaderComponent::CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	char* pErrorString = NULL;
	if (pd3dErrorBlob) pErrorString = (char*)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}
#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif
D3D12_SHADER_BYTECODE ShaderComponent::ReadCompiledShaderFromFile(WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE* pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE* pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE* pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char*)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

void ShaderComponent::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	//m_nPipelineStates = 1;
	//m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];//파이프라인을 배열로 여기서 만든다.//삭제

	//CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	//if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	//if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	//if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void ShaderComponent::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState)
{

	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];//파이프라인을 배열로 여기서 만든다.

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(nPipelineState);
	d3dPipelineStateDesc.GS = CreateGeometryShader(nPipelineState);
	d3dPipelineStateDesc.PS = CreatePixelShader(nPipelineState);
	d3dPipelineStateDesc.StreamOutput = CreateStreamOuputState(nPipelineState);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState(nPipelineState);
	d3dPipelineStateDesc.BlendState = CreateBlendState(nPipelineState);
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState(nPipelineState);
	d3dPipelineStateDesc.InputLayout = CreateInputLayout(nPipelineState);
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = GetPrimitiveTopologyType(nPipelineState);

	d3dPipelineStateDesc.NumRenderTargets = GetNumRenderTargets(nPipelineState);
	//d3dPipelineStateDesc.RTVFormats[0] = GetRTVFormat(nPipelineState, 0);
	for (UINT i = 0; i < GetNumRenderTargets(nPipelineState); i++)
		d3dPipelineStateDesc.RTVFormats[i] = GetRTVFormat(nPipelineState, nPipelineState); 
	d3dPipelineStateDesc.DSVFormat = GetDSVFormat(nPipelineState);
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[nPipelineState]);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();
	//if (m_pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void ShaderComponent::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}

void ShaderComponent::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World, MaterialComponent* ppMaterialsComponent)
{
	MATERIAL m_material;
	if (ppMaterialsComponent)
	{
		m_material.m_xmf4Ambient = ppMaterialsComponent->m_xmf4AmbientColor;
		m_material.m_xmf4Diffuse = ppMaterialsComponent->m_xmf4AlbedoColor;
		m_material.m_xmf4Emissive = ppMaterialsComponent->m_xmf4EmissiveColor;
		m_material.m_xmf4Specular = ppMaterialsComponent->m_xmf4SpecularColor;
		::memcpy(&m_pcbMappedGameObjects->m_material, &m_material, sizeof(MATERIAL));//메테리얼을 업데이트 해준다.
		::memcpy(&m_pcbMappedGameObjects->m_nType, &ppMaterialsComponent->m_nType, sizeof(UINT));//타입을 업데이트 해준다.
		::memcpy(&m_pcbMappedGameObjects->m_bAnimateshader, &ppMaterialsComponent->m_isAnimationShader, sizeof(bool));//에니메이션 상태를 업데이트 해준다.
	}

	XMStoreFloat4x4(&m_pcbMappedGameObjects->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));//오브젝트의 월드좌표계를 변환시켜준다.
	
	if (ppMaterialsComponent) {

		for (int i = 0; i < ppMaterialsComponent->m_nTextures; i++)
		{
			if (ppMaterialsComponent->m_ppTextures[i])
				ppMaterialsComponent->m_ppTextures[i]->UpdateShaderVariables(pd3dCommandList);
		}
	}
}

void ShaderComponent::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
}

void ShaderComponent::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
}

void ShaderComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	if (pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);
	if (m_ppd3dPipelineStates && !bPrerender) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);


	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	//UpdateShaderVariables(pd3dCommandList); //삭제예정
}

void ShaderComponent::SetPipelineState(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates ) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
}

void ShaderComponent::ReleaseUploadBuffers()
{
}

void ShaderComponent::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);

	m_d3dSrvCPUDescriptorNextHandle = m_d3dSrvCPUDescriptorStartHandle;
	m_d3dSrvGPUDescriptorNextHandle = m_d3dSrvGPUDescriptorStartHandle;
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr;
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr;
}

void ShaderComponent::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbGameObjects->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void ShaderComponent::CreateShaderResourceViews(ID3D12Device* pd3dDevice, TextureComponent* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	UINT nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int i = 0; i < nRootParameters; i++) pTexture->SetRootParameterIndex(i, nRootParameterStartIndex + i);
}
void ShaderComponent::CreateShaderResourceViews(ID3D12Device* pd3dDevice, TextureComponent* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex, ID3D12Resource* pShadowMap)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	UINT nTextureType = pTexture->GetTextureType();
	int nTotalTextures = nTextures;

	if (pShadowMap != NULL)
		nTotalTextures = nTextures + 1;

	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int i = 0; i < nRootParameters; i++) pTexture->SetRootParameterIndex(i, nRootParameterStartIndex + i);

	if (nTotalTextures > nTextures)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceDsec;

		d3dShaderResourceDsec.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		d3dShaderResourceDsec.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceDsec.Texture2D.MipLevels = 1;
		d3dShaderResourceDsec.Texture2D.PlaneSlice = 0;
		d3dShaderResourceDsec.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceDsec.Texture2D.ResourceMinLODClamp = 0;
		d3dShaderResourceDsec.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		pd3dDevice->CreateShaderResourceView(pShadowMap, &d3dShaderResourceDsec, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dShadowGPUDescriptorHandle = m_d3dSrvGPUDescriptorNextHandle;
	}
}

void ShaderComponent::CreateShaderResourceView(ID3D12Device* pd3dDevice, TextureComponent* pTexture, int nIndex)
{
}

SkyBoxShaderComponent::SkyBoxShaderComponent()
{
}

SkyBoxShaderComponent::~SkyBoxShaderComponent()
{
}

D3D12_INPUT_LAYOUT_DESC SkyBoxShaderComponent::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC SkyBoxShaderComponent::CreateDepthStencilState(int nPipelineState)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE SkyBoxShaderComponent::CreateVertexShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "VSSkyBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE SkyBoxShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_INPUT_LAYOUT_DESC BoundingBoxShaderComponent::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE BoundingBoxShaderComponent::CreateVertexShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "VSBoundingBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE BoundingBoxShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "PSBoundingBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_RASTERIZER_DESC BoundingBoxShaderComponent::CreateRasterizerState(int nPipelineState)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_SHADER_BYTECODE SphereShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "PSSphere", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_RASTERIZER_DESC SphereShaderComponent::CreateRasterizerState(int nPipelineState)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_SHADER_BYTECODE NaviMeshShaderComponent::CreateVertexShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "VSBoundingBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE NaviMeshShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "PSNaviMesh", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_BLEND_DESC NaviMeshShaderComponent::CreateBlendState(int nPipelineState)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_INPUT_LAYOUT_DESC BlendShaderComponent::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE BlendShaderComponent::CreateVertexShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "VSBlendTextured", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE BlendShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "PSBlendTextured", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_BLEND_DESC BlendShaderComponent::CreateBlendState(int nPipelineState)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_INPUT_LAYOUT_DESC CylinderShaderComponent::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CylinderShaderComponent::CreateVertexShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "VSBoundingBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CylinderShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "PSNaviMesh", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_BLEND_DESC CylinderShaderComponent::CreateBlendState(int nPipelineState)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_RASTERIZER_DESC CylinderShaderComponent::CreateRasterizerState(int nPipelineState)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_INPUT_LAYOUT_DESC SquareShaderComponent::CreateInputLayout(int nPipelineState)
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE SquareShaderComponent::CreateVertexShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "VSHealRange", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE SquareShaderComponent::CreatePixelShader(int nPipelineState)
{
	return(ShaderComponent::CompileShaderFromFile(L"Shaders.hlsl", "PSHealRange", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_BLEND_DESC SquareShaderComponent::CreateBlendState(int nPipelineState)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

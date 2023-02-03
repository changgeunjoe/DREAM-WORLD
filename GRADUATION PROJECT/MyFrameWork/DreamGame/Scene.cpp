#include "Scene.h"
CScene::CScene()
{

}

CScene::~CScene()
{
}

bool CScene::onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pObjectManager->onProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	return false;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS

	RootSignature.Descriptorrange.resize(3);
	RootSignature.Descriptorrange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	RootSignature.Descriptorrange[0].NumDescriptors = 1;
	RootSignature.Descriptorrange[0].BaseShaderRegister = 0; //GameObject //b0
	RootSignature.Descriptorrange[0].RegisterSpace = 0;
	RootSignature.Descriptorrange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	RootSignature.Descriptorrange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	RootSignature.Descriptorrange[1].NumDescriptors = 1;
	RootSignature.Descriptorrange[1].BaseShaderRegister = 0; //gtxtexture
	RootSignature.Descriptorrange[1].RegisterSpace = 0;
	RootSignature.Descriptorrange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	RootSignature.Descriptorrange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	RootSignature.Descriptorrange[2].NumDescriptors = 1;
	RootSignature.Descriptorrange[2].BaseShaderRegister = 1; //shadowMap
	RootSignature.Descriptorrange[2].RegisterSpace = 0;
	RootSignature.Descriptorrange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

//-------------------------------rootParameter----------------------------------------------------    
	RootSignature.RootParameter.resize(5);
	//GameObject(b0)Shaders.hlsl
	RootSignature.RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootSignature.RootParameter[0].DescriptorTable.NumDescriptorRanges = 1;
	RootSignature.RootParameter[0].DescriptorTable.pDescriptorRanges = &(RootSignature.Descriptorrange[0]);
	RootSignature.RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//Camera(b1) Shaders.hlsl
	RootSignature.RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootSignature.RootParameter[1].Descriptor.ShaderRegister = 1;
	RootSignature.RootParameter[1].Descriptor.RegisterSpace = 0;
	RootSignature.RootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//Light(b2) Shadows.hlsl
	RootSignature.RootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootSignature.RootParameter[2].Descriptor.ShaderRegister = 2;
	RootSignature.RootParameter[2].Descriptor.RegisterSpace = 0;
	RootSignature.RootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//gtxTexture(t0) Shadows.hlsl
	RootSignature.RootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootSignature.RootParameter[3].DescriptorTable.NumDescriptorRanges = 1;
	RootSignature.RootParameter[3].DescriptorTable.pDescriptorRanges = &(RootSignature.Descriptorrange[1]);
	RootSignature.RootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//ShadowMap(t1) Shadows.hlsl
	RootSignature.RootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootSignature.RootParameter[4].DescriptorTable.NumDescriptorRanges = 1;
	RootSignature.RootParameter[4].DescriptorTable.pDescriptorRanges = &(RootSignature.Descriptorrange[2]);
	RootSignature.RootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//textureSampler
	RootSignature.TextureSamplerDescs.resize(3);
	RootSignature.TextureSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	RootSignature.TextureSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	RootSignature.TextureSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	RootSignature.TextureSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	RootSignature.TextureSamplerDescs[0].MipLODBias = 0;
	RootSignature.TextureSamplerDescs[0].MaxAnisotropy = 1;
	RootSignature.TextureSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	RootSignature.TextureSamplerDescs[0].MinLOD = 0;
	RootSignature.TextureSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	RootSignature.TextureSamplerDescs[0].ShaderRegister = 0;
	RootSignature.TextureSamplerDescs[0].RegisterSpace = 0;
	RootSignature.TextureSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	RootSignature.TextureSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	RootSignature.TextureSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	RootSignature.TextureSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	RootSignature.TextureSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	RootSignature.TextureSamplerDescs[1].MipLODBias = 0;
	RootSignature.TextureSamplerDescs[1].MaxAnisotropy = 1;
	RootSignature.TextureSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	RootSignature.TextureSamplerDescs[1].MinLOD = 0;
	RootSignature.TextureSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	RootSignature.TextureSamplerDescs[1].ShaderRegister = 1;
	RootSignature.TextureSamplerDescs[1].RegisterSpace = 0;
	RootSignature.TextureSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	RootSignature.TextureSamplerDescs[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	RootSignature.TextureSamplerDescs[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	RootSignature.TextureSamplerDescs[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	RootSignature.TextureSamplerDescs[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	RootSignature.TextureSamplerDescs[2].MipLODBias = 0;
	RootSignature.TextureSamplerDescs[2].MaxAnisotropy = 1;
	RootSignature.TextureSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	RootSignature.TextureSamplerDescs[2].MinLOD = 0;
	RootSignature.TextureSamplerDescs[2].MaxLOD = D3D12_FLOAT32_MAX;
	RootSignature.TextureSamplerDescs[2].ShaderRegister = 2;
	RootSignature.TextureSamplerDescs[2].RegisterSpace = 0;
	RootSignature.TextureSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
#endif
	::ZeroMemory(&RootSignature.RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignature.RootSignatureDesc.NumParameters = RootSignature.RootParameter.size();
	RootSignature.RootSignatureDesc.pParameters = RootSignature.RootParameter.data();
	RootSignature.RootSignatureDesc.NumStaticSamplers = RootSignature.TextureSamplerDescs.size();
	RootSignature.RootSignatureDesc.pStaticSamplers = RootSignature.TextureSamplerDescs.data(); //texture sampler 아직 없음
	RootSignature.RootSignatureDesc.Flags = RootSignature.RootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&RootSignature.RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}



void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_pObjectManager = new GameobjectManager();
	m_pObjectManager->BuildObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
}


void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}


bool CScene::ProcessInput()
{
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}
void CScene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (pCamera) pCamera->UpdateShaderVariables(pd3dCommandList);
	//씬을 렌더링하는 것은 씬을 구성하는 게임 객체(셰이더를 포함하는 객체)들을 렌더링하는 것이다. 

	m_pObjectManager->Render(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j])
			m_ppObjects[j]->ReleaseUploadBuffers();
	}
}


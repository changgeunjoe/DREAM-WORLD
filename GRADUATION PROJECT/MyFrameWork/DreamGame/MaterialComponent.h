#pragma once
#include"stdafx.h"
#include"ComponentBase.h"
class ShaderComponent;
class TextureComponent;
class MaterialComponent:public ComponentBase
{
public:
	MaterialComponent(int nTextures);
	virtual ~MaterialComponent();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	ShaderComponent* m_pShader = NULL;

	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);


	void SetShader(ShaderComponent* pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(TextureComponent* pTexture, UINT nTexture = 0);

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void ReleaseUploadBuffers();

public:
	UINT							m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

public:
	int 							m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	TextureComponent	**m_ppTextures = NULL; //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, TextureComponent** ppTexture, GameObject* pParent, FILE* pInFile, ShaderComponent* pShader);
	int nObjects=0;
public:
	static ShaderComponent* m_pStandardShader;
	static ShaderComponent* m_pSkinnedAnimationShader;

	static void MaterialComponent::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12Resource* m_pd3dcbGameObjects);
	
	static D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvStandardGPUDescriptorHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvSkinnedGPUDescriptorHandle;
	static void SetCbvGPUStandardDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvStandardGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	void SetCbvGPUSkinnedDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvSkinnedGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvStandardGPUDescriptorHandle); }


	void SetStandardShader() { MaterialComponent::SetShader(m_pStandardShader); }
	void SetSkinnedAnimationShader() { MaterialComponent::SetShader(m_pSkinnedAnimationShader); }
};
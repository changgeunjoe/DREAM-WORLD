#pragma once
#include "ComponentBase.h"
//
//class CMaterialComponent
//{
//public:
//	CMaterial(int nTextures);
//	virtual ~CMaterial();
//
//private:
//	int								m_nReferences = 0;
//
//public:
//	void AddRef() { m_nReferences++; }
//	void Release() { if (--m_nReferences <= 0) delete this; }
//
//public:
//	CShader* m_pShader = NULL;
//
//	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
//	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
//	XMFLOAT4						m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
//
//	void SetShader(CShader* pShader);
//	void SetMaterialType(UINT nType) { m_nType |= nType; }
//	void SetTexture(CTexture* pTexture, UINT nTexture = 0);
//
//	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList);
//
//	virtual void ReleaseUploadBuffers();
//
//public:
//	UINT							m_nType = 0x00;
//
//	float							m_fGlossiness = 0.0f;
//	float							m_fSmoothness = 0.0f;
//	float							m_fSpecularHighlight = 0.0f;
//	float							m_fMetallic = 0.0f;
//	float							m_fGlossyReflection = 0.0f;
//
//public:
//	int 							m_nTextures = 0;
//	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
//	CTexture** m_ppTextures = NULL; //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
//
//	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, CTexture** ppTexture, CGameObject* pParent, FILE* pInFile, CShader* pShader);
//
//public:
//	static SHADER_COMPONENT* m_pStandardShader;
//	static CShader* m_pSkinnedAnimationShader;
//	static CShader* m_pFaceSkinnedAnimationShader;
//
//	static void CMaterial::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
//
//	void SetStandardShader() { CMaterial::SetShader(m_pStandardShader); }
//	void SetSkinnedAnimationShader() { CMaterial::SetShader(m_pSkinnedAnimationShader); }
//	void SetFaceSkinnedAnimationShader() { CMaterial::SetShader(m_pFaceSkinnedAnimationShader); }
//};
//

#include "MaterialComponent.h"
#include"ShaderComponent.h"
#include"Scene.h"

MaterialComponent::MaterialComponent(int nTextures)
{
	m_nTextures = nTextures;

	m_ppTextures = new TextureComponent * [m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}


MaterialComponent::~MaterialComponent()
{
	if (m_pShader) m_pShader->Release();

	if (m_nTextures > 0)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
		delete[] m_ppTextures;

		if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;
	}
}

void MaterialComponent::SetShader(ShaderComponent* pShader)
{
	if (m_pShader) m_pShader->Release();//중요
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void MaterialComponent::SetTexture(TextureComponent* pTexture, UINT nTexture)
{
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->Release();
	m_ppTextures[nTexture] = pTexture;
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->AddRef();
}
ShaderComponent* MaterialComponent::m_pSkinnedAnimationShader = NULL;
ShaderComponent* MaterialComponent::m_pStandardShader = NULL;

void MaterialComponent::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i])
			m_ppTextures[i]->UpdateShaderVariables(pd3dCommandList);
	}
}

void MaterialComponent::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->ReleaseUploadBuffers();
	}
}

void MaterialComponent::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, TextureComponent** ppTexture, GameObject* pParent, FILE* pInFile, ShaderComponent* pShader)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = ::ReadStringFromFile(pInFile, pstrTextureName);

	bool bDuplicated = false;
	if (strcmp(pstrTextureName, "null"))
	{
		SetMaterialType(nType);
		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, pwstrTextureName, 64, pstrFilePath, _TRUNCATE);

#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', pwstrTextureName);
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			*ppTexture = new TextureComponent();
			(*ppTexture)->BuildTexture(1, RESOURCE_TEXTURE2D, 0,1);
			(*ppTexture)->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pwstrTextureName, RESOURCE_TEXTURE2D, 0);
			if (*ppTexture) (*ppTexture)->AddRef();
			//cout << "CreateTexture" << endl;
			pShader->CreateShaderResourceViews(pd3dDevice, *ppTexture,0, nRootParameter, false);
		}
		else
		{
			if (pParent)
			{
				while (pParent)
				{
					if (!pParent->m_pParent) break;
					pParent = pParent->m_pParent;
				}
				GameObject* pRootGameObject = pParent;
				*ppTexture = pRootGameObject->FindReplicatedTexture(pwstrTextureName);
				if (*ppTexture) (*ppTexture)->AddRef();
			}
		}
	}
}

void MaterialComponent::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12Resource* m_pd3dcbGameObjects)
{
	int nObjects = 0;
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);// 삭제 예정(변경)
	m_pStandardShader = new ShaderComponent();
	m_pStandardShader->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	m_pStandardShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 10, 20);
	m_pStandardShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pStandardShader->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObjects, ncbElementBytes);
	m_pStandardShader->SetCbvGPUDescriptorHandlePtr(m_pStandardShader->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
	
	m_pSkinnedAnimationShader = new ShaderComponent();
	m_pSkinnedAnimationShader->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	m_pSkinnedAnimationShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 10, 20);
	m_pSkinnedAnimationShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pSkinnedAnimationShader->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObjects, ncbElementBytes);
	m_pSkinnedAnimationShader->SetCbvGPUDescriptorHandlePtr(m_pStandardShader->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
	//m_pStandardShader->CreateShaderResourceViews(pd3dDevice, *m_ppTextures, 0, 3, pShadowMap);//texture입력
	//SetCbvGPUDescriptorHandle(PShaderComponent->GetCbvGPUDescriptorHandle());
	//SetCbvGPUStandardDescriptorHandlePtr(m_pStandardShader->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
}	

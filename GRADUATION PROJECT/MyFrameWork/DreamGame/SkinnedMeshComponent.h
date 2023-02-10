#pragma once
#include"MeshComponent.h"
class GameObject;

#define SKINNED_ANIMATION_BONES		128

class SkinnedMeshComponent : public StandardMeshComponent
{
public:
	SkinnedMeshComponent();
	virtual ~SkinnedMeshComponent();

protected:
	int								m_nBonesPerVertex = 4;

	XMINT4* m_pxmn4BoneIndices = NULL;
	XMFLOAT4* m_pxmf4BoneWeights = NULL;

	ID3D12Resource* m_pd3dBoneIndexBuffer = NULL;
	ID3D12Resource* m_pd3dBoneIndexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneIndexBufferView;

	ID3D12Resource* m_pd3dBoneWeightBuffer = NULL;
	ID3D12Resource* m_pd3dBoneWeightUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneWeightBufferView;

public:
	int								m_nSkinningBones = 0;

	char(*m_ppstrSkinningBoneNames)[64];
	GameObject** m_ppSkinningBoneFrameCaches = NULL; //[m_nSkinningBones]

	XMFLOAT4X4* m_pxmf4x4BindPoseBoneOffsets = NULL; //Transposed

	ID3D12Resource* m_pd3dcbBindPoseBoneOffsets = NULL;
	XMFLOAT4X4* m_pcbxmf4x4MappedBindPoseBoneOffsets = NULL;

	ID3D12Resource* m_pd3dcbSkinningBoneTransforms = NULL; //Pointer Only
	XMFLOAT4X4* m_pcbxmf4x4MappedSkinningBoneTransforms = NULL;

public:
	void PrepareSkinning(GameObject* pModelRootObject);
	void LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
};

#pragma once
#include"stdafx.h"
#include"Camera.h"
class GameObject;

class EffectObject
{
public:
	EffectObject();
	~EffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void RenderEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);

protected:
	array<GameObject*, 10> m_pSmokeObject;
	array<GameObject*, 10> m_pPointObject;
	array<GameObject*, 10> m_pArrowObject;
	array<GameObject*, 10> m_pFlareObject;

	

};


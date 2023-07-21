#pragma once
#include"stdafx.h"
#include"Camera.h"
#define GRAVITY 50;
class GameObject;
bool CompareGameObjects(const GameObject* obj1, const GameObject* obj2);
class EffectObject
{
public:
	EffectObject();
	~EffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void RenderEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);
	virtual void SortEffect();//���� ������Ʈ�� ī�޶�� �Ÿ����� sort�ϴ� �Լ��Դϴ�. ->����Ʈ�� ���尡 ���̴� �� �������� ��Ʈ 
	virtual void Particle();//�߷� ��ƼŬ

	
public:

	float	m_fgravity = 50;
	float	m_ftime = 0.0f;
	bool	m_bActive = false;
	ROLE	m_hostObject = ROLE::NONE_SELECT;

	vector< GameObject*> m_pEffectObjects;
	array<GameObject*, 10> m_pSmokeObject;
	array<GameObject*, 10> m_pPointObject;
	array<GameObject*, 10> m_pArrowObject;
	array<GameObject*, 10> m_pFlareObject;
	GameObject* m_pAttackObject;

	array<GameObject*, 100> m_ppParticleObjects;
};

class LightningEffectObject :
	public EffectObject
{

public:
	LightningEffectObject();
	~LightningEffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);

	array<GameObject*, 10> m_pLightningSpriteObject;

};



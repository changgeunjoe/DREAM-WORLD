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
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, vector<GameObject*> *mppEffectObject);
	virtual void RenderEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);
	virtual void SortEffect();//게임 오브젝트를 카메라와 거리별로 sort하는 함수입니다. ->이펙트가 블랜드가 꼬이는 걸 막기위한 소트 
	virtual void Particle(CCamera* pCamera,float fTimeElapsed, XMFLOAT3& xm3position);//중력 파티클
	virtual void AnimateLight(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime) {};
	//virtual void ParticleReset(float fTimeElapsed);
	void SetActive(bool bActive);
	
public:

	float	m_fgravity = 50;
	float	m_ftime = 0.0f;
	bool	m_bActive = false;
	ROLE	m_hostObject = ROLE::NONE_SELECT;
	float	m_fParticleLifeTime = 0.0f;
	float	m_fEarthquakeLifeTime = 0.0f;
	float	m_fEffectLifeTime = 0.0f;


	vector< GameObject*> m_pEffectObjects;
	array<GameObject*, 10> m_pSmokeObject;
	array<GameObject*, 10> m_pPointObject;
	array<GameObject*, 10> m_pArrowObject;
	array<GameObject*, 10> m_pFlareObject;
	GameObject* m_pAttackObject;

	array<GameObject*, 50> m_ppParticleObjects;
};

class LightningEffectObject :
	public EffectObject
{

public:
	LightningEffectObject();
	~LightningEffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,  vector<GameObject*> *mppEffectObject);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);
	virtual void AnimateLight(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);
	virtual void CheckActive(bool bactive);


	array<GameObject*, 5> m_pLightningSpriteObject;

};

class SheildEffectObject :
	public EffectObject
{

public:
	SheildEffectObject();
	~SheildEffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,  vector<GameObject*> *mppEffectObject);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);

	array<GameObject*, 3> m_pSheildEffectObject;

};

class PortalEffectObject :
	public EffectObject
{
public:
	PortalEffectObject();
	~PortalEffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, vector<GameObject*> *mppEffectObject);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);

	GameObject* m_pPortalEffectObject;
	GameObject* m_pPressGPuppleObject;

};

class TankerEffectObject :
	public EffectObject
{
public:
	TankerEffectObject();
	~TankerEffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, vector<GameObject*> *mppEffectObject);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);

	GameObject* m_pTankerEffectObject;
	XMFLOAT3 m_f3EarquakePos;

};
class PriestEffectObject :
	public EffectObject
{
public:
	PriestEffectObject();
	~PriestEffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, vector<GameObject*>* mppEffectObject);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);
	virtual void Particle(CCamera* pCamera, float fTimeElapsed, XMFLOAT3& xm3position);//중력 파티클

	GameObject* m_pTankerEffectObject;

};
class BossEffectObject :
	public EffectObject
{
public:
	BossEffectObject();
	~BossEffectObject();
public:
	virtual void BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, vector<GameObject*>* mppEffectObject);
	virtual void AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime);
	virtual void Particle(CCamera* pCamera, float fTimeElapsed, XMFLOAT3& xm3position);//중력 파티클

	GameObject* m_pTankerEffectObject;

};




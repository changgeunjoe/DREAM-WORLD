#pragma once
#include"stdafx.h"
#include"ComponentBase.h"
class GameObject;
class TrailComponent :
    public ComponentBase
{
public:
    TrailComponent();
    ~TrailComponent();

    void	ReadyComponent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, GameObject* mpTrailObject);
    void	Update_Component(const float& fTimeDelta);

    void			AddTrail( const XMFLOAT3& xmf3Top, const XMFLOAT3& xmf3Bottom);
    void			RenderTrail(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    void			RenderAstar(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, vector<int>& vecNodeQueue);
    void			SetRenderingTrail(bool isOn);
    bool            GetRenderState() { return m_bRender; }
public:
    static TrailComponent* Create(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    float							m_fTime = 0.f;
    float							m_fRenderTime = 0.f;
private:
    int								m_iMaxCount = 0;
    int								m_iDivide = 0;
    bool							m_bRender = true;
    float							m_fCreateTime = 0.f;

    list<pair<XMFLOAT3, XMFLOAT3>>	m_listPos; //Top,Bottom
    list<pair<XMFLOAT3, XMFLOAT3>>	m_listRomPos;
    GameObject*                     m_pTrailObject{ nullptr };
};


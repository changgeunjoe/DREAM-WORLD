#pragma once
#include"stdafx.h"
#include"ComponentBase.h"

class TrailComponent :
    public ComponentBase
{
    TrailComponent();
    ~TrailComponent();

    void	ReadyComponent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    void	Update_Component(const float& fTimeDelta);

    void			AddTrail(XMFLOAT3& xmf3Top, XMFLOAT3& xmf3Bottom);
    void			RenderTrail(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
    void			SetRenderingTrail(bool isOn);
public:
    static TrailComponent* Create(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
private:
    int								m_iMaxCount = 0;
    int								m_iDivide = 0;
    bool							m_bRender = false;
    float							m_fCreateTime = 0.f;
    float							m_fTime = 0.f;
    list<pair<XMFLOAT3, XMFLOAT3>>	m_listPos; //Top,Bottom
    list<pair<XMFLOAT3, XMFLOAT3>>	m_listRomPos;
   // CTrailObject* m_pTrailObject = nullptr;
};


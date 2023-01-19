#pragma once
#include "stdafx.h"


#define MAX_LIGHTS			50

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};

struct CB_LIGHT_INFO
{
	XMFLOAT4X4 m_xmf4x4LightView;
	XMFLOAT4X4 m_xmf4x4LightProjection;
	XMFLOAT4 m_xmf4ambientColor ;
	XMFLOAT4 m_xmf4diffuseColor ;
	XMFLOAT3 m_xmf3lightPosition;
	XMFLOAT3 m_xmf3lightLookat;
	float m_xmfpadding ;
};

class CLight 
{
public:
	CLight();
	~CLight();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void BuildLight();
private:// Light object
	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;
	LIGHT* m_pLights = NULL;
	int				m_nLights = 0;
	XMFLOAT4		m_xmf4GlobalAmbient;
};
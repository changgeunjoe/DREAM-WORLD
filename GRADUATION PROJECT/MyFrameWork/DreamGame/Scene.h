#pragma once
#include"stdafx.h"
#include"Timer.h"
#include "Network/Logic/Logic.h"



class UILayer;
class GameobjectManager;
class CCamera;
struct RootSignatureDesc
{
	D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS ;
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc ;
	std::vector<D3D12_DESCRIPTOR_RANGE> Descriptorrange;
	std::vector<D3D12_ROOT_PARAMETER> RootParameter;
	std::vector<D3D12_STATIC_SAMPLER_DESC> TextureSamplerDescs;
	//D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;;
};
class CScene
{
public:
	CScene();
	~CScene();

	//씬에서 키보드와 마우스 메시지를 처리한다
	bool onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	bool onProcessingKeyboardMessageLobby(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);

	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void TalkUIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void UIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera,float ftimeElapsed);
	void OnPreRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseUploadBuffers();
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	void SetDescriptorRange(D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[], int iIndex, D3D12_DESCRIPTOR_RANGE_TYPE RangeType, UINT NumDescriptors, UINT BaseShaderRegister, UINT RegisterSpace);
	void SetRootParameterCBV(D3D12_ROOT_PARAMETER pd3dRootParameter[], int iIndex, UINT ShaderRegister, UINT RegisterSpace, D3D12_SHADER_VISIBILITY ShaderVisibility);
	void SetRootParameterDescriptorTable(D3D12_ROOT_PARAMETER pd3dRootParameter[], int iIndex, UINT NumDescriptorRanges, const D3D12_DESCRIPTOR_RANGE* pDescriptorRanges, D3D12_SHADER_VISIBILITY ShaderVisibility);
	void SetRootParameterConstants(D3D12_ROOT_PARAMETER pd3dRootParameter[], int iIndex, UINT Num32BitValues, UINT ShaderRegister, UINT RegisterSpace, D3D12_SHADER_VISIBILITY ShaderVisibility);

	GameobjectManager* GetObjectManager() { return m_pObjectManager; }
protected:
	//씬은 게임 객체들의 집합이다.게임 객체는 셰이더를 포함한다.
	

	GameobjectManager* m_pObjectManager = NULL;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	RootSignatureDesc RootSignature;

	CCamera* m_pCamera{ NULL };
	//2dpont ui



	////루트 시그너쳐를 나타내는 인터페이스 포인터이다. 
	// ID3D12PipelineState *m_pd3dPipelineState = NULL;
	////파이프라인 상태를 나타내는 인터페이스 포인터이다.

	float m_fTime{};
	friend class Logic;
public:
	UILayer* m_pUILayer = NULL;
};



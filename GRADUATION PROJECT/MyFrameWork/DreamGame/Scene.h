#pragma once
#include"stdafx.h"
#include"Timer.h"
#include"GameobjectManager.h"

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

	//������ Ű����� ���콺 �޽����� ó���Ѵ�
	bool onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);

	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

protected:
	//���� ���� ��ü���� �����̴�.���� ��ü�� ���̴��� �����Ѵ�.
	

	GameobjectManager* m_pObjectManager=NULL;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	RootSignatureDesc RootSignature;

	////��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�. 
	// ID3D12PipelineState *m_pd3dPipelineState = NULL;
	////���������� ���¸� ��Ÿ���� �������̽� �������̴�.




};



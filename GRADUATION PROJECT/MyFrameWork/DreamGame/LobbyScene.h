#pragma once
#include"stdafx.h"
#include"Timer.h"
#include "Network/Logic/Logic.h"
#include"Scene.h"
class GameobjectManager;
class CCamera;

class LobbyCScene
{
public:
	LobbyCScene();
	~LobbyCScene();

	//������ Ű����� ���콺 �޽����� ó���Ѵ�
	bool onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);

	void BuildUIObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);

	void UIRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseUploadBuffers();
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
protected:
	//���� ���� ��ü���� �����̴�.���� ��ü�� ���̴��� �����Ѵ�.

	GameobjectManager* m_pObjectManager = NULL;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	RootSignatureDesc RootSignature;

	CCamera* m_pCamera{ NULL };



	friend class Logic;
};


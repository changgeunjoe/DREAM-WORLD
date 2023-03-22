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

	//씬에서 키보드와 마우스 메시지를 처리한다
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
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
protected:
	//씬은 게임 객체들의 집합이다.게임 객체는 셰이더를 포함한다.

	GameobjectManager* m_pObjectManager = NULL;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	RootSignatureDesc RootSignature;

	CCamera* m_pCamera{ NULL };



	friend class Logic;
};


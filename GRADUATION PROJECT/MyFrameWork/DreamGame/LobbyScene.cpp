
#include "stdafx.h"
#include "LobbyScene.h"
#include"GameobjectManager.h"
#include "Camera.h"

LobbyCScene::LobbyCScene()
{
}

LobbyCScene::~LobbyCScene()
{
}

bool LobbyCScene::onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool LobbyCScene::onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void LobbyCScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}

void LobbyCScene::ReleaseObjects()
{
}

bool LobbyCScene::ProcessInput()
{
	return false;
}

void LobbyCScene::AnimateObjects(float fTimeElapsed)
{
}

void LobbyCScene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}

void LobbyCScene::ReleaseUploadBuffers()
{
}

ID3D12RootSignature* LobbyCScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return nullptr;
}

ID3D12RootSignature* LobbyCScene::GetGraphicsRootSignature()
{
	return nullptr;
}

#include "stdafx.h"
#include "GameFramework.h"
#include"stdafx.h"
#include"Camera.h"
#include "GameObject.h"
#include "Network/Logic/Logic.h"
#include "Network/NetworkHelper.h"

extern Logic g_Logic;
extern NetworkHelper g_NetworkHelper;

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;//DXGI ���丮 �������̽��� ���� �������̴�.//IDXGIFactory�� ���(�׷���ī��)(adaptor),�����ġ(�����),(output),����̽��� �����ϰ� �ִ� �İ�ü�̴�.
	m_pdxgiSwapChain = NULL;///���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	m_pd3dDevice = NULL;//Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.

	m_pd3dCommandAllocator = NULL; //Command List���� ID3D12CommandAllocator�� �ϳ� �����ȴ�.Command List�� �߰��� Command���� �� Allocator�� �޸𸮿� ����ȴ�.
	m_pd3dCommandQueue = NULL; //��� ť, CPU�� Command List�� Direct3D API�� ���� GPU�� Command Queue�� �����Ѵ�.
	m_pd3dPipelineState = NULL;//�׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� �������̴�.
	m_pd3dCommandList = NULL;//��� ����Ʈ �������̽� ������->CPU���� �����Ѵ�.->Commanqueue�� ����

	for (int i = 0; i < m_nSwapChainBuffers; i++)m_ppd3dRenderTargetBuffers[i] = NULL;//�ĸ� ���۸� m_nSwapChainBuffers���� ��ŭ �����.
	m_pd3dDsvDescriptorHeap = NULL;//���� ���α׷��� �ʿ��� ������/ ����� ���� ������ ���� ������ �Ѵ�.
	m_nRtvDescriptorIncrementSize = 0;//����-���ٽ� ������ ������ ũ���̴�

	m_pd3dDepthStencilBuffer = NULL;//���� ���ٽ� ����->Z BUFFer ���ٽ�
	m_pd3dDsvDescriptorHeap = NULL;//���� ���α׷��� �ʿ��� ������/ ����� ���� ������ ���� ������ �Ѵ�
	m_nDsvDescriptorIncrementSize = 0;//����-���ٽ� ������ ������ ũ���̴�.

	m_nSwapChainBufferIndex = 0;//���� ���� ü���� �ĸ� ���� �ε����̴�.

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_nFenceValues[i] = 0;

#ifdef _WITH_DIRECT2D
		m_ppd3d11WrappedBackBuffers[i] = NULL;
		m_ppd2dRenderTargets[i] = NULL;
#endif
	}
	m_pScene = NULL;
	m_pLobbyScene = NULL;
	m_bLobbyScene = true;
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszFrameRate, _T("LapProject ("));

}

CGameFramework::~CGameFramework()
{
}

//���� �Լ��� �������α׷��� ���� �Ǿ� �� �����찡 �����Ǹ� ȣ��ȴٴ� �Ϳ� �����϶�.
bool CGameFramework::onCreate(HINSTANCE hinstance, HWND hmainwnd)
{
	m_hinstance = hinstance;
	m_hwnd = hmainwnd;

	//Direct3D ����̽�,��� ť�� ��� ����Ʈ ,���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�.
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();
	CreateDirect2D();


#ifdef _WITH_DIRECT2D
	CreateDirect2DDevice();
#endif



	BuildObjects();
	//�������� ���� ��ü�� �����Ѵ�.
	return(true);
}

void CGameFramework::OnDestroy()
{
	WaitForGpuComplete();
	//GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ���.->commandqueue�� �� ���� ���� �� ���� ��ٸ���.

	ReleaseObjects();
	//���� ��ü(���� ���� ��ü)�� �Ҹ��Ѵ�.

	::CloseHandle(m_hFenceEvent);//->��鰪�� ��ȯ
	for (int i = 0; i < m_nSwapChainBuffers; i++)if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();//->�ĸ� ���۸� ���� ����
	if (m_pd3dDepthStencilBuffer)m_pd3dDepthStencilBuffer->Release(); //rtzheap ����

	if (m_pd3dDepthStencilBuffer)m_pd3dDepthStencilBuffer->Release();//����-���ٽ� ���� ����
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();//DSVheap����

	if (m_pd3dCommandAllocator)m_pd3dCommandAllocator->Release();//->Commandlist allocator ->����
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();//�׷��� ī���� Ŀ�ǵ� ť ����
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();//cpuĿ�ǵ� ����Ʈ�� GPU Ŀ���ť�� �����ϴ� ���������� ����
	if (m_pd3dCommandList) m_pd3dCommandList->Release(); // cpu commandList����

	if (m_pd3dFence) m_pd3dFence->Release();//�׷��� ī���� �ҽ� ���� 

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);//��üȭ�� ����
	if (m_pdxgiSwapChain)m_pdxgiSwapChain->Release();//����ü��(�ĸ� ���ۿ� ���� ���۸� �մ� ü��)�� Release();
	if (m_pd3dDevice)m_pd3dDevice->Release();//���� ����̽�(�׷���ī��) ����
	if (m_pdxgiFactory)m_pdxgiFactory->Release();//���丮( ���(�׷���ī��)(adaptor),�����ġ(�����),(output),����̽��� )���� 


#ifdef _WITH_DIRECT2D
	if (m_pd2dbrBackground) m_pd2dbrBackground->Release();
	if (m_pd2dbrBorder) m_pd2dbrBorder->Release();
	if (m_pdwFont) m_pdwFont->Release();
	if (m_pdwTextLayout) m_pdwTextLayout->Release();
	if (m_pd2dbrText) m_pd2dbrText->Release();
	if (m_pd2dDeviceContext) m_pd2dDeviceContext->Release();
	if (m_pd2dDevice) m_pd2dDevice->Release();
	if (m_pdWriteFactory) m_pdWriteFactory->Release();
	if (m_pd3d11On12Device) m_pd3d11On12Device->Release();
	if (m_pd3d11DeviceContext) m_pd3d11DeviceContext->Release();
	if (m_pd2dFactory) m_pd2dFactory->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++)
	{
		if (m_ppd3d11WrappedBackBuffers[i]) m_ppd3d11WrappedBackBuffers[i]->Release();
		if (m_ppd2dRenderTargets[i]) m_ppd2dRenderTargets[i]->Release();
	}
#endif


#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();
	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);//��ü ȭ�� ���� ����� ���¸� �����ɴϴ�.���� ü���� ��ü ȭ�� ��忡 �ִ� ��� TRUE���� ü���� â ��忡 �ִ� ��� FALSE
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);//���÷��� ���¸� â �Ǵ� ��ü ȭ������ �����մϴ�.���÷��� ���¸� â�Ǵ� ��ü ȭ������ �������� ���θ� �����ϴ� Boolean ���Դϴ�. ��ü ȭ���� ��� TRUE, â�� ���� FALSE.

	DXGI_MODE_DESC dxgiTargetParameters;//���÷��� ��带 �����մϴ�.
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//->ȭ�� �ʱ�ȭ
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;//->�ʱ�ȭ ����Ʈ
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);
	//�ĸ� ���� �ʱ�ȭ
	for (int i = 0; i < m_nSwapChainBuffers; i++)if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();
	//->����ü���� �ٽ� �����.
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hwnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels -
		1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� �����Ѵ�. dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue,
		&dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);
	//#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
	//#endif

}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hwnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;//���� ����
	m_nWndClientHeight = rcClient.bottom - rcClient.top;//���� ����

	//DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	//::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	//dxgiSwapChainDesc.Width = m_nWndClientWidth; //�ػ� �ʺ� �����ϴ� ���Դϴ�
	//dxgiSwapChainDesc.Height = m_nWndClientHeight;//�ػ� ���̸� �����ϴ� ��
	//dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//���÷��� ������ �����ϴ� DXGI_FORMAT �����Դϴ�.
	//dxgiSwapChainDesc.SampleDesc.Count=(m_bMsaa4xEnable)?4:1;//�ȼ��� ��Ƽ ���� �����Դϴ�.
	//dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	//dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//�ĸ� ���ۿ� ���� ǥ�� ��뷮 �� CPU������ �ɼ��� �����ϴ� DXGI_USAGE���İ�
	//dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;//���� ü���� ���ۼ��� �����ϴ� ��,��ü ȭ�� ���� ü���� ���� �� �Ϲ������� �̰��� ���� ���۸� ����
	//dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;//�ĸ� ������ ũ�Ⱑ ��� ����� ���� ���� ��� ũ�� ���� DXGI_SCALING ������ ��.
	//dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//���� ü�ο��� ���Ǵ� ���������̼� �𵨰� ǥ���� ������ �� ���������̼� ������ ������ ó���ϴ� �ɼ��� �����ϴ� DXGI_SWAP_EFFECT ������ ���Դϴ�.
	//dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; //���� ü�� �ĸ� ������ ���� ������ �ĺ��ϴ� DXGI_ALPHA_MODE ���İ��Դϴ�.
	//dxgiSwapChainDesc.Flags = 0;//��Ʈ ���� �Ǵ� �۾��� ����Ͽ� ���յ� DXGI_SWAP_CHAIN_FLAG ���İ��� �����Դϴ�. ��� ���� ���� ü�� ���ۿ� ���� �ɼ��� �����մϴ�.

	////����ü���� ��ü ȭ�� ��带 �����Ѵ�.

	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	//::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	//dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;//���ΰ�ħ �� 60����
	//dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;//���ΰ�ħ �� 1����
	//dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//��ĵ �� �׸��� ��带 �����ϴ� DXGI_MODE_SCANLINE_ORDER  STRUCT ������ ����Դϴ�.
	//dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//���� ���� ��带 �����ϴ�  DXGI_MODE_SCALING Ȯ��� ������ ����Դϴ�
	//dxgiSwapChainFullScreenDesc.Windowed = TRUE; //���� ü���� â ��忡 �ִ��� ���θ� �����ϴ� Boole ���Դϴ�.���� ü���� â ��忡 �ִ� ��� TRUE; �׷��� ������ FALSE.

	//m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hwnd,
	//	&dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);//HWND �ڵ�� ����� ���� ü���� ���� ü���� ��� â�� ����ϴ�.
	////����ü���� �����Ѵ�.(in,in,in,in,in,out)->t������� &m_pdxgiSwapChain�� ����Ѵ�.,

	//m_pdxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);

	////ALT+ENTER Ű�� ������ ��Ȱ��ȭ �Ѵ�.

	//m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();//->���� �߿��ҵ�??


	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hwnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels -
		1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� �����Ѵ�. dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue,
		&dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);
	//#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	//	CreateRenderTargetViews();
	//#endif

}



void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;

#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);//�ٸ� DXGI ��ü�� �����ϴ� �� ����� �� �ִ� DXGI 1.3 ���͸��� ����ϴ�.
	IDXGIAdapter1* pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc; //->��� �����
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter,
			D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice)))break;
	}
	//��� �ϵ���� ����Ϳ� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�.

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;//msaa ����Ƽ ���� ����̽�
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//�ػ� �ʱ�ȭ
	d3dMsaaQualityLevels.SampleCount = 4;//Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;//���� (??) ���� ã�� ����
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;//->���� ���ø��� ǰ�� ���� Ȯ��
	//����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ�δ�.

	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�. 

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	//�潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�.

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*�潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����ϴ�(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�).
	�̺�Ʈ�� ����Ǹ�(Signal)�̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.*/

	//m_d3dviewport.TopLeftX = 0;
	//m_d3dviewport.TopLeftY = 0;
	//m_d3dviewport.Width = static_cast<float>(m_nWndClientWidth);
	//m_d3dviewport.Height = static_cast<float>(m_nWndClientHeight);
	//m_d3dviewport.MinDepth = 0.0f;
	//m_d3dviewport.MaxDepth = 1.0f;
	////��ǥƮ�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.

	//m_d3dScissorRect = { 0,0,m_nWndClientWidth,m_nWndClientHeight };
	////���� �簢���� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.

	if (pd3dAdapter)pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;//ť ����
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,
		__uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);
	//����(Direct) ��� ť�� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	//����(Direct)��� �Ҵ��ڸ� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//����(Direct)��� ����Ʈ�� �����Ѵ�.

	hResult = m_pd3dCommandList->Close();
	//��� ����Ʈ�� �����Ǹ� ����(Open)�����̹Ƿ� ����(Closed)���·� �����.

}




void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	//���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�.
	m_nRtvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//���� Ÿ�� ������ ���� ������ ũ�⸦ �����Ѵ�.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	//���� ���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.
	m_nDsvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//����-���ٽ� ������ ���� ������ ũ�⸦ �����Ѵ�.
}

void CGameFramework::CreateRenderTargetViews()
{
	//����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�ٺ並 �����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void
			**)&m_ppd3dRenderTargetBuffers[i]);//->����ü���� �ĸ���� �� �ϳ��� ������ �մϴ�.
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL,
			d3dRtvCPUDescriptorHandle);//->���� Ÿ�� ����
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;//->�̰� �迭 �������� �̸�ŭ�÷���� ���´�  ����.. �ٽ� ��ﳪ�� �����غ�
	}
}

void CGameFramework::CreateDepthStencilView()//���� ���ٽ� �並 �����.
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//���ҽ��� ����(��: D3D12_RESOURCE_DIMENSION_TEXTURE1D) �Ǵ� �������� ����((D3D12_RESOURCE_DIMENSION_BUFFER)�� �����ϴ� D3D12_RESOURCE_DIMENSION �� �� ��� .
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;//3D�� ��� ���ҽ��� ���̸� �����ϰ� 1D �Ǵ� 2D ���ҽ��� �迭�� ��� �迭 ũ�⸦ �����մϴ�.
	d3dResourceDesc.MipLevels = 1;//miplevel�� ��
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DXGI_FORMAT �� �� ����� ���� �մϴ�.
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;//�� �Ӽ��� �����մϴ�.
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//�� ������ �����ϴ� D3D12_HEAP_TYPE ���� �� �Դϴ� .
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;//���� ���� cpu������ �Ӽ��� ����(cpu�� ������ �Ӽ��� �� �� �����ϴ�.)
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;//���� ���� �޸� Ǯ�� ����(unknown�޸� Ǯ�� �˼������ϴ�.)
	d3dHeapProperties.CreationNodeMask = 1;//���� ������� �۾��� ��� ���ҽ��� �����ؾ� �ϴ� ��带 ��Ÿ����.
	d3dHeapProperties.VisibleNodeMask = 1;//���� ������� �۾��� ��� ���ҽ��� ǥ�õǴ� ��� ����

	D3D12_CLEAR_VALUE d3dClearValue;//Ư�� ���ҽ��� ���� ����� �۾��� ����ȭ�ϴ� �� ���Ǵ� ��
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
	//����-���ٽ� ���۸� �����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();//���� ������ ��Ÿ���� CPU ������ �ڵ��� �����ɴϴ�.
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL,
		d3dDsvCPUDescriptorHandle);
	//����-���ٽ� ���� �並 �����Ѵ�.

}

void CGameFramework::CreateDirect2D()
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
}

void CGameFramework::CreateDirect2DDevice()
{
	UINT nD3D11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG) || defined(DBG)
	nD3D11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ID3D11Device* pd3d11Device = NULL;
	ID3D12CommandQueue* ppd3dCommandQueues[] = { m_pd3dCommandQueue };
	HRESULT hResult = ::D3D11On12CreateDevice(m_pd3dDevice, nD3D11DeviceFlags, NULL, 0, reinterpret_cast<IUnknown**>(ppd3dCommandQueues), _countof(ppd3dCommandQueues), 0, &pd3d11Device, &m_pd3d11DeviceContext, NULL);
	hResult = pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)&m_pd3d11On12Device);
	if (pd3d11Device) pd3d11Device->Release();

	D2D1_FACTORY_OPTIONS nD2DFactoryOptions = { D2D1_DEBUG_LEVEL_NONE };
#if defined(_DEBUG) || defined(DBG)
	nD2DFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	ID3D12InfoQueue* pd3dInfoQueue = NULL;
	if (SUCCEEDED(m_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pd3dInfoQueue))))
	{
		D3D12_MESSAGE_SEVERITY pd3dSeverities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO,
		};

		D3D12_MESSAGE_ID pd3dDenyIds[] =
		{
			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
		};

		D3D12_INFO_QUEUE_FILTER d3dInforQueueFilter = { };
		d3dInforQueueFilter.DenyList.NumSeverities = _countof(pd3dSeverities);
		d3dInforQueueFilter.DenyList.pSeverityList = pd3dSeverities;
		d3dInforQueueFilter.DenyList.NumIDs = _countof(pd3dDenyIds);
		d3dInforQueueFilter.DenyList.pIDList = pd3dDenyIds;

		pd3dInfoQueue->PushStorageFilter(&d3dInforQueueFilter);
	}
	pd3dInfoQueue->Release();
#endif

	hResult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &nD2DFactoryOptions, (void**)&m_pd2dFactory);

	IDXGIDevice* pdxgiDevice = NULL;
	hResult = m_pd3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pdxgiDevice);
	hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);
	hResult = m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pd2dDeviceContext);
	hResult = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pdWriteFactory);
	if (pdxgiDevice) pdxgiDevice->Release();

	m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.0f, 0.0f, 0.5f), &m_pd2dbrBackground);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0x9ACD32, 1.0f)), &m_pd2dbrBorder);

	hResult = m_pdWriteFactory->CreateTextFormat(L"�ü�ü", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL, 48.0f, L"en-US", &m_pdwFont);
	hResult = m_pdwFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hResult = m_pdwFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Purple, 1.0f), &m_pd2dbrText);
	hResult = m_pdWriteFactory->CreateTextLayout(L"�ؽ�Ʈ ���̾ƿ�", 8, m_pdwFont, 4096.0f, 4096.0f, &m_pdwTextLayout);

	float fDpi = (float)GetDpiForWindow(m_hwnd);
	D2D1_BITMAP_PROPERTIES1 d2dBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), fDpi, fDpi);

	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		m_pd3d11On12Device->CreateWrappedResource(m_ppd3dRenderTargetBuffers[i], &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&m_ppd3d11WrappedBackBuffers[i]));
		IDXGISurface* pdxgiSurface = NULL;
		m_ppd3d11WrappedBackBuffers[i]->QueryInterface(__uuidof(IDXGISurface), (void**)&pdxgiSurface);
		m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(pdxgiSurface, &d2dBitmapProperties, &m_ppd2dRenderTargets[i]);
		if (pdxgiSurface) pdxgiSurface->Release();
	}

#ifdef _WITH_DIRECT2D_IMAGE_EFFECT
	CoInitialize(NULL);
	hResult = ::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&m_pwicImagingFactory);

	hResult = m_pd2dFactory->CreateDrawingStateBlock(&m_pd2dsbDrawingState);
	hResult = m_pd2dDeviceContext->CreateEffect(CLSID_D2D1BitmapSource, &m_pd2dfxBitmapSource);
	hResult = m_pd2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &m_pd2dfxGaussianBlur);
	hResult = m_pd2dDeviceContext->CreateEffect(CLSID_D2D1EdgeDetection, &m_pd2dfxEdgeDetection);

	IWICBitmapDecoder* pwicBitmapDecoder;
	hResult = m_pwicImagingFactory->CreateDecoderFromFilename(L"test.jpg", NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pwicBitmapDecoder);
	IWICBitmapFrameDecode* pwicFrameDecode;
	pwicBitmapDecoder->GetFrame(0, &pwicFrameDecode);
	m_pwicImagingFactory->CreateFormatConverter(&m_pwicFormatConverter);
	m_pwicFormatConverter->Initialize(pwicFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	m_pd2dfxBitmapSource->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, m_pwicFormatConverter);

	m_pd2dfxGaussianBlur->SetInputEffect(0, m_pd2dfxBitmapSource);

	m_pd2dfxEdgeDetection->SetInputEffect(0, m_pd2dfxBitmapSource);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_STRENGTH, 0.5f);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_MODE, D2D1_EDGEDETECTION_MODE_SOBEL);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, false);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_ALPHA_MODE_PREMULTIPLIED);

	if (pwicBitmapDecoder) pwicBitmapDecoder->Release();
	if (pwicFrameDecode) pwicFrameDecode->Release();
#endif

}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	//ī�޶� ��ü�� �����Ͽ� ��ǥƮ,���� �簢��,���� ��ȯ ���,ī�޶� ��ȯ ����� �����ϰ� �����Ѵ�.
	//Scene Camera ���� 
	m_pCamera = new CCamera();
	m_pCamera->SetViewport(0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, m_nWndClientWidth, m_nWndClientHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) /
		float(m_nWndClientHeight), 90.0f);
	m_pCamera->GenerateViewMatrix(XMFLOAT3(0.0f, 15.0f, -25.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pCamera->CreateShaderVariables(m_pd3dDevice, m_pd3dCommandList);
	//LobbyCamera ����
	m_pLobbyCamera = new CCamera();
	m_pLobbyCamera->SetScissorRect(0, 0, m_nWndClientWidth, m_nWndClientHeight);
	m_pLobbyCamera->GenerateOhortoMatrix(1.0f, 500.0f, float(m_nWndClientWidth),
		float(m_nWndClientHeight));
	m_pLobbyCamera->GenerateViewMatrix(XMFLOAT3(0.0f, 15.0f, -25.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pLobbyCamera->CreateShaderVariables(m_pd3dDevice, m_pd3dCommandList);


	//�� ��ü�� �����ϰ� ���� ���Ե� ���� ��ü���� �����Ѵ�.
	m_pScene = new CScene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList, m_pCamera);
	//�κ� �� ��ü�� �������ش�.
	m_pLobbyScene = new LobbyCScene();
	m_pLobbyScene->BuildUIObjects(m_pd3dDevice, m_pd3dCommandList, m_pLobbyCamera);
	//�� ��ü�� �����ϱ� ���Ͽ� �ʿ��� �׷��� ��� ����Ʈ���� ��� ť�� �߰��Ѵ�.
	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	//�׷��� ��� ����Ʈ���� ��� ���� �� ������ ��ٸ���.
	WaitForGpuComplete();

	//�׷��� ���ҽ����� �����ϴ� ������ ������ ���ε� ���۵��� �Ҹ��Ų��.
	if (m_pScene)m_pScene->ReleaseUploadBuffers();
	if (m_pLobbyScene)m_pLobbyScene->ReleaseUploadBuffers();
	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;

	if (m_pLobbyScene) m_pLobbyScene->ReleaseObjects();
	if (m_pLobbyScene) delete m_pLobbyScene;
}



void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	if (m_bLobbyScene) {
		m_pLobbyScene->onProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	}
	else if (!m_bLobbyScene) {
		m_pScene->onProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	}
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		//::SetCapture(hWnd);
		//::GetCursorPos(&m_ptOldCursorPos);
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_bLobbyScene) {
		m_pLobbyScene->onProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	}
	else if (!m_bLobbyScene) {
		m_pScene->onProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	}
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F8:
			break;
		case VK_F9:
			ChangeSwapChainState();
			break;
		default:
			break;

		}
		break;
	default:
		break;
	}
}

LRESULT CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	GetKeyboardState(pKeysBuffer);
	//if&& m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene && !pKeysBuffer[VK_F1])
	{
		DWORD dwDirection = 0;
		if (pKeysBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hwnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);

			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		if ((dwDirection != DIRECTION::IDLE) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				//if (pKeysBuffer[VK_RBUTTON] & 0xF0)
				//	m_pCamera->Rotate(cyDelta, 0.0f, -cxDelta);
				//else {
					//g_Logic.

					/*g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetUp(), -30.0f * fTimeElapsed);
					g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y -= 30.0f * fTimeElapsed;
					g_NetworkHelper.SendRotatePacket(ROTATE_AXIS::Y, -30.0f * fTimeElapsed);*/
				g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetUp(), cxDelta);
				g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += cxDelta;
				g_NetworkHelper.SendRotatePacket(ROTATE_AXIS::Y, cxDelta);

				m_pCamera->Rotate(cyDelta, cxDelta, 0.0f);
				//}
			}
			if (dwDirection != DIRECTION::IDLE) {
				m_pCamera->Move(dwDirection, 1.21f, true);
			}
		}
	}
	//m_pCamera->Update(m_GameTimer.GetTimeElapsed());

}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	//CPU�潺�� ���� �����Ѵ�.
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}
void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(60.0f);
	ProcessInput();
	AnimateObjects();
	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	//��� �Ҵ��ڿ� ��� ����Ʈ�� �����Ѵ�.

	if (!m_bLobbyScene)
		m_pScene->OnPreRender(m_pd3dDevice, m_pd3dCommandList, m_pCamera);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource =
		m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	/*���� ���� Ÿ�ٿ� ���� ������Ʈ�� �����⸦ ��ٸ���. ������Ʈ�� ������ ���� Ÿ�� ������ ���´� ������Ʈ ����
	(D3D12_RESOURCE_STATE_PRESENT)���� ���� Ÿ�� ����(D3D12_RESOURCE_STATE_RENDER_TARGET)�� ��
	�� ���̴�.*/

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex *
		m_nRtvDescriptorIncrementSize);
	//������ ���� Ÿ�ٿ� �ش��ϴ� �������� CPU �ּ�(�ڵ�)�� ����Ѵ�. 
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	//����Ʈ�� ���� �簢���� �����Ѵ�. 
	float pfClearColor[4] = { 0.7f, 1.0f, 1.0f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle,
		pfClearColor/*Colors::Azure*/, 0, NULL);
	//���ϴ� �������� ���� Ÿ��(��)�� �����.
	//����-���ٽ� �������� CPU �ּҸ� ����Ѵ�.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	//���ϴ� ������ ����-���ٽ�(��)�� �����.
	//m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, 
	//&d3dDsvCPUDescriptorHandle);
	//���� Ÿ�� ��(������)�� ����-���ٽ� ��(������)�� ���-���� �ܰ�(OM)�� �����Ѵ�. //������ �ڵ�� ���⿡ �߰��� ���̴�.
	//m_pScene->OnPreRender(m_pd3dDevice, m_pd3dCommandList, m_pCamera);
	//Render2DFont();
	if (m_bLobbyScene)
	{
		if (m_pLobbyScene) m_pLobbyScene->UIRender(m_pd3dDevice, m_pd3dCommandList, m_pCamera);
	}
	else if (!m_bLobbyScene)
	{
		if (m_pScene) m_pScene->Render(m_pd3dDevice, m_pd3dCommandList, m_pCamera);
	}


#ifndef _WITH_DIRECT2D
	//m_pScene->CreateGraphicsPipelineState(m_pd3dDevice);
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
#endif
	/*���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ���. GPU�� ���� Ÿ��(����)�� �� �̻� ������� ������ ���� Ÿ��
	�� ���´� ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)�� �ٲ� ���̴�.*/
	hResult = m_pd3dCommandList->Close();
	//��� ����Ʈ�� ���� ���·� �����. 
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

#ifdef _WITH_DIRECT2D
	//Direct2D Drawing
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();

	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
#ifdef _WITH_DIRECT2D_IMAGE_EFFECT
	D2D_POINT_2F d2dPoint = { 100.0f, 100.0f };
	D2D_RECT_F d2dRect = { 300.0f, 300.0f, 750.0f, 550.0f };
	m_pd2dDeviceContext->DrawImage((m_nDrawEffectImage == 0) ? m_pd2dfxGaussianBlur : m_pd2dfxEdgeDetection, &d2dPoint, &d2dRect);
#endif
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	D2D1_RECT_F rcUpperText = D2D1::RectF(0, 0, szRenderTarget.width, szRenderTarget.height * 0.25f);
	m_pd2dDeviceContext->DrawTextW(m_pszFrameRate, (UINT32)wcslen(m_pszFrameRate), m_pdwFont, &rcUpperText, m_pd2dbrText);

	D2D1_RECT_F rcLowerText = D2D1::RectF(0, szRenderTarget.height * 0.5f, szRenderTarget.width, szRenderTarget.height);
	m_pd2dDeviceContext->DrawTextW(L"�ѱ� �׽�Ʈ", (UINT32)wcslen(L"�ѱ� �׽�Ʈ"), m_pdwFont, &rcLowerText, m_pd2dbrText);

	m_pd2dDeviceContext->EndDraw();

	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd3d11DeviceContext->Flush();
#endif
	//��� ����Ʈ�� ��� ť�� �߰��Ͽ� �����Ѵ�. 
	WaitForGpuComplete();
	//GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ���.
	/*DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present(0, 0);*/
	m_pdxgiSwapChain->Present(0, 0);

	MoveToNextFrame();

	/*����ü���� ������Ʈ�Ѵ�. ������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű����� ���� Ÿ�� ��
	������ �ٲ� ���̴�.*/
	//m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hwnd, m_pszFrameRate);
}

void CGameFramework::Render2DFont()
{
	RECT rc;
	GetClientRect(m_hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	wchar_t* pText = L"Hello, World!";

	// Direct2D �귯��
	ID2D1SolidColorBrush* pBrush = nullptr;

	// DirectWrite �ؽ�Ʈ ���� �� ���̾ƿ�
	IDWriteTextFormat* pTextFormat = nullptr;
	IDWriteTextLayout* pTextLayout = nullptr;

	// Direct2D ���� Ÿ��
	ID2D1HwndRenderTarget* pRenderTarget = nullptr;

	// Direct2D ���� Ÿ�� ����
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

	pD2DFactory->CreateHwndRenderTarget(&props, &D2D1::HwndRenderTargetProperties(m_hwnd, size), &pRenderTarget);

	// Direct2D �귯�� ����
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);

	// DirectWrite �ؽ�Ʈ ���� ����
	pDWriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"en-us", &pTextFormat);

	// DirectWrite �ؽ�Ʈ ���̾ƿ� ����
	pDWriteFactory->CreateTextLayout(pText, lstrlenW(pText), pTextFormat, size.width, size.height, &pTextLayout);

	// Direct2D ���� Ÿ�ٿ� �ؽ�Ʈ ���̾ƿ��� �׸�
	pRenderTarget->BeginDraw();
	pRenderTarget->DrawTextLayout(D2D1::Point2F(0, 0), pTextLayout, pBrush);
	pRenderTarget->EndDraw();
}

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
	m_pdxgiFactory = NULL;//DXGI 팩토리 인터페이스에 대한 포인터이다.//IDXGIFactory는 어뎁터(그래픽카드)(adaptor),출력장치(모니터),(output),디바이스를 포함하고 있는 컴객체이다.
	m_pdxgiSwapChain = NULL;///스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	m_pd3dDevice = NULL;//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.

	m_pd3dCommandAllocator = NULL; //Command List에는 ID3D12CommandAllocator가 하나 연관된다.Command List에 추가된 Command들은 이 Allocator의 메모리에 저장된다.
	m_pd3dCommandQueue = NULL; //명령 큐, CPU는 Command List를 Direct3D API를 통해 GPU의 Command Queue에 제출한다.
	m_pd3dPipelineState = NULL;//그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다.
	m_pd3dCommandList = NULL;//명령 리스트 인터페이스 포인터->CPU에서 관리한다.->Commanqueue로 보냄

	for (int i = 0; i < m_nSwapChainBuffers; i++)m_ppd3dRenderTargetBuffers[i] = NULL;//후면 버퍼를 m_nSwapChainBuffers갯수 만큼 만든다.
	m_pd3dDsvDescriptorHeap = NULL;//응용 프로그램에 필요한 서술자/ 뷰들을 담을 서술자 힙을 만들어야 한다.
	m_nRtvDescriptorIncrementSize = 0;//깊이-스텐실 서술자 원소의 크기이다

	m_pd3dDepthStencilBuffer = NULL;//깊이 스텐실 버퍼->Z BUFFer 스텐실
	m_pd3dDsvDescriptorHeap = NULL;//응용 프로그램에 필요한 서술자/ 뷰들을 담을 서술자 힙을 만들어야 한다
	m_nDsvDescriptorIncrementSize = 0;//깊이-스텐실 서술자 원소의 크기이다.

	m_nSwapChainBufferIndex = 0;//현재 스왑 체인의 후면 버퍼 인덱스이다.

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	m_pScene = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszFrameRate, _T("LapProject ("));

}

CGameFramework::~CGameFramework()
{
}

//다음 함수는 응용프로그램이 실행 되어 주 윈도우가 생성되면 호출된다는 것에 유의하라.
bool CGameFramework::onCreate(HINSTANCE hinstance, HWND hmainwnd)
{
	m_hinstance = hinstance;
	m_hwnd = hmainwnd;

	//Direct3D 디바이스,명령 큐와 명령 리스트 ,스왑 체인 등을 생성하는 함수를 호출한다.
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();



	BuildObjects();
	//랜더링할 게임 객체를 생성한다.
	return(true);
}

void CGameFramework::OnDestroy()
{
	WaitForGpuComplete();
	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.->commandqueue가 다 빠져 나갈 때 까지 기다린다.

	ReleaseObjects();
	//게임 객체(게임 월드 객체)를 소멸한다.

	::CloseHandle(m_hFenceEvent);//->헨들값을 반환
	for (int i = 0; i < m_nSwapChainBuffers; i++)if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();//->후면 버퍼를 전부 제거
	if (m_pd3dDepthStencilBuffer)m_pd3dDepthStencilBuffer->Release(); //rtzheap 제거

	if (m_pd3dDepthStencilBuffer)m_pd3dDepthStencilBuffer->Release();//깊이-스텐실 버퍼 제거
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();//DSVheap제거

	if (m_pd3dCommandAllocator)m_pd3dCommandAllocator->Release();//->Commandlist allocator ->제거
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();//그래픽 카드의 커맨드 큐 제거
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();//cpu커맨드 리스트와 GPU 커멘드큐를 연결하는 파이프라인 제거
	if (m_pd3dCommandList) m_pd3dCommandList->Release(); // cpu commandList제거

	if (m_pd3dFence) m_pd3dFence->Release();//그래픽 카드의 팬스 제거 

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);//전체화면 제거
	if (m_pdxgiSwapChain)m_pdxgiSwapChain->Release();//스왑체인(후면 버퍼와 전면 버퍼를 잇는 체인)을 Release();
	if (m_pd3dDevice)m_pd3dDevice->Release();//가상 디바이스(그래픽카드) 제거
	if (m_pdxgiFactory)m_pdxgiFactory->Release();//팩토리( 어뎁터(그래픽카드)(adaptor),출력장치(모니터),(output),디바이스를 )제거 

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
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);//전체 화면 모드와 연결된 상태를 가져옵니다.스왑 체인이 전체 화면 모드에 있는 경우 TRUE스왑 체인이 창 모드에 있는 경우 FALSE
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);//디스플레이 상태를 창 또는 전체 화면으로 설정합니다.디스플레이 상태를 창또는 전체 화면으로 설정할지 여부를 지정하는 Boolean 값입니다. 전체 화면의 경우 TRUE, 창에 대한 FALSE.

	DXGI_MODE_DESC dxgiTargetParameters;//디스플레이 모드를 설명합니다.
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//->화면 초기화
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;//->초기화 레이트
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);
	//후면 버퍼 초기화
	for (int i = 0; i < m_nSwapChainBuffers; i++)if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();
	//->스왑체인을 다시 만든다.
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
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다. dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
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
	m_nWndClientWidth = rcClient.right - rcClient.left;//가로 길이
	m_nWndClientHeight = rcClient.bottom - rcClient.top;//세로 길이

	//DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	//::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	//dxgiSwapChainDesc.Width = m_nWndClientWidth; //해상도 너비를 설명하는 값입니다
	//dxgiSwapChainDesc.Height = m_nWndClientHeight;//해상도 높이를 설명하는 값
	//dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//디스플레이 형식을 설명하는 DXGI_FORMAT 구조입니다.
	//dxgiSwapChainDesc.SampleDesc.Count=(m_bMsaa4xEnable)?4:1;//픽셀당 멀티 셈플 개수입니다.
	//dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	//dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//후면 버퍼에 대한 표면 사용량 및 CPU엑세스 옵션을 설명하는 DXGI_USAGE형식값
	//dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;//스왑 체인의 버퍼수를 설명하는 값,전체 화면 스왑 체인을 만들 때 일반적으로 이값에 전면 버퍼를 포함
	//dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;//후면 버퍼의 크기가 대상 출려과 같지 않을 경우 크기 조절 DXGI_SCALING 형식의 값.
	//dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//스왑 체인에서 사용되는 프레젠테이션 모델과 표면을 제시한 후 프레젠테이션 버퍼의 내용을 처리하는 옵션을 설명하는 DXGI_SWAP_EFFECT 형식의 값입니다.
	//dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; //스왑 체인 후면 버퍼의 투명도 동작을 식별하는 DXGI_ALPHA_MODE 형식값입니다.
	//dxgiSwapChainDesc.Flags = 0;//비트 방향 또는 작업을 사용하여 결합된 DXGI_SWAP_CHAIN_FLAG 형식값의 조합입니다. 결과 값은 스왑 체인 동작에 대한 옵션을 지정합니다.

	////스왑체인의 전체 화면 모드를 설명한다.

	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	//::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	//dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;//새로고침 빈도 60분자
	//dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;//새로고침 빈도 1분자
	//dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//스캔 선 그리기 모드를 설명하는 DXGI_MODE_SCANLINE_ORDER  STRUCT 형식의 멤버입니다.
	//dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//배율 조정 모드를 설명하는  DXGI_MODE_SCALING 확대된 형식의 멤버입니다
	//dxgiSwapChainFullScreenDesc.Windowed = TRUE; //스왑 체인이 창 모드에 있는지 여부를 지정하는 Boole 값입니다.스왑 체인이 창 모드에 있는 경우 TRUE; 그렇지 않으면 FALSE.

	//m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hwnd,
	//	&dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);//HWND 핸들과 연결된 스왑 체인을 스왑 체인의 출력 창에 만듭니다.
	////스왑체인을 생성한다.(in,in,in,in,in,out)->t순서대로 &m_pdxgiSwapChain로 출력한다.,

	//m_pdxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);

	////ALT+ENTER 키의 동작을 비활성화 한다.

	//m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();//->가장 중요할듯??


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
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다. dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
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

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);//다른 DXGI 개체를 생성하는 데 사용할 수 있는 DXGI 1.3 팩터리를 만듭니다.
	IDXGIAdapter1* pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc; //->어뎁터 만들기
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter,
			D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice)))break;
	}
	//모든 하드웨어 어댑터에 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//특성 레벨 12.0를 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;//msaa 퀄리티 레벨 디바이스
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//해상도 초기화
	d3dMsaaQualityLevels.SampleCount = 4;//Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;//동작 (??) 좀더 찾아 보자
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;//->다중 샘플링의 품질 수준 확인
	//디바이스가 지원하는 다중 샘플의 품질 수준을 확인다.

	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다. 

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	//펜스를 생성하고 펜스 값을 0으로 설정한다.

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*펜스와 동기화를 위한 이벤트 객체를 생성하다(이벤트 객체의 초기값을 FALSE이다).
	이벤트가 실행되면(Signal)이벤트의 값을 자동적으로 FALSE가 되도록 생성한다.*/

	//m_d3dviewport.TopLeftX = 0;
	//m_d3dviewport.TopLeftY = 0;
	//m_d3dviewport.Width = static_cast<float>(m_nWndClientWidth);
	//m_d3dviewport.Height = static_cast<float>(m_nWndClientHeight);
	//m_d3dviewport.MinDepth = 0.0f;
	//m_d3dviewport.MaxDepth = 1.0f;
	////뷰표트를 주 윈도우의 클라이언트 영역 전체로 설정한다.

	//m_d3dScissorRect = { 0,0,m_nWndClientWidth,m_nWndClientHeight };
	////씨저 사각형을 주 윈도우의 클라이언트 영역 전체로 설정한다.

	if (pd3dAdapter)pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;//큐 생성
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,
		__uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);
	//직접(Direct) 명령 큐를 생성한다.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	//직접(Direct)명령 할당자를 생성한다.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//직접(Direct)명령 리스트를 생성한다.

	hResult = m_pd3dCommandList->Close();
	//명령 리스트는 생성되면 열린(Open)상태이므로 닫힌(Closed)상태로 만든다.

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
	//렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다.
	m_nRtvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//렌더 타겟 서술자 힙의 원소의 크기를 저장한다.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	//깊이 스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.
	m_nDsvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//깊이-스텐실 서술자 힙의 원소의 크기를 저장한다.
}

void CGameFramework::CreateRenderTargetViews()
{
	//스왑체인의 각 후면 버퍼에 대한 렌더 타겟뷰를 생성한다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void
			**)&m_ppd3dRenderTargetBuffers[i]);//->스왑체인의 후면버퍼 중 하나에 엑세스 합니다.
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL,
			d3dRtvCPUDescriptorHandle);//->랜더 타겟 생성
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;//->이게 배열 느낌으로 이만큼올려줘야 나온다  였다.. 다시 기억나면 수정해봄
	}
}

void CGameFramework::CreateDepthStencilView()//깊이 스텐실 뷰를 만든다.
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//리소스의 차원(예: D3D12_RESOURCE_DIMENSION_TEXTURE1D) 또는 버퍼인지 여부((D3D12_RESOURCE_DIMENSION_BUFFER)를 지정하는 D3D12_RESOURCE_DIMENSION 의 한 멤버 .
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;//3D인 경우 리소스의 깊이를 지정하고 1D 또는 2D 리소스의 배열인 경우 배열 크기를 지정합니다.
	d3dResourceDesc.MipLevels = 1;//miplevel의 수
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DXGI_FORMAT 의 한 멤버를 지정 합니다.
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;//힙 속성을 설명합니다.
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//힙 유형을 지정하는 D3D12_HEAP_TYPE 유형 값 입니다 .
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;//힙에 대한 cpu페이지 속성을 저장(cpu가 페이지 속성을 알 수 없습니다.)
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;//힙에 대한 메모리 풀을 설정(unknown메모리 풀을 알수없습니다.)
	d3dHeapProperties.CreationNodeMask = 1;//다중 어댑터의 작업의 경우 리소스를 생성해야 하는 노드를 나타낸다.
	d3dHeapProperties.VisibleNodeMask = 1;//다중 어댑터의 작업의 경우 리소스가 표시되는 노드 집합

	D3D12_CLEAR_VALUE d3dClearValue;//특정 리소스에 대한 지우기 작업을 최적화하는 데 사용되는 값
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
	//깊이-스텐실 버퍼를 생성한다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();//힙의 시작을 나타내는 CPU 설명자 핸들을 가져옵니다.
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL,
		d3dDsvCPUDescriptorHandle);
	//깊이-스텐실 버퍼 뷰를 생성한다.

}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	//카메라 객체를 생성하여 뷰표트,씨저 사각형,투영 변환 행렬,카메라 변환 행렬을 생성하고 설정한다.
	m_pCamera = new CCamera();
	m_pCamera->SetViewport(0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, m_nWndClientWidth, m_nWndClientHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) /
		float(m_nWndClientHeight), 90.0f);
	m_pCamera->GenerateViewMatrix(XMFLOAT3(0.0f, 15.0f, -25.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pCamera->CreateShaderVariables(m_pd3dDevice, m_pd3dCommandList);
	//씬 객체를 생성하고 씬에 포함될 게임 객체들을 생성한다.
	m_pScene = new CScene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList, m_pCamera);
	//씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가한다.
	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	//그래픽 명령 리스트들이 모두 실행 될 때까지 기다린다.
	WaitForGpuComplete();

	//그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸시킨다.
	if (m_pScene)m_pScene->ReleaseUploadBuffers();

	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}



void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pScene->onProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
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
	m_pScene->onProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
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
				if (pKeysBuffer[VK_RBUTTON] & 0xF0)
					m_pCamera->Rotate(cyDelta, 0.0f, -cxDelta);
				else {
					//g_Logic.

					/*g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetUp(), -30.0f * fTimeElapsed);
					g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y -= 30.0f * fTimeElapsed;
					g_NetworkHelper.SendRotatePacket(ROTATE_AXIS::Y, -30.0f * fTimeElapsed);*/
					g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->Rotate(&g_Logic.m_inGamePlayerSession[0].m_currentPlayGameObject->GetUp(), cxDelta);
					g_Logic.m_inGamePlayerSession[0].m_rotateAngle.y += cxDelta;
					g_NetworkHelper.SendRotatePacket(ROTATE_AXIS::Y, cxDelta);

					m_pCamera->Rotate(cyDelta, cxDelta, 0.0f);
				}
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
	//CPU펜스의 값을 증가한다.
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
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
	//명령 할당자와 명령 리스트를 리셋한다.

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
	/*현재 렌더 타겟에 대한 프리젠트가 끝나기를 기다린다. 프리젠트가 끝나면 렌더 타겟 버퍼의 상태는 프리젠트 상태
	(D3D12_RESOURCE_STATE_PRESENT)에서 렌더 타겟 상태(D3D12_RESOURCE_STATE_RENDER_TARGET)로 바
	뀔 것이다.*/

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex *
		m_nRtvDescriptorIncrementSize);
	//현재의 렌더 타겟에 해당하는 서술자의 CPU 주소(핸들)를 계산한다. 
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, FALSE, &d3dDsvCPUDescriptorHandle);
	//뷰포트와 씨저 사각형을 설정한다. 
	float pfClearColor[4] = { 1.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle,
		pfClearColor/*Colors::Azure*/, 0, NULL);
	//원하는 색상으로 렌더 타겟(뷰)을 지운다.
	//깊이-스텐실 서술자의 CPU 주소를 계산한다.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	//원하는 값으로 깊이-스텐실(뷰)을 지운다.
	//m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, 
	//&d3dDsvCPUDescriptorHandle);
	//렌더 타겟 뷰(서술자)와 깊이-스텐실 뷰(서술자)를 출력-병합 단계(OM)에 연결한다. //렌더링 코드는 여기에 추가될 것이다.
	if (m_pScene) m_pScene->Render(m_pd3dDevice, m_pd3dCommandList, m_pCamera);
	//m_pScene->CreateGraphicsPipelineState(m_pd3dDevice);
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	/*현재 렌더 타겟에 대한 렌더링이 끝나기를 기다린다. GPU가 렌더 타겟(버퍼)을 더 이상 사용하지 않으면 렌더 타겟
	의 상태는 프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)로 바뀔 것이다.*/
	hResult = m_pd3dCommandList->Close();
	//명령 리스트를 닫힌 상태로 만든다. 
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);
	//명령 리스트를 명령 큐에 추가하여 실행한다. 
	WaitForGpuComplete();
	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.
	/*DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present(0, 0);*/
	m_pdxgiSwapChain->Present(0, 0);

	MoveToNextFrame();

	/*스왑체인을 프리젠트한다. 프리젠트를 하면 현재 렌더 타겟(후면버퍼)의 내용이 전면버퍼로 옮겨지고 렌더 타겟 인
	덱스가 바뀔 것이다.*/
	//m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hwnd, m_pszFrameRate);
}
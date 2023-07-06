#pragma once
#include"stdafx.h"
#include"Timer.h"
#include"Scene.h"
#include"LobbyScene.h"


class UILayer;
class CCamera;
class CGameFramework
{

private:
	HINSTANCE m_hinstance;//우리가 만들 exe를 나타내주는 변수이다.
	HWND m_hwnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;
	bool m_bMouseCaptured;
	IDXGIFactory4* m_pdxgiFactory;//DXGI 팩토리 인터페이스에 대한 포인터이다.//
	//IDXGIFactory는 어뎁터(그래픽카드)(adaptor),출력장치(모니터),(output),디바이스를 포함하고 있는 컴객체이다.
	IDXGISwapChain3* m_pdxgiSwapChain;
	//스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	ID3D12Device* m_pd3dDevice;
	//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.

	bool m_bMsaa4xEnable = false;
	bool m_bFullScreen=false;
	UINT m_nMsaa4xQualityLevels = 0;
	//MSAA(Multi Sampleing AA) 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다. SSAA(SUPER SAMPLING AA)도 있다.

	static const UINT m_nSwapChainBuffers = 2;
	//스왑 체인의 후면 버퍼의 개수이다.

	UINT m_nSwapChainBufferIndex;
	//현재 스왑 체인의 후면 버퍼 인덱스이다.

	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];//렌더 타겟버퍼는 전면 m_nRtvDescriptorIncrementSize버퍼를 나타내는 것이 아닌 
	//후면 버퍼를 나타내는 것이다.배열을 쓰는 이유는 후면 버퍼가 여러개 준뵈 될 수 있어서 이다.
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	//다음으로, 응용 프로그램에 필요한 서술자 / 뷰들을 담을 서술자 힙을 만들어야 한다.
	UINT m_nRtvDescriptorIncrementSize;
	//렌더 타겟 서술자 원소의 크기이다.

	ID3D12Resource* m_pd3dDepthStencilBuffer;//깊이 스텐실 버퍼->Z BUFFER 스텐실 ->안 보이는 것은 삭제하는 것
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;//응용 프로그램에 필요한 서술자/ 뷰들을 담을 서술자 힙을 만들어야 한다.ㅂ
	UINT m_nDsvDescriptorIncrementSize;//깊이-스텐실 서술자 원소의 크기이다.

	ID3D12CommandQueue* m_pd3dCommandQueue;//명령 큐,CPU는 Command List를 Direct3D API를 통해 GPU의 Command Queue에 제출한다.
	ID3D12CommandAllocator* m_pd3dCommandAllocator;//Command List에는 ID3D12CommandAllocator가 하나 연관된다.Command List에 추가된 Command들은 이 Allocator의 메모리에 저장된다.
	ID3D12GraphicsCommandList* m_pd3dCommandList;//명령 리스트 인터페이스 포인터

	ID3D12PipelineState* m_pd3dPipelineState;
	//그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다.

	ID3D12Fence* m_pd3dFence; //이때 필요한 것이 바로 울타리(Fence)이다.울타리(펜스)는 ID3D12Fence 인터페이스로 대표되며, GPU와 CPU의 동기화를 위한 수단으로 쓰인다.다음은 펜스 객체를 생성하는 메서드이다
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;
	//펜스 인터페이스 포인터,펜스의 값,이벤트 핸들이다.

	CScene* m_pScene;
	LobbyCScene* m_pLobbyScene;

	bool m_bLobbyScene;
	bool m_bSceneBuild{false};

	CCamera* m_pCamera = NULL;
	CCamera* m_pUICamera = NULL;

	//다음은 게임 프레임 워크에서 사용할 타이머이다.
	CGameTimer m_GameTimer;

	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.
	_TCHAR m_pszFrameRate[50];

	UILayer* m_pUILayer = NULL;

	//2d ui리소스를 띄우는 
	ID2D1Factory* pD2DFactory = nullptr;
	IDWriteFactory* pDWriteFactory = nullptr;

#ifdef _WITH_DIRECT2D
	ID3D11On12Device* m_pd3d11On12Device = NULL;
	ID3D11DeviceContext* m_pd3d11DeviceContext = NULL;
	ID2D1Factory3* m_pd2dFactory = NULL;
	IDWriteFactory* m_pdWriteFactory = NULL;
	ID2D1Device2* m_pd2dDevice = NULL;
	ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

	ID3D11Resource* m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1* m_ppd2dRenderTargets[m_nSwapChainBuffers];

	ID2D1SolidColorBrush* m_pd2dbrBackground = NULL;
	ID2D1SolidColorBrush* m_pd2dbrBorder = NULL;
	IDWriteTextFormat* m_pdwFont = NULL;
	IDWriteTextLayout* m_pdwTextLayout = NULL;
	ID2D1SolidColorBrush* m_pd2dbrText = NULL;
#endif



	POINT						m_ptOldCursorPos;
public:
	CGameFramework();
	~CGameFramework();

	bool onCreate(HINSTANCE hinstance, HWND hmainwnd);
	//프레임워크를  초기화하는 함수이다(주 윈도우가 생성되면 호출된다.)
	void OnDestroy();
	//프레임워크 파괴

	void ChangeSwapChainState();

	void CreateSwapChain();//스왑체인
	void CreateRtvAndDsvDescriptorHeaps();//서술자 힙
	void CreateDirect3DDevice();//디바이스 생성
	void CreateCommandQueueAndList();//명령 큐,할당자,리스트를 생성하는 함수
	//스왑 체인,디바이스 ,서술자 힙,명령 큐/할당자/리스트를 생성하는 함수이다.

	void CreateRenderTargetViews();//랜더 타겟 뷰를 생성하는 함수
	void CreateDepthStencilView();// 깊이 - 스텐실 뷰를 생성하는 함수이다.
	void CreateDirect2D();
	//랜더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다.

#ifdef _WITH_DIRECT2D
	void CreateDirect2DDevice();
#endif

	void BuildObjects();
	void ReleaseObjects();
	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다.

	//프레임워크 핵심(사용자 입력,애니메이션,렌더링)을 구성하는 함수이다.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void Render2DFont();
	
	void WaitForGpuComplete();//CPU와 GPu를 동기화하는 함수이다.
	//https://vitacpp.tistory.com/m/50

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);//LRESULT CALLBACK OnProcessingWindowMessage->CALLBACK이 안되는 이유를 모르겠음

	void SetMouseCapture(bool capture) { m_bMouseCaptured = capture; }

	CScene* GetScene() { return m_pScene; }

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다.

	void MoveToNextFrame();
	friend class Logic;
};

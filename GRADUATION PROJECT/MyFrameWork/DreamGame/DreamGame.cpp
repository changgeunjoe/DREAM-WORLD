#include "stdafx.h"
// LabProject00.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//


#include "DreamGame.h"
#include"GameFramework.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"

#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

CGameFramework gGameFramework;
NetworkHelper g_NetworkHelper;
clientNet::Logic g_Logic;


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK IPProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	while (!g_NetworkHelper.TryConnect());
	g_NetworkHelper.Start();
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_LogIn), NULL, IPProc);
	LPCTSTR lpszClass = _T("Dream World");
	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DREAMGAME, szWindowClass, MAX_LOADSTRING);
	//윈도우클래스를 등록한다
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화(주 윈도우 생성)를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DREAMGAME));


	MSG msg;
	
	// 기본 메시지 루프입니다:
	//다. GetMessage() API 함수가 FALSE를 반환하는 경우는 메시지 큐에서 가져온 메시지가 WM_QUIT 메시지일 때이다.WM_QUIT 메시지는 응용 프로그램을 종료하는 경우에 발생한다.다른 메시지의 경우에는
	//TRUE를 반환한다.그러므로 다음의 메시지 루프는 WM_QUIT 메시지를 처리할 때까지 계속
	//반복하여 실행될 것이다.
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			gGameFramework.FrameAdvance();
		}
	}

	gGameFramework.OnDestroy();
}
/*응용 프로그램 마법사가 생성한 코드의 메시지 루프는 응용 프로그램이 처리해야 할 윈도우 메시지가 메
시지 큐에 있으면 꺼내와서 처리를 하고 메시지가 없으면 CPU를 운영체제로 반납하도록 되어있다
(GetMessage() API 함수).그러나 게임 프로그램은 프로그램이 처리할 메시지가 없더라도 화면 렌더링, 사용자 입력처리, 길찾기 등의 작업이 계속 진행되어야 한다.그러므로 만약 처리할 메시지가 없더라도
CPU를 반납하지 않고 게임이 계속 진행되도록 해야 한다.이를 위해서 윈도우 메시지 루프를
PeekMessage() API 함수를 사용하여 변경한다.PeekMessage() API 함수는 메시지 큐를 살펴보고 메시
지가 있으면 메시지를 꺼내고 TRUE를 반환한다.만약 메시지 큐에 메시지가 없으면 FALSE를 반환한다.그러므로 PeekMessage() 함수가 TRUE를 반환하는 경우(응용 프로그램이 처리해야 할 윈도우 메시지가
	메시지 큐에 있으면) 정상적인 윈도우 메시지 처리 과정을 수행해야 한다.그러나 FALSE를 반환하는 경
	우(메시지 큐가 비어있으면) gGameFramework.FrameAdvance() 함수를 호출하여 게임 프로그램이 CPU
	를 사용할 수 있도록 해야 한다.그리고 이 과정은 사용자가 프로그램을 종료할 때까지 계속 반복되도록
	한다.그리고 메시지 루프가 종료되면 gGameFramework.OnDestroy() 함수를 호출하여 프레임워크 객체를 소
멸하도록 한다.*/


//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;//윈도우 프로시져를 설정한다.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_DREAMGAME));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LABPROJECT00);
	wcex.lpszMenuName = NULL;//->주 윈도우의 메뉴가 나타나지 않도록 한다.
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	//wcex.lpszMenuName=NULL
	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)//주 윈도우를 생성하고 화면에 보이도록 하는 함수이다.
{
	//hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU |
		WS_BORDER;
	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH,FRAME_BUFFER_HEIGHT };
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT,
		CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);//윈도우를 생성하는 윈도우 API Creat Window

	if (!hMainWnd)
	{
		return FALSE;
	}
	gGameFramework.onCreate(hInstance, hMainWnd);

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

#ifdef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	gGameFramework.ChangeSwapChainState();
#endif
	return TRUE;
}
//프로그램의 주 윈도우가 생성되면 CGameFramework 클래스의 OnCreate() 함수를 호출하여 프레임워크 
//객체를 초기화하도록 한다

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//DisPatchmessage실행되면 실행
{
	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return(::DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


const int g_MaximumIdLength = 20;
const int g_MaximumPwLength = 20;
char userID[g_MaximumIdLength] = { 0 };
char userPW[g_MaximumPwLength] = { 0 };

INT_PTR CALLBACK IPProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int i = 0;
	switch (uMsg) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK1: // 로그인 버튼 누르면 입력한 데이터를 가져오는 곳
		{
			int getID = GetDlgItemTextA(hDlg, IDC_EDIT1, userID, g_MaximumIdLength);		// 입력받은 길이를 리턴
			int getPW = GetDlgItemTextA(hDlg, IDC_EDIT2, userPW, g_MaximumPwLength);
			if (!getID || !getPW || getID > g_MaximumIdLength || getPW > g_MaximumPwLength)	// 아이디 혹은 비밀번호를 입력하지 않았으면 창이 안꺼지게 설정
			{
				return TRUE;
			}
			userID[getID] = 0;
			userPW[getPW] = 0;
			g_NetworkHelper.SendLoginData(userID, userPW);
			cout << "ID : " << userID << endl;
			cout << "PW : " << userPW << endl;
			EndDialog(hDlg, IDCANCEL);
		}
		case IDCANCEL: // X버튼 누르면 끄는 작업.
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
	}
	return FALSE;
}
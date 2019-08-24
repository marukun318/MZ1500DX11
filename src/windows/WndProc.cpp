// WinProc.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//
// Direct3D11 ��{�I�ȏ������{�|���S�����_�J���[�`��
//

#include	"WinAppDX11.h"
#include	<d3d11_1.h>
#include	"../windows/CMD3D11/CMD3D11.h"
#include	"../windows/CMDINPUT/CMDINPUT.h"
#include	"../windows/CMXINPUT/CMXINPUT.h"


#define MAX_LOADSTRING 100

// �R�����g�A�E�g�����mmtimer���g��
#define UseQueryPerformanceCounter

// �z��t���[����
#define FRAME_PER_SECONDS	60

// �O���[�o���ϐ�:
HWND                    g_hWnd = nullptr;
HINSTANCE				g_hInst = nullptr;       // ���݂̃C���^�[�t�F�C�X
FLOAT					g_dpiX;
FLOAT					g_dpiY;

WCHAR szTitle[MAX_LOADSTRING];                  // �^�C�g�� �o�[�̃e�L�X�g
WCHAR szWindowClass[MAX_LOADSTRING];            // ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// DirectX ������
CMD3D11		d3d11;
CMDINPUT	dinput;		// dinput
CMXINPUT	xinput;		// xinput

//
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �����ɃR�[�h��}�����Ă��������B
	// ref: https://msdn.microsoft.com/ja-jp/library/windows/desktop/ff485844(v=vs.85).aspx
	// �E�B���h�E���쐬����X���b�h�̏ꍇ�� COINIT_APARTMENTTHREADED �t���O���A���̑��̃X���b�h�� COINIT_MULTITHREADED ���g�p����̂���ʓI�ł��B
	// ��L�Ő��������t���O�ɉ����āAdwCoInit �p�����[�^�[�� COINIT_DISABLE_OLE1DDE �t���O��ݒ肷����@����������܂��B
	// ���̃t���O��ݒ肷�邱�ƂŁA���݂͎g�p����Ȃ��Ȃ��� OLE (Object Linking and Embedding) 1.0 �e�N�m���W�ɔ����I�[�o�[�w�b�h�̈ꕔ������ł��܂��B
	HRESULT hr;
	if (FAILED(hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
	{
		MessageBox(nullptr, L"CoInitializeEx Init Error", L"Error", MB_OK);
		return FALSE;
	}

#ifdef _DEBUG
	// �u�b�����^�C���f�o�b�O���[�h�n�m
	int nFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	nFlag &= ~(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF |
		_CRTDBG_CHECK_EVERY_16_DF | _CRTDBG_CHECK_EVERY_128_DF | _CRTDBG_CHECK_EVERY_1024_DF);
	nFlag |= (_CRTDBG_ALLOC_MEM_DF | /* _CRTDBG_CHECK_CRT_DF | */ _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	_CrtSetDbgFlag(nFlag);
#endif


#ifdef UseQueryPerformanceCounter
	LARGE_INTEGER liFreq;
	QueryPerformanceFrequency(&liFreq);
#endif

    // �O���[�o������������������Ă��܂��B
	g_hInst = hInstance;
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // �A�v���P�[�V�����̏����������s���܂�:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPP));

	// DirectX11�����������܂��B
	hr = d3d11.InitDevice();
	if (FAILED(hr))
	{
		MessageBox(g_hWnd, L"DirectX11 initialization failed.", L"Error", MB_OK);
		return 0;
	}

	// DirectInput Init
	dinput.Init(hInstance, g_hWnd);
	// XInput Init
	xinput.Init();




#ifdef UseQueryPerformanceCounter
	LARGE_INTEGER liTimeOld;
	LARGE_INTEGER liTimeNow;
	LARGE_INTEGER liTimePeriod;
	QueryPerformanceCounter(&liTimeOld);
	QueryPerformanceCounter(&liTimeNow);
	liTimePeriod.QuadPart = liFreq.QuadPart / FRAME_PER_SECONDS;
#else
	timeBeginPeriod(1);
	DWORD dwTimeOld = timeGetTime();
	DWORD dwTimeNow = dwTimeOld;
#endif

	MSG msg = { 0 };

	// ���C�� ���b�Z�[�W ���[�v:
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			// �����Ƀ��C���^�X�N�̏���������
			// �X�V���Ԃ��v��
#ifdef UseQueryPerformanceCounter
			QueryPerformanceCounter(&liTimeNow);
			if ((liTimeNow.QuadPart - liTimeOld.QuadPart) >= liTimePeriod.QuadPart)
#else
			dwTimeNow = timeGetTime();
			if ((dwTimeNow - dwTimeOld) >= 17)
#endif
			{
				// ��莞�Ԍo�����Ȃ�
#ifdef UseQueryPerformanceCounter
				QueryPerformanceCounter(&liTimeOld);
#else
				dwTimeOld = dwTimeNow;
#endif
				dinput.Update();							// DirectInput ����
				xinput.Update();							// XInput ����
				// User job
				d3d11.Render();
			}
		}
	}

#ifndef UseQueryPerformanceCounter
	timeEndPeriod(1);
#endif

	// ���C�u�����̉��
	xinput.Cleanup();										// XInput
	dinput.Cleanup();										// DirectInput


	UnregisterClass(nullptr, hInstance);

    return (int) msg.wParam;
}

//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

  //�E�C���h�E�̃N���C�A���g�̈�i=DirectX�̕`��̈�j���w��
   const DWORD dwStyle = WS_OVERLAPPEDWINDOW & (~(WS_MAXIMIZEBOX |WS_THICKFRAME));
   RECT rc = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
#if _MSC_VER >= 1900	// Visual Studio 2015 �ȍ~��������Window�c��L�΂�
   rc.bottom += 20;
#endif
   // Because the CreateWindow function takes its size in pixels,
   // obtain the system DPI and use it to scale the window size.
   ID2D1Factory1* pDirect2dFactory = nullptr;

   // Create a Direct2D factory
   D2D1_FACTORY_OPTIONS options = {};
#ifdef _DEBUG
   options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
   HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &options, reinterpret_cast<void**>(&pDirect2dFactory));
   if (hr == E_NOINTERFACE) {
	   MessageBox(g_hWnd, L"D2D1CreateFactory() : Update required.", L"Error", MB_OK);
	   return FALSE;
   }
   if (FAILED(hr)) {
	   MessageBox(g_hWnd, L"D2D1CreateFactory() failed.", L"Error", MB_OK);
	   return FALSE;
   }

   // The factory returns the current system DPI. This is also the value it will use
   // to create its own windows.
   pDirect2dFactory->GetDesktopDpi(&g_dpiX, &g_dpiY);

   if (pDirect2dFactory != nullptr) {
	   pDirect2dFactory->Release();
	   pDirect2dFactory =  nullptr;
   }

   rc.right = static_cast<UINT>(ceil((FLOAT)rc.right * g_dpiX / 96.f));
   rc.bottom = static_cast<UINT>(ceil((FLOAT)rc.bottom * g_dpiY / 96.f));

   AdjustWindowRect(&rc, dwStyle, FALSE);

   // Window Size��ύX�����Ȃ�
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle,
	   CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);


   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;				// �O���[�o���ϐ��ɓ����

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:    ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND  - �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT    - ���C�� �E�B���h�E�̕`��
//  WM_DESTROY  - ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �I�����ꂽ���j���[�̉��:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_ACTIVATE:
		if (WA_INACTIVE == wParam)
		{
			// Window��A�N�e�B�u
			dinput.Unacquire();
		}
		else {
			// Window�A�N�e�B�u
			dinput.Acquire();
		}
		return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC ���g�p����`��R�[�h�������ɒǉ����Ă�������...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
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

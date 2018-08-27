// Client.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Client.h"
#include "../GameObjects.h"


#define PIPE_NAME TEXT("\\\\.\\pipe\\comunicationpipe")
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

GameData gamedata;
int startGame;
TCHAR name[50];
HANDLE hPipe;
int send = 0;

HANDLE IOReadyPedido;
OVERLAPPED ovPedido;

DWORD WINAPI ReceviCurrentData(LPVOID param);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Play play;
	DWORD n;	
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
			if (send == 0)
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, About);
				send = 1;
			}
        }
        break;
	case WM_KEYDOWN:
		play.action = -1;
		switch (wParam)
		{
		case VK_RIGHT:
			play.action = 'd';
			break;
		case VK_LEFT:
			play.action = 'a';
			break;
		case VK_UP:
			play.action = 'w';
			break;
		case VK_DOWN:
			play.action = 's';
			break;
		case VK_SPACE:
			play.action = 32;
			break;
		case VK_F5:
			play.action = 'b';
			break;
		default:
			break;
		}
		if (play.action != -1)
		{
			_tcscpy(play.user, name);

			ZeroMemory(&ovPedido, sizeof(ovPedido));
			ResetEvent(IOReadyPedido);
			ovPedido.hEvent = IOReadyPedido;

			if (!WriteFile(hPipe, &play, sizeof(Play), NULL, &ovPedido))
			{
				exit(-1);
			}
			WaitForSingleObject(IOReadyPedido, INFINITE);
			GetOverlappedResult(hPipe, &ovPedido, &n, FALSE);
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Play play;
	startGame = 1;
	HANDLE receviCurrentData;
	DWORD n;
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK  )
        {
			//ler dados das editboxes e mandar para o Gateway
			GetDlgItemText(hDlg, IDC_EDIT1, name, 50);


			_tprintf(TEXT("[CLIENTE] Esperar pelo pipe '%s' (WaitNamedPipe)\n"), PIPE_NAME);
			if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER))
			{
				_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), PIPE_NAME);
				exit(-1);
			}

			_tprintf(TEXT("[CLIENTE] Ligação ao pipe do gateway... (CreateFile)\n"));
			hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			if (hPipe == NULL)
			{
				_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), PIPE_NAME);
				exit(-1);
			}
			DWORD code = PIPE_READMODE_MESSAGE;
			SetNamedPipeHandleState(hPipe, &code, NULL, NULL);

			receviCurrentData = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceviCurrentData, NULL, 0, NULL);

			play.action = 1;
			_tcscpy(play.user, name);

			ZeroMemory(&ovPedido, sizeof(ovPedido));
			ResetEvent(IOReadyPedido);
			ovPedido.hEvent = IOReadyPedido;

			if (!WriteFile(hPipe, &play, sizeof(Play), NULL, &ovPedido))
			{
				exit(-1);
			}
			WaitForSingleObject(IOReadyPedido, INFINITE);
			GetOverlappedResult(hPipe, &ovPedido, &n, FALSE);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

DWORD WINAPI ReceviCurrentData(LPVOID param)
{
	HANDLE IOReady;
	OVERLAPPED ov;
	DWORD n;
	int ret;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	while (startGame == 1)
	{
		ZeroMemory(&ov, sizeof(ov));
		ResetEvent(IOReady);
		ov.hEvent = IOReady;

		ReadFile(hPipe, &gamedata, sizeof(GameData), NULL, &ov);

		WaitForSingleObject(IOReady, INFINITE);
		ret = GetOverlappedResult(hPipe, &ov, &n, FALSE);

		//if (ret && n)
			//InvalidateRect(hWnd, NULL, FALSE);
	}
	CloseHandle(IOReady);
	return 0;
}
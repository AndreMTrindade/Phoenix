// Server.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Server.h"

#include "../GameObjects.h"
#include "../ComunicationDLL/functionsCommunicationDLL.h"

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


Game game;
ControlData gameData;
ControlPlays playsData;
int info = 0;
Client *lPlayers = NULL;
int ID = 0;
HWND playersIn;

void WINAPI ClientRequest(void * dados);
INT_PTR CALLBACK InitialConfiguration(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK StartGame(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

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
	LoadStringW(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVER));

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

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SERVER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case WM_CREATE:
			game.startGame = 1;
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
		if (info == 0)
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, InitialConfiguration);
			info = 1;

		}
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

void WINAPI ClientRequest(void * dados)
{
	ControlPlays* shared = (ControlPlays*)dados;
	Play play;
	Client *newc;
	Client *last;
	TCHAR temp[100] = TEXT("");
	last = lPlayers;
	int i = 0;
	while (1)
	{
		_tprintf(TEXT("Esperando por clientes:\n"));
		ReadClientRequest(&play, shared->data);
		if (play.action == 1)
		{
			_tprintf(TEXT("Cliente: %s Acao: %d\n"), play.user, play.action);
			_tcscat(play.user, TEXT("\n"));
			_tcscat(temp, play.user);
			SetDlgItemText(playersIn, IDC_PLAYERS, temp);
			newc = (Client*)malloc(sizeof(Client));
			_tcscpy_s(newc->user, play.user);
			ID++;
			newc->PID = ID;
			newc->p = NULL;
			last->p = newc;
			last = newc;
		}
	}
}

INT_PTR CALLBACK InitialConfiguration(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			int temp;
			TCHAR data[20];

			GetDlgItemText(hDlg, IDC_NPLAYERS, data, 3);
			if (_tccmp(data, TEXT("")) == 0)
				game.configuration.nplayers = 1;
			else
			temp = _tstoi(data);
			game.configuration.nplayers = temp;

			GetDlgItemText(hDlg, IDC_VBOOBS, data, 3);
			if (_tccmp(data, TEXT("")) == 0)
				game.configuration.v_bombs = 1;
			else
			temp = _tstoi(data);
			game.configuration.v_bombs = temp;

			GetDlgItemText(hDlg, IDC_VPOWERUP, data, 3);
			if (_tccmp(data, TEXT("")) == 0)
				game.configuration.v_powerUp = 1;
			else
			temp = _tstoi(data);
			game.configuration.v_powerUp = temp;

			GetDlgItemText(hDlg, IDC_VSHOOT, data, 3);
			if (_tccmp(data, TEXT("")) == 0)
				game.configuration.v_shoots = 1;
			else
			temp = _tstoi(data);
			game.configuration.v_shoots = temp;

			GetDlgItemText(hDlg, IDC_SHOTTRATE, data, 3);
			if (_tccmp(data, TEXT("")) == 0)
				game.configuration.shootRate = 1;
			else
			temp = _tstoi(data);
			game.configuration.shootRate = temp;

			GetDlgItemText(hDlg, IDC_POWERUPSTIME, data, 3);
			if (_tccmp(data, TEXT("")) == 0)
				game.configuration.powerUpsTime = 1;
			else
			temp = _tstoi(data);
			game.configuration.powerUpsTime = temp;


			GetDlgItemText(hDlg, IDC_LIVES, data, 3);
			if (_tccmp(data, TEXT("")) == 0)
				game.configuration.lives = 1;
			else
			temp = _tstoi(data);
			game.configuration.lives = temp;

			EndDialog(hDlg, LOWORD(wParam));

			gameData.hMapFile = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(GameData), SHAREDMEMORYSKELETON);
			if (gameData.hMapFile == NULL)
			{
				_tprintf(TEXT("[SERVIDOR]: [ERRO] Criar objetos mapeamentos (%d)\n"), GetLastError());
				return -1;
			}

			gameData.data = (GameData*)MapViewOfFile(gameData.hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(GameData));
			if (gameData.data == NULL)
			{
				_tprintf(TEXT("[SERVIDOR]: [ERRO] Mapear memória (%d)\n"), GetLastError());
				CloseHandle(gameData.hMapFile);
				return -1;
			}

			playsData.hMapFile = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(Play) * 400, SHAREDMEMORYPLAYS);
			if (playsData.hMapFile == NULL)
			{
				CloseHandle(gameData.hMapFile);
				_tprintf(TEXT("[SERVIDOR]: [ERRO] Criar objetos mapeamentos(%d)\n"), GetLastError());
				return -1;
			}

			playsData.data = (PlaysData*)MapViewOfFile(playsData.hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Play) * 400);
			if (playsData.hMapFile == NULL)
			{
				_tprintf(TEXT("[SERVIDOR]: [ERRO] Mapear memória (%d)\n"), GetLastError());
				CloseHandle(gameData.hMapFile);
				CloseHandle(playsData.hMapFile);
				return -1;
			}

			lPlayers = (Client*)malloc(sizeof(Client));
			lPlayers->PID = -1;

			HANDLE THREAD = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientRequest, NULL, 0, NULL);
			
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hDlg, StartGame);
			
			WaitForSingleObject(THREAD, INFINITE);
			game.startGame = 1;
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK StartGame(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	playersIn = hDlg;
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
#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <conio.h>

#include "../GameObjects.h"
#include "../ComunicationDLL/functionsCommunicationDLL.h"

#define PIPE_NAME TEXT("\\\\.\\pipe\\comunicationpipe")


GameData gameData;
HWND hWnd;
int startGame;
HANDLE hPipe;
TCHAR username[50];
OVERLAPPED ovPedido;
HANDLE IOReadyPedido;

DWORD WINAPI ReceiveCurrentData(LPVOID param);

int _tmain(int argc, LPTSTR argv[])
{
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	HANDLE hPipe;
	DWORD n;
	OVERLAPPED ovPedido;
	HANDLE IOReadyPedido;

	Play data;

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
	IOReadyPedido = CreateEvent(NULL, TRUE, FALSE, NULL);
	do {
		data.action = 1;
		_tcscpy_s(data.user, 50, TEXT("OLA MUNDO"));
		system("pause");
		ZeroMemory(&ovPedido, sizeof(ovPedido));
		ResetEvent(IOReadyPedido);
		ovPedido.hEvent = IOReadyPedido;

		if (!WriteFile(hPipe, &data, sizeof(data), NULL, &ovPedido))
		{
			exit(-1);
		}

		WaitForSingleObject(IOReadyPedido, INFINITE);

		GetOverlappedResult(hPipe, &ovPedido, &n, FALSE);
		system("pause");
	} while (1);

}

//HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveCurrentData, NULL, 0, NULL);


DWORD WINAPI ReceiveCurrentData(LPVOID param)
{
	HANDLE IOReady;
	DWORD n;
	int ret;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	while (startGame == 1)
	{
		ZeroMemory(&ovPedido, sizeof(ovPedido));
		ResetEvent(IOReadyPedido);
		ovPedido.hEvent = IOReadyPedido;

		ReadFile(hPipe, &gameData, sizeof(GameData), NULL, &ovPedido);

		WaitForSingleObject(IOReady, INFINITE);
		ret = GetOverlappedResult(hPipe, &ovPedido, &n, FALSE);
		if (ret && n)
			InvalidateRect(hWnd, NULL, FALSE);
	}
	CloseHandle(IOReady);
	return 0;
}
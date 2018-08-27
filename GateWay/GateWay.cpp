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
#define BUFFERSIZE 200



typedef struct {
	HANDLE hPipe, thread;
	TCHAR nome[30];
}ClientData;

ClientData lClients[BUFFERSIZE];
ControlData gamedata;
ControlPlays plyasdata;
int StartGame;


DWORD WINAPI ClientRequest(LPVOID param);
DWORD WINAPI CurrentData(LPVOID * param);

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hPipe;
	int i;
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	gamedata.hMapFile = CreateFileMapping(NULL, NULL, PAGE_READONLY, 0, sizeof(GameData), SHAREDMEMORYSKELETON);
	if (gamedata.hMapFile == NULL)
	{
		_tprintf(TEXT("[SERVIDOR]: [ERRO] Criar objetos mapeamentos para o mapa (%d)\n"), GetLastError());
		return -1;
	}

	gamedata.data = (GameData*)MapViewOfFile(gamedata.hMapFile, FILE_MAP_READ, 0, 0, sizeof(GameData));
	if (gamedata.data == NULL)
	{
		_tprintf(TEXT("[SERVIDOR]: [ERRO] Mapear memória do mapa (%d)\n"), GetLastError());
		CloseHandle(gamedata.hMapFile);
		return -1;
	}

	plyasdata.hMapFile = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(Play) * MAXPLAYS, SHAREDMEMORYPLAYS);
	if (plyasdata.hMapFile == NULL)
	{
		CloseHandle(plyasdata.hMapFile);
		_tprintf(TEXT("[SERVIDOR]: [ERRO] Criar objetos mapeamentos para as mensagens (%d)\n"), GetLastError());
		return -1;
	}

	plyasdata.data = (PlaysData*)MapViewOfFile(plyasdata.hMapFile, FILE_MAP_WRITE, 0, 0, sizeof(Play) * MAXPLAYS);
	if (plyasdata.data == NULL)
	{
		_tprintf(TEXT("[SERVIDOR]: [ERRO] Mapear memória das mensagens (%d)\n"), GetLastError());
		CloseHandle(gamedata.data);
		CloseHandle(plyasdata.hMapFile);
		return -1;
	}

	StartGame = 1;



	for (i = 0; i < BUFFERSIZE; i++)
		lClients[i].thread = NULL;

	do
	{
		_tprintf(TEXT("[GATEWAY] Criar uma cópia do pipe '%s' (CreateNamedPipe)\n"), PIPE_NAME);
		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 20, sizeof(Play), sizeof(Play), 1000, NULL);

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
			exit(-1);
		}
		_tprintf(TEXT("Esperar ligação de um cliente (ConnectNamedPipe)\n"));

		if (!ConnectNamedPipe(hPipe, NULL))
		{
			_tprintf(TEXT("[ERRO] Ligação ao cliente! (ConnectNamedPipe\n"));
			exit(-1);
		}
		else {
			for (i = 0; i < MAXPLAYS; i++)
				if (lClients[i].thread == NULL)
					break;
			if (i < MAXPLAYS)
			{
				lClients[i].hPipe = hPipe;
				lClients[i].thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientRequest, (LPVOID)hPipe, 0, NULL);
			}
		}

	} while (StartGame == 1);

	HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CurrentData, NULL, 0, NULL);


	WaitForSingleObject(thread, INFINITE);
	CloseHandle(plyasdata.hMapFile);
}


DWORD WINAPI ClientRequest(LPVOID param)
{
	HANDLE hPipe = (HANDLE)param;
	HANDLE IOReady;
	OVERLAPPED ov;
	TCHAR name[50] = TEXT("");
	Play play;
	DWORD n;
	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	

	while (StartGame == 1) {
		ZeroMemory(&ov, sizeof(ov));
		ResetEvent(IOReady);
		ov.hEvent = IOReady;
		ReadFile(hPipe, &play, sizeof(Play), NULL, &ov);
		_tprintf(TEXT("Nome:  %s acao: %d\n"), play.user, play.action);
		WaitForSingleObject(IOReady, INFINITE);
		GetOverlappedResult(hPipe, &ov, &n, FALSE);

		if (_tcscmp(name, TEXT("")) == 0)
			_tcscpy(name, play.user);

		if (_tcscmp(name, play.user) == 0)
		{
			WriteClientRequest(plyasdata.data, &play); 
		}
	}


	CloseHandle(IOReady);
	CloseHandle(hPipe);
	return 0;
}

DWORD WINAPI CurrentData(LPVOID * param)
{
	GameData newGameData;
	DWORD n;
	int i;

	HANDLE IOReady;
	OVERLAPPED ov;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	while (StartGame == 1)
	{
		WaitForSingleObject(SMShareSkeleton, INFINITE);
		ReadGameData(gamedata.data, &newGameData);

		_tprintf(TEXT("Mais uma alteração\n"));

		for (i = 0; i < BUFFERSIZE; i++)
		{
			if (lClients[i].hPipe != INVALID_HANDLE_VALUE)
			{
				ZeroMemory(&ov, sizeof(ov));
				ResetEvent(IOReady);
				ov.hEvent = IOReady;

				WriteFile(lClients[i].hPipe, &newGameData, sizeof(GameData), NULL, &ov);
				WaitForSingleObject(IOReady, INFINITE);
				GetOverlappedResult(lClients[i].hPipe, &ov, &n, FALSE);

			}
		}
	}
	CloseHandle(IOReady);
	return 0;
}



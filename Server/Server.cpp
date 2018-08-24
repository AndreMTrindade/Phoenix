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


ControlData gamedata;
ControlPlays playsdata;
Client *lPlayers;
int IDC = 0;

void WINAPI ClientRequest(void * dados);


int _tmain(int argc, LPTSTR argv[])
{
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	gamedata.hMapFile = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(GameData), SHAREDMEMORYSKELETON);
	if (gamedata.hMapFile == NULL)
	{
		_tprintf(TEXT("[SERVIDOR]: [ERRO] Criar objetos mapeamentos para o mapa (%d)\n"), GetLastError());
		return -1;
	}


	gamedata.data = (GameData*)MapViewOfFile(gamedata.hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(GameData));
	if (gamedata.data == NULL)
	{
		_tprintf(TEXT("[SERVIDOR]: [ERRO] Mapear memória do mapa (%d)\n"), GetLastError());
		CloseHandle(gamedata.hMapFile);
		return -1;
	}

	playsdata.hMapFile = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(Play) * 400, SHAREDMEMORYPLAYS);
	if (playsdata.hMapFile == NULL)
	{
		CloseHandle(playsdata.hMapFile);
		_tprintf(TEXT("[SERVIDOR]: [ERRO] Criar objetos mapeamentos para as mensagens (%d)\n"), GetLastError());
		return -1;
	}

	playsdata.data = (PlaysData*)MapViewOfFile(playsdata.hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Play) * 400);
	if (playsdata.data == NULL)
	{
		_tprintf(TEXT("[SERVIDOR]: [ERRO] Mapear memória das mensagens (%d)\n"), GetLastError());
		CloseHandle(gamedata.hMapFile);
		CloseHandle(playsdata.hMapFile);
		return -1;
	}



	lPlayers = (Client*)malloc(sizeof(Client));
	lPlayers->PID = -1;

	HANDLE THREAD1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientRequest, (void*)&playsdata, 0, NULL);
	WaitForSingleObject(THREAD1, INFINITE);
}


void WINAPI ClientRequest(void * dados)
{
	ControlPlays* shared = (ControlPlays*)dados;
	Play play;
	Client *newc;
	Client *last;

	last = lPlayers;
	int i = 0;
	while (1)
	{
		_tprintf(TEXT("Esperando por clientes:\n"));
		WaitForSingleObject(SEMReviceMensages, INFINITE);
		ReadClientRequest(&play, shared->data);
		if (play.action == 1)
		{
			_tprintf(TEXT("Cliente: %s Acao: %d\n"), play.user, play.action);
			newc = (Client*)malloc(sizeof(Client));
			_tcscpy_s(newc->user, play.user);
			newc->PID = IDC;
			IDC++;
			newc->p = NULL;
			last->p = newc;
			last = newc;
		}
		ReleaseSemaphore(SEMSendMessages, 1, NULL);
	}
}


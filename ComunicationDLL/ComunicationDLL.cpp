
#include "stdafx.h"
#include "functionsCommunicationDLL.h"

#define BUFFERSIZE 400

//Para verificar ao carregar a dll que a aplica��o ir� ocupar mais mem�ria
char ponteiro[40960];

int in = 0, out = 0;

TCHAR SMClientRequest[] = TEXT("MEM_P");
TCHAR SMShareSkeleton[] = TEXT("MEM_P22");
TCHAR EVENTCliente2GateWay[] = TEXT("EV1");

HANDLE MUTEXSKELETON = CreateMutex(NULL, FALSE, TEXT("MUTEXT1"));
HANDLE MUTEXTPLAYSWERITE = CreateMutex(NULL, FALSE, TEXT("MUTEX2"));
HANDLE EVENTGATWAYON = CreateEvent(NULL, FALSE, FALSE, TEXT("EVENT2"));
HANDLE EVEMTSERVERON = CreateEvent(NULL, FALSE, FALSE, TEXT("EVENT3"));

HANDLE EVEMTWRITESKELETON = CreateEvent(NULL, FALSE, FALSE, TEXT("EVENT1"));
HANDLE EVENTCHANGESKELETON = CreateEvent(NULL, FALSE, FALSE, EVENTCliente2GateWay);

HANDLE SEMPLAY = CreateSemaphore(NULL, 0, 400, TEXT("SEMAFORO1"));
HANDLE SEMPLAYSFREE = CreateSemaphore(NULL, 400, 400, TEXT("SEMAFORO2"));


void reset()
{
	in = out = 0;
}

void WriteClientRequest(PlaysData *shared, Play *newClient)
{
	//mutex
	WaitForSingleObject(SEMPLAYSFREE, INFINITE);
	WaitForSingleObject(MUTEXTPLAYSWERITE, INFINITE);
	CopyMemory(shared, newClient, sizeof(Play));
	in = (in++) % BUFFERSIZE;
	ReleaseSemaphore(SEMPLAY, 1, NULL);
	ReleaseMutex(MUTEXTPLAYSWERITE);
	//mutex
}

void ReadClientRequest(Play * newClient, PlaysData *shared)
{
	//fechar mutext
	if (shared == NULL) return;
	WaitForSingleObject(SEMPLAY, INFINITE);
	CopyMemory(newClient, shared, sizeof(Play));
	out = (out++) % BUFFERSIZE;
	ReleaseSemaphore(SEMPLAYSFREE, 1, NULL);
	//abrir mutex
}

void ReadGameData(GameData *shared, GameData *newSkeleton) {
	WaitForSingleObject(EVEMTWRITESKELETON, INFINITE);
	CopyMemory(shared, newSkeleton, sizeof(GameData));
	ReleaseMutex(MUTEXSKELETON);
}

void WriteGameData(GameData *newSkeleton, GameData *shared) {
	CopyMemory(newSkeleton, shared, sizeof(Client));
	out = (out++) % BUFFERSIZE;
}

void WriteSkeleton(GameData *shared, Skeleton *newSkeleton) {
	int i;
	WaitForSingleObject(MUTEXSKELETON, INFINITE);
	for (i = 0; i < shared->nSkeleton; i++)
	{
		if (shared->skeletons->id == newSkeleton->id)
		{
			CopyMemory(&shared->skeletons[i], newSkeleton, sizeof(Skeleton));
			ReleaseMutex(MUTEXSKELETON);
			SetEvent(EVEMTWRITESKELETON);
			return;
		}
	}

	shared->nSkeleton++;
	CopyMemory(&shared->skeletons[shared->nSkeleton], newSkeleton, sizeof(Skeleton));
	ReleaseMutex(MUTEXSKELETON);
	SetEvent(EVEMTWRITESKELETON);
}

void RemoveSkeleton(GameData *shared, Skeleton *newSkeleton) {
	int i;
	WaitForSingleObject(MUTEXSKELETON, INFINITE);
	for (i = 0; i < shared->nSkeleton; i++)
	{
		if (shared->skeletons->id == newSkeleton->id)
		{
			shared->nSkeleton--;
			for (i += 1; i < shared->nSkeleton; i++)
				shared->skeletons[i - 1] = shared->skeletons[i];
			break;
		}
	}

	ReleaseMutex(MUTEXSKELETON);
	SetEvent(EVEMTWRITESKELETON);

}

/*
Retangulo * isOcupado(DadosMapa * mem_mapa, Retangulo retangulo)
{
DadosMapa mapa;
Retangulo * outro;
int i, x, y, lim_i_x_outro, lim_s_x_outro, lim_i_y_outro, lim_s_y_outro, a, lim_i_x_ret, lim_s_x_ret, lim_i_y_ret, lim_s_y_ret;

LePosicoes(mem_mapa, &mapa);

for (i = 0; i < mapa.num_elementos; i++)
{
outro = &mapa.elementos[i];
if (outro->canto_se.id != retangulo.canto_se.id)
{
lim_i_x_outro = outro->canto_se.x;
lim_s_x_outro = outro->canto_se.x + outro->comprimento;
lim_i_y_outro = outro->canto_se.y;
lim_s_y_outro = outro->canto_se.y + outro->altura;

lim_i_x_ret = retangulo.canto_se.x;
lim_s_x_ret = retangulo.canto_se.x + retangulo.comprimento;
lim_i_y_ret = retangulo.canto_se.y;
lim_s_y_ret = retangulo.canto_se.y + retangulo.altura;

if (lim_i_x_outro >= lim_i_x_ret && lim_s_x_outro <= lim_s_x_ret && lim_i_y_outro >= lim_i_y_ret && lim_s_y_outro <= lim_s_y_ret)
{
ReleaseMutex(MUTEXSKELETON);
return outro;
}

if (lim_i_x_ret >= lim_i_x_outro && lim_s_x_ret <= lim_s_x_outro && lim_i_y_ret >= lim_i_y_outro && lim_s_y_ret <= lim_s_y_outro)
{
ReleaseMutex(MUTEXSKELETON);
return outro;
}

x = retangulo.canto_se.x;
y = retangulo.canto_se.y;
if (x >= lim_i_x_outro && x <= lim_s_x_outro && y >= lim_i_y_outro && y <= lim_s_y_outro)
{
ReleaseMutex(MUTEXSKELETON);
return outro;
}

x = retangulo.canto_se.x + retangulo.comprimento;
y = retangulo.canto_se.y + retangulo.altura;
if (x >= lim_i_x_outro && x <= lim_s_x_outro && y >= lim_i_y_outro && y <= lim_s_y_outro)
{
ReleaseMutex(MUTEXSKELETON);
return outro;
}

x = retangulo.canto_se.x;
y = retangulo.canto_se.y + retangulo.altura;
if (x >= lim_i_x_outro && x <= lim_s_x_outro && y >= lim_i_y_outro && y <= lim_s_y_outro)
{
ReleaseMutex(MUTEXSKELETON);
return outro;
}

x = retangulo.canto_se.x + retangulo.comprimento;
y = retangulo.canto_se.y;
if (x >= lim_i_x_outro && x <= lim_s_x_outro && y >= lim_i_y_outro && y <= lim_s_y_outro)
{
ReleaseMutex(MUTEXSKELETON);
return outro;
}
}
}
ReleaseMutex(MUTEXSKELETON);
return NULL;
}


*/


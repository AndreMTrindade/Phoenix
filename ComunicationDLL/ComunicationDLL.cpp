
#include "stdafx.h"
#include "functionsCommunicationDLL.h"

#define BUFFERSIZE 400

//Para verificar ao carregar a dll que a aplicação irá ocupar mais memória
char ponteiro[40960];

int in = 0, out = 0;

TCHAR SMClientRequest[] = TEXT("MEM_P");
TCHAR SMShareSkeleton[] = TEXT("MEM_P22");
TCHAR EVENTCliente2GateWay[] = TEXT("EV1");

HANDLE SEMSendInitialConf = CreateSemaphore(NULL, 1, BUFFERSIZE, TEXT("SEMSendInitialConf"));
HANDLE SEMReciveInitialData = CreateSemaphore(NULL, 0, BUFFERSIZE, TEXT("SEMReciveInitialData"));

HANDLE SEMReviceMensages = CreateSemaphore(NULL, 0, BUFFERSIZE, TEXT("SEM1"));
HANDLE SEMSendMessages = CreateSemaphore(NULL, BUFFERSIZE, BUFFERSIZE, TEXT("SEM2"));

HANDLE SEMMessages = CreateMutex(NULL, FALSE, TEXT("M2"));
HANDLE MUTEXTSKELETON = CreateMutex(NULL, FALSE, TEXT("M1"));

HANDLE EVENTCHANGESKELETON = CreateEvent(NULL, FALSE, FALSE, EVENTCliente2GateWay);

void reset()
{
	in = out = 0;
}

void WriteClientRequest(PlaysData *shared, Play *newClient)
{
	//mutex
	WaitForSingleObject(SEMMessages, INFINITE);
	CopyMemory(shared, newClient, sizeof(Client));
	in = (in++) % BUFFERSIZE;
	ReleaseMutex(SEMMessages);

	//mutex
}

void ReadClientRequest(Play * newClient, PlaysData *shared)
{
	//fechar mutext
	CopyMemory(newClient, shared, sizeof(Client));
	out = (out++) % BUFFERSIZE;

	//abrir mutex
}

void ReadGameData(GameData *shared, GameData *newSkeleton) {
	CopyMemory(shared, newSkeleton, sizeof(Client));
	in = (in++) % BUFFERSIZE;
}

void WriteGameData(GameData *newSkeleton, GameData *shared) {
	CopyMemory(newSkeleton, shared, sizeof(Client));
	out = (out++) % BUFFERSIZE;
}

void WriteSkeleton(GameData *shared, Skeleton *newSkeleton) {
	int i;
	WaitForSingleObject(MUTEXTSKELETON, INFINITE);
	for (i = 0; i < shared->nSkeleton; i++)
	{
		if (shared->skeletons->id == newSkeleton->id)
		{
			CopyMemory(&shared->skeletons[i], newSkeleton, sizeof(Skeleton));
			ReleaseMutex(MUTEXTSKELETON);
			SetEvent(EVENTCHANGESKELETON);
			return;
		}
	}

	shared->nSkeleton++;
	CopyMemory(&shared->skeletons[shared->nSkeleton], newSkeleton, sizeof(Skeleton));

	ReleaseMutex(MUTEXTSKELETON);
	SetEvent(EVENTCHANGESKELETON);
}

void RemoveSkeleton(GameData *shared, Skeleton *newSkeleton) {
	int i;
	WaitForSingleObject(MUTEXTSKELETON, INFINITE);
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

	ReleaseMutex(MUTEXTSKELETON);
	SetEvent(EVENTCHANGESKELETON);

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
ReleaseMutex(h_mutex_posicoes);
return outro;
}

if (lim_i_x_ret >= lim_i_x_outro && lim_s_x_ret <= lim_s_x_outro && lim_i_y_ret >= lim_i_y_outro && lim_s_y_ret <= lim_s_y_outro)
{
ReleaseMutex(h_mutex_posicoes);
return outro;
}

x = retangulo.canto_se.x;
y = retangulo.canto_se.y;
if (x >= lim_i_x_outro && x <= lim_s_x_outro && y >= lim_i_y_outro && y <= lim_s_y_outro)
{
ReleaseMutex(h_mutex_posicoes);
return outro;
}

x = retangulo.canto_se.x + retangulo.comprimento;
y = retangulo.canto_se.y + retangulo.altura;
if (x >= lim_i_x_outro && x <= lim_s_x_outro && y >= lim_i_y_outro && y <= lim_s_y_outro)
{
ReleaseMutex(h_mutex_posicoes);
return outro;
}

x = retangulo.canto_se.x;
y = retangulo.canto_se.y + retangulo.altura;
if (x >= lim_i_x_outro && x <= lim_s_x_outro && y >= lim_i_y_outro && y <= lim_s_y_outro)
{
ReleaseMutex(h_mutex_posicoes);
return outro;
}

x = retangulo.canto_se.x + retangulo.comprimento;
y = retangulo.canto_se.y;
if (x >= lim_i_x_outro && x <= lim_s_x_outro && y >= lim_i_y_outro && y <= lim_s_y_outro)
{
ReleaseMutex(h_mutex_posicoes);
return outro;
}
}
}
ReleaseMutex(h_mutex_posicoes);
return NULL;
}


*/


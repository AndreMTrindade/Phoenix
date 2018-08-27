#pragma once
#include "../GameObjects.h"

#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif


extern "C"
{
	//Variável global da DLL
	extern DLL_IMP_API int in, out;
	DLL_IMP_API TCHAR EVENTCliente2GateWay[];

	DLL_IMP_API TCHAR SMClientRequest[];
	DLL_IMP_API TCHAR SMShareSkeleton[];

	DLL_IMP_API HANDLE MUTEXSKELETON;
	DLL_IMP_API HANDLE MUTEXTPLAYSWERITE;
	DLL_IMP_API HANDLE EVEMTWRITESKELETON;
	DLL_IMP_API HANDLE EVENTCHANGESKELETON;
	DLL_IMP_API HANDLE SEMPLAY;
	DLL_IMP_API HANDLE SEMPLAYSFREE;


	//Funções a serem exportadas/importadas
	DLL_IMP_API void ReadClientRequest(Play *newClient, PlaysData *shared);
	DLL_IMP_API void WriteClientRequest(PlaysData *shared, Play *newClient);
	DLL_IMP_API void ReadGameData(GameData *shared, GameData *newSkeleton);
	DLL_IMP_API void WriteGameData(GameData *newSkeleton, GameData *shared);
	DLL_IMP_API void WriteSkeleton(GameData *shared, Skeleton *newSkeleton);
	DLL_IMP_API void RemoveSkeleton(GameData *shared, Skeleton *newSkeleton);
	DLL_IMP_API void reset();

}

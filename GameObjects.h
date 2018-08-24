#ifndef GameObjects   
#define GameObjects

#include "Skeleton.h"

#define BASIC 19785
#define DODGE -34109
#define BOSS 999

#define SHIELD 80001
#define ICE 80002
#define BATTERY 80003
#define PLUS 80004
#define LIVE 80005
#define ALCOHOL 80006
#define LITTLE 80007
#define MAXPLAYS 400

TCHAR SHAREDMEMORYSKELETON[] = TEXT("MEMORYSHARED1");
TCHAR SHAREDMEMORYPLAYS[] = TEXT("MEMORYSHARED22");

typedef struct Defender {
	Skeleton body;
	int score;
	int lives;

	struct Defender * proxDefender;
}Defender;

/*
type:
	1 - Basico
	2 - Esquiva
	3 - x
*/
typedef struct Invader {
	Skeleton body;
	int resistance;
	double velocity;
	double shotFrequency;
	int type;

	struct Invader *proxInvader;
}Invader;

typedef struct PowerUp {
	Skeleton body;
	double time;
	double frequency;
	int type;

	struct PowerUp * proxPowerUp;
}PowerUp;

typedef struct Shot {
	Skeleton body;
	double velocity;
	Defender *def;

	struct Shot *proxShot;
}Shot;

typedef struct Bomb {
	Skeleton body;
	double velocity;

	struct Bomb *proxBomb;
}Bomb;

typedef struct Client
{
	int PID;
	TCHAR user[50];
	int option;
	struct Client *p;
} Client;


typedef struct _Game{
	Invader *inv;
	Defender *def;
	PowerUp *pu;
	Shot *shot;
	Client *cli;
	int MaxPlayers;
	struct _Game *p;
}Game;

typedef struct GameData {
	int nSkeleton;
	Skeleton skeletons[200];
}GameData;


typedef struct {
	HANDLE hMapFile;
	GameData *data;
	int thread;
}ControlData;

typedef struct Play {
	TCHAR user[50];
	int action;
}Play;

typedef struct PlaysData {
	int nPlays;
	Play plays[200];
}PlaysData;


typedef struct {
	HANDLE hMapFile;
	PlaysData *data;
	int thread;
}ControlPlays;







Defender* CreateDefender(int width, int height, int lives);

Invader* CreateInvader(int x, int y, int type, int width, int height);

PowerUp* CreatePowerUp(int x, int y, int type);

Shot* CreateShot(Defender* d, int velocity);

Bomb* CreatBomb();

void MoveInvader(Invader *inv, int type);

bool CheckPossibleMove(Skeleton s, Game g);

Game* CreateGame(Invader *inv, Defender *def, PowerUp *pu, Shot *shot, Client *cli);

#endif
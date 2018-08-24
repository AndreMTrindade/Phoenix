#include "gameObjects.h"

Defender* CreateDefender(int width, int height, int lives)
{
	Defender *temp;

	temp = (Defender*)malloc(sizeof(Defender));
	temp->body = CreateSkeleton(5, 30, width, height);
	temp->lives = lives;
	temp->score = 0;
	return temp;
}

Invader* CreateInvader(int x, int y, int type, int width, int height)
{
	Invader *temp;
	switch (type)
	{
	case BASIC:
		temp = (Invader*)malloc(sizeof(Invader));
		temp->body = CreateSkeleton(x, y, width, height);
		temp->resistance = 1;
		temp->shotFrequency = /*TODO*/0.1;
		temp->type = BASIC;
		temp->velocity = 1;
		return temp;
	case DODGE:
		temp = (Invader*)malloc(sizeof(Invader));
		temp->body = CreateSkeleton(x, y, width, height);
		temp->resistance = 1;
		temp->shotFrequency = /*TODO*/0.1;
		temp->type = DODGE;
		temp->velocity = 1.1;
		return temp;
	case BOSS:
		temp = (Invader*)malloc(sizeof(Invader));
		temp->body = CreateSkeleton(x, y, width, height);
		temp->resistance = 50;
		temp->shotFrequency = /*TODO*/0.1;
		temp->type = DODGE;
		temp->velocity = 1.1;
		return temp;
	}
}

PowerUp* CreatePowerUp(int x, int y, int type)
{
	PowerUp * temp;
	temp = (PowerUp*)malloc(sizeof(PowerUp));
	temp->body = CreateSkeleton(x, y, 2, 2);
	temp->type = type;

	switch (type)
	{
	case SHIELD:
		temp->frequency = 0.7;
		temp->time = 8;
		break;
	case ICE:
		temp->frequency = 0.3;
		temp->time = 8;
		break;
	case BATTERY:
		temp->frequency = 0.3;
		temp->time = 8;
		break;
	case PLUS:
		temp->frequency = 0.7;
		temp->time = 8;
		break;
	case LIVE:
		temp->frequency = 0.1;
		temp->time = -1;
		break;
	case ALCOHOL:
		temp->frequency = 0.3;
		temp->time = 8;
		break;
	case LITTLE:
		temp->frequency = 0.3;
		temp->time = 8;
		break;
	default:
		return NULL;
	}
	return temp;
}

Shot* CreateShot(Defender* d, int velocity)
{
	Shot *temp = (Shot*)malloc(sizeof(Shot));
	temp->body = CreateSkeleton(d->body.point.x, d->body.point.y, 1, 1);
	temp->def = d;
	temp->velocity = velocity;

	return temp;
}

Bomb* CreatBomb();

void MoveInvader(Invader *inv, int type)
{
	switch (type)
	{
	case BASIC:

		break;


	default:
		break;
	}
}

bool CheckPossibleMove(Skeleton s, Game g)
{
	Invader *inv = g.inv;
	while (inv != NULL)
	{
		if (Collision(s, inv->body))
		{
			return true;
		}
		inv = inv->proxInvader;
	}
	return true;

}

Game* CreateGame(Invader *inv, Defender *def, PowerUp *pu, Shot *shot, Client *cli, int maxPlayers)
{
	Game *g = (Game*)malloc(sizeof(Game));
	g->inv = inv;
	g->def = def;
	g->pu = pu;
	g->shot = shot;
	g->cli = cli;
	g->MaxPlayers = maxPlayers;
	return g;
}
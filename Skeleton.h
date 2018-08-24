#ifndef skeleton   
#define skeleton

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <conio.h>

#define UP 1
#define DOWN -1
#define RIGHT 2
#define LEFT 3


typedef struct Point {
	int x, y;
} Point;

typedef struct Skeleton {
	int id;
	Point point;
	int width;
	int heigh;
} Skeleton;

/*
Create a Skeleton
Argyuments:  x, y, width, height
Return: Skeleton with new Position
*/
Skeleton CreateSkeleton(int x, int y, int width, int height);

/*
Print the Skeleton on screen
Arguments: skeleton
Return:
*/
void PrintSkeleton(Skeleton p);

/*
Move Skeleton
Arguments: *skeleton, (UP, DOWN; RIGHT, LEFT)
Return:
*/
void MoveSkeleton(Skeleton *skeleton, int op);

/*
Check Collision
Arguments: Skeleton1, Skeleton2
return: 1 - True  0 - False
*/
int Collision(Skeleton r1, Skeleton r2);

int Collision(Point p, Skeleton r);
Point getP1(Skeleton r);
Point getP2(Skeleton r);
Point getP3(Skeleton r);
Point getP4(Skeleton r);

void gotoxy(int x, int y);

#endif
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

Skeleton CreateSkeleton(int x, int y, int width, int height) {
	Skeleton temp;
	temp.point.x = x;
	temp.point.y = y;
	temp.heigh = height;
	temp.width = width;

	return temp;
}


/*
Move Skeleton
Arguments: *skeleton, (UP, DOWN; RIGHT, LEFT)
Return:
*/

void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/*
Check Collision
Arguments: Skeleton1, Skeleton2
return: 1 - True  0 - False
*/

Point getP1(Skeleton r) {
	return r.point;
}
Point getP2(Skeleton r) {
	Point ret;
	ret.x = r.point.x + r.heigh;
	ret.y = r.point.y;
	return ret;
}
Point getP3(Skeleton r) {
	Point ret;
	ret.x = r.point.x;
	ret.y = r.point.y + r.width;
	return ret;
}
Point getP4(Skeleton r) {
	Point ret;
	ret.x = r.point.x + r.width;
	ret.y = r.point.y + r.heigh;
	return ret;
}


int Collision(Point p, Skeleton r) {
	Point p1 = getP1(r), p4 = getP4(r);
	if (p.x >= p1.x && p.x <= p4.x) {
		if (p.y >= p1.y && p.y <= p4.y) {
			return 1;
		}
	}
	return 0;
}

int Collision(Skeleton r1, Skeleton r2) {
	Point p1 = getP1(r1), p2 = getP2(r1), p3 = getP3(r1), p4 = getP4(r1);
	if (Collision(p1, r2))
		return 1;
	else if (Collision(p2, r2))
		return 1;
	else if (Collision(p3, r2))
		return 1;
	else if (Collision(p4, r2))
		return 0;

	return -1;
}
 


#endif
#pragma once
#include <stdlib.h>

enum areaCell
{
	AREA_BOMB = 1,
	AREA_OUT = -1
};

class area
{

private:
	
	int x_;
	int y_;
	int z_;

	int ***map;

	double blocks;

public:
	area(void);
	area(int x, int y, int z);

	void generate(int x, int y, int z, int nareas);
	
	int operator()(int x,int y,int z);
	double howmany(){return blocks;}

	~area(void);
};


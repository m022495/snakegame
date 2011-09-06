#include "area.h"


area::area(int x, int y, int z):
	map(0),
	x_(x),
	y_(y),
	z_(z),
	blocks(0)
{
	map = new int**[x];
	for(int i=0;i<x;i++)
	{
		map[i] = new int*[y];
		for(int j=0;j<y;j++)
		{
			map[i][j] = new int[z];
			for(int k=0;k<z;k++)
			{
				map[i][j][k] = 0;
			}
		}
	}
}

void area::generate(int x, int y, int z, int nareas)
{
	if(x>=0 && x<x_ && y>=0 && y<y_ && z>=0 && z<z_ && map[x][y][z]==0 && nareas>0)
	{
		map[x][y][z] = 1;
		blocks++;
		for(int i=0;i<nareas;i++)
		{
			int dx = 1-rand()%2;
			int dy = 1-rand()%2;
			int dz = 1-rand()%2;
			int n = rand()%3;
			generate(x+dx,y+dy,z+dz,nareas-n);
		}
	}

	/*for(int i=0;i<x_;i++)
	{
		for(int j=0;j<y_;j++)
		{
			for(int k=0;k<z_;k++)
			{
				map[i][j][k] = (int)rand()%100;
			}
		}
	}*/
}

int area::operator()(int x,int y,int z)
{
	if(x>=0 && x<x_ && y>=0 && y<y_ && z>=0 && z<z_)
		return map[x][y][z];
	else
		return AREA_OUT;
}

area::~area(void)
{
}

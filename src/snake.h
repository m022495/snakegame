#pragma once
#include "datatype.h"
#include "mmatrix.h"
#include "matrix_3dvector.h"
#include <iostream>


struct snakeCell
{
	struct guts {

		Vector3i nextmove;
		Vector3i globalpos;
		float fat;

		guts():
			fat(1.0f),
			nextmove(0,0,0),
			globalpos(0,0,0){}
	};

	snakeCell *next;
	snakeCell *prev;
	guts cellGuts;

	void setFat(float fat){cellGuts.fat = fat;}
	snakeCell():next(0),prev(0){}
};

class snake
{
private:

	snakeCell *head;
	snakeCell *last;

	int snakeLength;

public:

	snake(const Vector3i &invector, const Vector3i &playerPos);

	~snake(void);

	void add();
	void move(const Vector3i &direction);
	//void move(int x, int y, int z);
	void setHeadFat(float fat);
	void setLastFat(float fat);

	const snakeCell *getHead() const {return head;}
	const snakeCell *getLast() const {return last;}

	const snakeCell *getNext(const snakeCell *cell){return cell->next;};

	int getLength() {return snakeLength;}
	bool searchPos(const Vector3i &invector);
	bool isLast(const snakeCell* cell){return cell==last;};

	void outputSnake();
};


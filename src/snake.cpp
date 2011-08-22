#include "snake.h"


snake::snake(const Vector3i &invector, const Vector3i &playerPos):
	snakeLength(1),
		head(0),
		last(0)
{
	head = new snakeCell;
	head->cellGuts.globalpos = playerPos;
	last = head;
}

void snake::add()
{
	last -> next = new snakeCell;
	last->next->prev = last;
	last = last->next;
	last->cellGuts = last->prev->cellGuts;
	last->next = 0;

	snakeLength++;
}

void snake::move(const Vector3i &direction)
{
	snakeCell *cell = last;

	while(cell != head) 
	{
		cell -> cellGuts = cell->prev->cellGuts;
		cell = cell->prev;
	}

	head->cellGuts.nextmove = direction;
	head->cellGuts.globalpos += direction;

}

bool snake::searchPos(const Vector3i &invector)
{

	const snakeCell *cell = head->next;
	bool isfound = false;

	while(cell)
	{
		if(invector == cell->cellGuts.globalpos)
		{
			isfound = true;
			break;
		}
		cell = cell->next;
	}

	return isfound;
}

//'interface' for a 'fat' propertie
void snake::setHeadFat(float fat)
{
	head->setFat(fat);
}
void snake::setLastFat(float fat)
{
	last->setFat(fat);
}

void snake::outputSnake() 
{
	snakeCell *cell = head;
	int i = 0;

	do
	{
		std::cout<<i<<": "<<cell->cellGuts.globalpos.x()<<" "<<cell->cellGuts.globalpos.y()<<" "<<cell->cellGuts.globalpos.z()<<std::endl;	
		cell = cell->next;
		
		i++;
	} while(cell != 0);

	std::cout<<std::endl;
}

snake::~snake(void)
{
	snakeCell *cell = head;
	snakeCell *temp = 0;

	do
	{	
		temp = cell->next;
		delete cell;
		cell = temp;

	} while(cell);

}

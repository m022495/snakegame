/*
 * gameController.cpp
 *
 *  Created on: 16.07.2011
 *      Author: никита
 */
#include <iostream>
#include "gameController.h"

#define fixedFPS 60
#define FRAMES_PER_STEP 8

#define X_ 0
#define Y_ 1
#define Z_ 2

void loadBand(int n);
void waiter(long wticks);

namespace GController
{

	gameController *gameController::selfptr = 0;

	gameController::gameController(HINSTANCE hInst):
				hInstance(hInst),
				window(0),
				opengl(0),
				frames(0),
				arrowPressed(false),
				gamePaused(false),
				fixCamera(true),
				animooted(true),
				start_tick(0),
				end_tick(0),
				frame_time(0),
				fpsUpdateType(FIXED_FPS)
	{

		OutputDebugString("aaa");
		selfptr = this;

		direction[X_] = 1;	//x vector {1,0,0}
		direction[Y_] = 0;
		direction[Z_] = 0;

		//initial position
		playerPos[X_] = 0;
		playerPos[Y_] = 0;
		playerPos[Z_] = 0;

		fruitPos[X_] = -1;
		fruitPos[Y_] = -1;
		fruitPos[Z_] = -1;

		srand (time(NULL));
	}

	gameController::~gameController()
	{
		delete window;
		delete opengl;
	}

	bool gameController::init()
	{
		window = new winapiwrapper(hInstance);
		playerSnake = new snake(1,0,0);
		curLevel = new area(50,50,50);
		spawnFruit(10,0,10);
		playerSnake->add();
		playerSnake->add();

		try
		{
			window->createWindow(300,300,500,500);
			window->WshowWindow();
			window->setCallBack(&gameController::callBack);

			opengl = new openglwrapper(window->getHDC());
			opengl->EnableOpenGL(window->getHWND());
		}
		catch ( std::exception &e )
		{
			std::cerr << "Caught " << e.what( ) << std::endl;
			return false;
		};

		return true;

	}

	void gameController::go()
	{
		MSG Msg;
		BOOL bRet;

		while (1)
		{
			if((bRet = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) != 0)
			{
				if (Msg.message == WM_QUIT)
					break;
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
			else
			{
				//main game loop
				mainloop();
			}
		}

	}

	bool gameController::callBack(int type, long param0, long param1)
	{
		if(!selfptr)
			return false;

		selfptr->ProcessEvent(type, param0, param1);
		return true;
	}

	void gameController::ProcessEvent(int type, long param0, long param1)
	{

		if (type == WM_KEYDOWN)
		{
			switch (param0)
			{

				case(VK_UP):

					if(!arrowPressed)
					{
						direction[X_] = 1;
						direction[Y_] = 0;
						direction[Z_] = 0;

						arrowPressed = true;
					}

				break;

				case(VK_DOWN):

					if(!arrowPressed)
					{
						direction[X_] = -1;
						direction[Y_] = 0;
						direction[Z_] = 0;

						arrowPressed = true;
					}

				break;

				case(VK_LEFT):
					if(!arrowPressed)
					{
						direction[X_] = 0;
						direction[Y_] = 0;
						direction[Z_] = -1;

						arrowPressed = true;
					}
				break;

				case(VK_RIGHT):
					if(!arrowPressed)
					{
						direction[X_] = 0;
						direction[Y_] = 0;
						direction[Z_] = 1;

						arrowPressed = true;
					}

				break;

				case(VK_HOME):
					direction[X_] = 0;
					direction[Y_] = 1;
					direction[Z_] = 0;
				break;

				case(VK_END):
					direction[X_] = 0;
					direction[Y_] = -1;
					direction[Z_] = 0;
				break;

				case(VK_SPACE):
					animooted = !animooted;
				break;

				case(0x41):
					playerSnake->add();
				break;

				case(0x43):
					if(!fixCamera) 
					{
						opengl->initCamera();
						fixCamera = true;
					}
					else
						fixCamera = false;
				break;

				case(VK_NUMPAD8):
					opengl->setYSpeed(1.0f);
				break;

				case(VK_NUMPAD2):
					opengl->setYSpeed(-1.0f);
				break;

				case(VK_NUMPAD4):
					opengl->setZSpeed(1.0f);
				break;

				case(VK_NUMPAD6):
					opengl->setZSpeed(-1.0f);
				break;

				case(VK_NUMPAD7):
					opengl->setXSpeed(1.0f);
				break;

				case(VK_NUMPAD9):
					opengl->setXSpeed(-1.0f);
				break;

				case(VK_NUMPAD5):
					opengl->setXSpeed(0.0f);
					opengl->setYSpeed(0.0f);
					opengl->setZSpeed(0.0f);
				break;

				case(VK_ESCAPE):
					opengl->DisableOpenGL();
					window->quit();
				break;
			}
		}
		if (type == WM_SIZE)
		{
			opengl->reshape(LOWORD(param1), HIWORD(param1));
		}
		if(type == WM_LBUTTONDOWN)
		{
				opengl->switcher1();
		}
		if(type == WM_RBUTTONDOWN)
		{
				opengl->switcher2();
		}

		if(type == WM_MOUSEWHEEL)
		{
				opengl->upZoom(GET_WHEEL_DELTA_WPARAM(param0));
		}

	}

	void gameController::mainloop()
	{
		//FPS counter
		loopTimer.start();

		opengl->begindraw();
		//opengl->drawAxis();
		//opengl->drawCube(0,0,0);

		//logic moving the snake

		if (frames == FRAMES_PER_STEP)	//if new step started
		{
			//do move

			playerPos[X_] += direction[X_];	//move the snake head
			playerPos[Y_] += direction[Y_];
			playerPos[Z_] += direction[Z_];

			playerSnake->move(direction);	//move the tail

			frames = 0;	
			arrowPressed = false;
		}
			
		if(playerPos[X_] == fruitPos[X_] &&		//if the fruit hit
			playerPos[Y_] == fruitPos[Y_] && 
			playerPos[Z_] == fruitPos[Z_])
		{
			playerSnake->add();	//add body cell
			spawnFruit(10,0,10);
		}

		//drawing the snake hear

		if(!fixCamera)	//follow the snake head by the camera or set camera to the def points
			opengl->setCameraCellDelta(
									playerPos[X_], playerPos[Y_], playerPos[Z_], 
									direction[X_] * (float)frames/FRAMES_PER_STEP,
									direction[Y_] * (float)frames/FRAMES_PER_STEP,
									direction[Z_] * (float)frames/FRAMES_PER_STEP,
									direction[X_], direction[Y_], direction[Z_]
									);

		opengl->drawCubeCellsDelta(			//snake head
									playerPos[X_], 
									playerPos[Y_], 
									playerPos[Z_],
									direction[X_] * (float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),//animate if enabled
									direction[Y_] * (float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),
									direction[Z_] * (float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0)
									);	

		//draw snake body
		txyz prev = { playerPos[X_],  playerPos[Y_],  playerPos[Z_]};
		const snakeCell *next = playerSnake->getHead()->next;

		//go thru the linked list
		while(next)
		{
				
			prev[X_] -= next->move.x;
			prev[Y_] -= next->move.y;
			prev[Z_] -= next->move.z;

			//draw each body part
			opengl->drawCubeCellsDelta(
					prev[X_],
					prev[Y_],
					prev[Z_],
					next->move.x*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),	//animate if enabled
					next->move.y*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),
					next->move.z*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0)
				);

			next = playerSnake->getNext(next);
		}
			
		//draw the fruit
		if(fruitPos[X_] != -1)
		{
			opengl->drawCubeCells(fruitPos[X_], fruitPos[Y_], fruitPos[Z_]);	
		}

		frames++;
		//loadBand(2000);
		opengl->enddraw();

		//wait until fixed frame time is reached
		float fps = (float)loopTimer.getTicksRate().QuadPart/loopTimer.getTime().QuadPart;
	
		if(fps > fixedFPS)
		{
			waiter(long(loopTimer.getTicksRate().QuadPart/fixedFPS - loopTimer.getTime().QuadPart));
		}

		//fps = (float)loopTimer.getTicksRate().QuadPart/loopTimer.getTime().QuadPart;
		//std::cout<<fps<<std::endl;

		loopTimer.reset();
	}

	void gameController::spawnFruit(int xr, int yr, int zr)
	{
		fruitPos[X_] = (xr!=0)?(int)rand()%xr:0;
		fruitPos[Y_] = (yr!=0)?(int)rand()%yr:0;
		fruitPos[Z_] = (zr!=0)?(int)rand()%zr:0;
	}

}

//just waste cpu time
void loadBand(int n)
{
	for(int i=50;i<50+n;i++)
	{
			for(int j=30;j<30+n;j++)
			{
				int x = rand();
			}
	}
}

//wait wticks ticks
void waiter(long wticks)
{
	if(wticks <= 0)
		return;

	Timer wtimer;
	wtimer.start();

	do{} while( wtimer.getTime().QuadPart < wticks );

}
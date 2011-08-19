/*
 * gameController.cpp
 *
 *  Created on: 16.07.2011
 *      Author: никита
 */
#include <iostream>
#include "gameController.h"

#define fixedFPS 60
#define FRAMES_PER_STEP 12
#define CAMERA_STEPS 7
#define PI 3.1415926536f

#define CAMDEFX 10
#define CAMDEFY 10
#define CAMDEFZ 10



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
				direction(1,0,0),
				prevdirection(direction),
				playerPos(0,0,0),
				fruitPos(-1,-1,-1),
				cameraPos(CAMDEFX,CAMDEFY,CAMDEFZ),
				cameraframes(0),
				cameraRot(0,0,0),
				xRelRot(0,0,0)
	{


		selfptr = this;
	//	srand (time((time_t)NULL));
	}

	gameController::~gameController()
	{
		delete window;
		delete opengl;
	}

	bool gameController::init()
	{
		window = new winapiwrapper(hInstance);
		playerSnake = new snake(direction, playerPos);

		spawnFruit(10,10,0);
		playerSnake->add();
		playerSnake->add();

		try
		{
			window->createWindow(700,200,700,700);
			window->WshowWindow();
			window->setCallBack(&gameController::callBack);

			opengl = new openglwrapper(window->getHDC());
			opengl->EnableOpenGL(window->getHWND());
			opengl->initCamera(cameraPos.x(), cameraPos.y(),cameraPos.z());

		}
		catch ( std::exception &e )
		{
			std::cerr << "Caught " << e.what( ) << std::endl;
			return false;
		};

		return true;

	}

	void gameController::restartGame()
	{
		delete playerSnake;
		playerSnake = 0;

		direction.set(1,0,0);
		playerPos.set(0,0,0);

		playerSnake = new snake(direction, playerPos);
		playerSnake->add();
		playerSnake->add();

		spawnFruit(10,10,0);

		animooted = true;
		gamePaused = false;
		fixCamera = false;
		
		xRelRot.setAngle(0,0,0);
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
				loopTimer.start();

				mainloop();

				float fps = (float)loopTimer.getTicksRate().QuadPart/loopTimer.getTime().QuadPart;
				if	(fps > fixedFPS)
					waiter(long(loopTimer.getTicksRate().QuadPart/fixedFPS - loopTimer.getTime().QuadPart));

				loopTimer.reset();
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
			Vector3i xvect(1,0,0);

			switch (param0)
			{
				case(VK_UP):
					if(!arrowPressed)
					{
						
						RotMatrix3f rot_up(0, PI/2, 0);
						xRelRot = rot_up*xRelRot;	
						xvect *= xRelRot;

						direction = xvect;
						
						cameraframes = 1;
						arrowPressed = true;
					}
				break;

				case(VK_DOWN):
					if(!arrowPressed)
					{
						RotMatrix3f rot_down(0, -PI/2, 0);
						xRelRot = rot_down*xRelRot;	
						xvect *= xRelRot;

						direction = xvect;
						
						cameraframes = 1;
						arrowPressed = true;
					}
				break;

				case(VK_LEFT):
					if(!arrowPressed)
					{
						
						RotMatrix3f rot_left(0, 0, -PI/2);
						xRelRot = rot_left*xRelRot;	
						xvect *= xRelRot;
						
						direction = xvect;

						cameraframes = 1;
						arrowPressed = true;
					}
				break;

				case(VK_RIGHT):
					if(!arrowPressed)
					{		

						RotMatrix3f rot_right(0, 0, PI/2);
						xRelRot = rot_right*xRelRot;	
						xvect *= xRelRot;

						direction = xvect;

						cameraframes = 1;
						arrowPressed = true;
					}

				break;

				case(VK_HOME):
					direction.set(0,1,0);
				break;

				case(VK_END):
					direction.set(0,-1,0);
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
						cameraPos.set(CAMDEFX, CAMDEFY, CAMDEFZ);
						opengl->initCamera(cameraPos.z(), cameraPos.y(), cameraPos.z());
						fixCamera = true;
					}
					else
						fixCamera = false;
				break;

				case(VK_ESCAPE):
					opengl->DisableOpenGL();
					window->quit();
				break;

				case(VK_F1):
					restartGame();
				break;

			}
			direction.output();
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

		if (frames == FRAMES_PER_STEP)
		{ 
			//current head position = previos + current moving direction
			if(!gamePaused)
			{
				playerPos += direction;	
				playerSnake->move(direction);
				checkCollision();
			}

			frames = 0;	
			arrowPressed = false;
		}
			
				//drawing the frame here
		
		begindraw();

		setCamera();
		drawSnake();
		drawFruit();
		opengl->drawAxis(0,0,0);
		enddraw();
	
		//if(cameraframes  == CAMERA_STEPS)
			//cameraframes = 0;
		if(cameraframes  <= CAMERA_STEPS && cameraframes>0)
			cameraframes++;

		frames++;
		//cameraframes++;
	}
	

	void gameController::setCamera()
	{
		
		if(cameraframes == CAMERA_STEPS)
		{
			prevdirection = direction;
			cameraframes = 0;
		}

		float dfr = (float)frames/FRAMES_PER_STEP;
		float cfr = (float)cameraframes/CAMERA_STEPS;

		cameraPos.set(-8,0,8);
		//cameraRot
		Vector3i zvect(0,0,3);

		zvect *= xRelRot;
		cameraPos *= xRelRot;

		if(!fixCamera)	//(float)frames/FRAMES_PER_STEP
			opengl->setCamera(	
									//camera position
									playerPos.x()+dfr*direction.x()+cameraPos.x(), 
									playerPos.y()+dfr*direction.y()+cameraPos.y(), 
									playerPos.z()+dfr*direction.z()+cameraPos.z(), 
									//aim position
									playerPos.x()+dfr*direction.x(),
									playerPos.y()+dfr*direction.y(),
									playerPos.z()+dfr*direction.z(),
									//normal to the camera
									zvect.x(),zvect.y(),zvect.z()
									//direction.x(), direction.y(), direction.z()
									);

		opengl->drawLine(
				playerPos.x()+cameraPos.x()+dfr*direction.x(),
				playerPos.y()+cameraPos.y()+dfr*direction.y(), 
				playerPos.z()+cameraPos.z()+dfr*direction.z(),
				playerPos.x()+dfr*direction.x(),
				playerPos.y()+dfr*direction.y(),
				playerPos.z()+dfr*direction.z()
				);

		opengl->drawLine(
				playerPos.x()+cameraPos.x()+dfr*direction.x(),
				playerPos.y()+cameraPos.y()+dfr*direction.y(), 
				playerPos.z()+cameraPos.z()+dfr*direction.z(),
				playerPos.x()+cameraPos.x()+dfr*direction.x()+zvect.x(),
				playerPos.y()+cameraPos.y()+dfr*direction.y()+zvect.y(),
				playerPos.z()+cameraPos.z()+dfr*direction.z()+zvect.z()
		);
	}

	void gameController::drawSnake()
	{

		//draw snake body
		Vector3i prev(playerPos);
		const snakeCell *next = playerSnake->getHead();

		//go thru the linked list
		while(next)
		{
			prev -= next->cellGuts.nextmove;

			//draw each body part
			opengl->drawCubeCellsDelta(
					prev.x(),
					prev.y(),
					prev.z(),
					next->cellGuts.nextmove.x()*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),	//animate if enabled
					next->cellGuts.nextmove.y()*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),
					next->cellGuts.nextmove.z()*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0)
				);

			next = playerSnake->getNext(next);
		}
	}

		void gameController::checkCollision()
	{
		if(playerPos == fruitPos)
		{
			//add a body cell
			playerSnake->add();	
			spawnFruit(10,10,0);
		}

		if(playerSnake->searchPos(playerPos))
		{
			std::cout<<"Tail hit."<<std::endl;
			gamePaused = true;
			animooted = false;
			fixCamera = true;
		}

		//playerSnake->outputSnake();
		//
	}

	void gameController::drawFruit()
	{
		if(fruitPos.x() != -1)
			opengl->drawCubeCells(fruitPos.x(), fruitPos.y(), fruitPos.z());
	}

	void gameController::spawnFruit(int xr, int yr, int zr)
	{
		do 
		{
			fruitPos.set(
						(xr!=0)?(int)rand()%xr:0,
						(yr!=0)?(int)rand()%yr:0,
						(zr!=0)?(int)rand()%zr:0);
		} while(playerSnake->searchPos(fruitPos));
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
}




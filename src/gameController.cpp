/*
 * gameController.cpp
 *
 *  Created on: 16.07.2011
 *      Author: никита
 */
#include <iostream>
#include "gameController.h"

#define fixedFPS 60	//game fps
#define FRAMES_PER_STEP 12	//specifies how many frames it takes to move between cells
#define CAMERA_STEPS 90		//frames it takes for the full camera rotation while turning the snake
#define PI 3.1415926536f	//just PI

#define CAMDEFX 10			//the camera's default position when it's fixed
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
				gamePaused(false),	// is game paused
				fixCamera(false),	// is camera fix or not(follows the snake)
				animooted(true),	// smooth moving or cell by cell moving
				direction(1,0,0),	// init moving direction
				playerPos(0,0,0),	// init playerPos
				fruitPos(-1,-1,-1),
				cameraPos(CAMDEFX,CAMDEFY,CAMDEFZ),
				defCameraPos(-4,0,10),	//the default camera's position relating to the snake's head
				defCameraNormal(0,0,1),	//a normal to the, default (0,0,1)
				cameraframes(CAMERA_STEPS),
				xRelRot(0,0,0),			//snake ingame rotation relating to (1,0,0) vector
				prevxRelRot(xRelRot),
				btnUpPressed(false),	//some bad ways to make the camera rotation more sense
				addOnNextCircle(false),	//some bad ways for a food moving inside the snake

				rot_up(0, PI/2, 0),		//default rotation matrices
				rot_down(0, -PI/2, 0),
				rot_left(0, 0, -PI/2),
				rot_right(0, 0, PI/2)
	{


		selfptr = this;
		srand (time((time_t)NULL));
	}

	gameController::~gameController()
	{
		delete window;
		delete opengl;
	}

	//init a window, spawn the snake
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

	//restarting the game
	void gameController::restartGame()
	{
		delete playerSnake;
		playerSnake = 0;

		direction.set(1,0,0);
		playerPos.set(0,0,0);

		playerSnake = new snake(direction, playerPos);
		playerSnake->add();
		playerSnake->add();

		spawnFruit(10,10,10);

		animooted = true;
		gamePaused = false;
		fixCamera = false;
		
		xRelRot.setAngle(0,0,0);
		prevxRelRot = xRelRot;
		cameraframes = CAMERA_STEPS;
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
				case(VK_DOWN):
					if(!arrowPressed)
					{
						//rotate the current direction vector, compute the general rotation matrix 'xRelRot'
						prevxRelRot = curRelRot;
						xRelRot = rot_up*xRelRot;	
						xvect *= xRelRot;

						direction = xvect;

						//by setting 'cameraframes' to 0 start the camera animation
						cameraframes = 0;

						arrowPressed = true;
						btnUpPressed = false;
					}
				break;

				case(VK_UP):
					if(!arrowPressed)
					{
						prevxRelRot = curRelRot;

						xRelRot = rot_down*xRelRot;	
						xvect *= xRelRot;

						direction = xvect;
						
						cameraframes = 0;
						arrowPressed = true;
						btnUpPressed = true;
					}
				break;

				case(VK_LEFT):
					if(!arrowPressed)
					{
						prevxRelRot = curRelRot;
						xRelRot = rot_left*xRelRot;	
						xvect *= xRelRot;

						direction = xvect;
						cameraframes = 0;

						arrowPressed = true;
						btnUpPressed = false;
					}
				break;

				case(VK_RIGHT):
					if(!arrowPressed)
					{		
						prevxRelRot = curRelRot;
						xRelRot = rot_right*xRelRot;	
						xvect *= xRelRot;

						direction = xvect;


						cameraframes = 0;

						arrowPressed = true;
						btnUpPressed = false;
					}

				break;

				case(VK_SPACE):
					animooted = !animooted;
				break;

				//'a'
				case(0x41):
					playerSnake->add();
				break;

				//'c'
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
		//1 step = 1 move between cells
		if (frames == FRAMES_PER_STEP)
		{ 
			//current head position = previos + current moving direction
			//move the tail and check collisions, also animate food moving (bad)
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
		opengl->drawCagePlayer(10,10,10,fruitPos.x()+1,fruitPos.y()+1,fruitPos.z()+1);
		enddraw();
	
		if(cameraframes  < CAMERA_STEPS)
			cameraframes++;

		frames++;
	}

	void gameController::setCamera()
	{
		//defaults
		cameraPos = defCameraPos;
		Vector3f zvect(defCameraNormal);

		if(cameraframes == CAMERA_STEPS)
		{
			prevxRelRot = xRelRot;
			btnUpPressed = false;
		}
		else
		{	
			cameraframes++;
		}

		float dfr = (float)frames/FRAMES_PER_STEP;
		float cfr = (float)cameraframes/CAMERA_STEPS;
		
		//rotations, should rework this place
		if(btnUpPressed)
			zvect *= xRelRot;
		else
			zvect *= prevxRelRot;

		//the current rotations angles are the weighted sum of the previous rotation and the next rotation angles
		curRelRot = xRelRot*cfr + prevxRelRot*(1-cfr);
		cameraPos *= curRelRot;

		if(!fixCamera)	
			opengl->setCamera(	
									//camera position
									playerPos.x()+dfr*direction.x()+cameraPos.x()+0.5f, 
									playerPos.y()+dfr*direction.y()+cameraPos.y()+0.5f, 
									playerPos.z()+dfr*direction.z()+cameraPos.z()+0.5f, 
									//aim position
									playerPos.x()+dfr*direction.x()+0.5f,
									playerPos.y()+dfr*direction.y()+0.5f,
									playerPos.z()+dfr*direction.z()+0.5f,
									//normal to the camera
									zvect.x(),zvect.y(),zvect.z()
									);

		if(fixCamera)
		{
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
	}

	void gameController::drawSnake()
	{

		//draw the snake's body
		Vector3i prev(playerPos);
		const snakeCell *next = playerSnake->getHead();

		//go thru a linked list
		while(next)
		{
			prev -= next->cellGuts.nextmove;

			//draw each body part
			opengl->drawCubeCellsDeltaFat(
					prev.x(),
					prev.y(),
					prev.z(),
					next->cellGuts.nextmove.x()*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),	//animate if enabled
					next->cellGuts.nextmove.y()*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),
					next->cellGuts.nextmove.z()*(float)frames/FRAMES_PER_STEP * ((animooted)?1.0f:0),
					next->cellGuts.fat
				);

			next = playerSnake->getNext(next);
		}
	}


	//check collisions and animate how a food moves thru the snake (should make a different function)
	void gameController::checkCollision()
	{	
		if(playerPos == fruitPos)
		{
			playerSnake->setHeadFat(1.26f);
			spawnFruit(10,10,10);
		}
		else
			playerSnake->setHeadFat(1.0f);

		if(addOnNextCircle)
		{
			playerSnake->add();
			addOnNextCircle = false;
		}

		if(playerSnake->getLast()->cellGuts.fat > 1.0f)
			addOnNextCircle = true;

		if(playerSnake->searchPos(playerPos))
		{
			std::cout<<"Tail hit."<<std::endl;
			gamePaused = true;
			animooted = false;
			fixCamera = true;
		}
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




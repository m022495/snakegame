/*
 * gameController.cpp
 *
 *  Created on: 16.07.2011
 *      Author: никита
 */
#include <iostream>
#include "gameController.h"

#define fixedFPS 60	//game fps
#define FRAMES_PER_STEP 14	//specifies how many frames it takes to move between cells
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
				openal(0),
				frames(0),
				zone(0),
				zoneSize(30,30,30),
				moveDirection(1,0,0),	// init moving direction
				playerPosition(0,0,0),	// init playerPosition
				fruitPosition(-1,-1,-1),
				cameraPosition(CAMDEFX,CAMDEFY,CAMDEFZ),
				defCameraPosition(-5,0,6),	//the default camera's position relating to the snake's head
				defCameraNormal(0,0,1),	//a normal to the camera view, default (0,0,1)
				cameraframes(CAMERA_STEPS),
				xRelRot(0,0,0),			//current snake rotation relating to (1,0,0) vector
				prevxRelRot(xRelRot),	//previous camera position
				curRelRot(xRelRot),		//current shifted camera position
				addOnNextCircle(false),	//some bad way to animate a food moving inside the snake

				rot_up(0, PI/2, 0),		//default rotation matrices
				rot_down(0, -PI/2, 0),
				rot_left(0, 0, -PI/2),
				rot_right(0, 0, PI/2),

				xVector(1,0,0),
				collected(0),		//how many fruits have been collected
				nervousfat(1.0f),
				fruitfat(1.0f),
				arrowPressed(false),
				gamePaused(false),	// is the game paused
				fixCamera(false),	// is the camera fixed or not (follows the snake)
				animooted(true),	// smooth moving or cell by cell moving
				loops(0)
	{


		selfptr = this;
		srand (time((time_t)NULL));
	}

	gameController::~gameController()
	{
		delete window;
		delete opengl;
		delete openal;
	}
	bool gameController::initSound()
	{
		opengl->initCamera(cameraPosition.x(), cameraPosition.y(),cameraPosition.z());
		openal = new openALwrapper;
		openal->openDevice("defdevice",openal->getDefaultDevice());
		openal->addContext("defdevice","defcontext");
		openal->enableContext("defcontext");
		openal->addBuffer("kick");
		openal->addSource("s_kick");
		openal->loadWavToBuffer("sounds/kick.wav","kick");
		openal->addBufferToSource("kick","s_kick");
		return true;
	}

	//init a window, spawn the snake
	bool gameController::init()
	{
		//create window
		window = new winapiwrapper(hInstance);
		try
		{
			window->createWindow(100,100,800,800);
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

		zone = new area(zoneSize.x(),zoneSize.y(),zoneSize.z());
		
		for(int i=0;i<50;i++)
		{
			zone->generate( rand()%(zoneSize.x()),
							rand()%(zoneSize.y()),
							rand()%(zoneSize.z()),
							2+rand()%5);
		}

		playerSnake = new snake(moveDirection, playerPosition);

		spawnFruit(zoneSize.x(),zoneSize.y(),zoneSize.z());
		playerSnake->add();
		playerSnake->add();

		cout<<zone->howmany()<<endl;

		initSound();

		return true;
	}

	//restarting the game
	void gameController::restartGame()
	{
		delete playerSnake;
		playerSnake = 0;

		moveDirection.set(1,0,0);
		playerPosition.set(0,0,0);

		playerSnake = new snake(moveDirection, playerPosition);
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
				{
					delete this;
					break;
				}
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
			else
			{
				//main game loop
				loopTimer.start();

				mainloop();

				float fps = (float)loopTimer.getTicksRate().QuadPart/loopTimer.getTime().QuadPart;
				//if	(fps > fixedFPS)
					//waiter(long(loopTimer.getTicksRate().QuadPart/fixedFPS - loopTimer.getTime().QuadPart));
				if(loops == 50)
				{
					cout<<fps<<endl;
					loops = 0;
				}

				loops++;
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

	void gameController::processKeyDown()
	{
		//rotate the current direction vector, compute the general rotation matrix 'xRelRot'
		doAllRotations(rot_up);
		//start the camera animation by setting 'cameraframes' to 0 
		startCameraMoving();
	}
	
	void gameController::processKeyUp()
	{
		//rotate the current direction vector, compute the general rotation matrix 'xRelRot'
		doAllRotations(rot_down);
		//start the camera animation by setting 'cameraframes' to 0 
		startCameraMoving();
	}
	
	void gameController::processKeyLeft()
	{
		//rotate the current direction vector, compute the general rotation matrix 'xRelRot'
		doAllRotations(rot_left);
		//start the camera animation by setting 'cameraframes' to 0 
		startCameraMoving();
	}
	
	void gameController::processKeyRight()
	{
		//rotate the current direction vector, compute the general rotation matrix 'xRelRot'
		doAllRotations(rot_right);
		//start the camera animation by setting 'cameraframes' to 0 
		startCameraMoving();
	}

	void gameController::ProcessEvent(int type, long param0, long param1)
	{
		if (type == WM_KEYDOWN)
		{
			switch (param0)
			{
				case(VK_DOWN):
					if(!arrowPressed)
					{
						processKeyDown();
						arrowPressed = true;
					}
				break;

				case(VK_UP):
					if(!arrowPressed)
					{
						processKeyUp();
						arrowPressed = true;
					}
				break;

				case(VK_LEFT):
					if(!arrowPressed)
					{
						processKeyLeft();
						arrowPressed = true;
					}
				break;

				case(VK_RIGHT):
					if(!arrowPressed)
					{		
						processKeyRight();
						arrowPressed = true;
					}

				break;

				case(VK_NUMPAD0):
						openal->stopSource("s_kick");
						openal->playSource("s_kick");
						nervousfat = 1.3f;
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
						cameraPosition.set(CAMDEFX, CAMDEFY, CAMDEFZ);
						opengl->initCamera(cameraPosition.z(), cameraPosition.y(), cameraPosition.z());
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
			//moveDirection.output();
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

	//rotate the default direction (1,0,0) to the current snake direction
	void gameController::doAllRotations(const RotMatrix3f &rot)
	{
		Vector3i xvect_temp(xVector);	
		prevxRelRot = curRelRot;
		xRelRot = rot*xRelRot;	
		xvect_temp *= xRelRot;
		moveDirection = xvect_temp;
	}

	void gameController::mainloop()
	{
		if (frames == FRAMES_PER_STEP)
		{ 
			//1 step = 1 move between cells
			//current head position = previos + current moving direction
			//move the tail and check collisions, also animate food moving (bad)
			if(!gamePaused)
			{
				playerPosition += moveDirection;	
				playerSnake->move(moveDirection);
				checkCollision();
			}

			frames = 0;	
			arrowPressed = false; //the camera tweak
		}
			
		if(nervousfat>1.01f)
			nervousfat -= 0.005f;
		else
			nervousfat = 1.0f;

		if(fruitfat>1.0f)
			fruitfat -= 0.01f;
		else
			fruitfat = 1.4f;

		//drawing the frame here
		begindraw();
			setCamera();
			drawSnake();
			drawFruit();
			drawArea();
			opengl->drawAxis(0,0,0);
			opengl->drawCagePlayer(50,50,50,fruitPosition.x()+1,fruitPosition.y()+1,fruitPosition.z()+1);
		enddraw();

		if(cameraframes  < CAMERA_STEPS)
			cameraframes++;

		frames++;
	}

	void gameController::setCamera()
	{
		//defaults
		cameraPosition = defCameraPosition;
		//the default camera position is at (-4,0,10)
		Vector3f zvect(defCameraNormal);

		if(cameraframes == CAMERA_STEPS)
		{
			//if there is no any necessary rotation
			prevxRelRot = xRelRot;
		}
		else
		{	
			cameraframes++;
		}

		//shift between cells
		float dfr = (float)frames/FRAMES_PER_STEP; 

		//shift between the old camera position and the new one
		float cfr = (float)cameraframes/CAMERA_STEPS;	
		
		//rotations. should better rework this place.

		//the current rotations angles are the weighted sum of the previous rotation and the next rotation angles
		curRelRot = xRelRot*cfr + prevxRelRot*(1-cfr);
		cameraPosition *= curRelRot; //rotate camera to the actual position

		zvect *= curRelRot;

		if(!fixCamera)	
			opengl->setCamera(	
									//camera position
									playerPosition.x()+dfr*moveDirection.x()+cameraPosition.x()+0.5f, 
									playerPosition.y()+dfr*moveDirection.y()+cameraPosition.y()+0.5f, 
									playerPosition.z()+dfr*moveDirection.z()+cameraPosition.z()+0.5f, 
									//aim position
									playerPosition.x()+dfr*moveDirection.x()+0.5f,
									playerPosition.y()+dfr*moveDirection.y()+0.5f,
									playerPosition.z()+dfr*moveDirection.z()+0.5f,
									//normal to the camera
									zvect.x(),zvect.y(),zvect.z()
									);
	}

	void gameController::drawSnake()
	{

		//draw the snake's body
		Vector3i prev(playerPosition);
		const snakeCell *next = playerSnake->getHead();

		//go through a linked list
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
					next->cellGuts.fat*nervousfat
				);

			next = playerSnake->getNext(next);
		}
	}


	//check collisions and animate a food moving through the snake (should make a different function)
	void gameController::checkCollision()
	{	
		if(playerPosition == fruitPosition)
		{
			//better call the function 'fruit hit'	
			playerSnake->setHeadFat(1.2f);
			spawnFruit(zoneSize.x(),zoneSize.y(),zoneSize.z());
			collected++;
		}
		else
			playerSnake->setHeadFat(1.0f);

		if(addOnNextCircle)
		{
			playerSnake->add();
			addOnNextCircle = false;
		}

		if(playerSnake->getLast()->cellGuts.fat > 1.0f) //very baf
			addOnNextCircle = true;

		if(playerSnake->searchPos(playerPosition) || 
			(*zone)(playerPosition.x(),playerPosition.y(),playerPosition.z()) == AREA_BOMB)
		{
			std::cout<<"hit."<<std::endl;

			gamePaused = true;
			animooted = false;
			fixCamera = true;
		}
	}

	void gameController::drawFruit()
	{
		if(fruitPosition.x() != -1)
			opengl->drawCubeCellsDeltaFat(fruitPosition.x(), fruitPosition.y(), fruitPosition.z(),0,0,0,fruitfat);
	}

	void gameController::drawArea()
	{
		for(int i=0;i<zoneSize.x();i++)
		{
			for(int j=0;j<zoneSize.y();j++)
			{
				for(int k=0;k<zoneSize.z();k++)
				{
					//cout<<(*zone)(i,j,k)<<endl;
					if((*zone)(i,j,k)== AREA_BOMB) //make draw only visible ones
					{
						opengl->drawCubeCellsDeltaFatBlack(i,j,k,0,0,0,nervousfat);
						//opengl->drawCubeCellsDeltaFatBlack(zoneSize.x()+i,j,k,0,0,0,nervousfat);
						//opengl->drawCubeCellsDeltaFatBlack(i,zoneSize.y()+j,k,0,0,0,nervousfat);
						//opengl->drawCubeCellsDeltaFatBlack(i,j,zoneSize.z()+k,0,0,0,nervousfat);
						//opengl->drawCubeCellsDeltaFatBlack(-zoneSize.x()+i,j,k,0,0,0,nervousfat);
						//opengl->drawCubeCellsDeltaFatBlack(i,-zoneSize.y()+j,k,0,0,0,nervousfat);
						//opengl->drawCubeCellsDeltaFatBlack(i,j,-zoneSize.z()+k,0,0,0,nervousfat);
					}
				}
			}
		}
	}

	void gameController::spawnFruit(int xr, int yr, int zr)
	{
		do 
		{
			fruitPosition.set(
						(xr!=0)?(int)rand()%xr:0,
						(yr!=0)?(int)rand()%yr:0,
						(zr!=0)?(int)rand()%zr:0);
		} while(playerSnake->searchPos(fruitPosition) || 
			(*zone)(fruitPosition.x(),fruitPosition.y(),fruitPosition.z())>0);
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




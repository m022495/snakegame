/*
 * gameController.h
 *
 *  Created on: 16.07.2011
 *      Author: никита
 */

#include "windows.h"
#include <stdexcept>
#include <typeinfo>
#include <windows.h>
#include <time.h>

#include "winapiwrapper.h"
#include "openglwrapper.h"
#include "datatype.h"
#include "snake.h"
#include "area.h"
#include "timer.h"

#include "mmatrix.h"
#include "matrix_rotation_matrix.h"
#include "matrix_3dvector.h"
#include "openalwrapper.h"

#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

typedef LARGE_INTEGER winlarge;

namespace GController
{
	class gameController
	{

	private:
		HINSTANCE hInstance;

		static gameController *selfptr;

		winapiwrapper *window;
		openglwrapper *opengl;
		openALwrapper *openal;
		snake *playerSnake;
		area *zone;

		Vector3i zoneSize;

		Vector3i playerPosition;
		Vector3i fruitPosition;
		Vector3i moveDirection;

		Vector3f cameraPosition;
		Vector3f defCameraPosition;
		Vector3f defCameraNormal;

		RotMatrix3f xRelRot;
		RotMatrix3f prevxRelRot;
		RotMatrix3f curRelRot;

		float nervousfat;
		float fruitfat;
		int loops;

		const RotMatrix3f rot_up;
		const RotMatrix3f rot_down;
		const RotMatrix3f rot_left;
		const RotMatrix3f rot_right;
		const Vector3i xVector;

		void doAllRotations(const RotMatrix3f &rot);

		void processKeyUp();
		void processKeyDown();
		void processKeyLeft();
		void processKeyRight();

		bool arrowPressed;
		bool gamePaused;
		bool fixCamera;
		bool animooted;

		bool addOnNextCircle;

		int frames;
		int cameraframes;
		int collected;

		void spawnFruit(int,int,int); //spawn fruit in the specified cells range
		void checkCollision();
		void setCamera();
		void drawSnake();
		void drawFruit();
		void drawArea();

		void startCameraMoving(){cameraframes = 0;}

		void begindraw() {opengl->begindraw();}
		void enddraw() {opengl->enddraw();}

		Timer loopTimer;

		bool initSound();

	public:
		gameController(HINSTANCE hInst);
	   ~gameController();

		bool init();
		void go();
		void restartGame();

		void mainloop();
		static bool callBack(int, long, long);
		void ProcessEvent(int, long, long);

	};

	void waiter(long wticks);
}
#endif /* GAMECONTROLLER_H_ */

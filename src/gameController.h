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
		snake *playerSnake;

		Vector3i playerPos;
		Vector3i fruitPos;
		Vector3i direction;
		Vector3f cameraPos;
		//Vector3f prevCameraPos;
		//RotMatrix3f cameraRot;	//temp variable

		RotMatrix3f xRelRot;
		RotMatrix3f prevxRelRot;
		RotMatrix3f curRelRot;

		bool arrowPressed;
		bool gamePaused;
		bool fixCamera;
		bool animooted;
		int frames;
		int cameraframes;

		void spawnFruit(int,int,int); //spawn fruit in range
		void checkCollision();
		void setCamera();
		void drawSnake();
		void drawFruit();

		void begindraw() {opengl->begindraw();}
		void enddraw() {opengl->enddraw();}

		bool btnUpPressed;

		Timer loopTimer;

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

	void loadBand(int n);
	void waiter(long wticks);
}
#endif /* GAMECONTROLLER_H_ */

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


//#define GC_KEYDOWN 1
//#define GC_KEYUP 2
//#define GC_LMOUSE 2

#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

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
		area *curLevel;

		xyz playerPos;
		xyz fruitPos;
		xyz direction;

		bool arrowPressed;
		bool gamePaused;
		bool fixCamera;
		bool animooted;
		int frames;

		time_t start_tick;
		time_t end_tick;
		double frame_time;

		void spawnFruit(int,int,int); //spawn fruit in range

	public:
		gameController(HINSTANCE hInst);
		~gameController();
		bool init();
		void go();
		void mainloop();
		static bool callBack(int, long, long);
		void ProcessEvent(int, long, long);
	};
}
#endif /* GAMECONTROLLER_H_ */

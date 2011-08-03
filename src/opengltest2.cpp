#include <windows.h>
#include <iostream>

#include "winapiwrapper.h"
#include "openglwrapper.h"
#include "gameController.h"

using namespace GController;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
               LPSTR lpCmdLine, int nCmdShow)
{
	gameController *snake = new gameController(hInstance);
	//gameControllerSetObj(snake);	//for callback methods

	std::cout<<"Hi!"<<std::endl;

	try
	{
		if(snake->init())
			snake->go();
		else
		{
			std::cout<<"Error during game initialization."<<std::endl;
			return 1;
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	std::cout<<"quiting...";

	return 0;

}

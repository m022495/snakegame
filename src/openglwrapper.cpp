/*
 * openglwrapper.cpp
 *
 *  Created on: 15.07.2011
 *      Author: никита
 */

#include "openglwrapper.h"
#include <iostream>


#define COORDS_RATIO 1.0f		//   gamecoords / openglcoords
#define	CELL_SIZE COORDS_RATIO * 1.0f
#define CUBE_EDGE ((CELL_SIZE/2)-0.07f)	

using namespace std;

const GLubyte openglwrapper::frontIndicies[] = {0, 1, 2, 3};
const GLubyte openglwrapper::leftIndicies[] = {0, 4, 7, 1};
const GLubyte openglwrapper::rightIndicies[] = {3, 2, 6, 5};
const GLubyte openglwrapper::botIndicies[] = {0, 3, 5, 4};
const GLubyte openglwrapper::topIndicies[] = {1, 7, 6, 2};
const GLubyte openglwrapper::backIndicies[] = {6, 7, 4, 5};

GLfloat openglwrapper::cellSize = CELL_SIZE;

//инициализация указателя на реализацию класса. Поэтому реализация ограничивается одним объектом openglwrapper.
openglwrapper *openglwrapper::selfptr = NULL;

openglwrapper::openglwrapper(HDC hDC):
						isPixelFormatSet(false),
						fTriangleTheta(0.9f),
						fXSpeed(0.0f),
						fYSpeed(0.0f),
						fZSpeed(0.0f),
						switched1(false),
						switched2(false),
						inited(false),
						fval(-1.0f),
						fzoom(1.0f),
						defCubeVertices(0)
{
	this->hDC = hDC;
	selfptr = this;
	
	defCubeVertices = new GLfloat[24];
	setCubeVertices(defCubeVertices, CUBE_EDGE);
	defCubeVertices;
}

openglwrapper::~openglwrapper()
{
	delete[] defCubeVertices;
}

BOOL openglwrapper::SetPixelFormat()
{
	return SetPixelFormat(PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 24, 16);
}

BOOL openglwrapper::SetPixelFormat(DWORD dwFlags, BYTE iPixelType, BYTE cColorBits, BYTE cDepthBits)
{
	//структура, описывающая пиксельный формат устройства
	PIXELFORMATDESCRIPTOR pfd;

	//выделяем память под всю структуру
	ZeroMemory( &pfd, sizeof( pfd ) );

	pfd.nSize = sizeof( pfd );

	pfd.nVersion = 1;

	//параметры пиксельного буфера	http://msdn.microsoft.com/en-us/library/dd368826(v=vs.85).aspx
	pfd.dwFlags = dwFlags;

	//тип цвета пикселя: RGB или по индексу
	pfd.iPixelType = iPixelType;
	pfd.cColorBits = cColorBits;						//

	//Specifies the depth of the depth (z-axis) buffer.
	pfd.cDepthBits = cDepthBits;

	//выбирает какой-то подходящий пиксельный формат
	int iFormat = ChoosePixelFormat(hDC, &pfd);

	if (!iFormat)
	{
		throw std::runtime_error(std::string("openglwrapper::SetPixelFormat(): error during ChoosePixelFormat()"));
		return false;
	}
	else
	{
		isPixelFormatSet = true;
		return ::SetPixelFormat(hDC, iFormat, &pfd);
	}
}

bool openglwrapper::CreateContext()
{
	if(!hDC)
	{
		throw std::runtime_error(std::string("openglwrapper::CreateContext(): Trying to create context while hDC haven't set"));
		return false;
	}

	hRC = wglCreateContext( hDC );

	if(!hRC)
	{
		throw std::runtime_error(std::string("openglwrapper::CreateContext(): Cannot create context via wglCreateContext()"));
		return false;
	}

	if(!wglMakeCurrent( hDC, hRC ))
	{
		throw std::runtime_error(std::string("openglwrapper::CreateContext(): Cannot enable context via wglMakeCurrent()"));
		return false;
	}

	return true;
}

void openglwrapper::begindraw()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);	
	glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*3, defCubeVertices);
}

void openglwrapper::enddraw()
{
	SwapBuffers( hDC );
}

void openglwrapper::initSettings()
{
		//задаем цвет фона
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f ); 
		//задаем дефолтное значение для буферы глубины
		glClearDepth(1.0f);

		//включаем сглаживание точек
		glEnable(GL_POINT_SMOOTH);  
		glShadeModel(GL_SMOOTH);

		glEnable(GL_DEPTH_TEST);							// включаем поддержку глубины
		glDepthFunc(GL_LEQUAL);								// задает способ сравнения при работе метода z-буфера.
 		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		
		//размер точек
		glPointSize(10);	
		glLineWidth(0.1f);

}

void openglwrapper::drawCubeCells(int x, int y, int z)
{

		drawCube((float)x*cellSize +(float)cellSize/2, 
				 (float)y*cellSize +(float)cellSize/2, 
				(float)z*cellSize +(float)cellSize/2);

}

void openglwrapper::drawCubeCellsDelta(int x, int y, int z, float dx, float dy, float dz)
{

		drawCube((float)x*cellSize +(float)cellSize/2 +(float)dx*cellSize, 
				 (float)y*cellSize +(float)cellSize/2 +(float)dy*cellSize, 
				(float)z*cellSize +(float)cellSize/2 +(float)dz*cellSize);
}

void openglwrapper::drawCubeCellsDeltaFat(int x, int y, int z, float dx, float dy, float dz, float fat)
{

		GLfloat *vertices = new GLfloat[24];
		setCubeVertices(vertices, CUBE_EDGE*fat);
		glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*3, vertices);

		drawCube((float)x*cellSize +(float)cellSize/2 +(float)dx*cellSize, 
				 (float)y*cellSize +(float)cellSize/2 +(float)dy*cellSize, 
				(float)z*cellSize +(float)cellSize/2 +(float)dz*cellSize);

		delete[] vertices;
		glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*3, defCubeVertices);
}

void openglwrapper::setCamera(GLfloat cx, GLfloat cy, GLfloat cz,  GLfloat px, GLfloat py, GLfloat pz,
			GLfloat upx, GLfloat upy, GLfloat upz)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt (
		cx, 
		cy, 
		cz,

		px, 
		py, 
		pz,    
		
		upx, upy, upz);
}

void openglwrapper::setCameraCell(int cx, int cy, int cz, int upx, int upy, int upz)
{
		setCamera(
			cx*cellSize, 
			cy*cellSize, 
			cz*cellSize+(30*fzoom), 
		
			(cx+1)*cellSize, 
			cy*cellSize, 
			cz*cellSize,    
		
			0, 1, 0);

}

void openglwrapper::setCameraCellDelta(int cx, int cy, int cz, float dx, float dy, float dz, int upx, int upy, int upz)
{
	setCamera (
		(cx-5*upx) * cellSize + (float)dx*cellSize, //camera position
		(cy-5*upy) * cellSize + (float)dy*cellSize, 
		(cz-5*upz) * cellSize + (5*fzoom), 
		
		cx*cellSize+(float)dx*cellSize,			//point to spectate
		cy*cellSize+(float)dy*cellSize, 
		cz*cellSize,    
		
		upx, upy, upz);

}

void openglwrapper::drawCube(GLfloat x, GLfloat y, GLfloat z)
{

	glPushMatrix();

	glTranslatef(x,y,z);

	glColor3f(0.7f, 0.7f, 0);

	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, frontIndicies);
	glColor3f(0.3f, 0, 1.0f);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, leftIndicies);
	glColor3f(1.0f, 0.4f, 0);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, rightIndicies);
	glColor3f(0.1f, 0, 0.5f);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, botIndicies);
	glColor3f(0.3f, 0.3f, 0.3f);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, topIndicies);
	glColor3f(0.4f, 0.1f, 0.6f);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, backIndicies);

	glPopMatrix();
}

void openglwrapper::moveSpawn(GLfloat x, GLfloat y, GLfloat z)
{
	glTranslatef(x,y,z);
}

void openglwrapper::initCamera(float x, float y, float z)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1, 100.0);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (x, y, z,    0, 0, 0,    0.0, 0.0, 1.0);

}

void drawVector(float x, float y, float z)
{
	glBegin(GL_LINES);


	glEnd();

}

void openglwrapper::drawCage(int xs, int ys, int zs)
{
	glColor3f(0.8f,0.8f,0.8f);
	glBegin(GL_LINES);
	
	for (int i = 0; i<xs; i++)
	{
		for(int j=0; j<ys; j++)
		{
			glVertex3f(i*cellSize,j*cellSize,0);
			glVertex3f(i*cellSize,j*cellSize,zs*cellSize);
		}
		for(int k=0; k<zs; k++)
		{
			glVertex3f(i*cellSize,0,k*cellSize);
			glVertex3f(i*cellSize,ys*cellSize,k*cellSize);
		}
	}
	for (int j=0; j<ys; j++) 
	{
		for(int k=0; k<zs; k++)
		{
			glVertex3f(0,j*cellSize,k*cellSize);
			glVertex3f(xs*cellSize,j*cellSize,k*cellSize);
		}
	}
	glEnd();
}

void openglwrapper::drawCagePlayer(int xs, int ys, int zs, int ppx, int ppy, int ppz)
{

	glColor3f(0.8f,0.8f,0.8f);
	glBegin(GL_LINES);
	
	for (int i = ppx-1; i<ppx+1; i++)
	{
		for(int j=ppy-1; j<ppy+1; j++)
		{
			glVertex3f(i*cellSize,j*cellSize,0);
			glVertex3f(i*cellSize,j*cellSize,zs*cellSize);
		}
		for(int k=ppz-1; k<ppz+1; k++)
		{
			glVertex3f(i*cellSize,0,k*cellSize);
			glVertex3f(i*cellSize,ys*cellSize,k*cellSize);
		}
	}
	for (int j=ppy-1; j<ppy+1; j++) 
	{
		for(int k=ppz-1; k<ppz+1; k++)
		{
			glVertex3f(0,j*cellSize,k*cellSize);
			glVertex3f(xs*cellSize,j*cellSize,k*cellSize);
		}
	}
	glEnd();
}

void openglwrapper::drawAxis(float x, float y, float z)
{
	glBegin(GL_LINES);

	//glColor3f(1.0,0,0);	//red x
	glColor3f(0.8f,0.8f,0.8f);	
	glVertex3d(x-50,y,z);
	glVertex3d(x+50,y,z);

	//glColor3f(0,1.0,0);	//green y
	glColor3f(0.5f,0.5f,0.5f);	
	glVertex3d(x,y-50,z);
	glVertex3d(x,y+50,z);

	//glColor3f(0,0,1.0);	//blue z
	glColor3f(0.4f,0.4f,0.3f);	
	glVertex3d(x,y,z-50);
	glVertex3d(x,y,z+50);

	glEnd();

	glBegin(GL_POINTS);
		glVertex3d(0,0,0);
	glEnd();

}

void openglwrapper::drawLine(float x1, float y1, float z1, float x2, float y2, float z2)
{
	glBegin(GL_LINES);
		glColor3f(0 , 0, 0);
		glVertex3f(x1,y1,z1);
		glVertex3f(x2,y2,z2);
	glEnd();

}

void openglwrapper::switcher1()
{
	if(switched1)
	{
		glFrontFace(GL_CCW);
	}
	else
	{
		glFrontFace(GL_CW);
	}
	switched1 = !switched1;
}
void openglwrapper::switcher2()
{
	if(switched2)
	{
		glCullFace( GL_BACK);
	}
	else
	{
		glCullFace( GL_FRONT);
	}
	switched2 = !switched2;
}

bool openglwrapper::EnableOpenGL(HWND hWnd)
{
	if(!hWnd)
	{
		throw  std::runtime_error(std::string("openglwrapper::EnableOpenGL(): NULL window handle specified"));
		return false;
	}
	if	(!isPixelFormatSet)
		if(!SetPixelFormat())
		{
			throw  std::runtime_error(std::string("openglwrapper::EnableOpenGL(): Cannot set pixel format via SetPixelFormat()"));
			return false;
		}

	if(!CreateContext())
			return false;

	this->hWnd = hWnd;

	initSettings();
	//initCamera();

	return true;
}

void openglwrapper::reshape(int width, int height)
{
	// задает область просмотра.
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	initSettings();
	//initCamera();
}

bool openglwrapper::DisableOpenGL()
{
	 wglMakeCurrent( NULL, NULL );
	 wglDeleteContext( hRC );
	 ReleaseDC( hWnd, hDC );

	 hRC = 0;
	 hWnd = 0;
	 hDC = 0;
	return true;
}

void openglwrapper::setCubeVertices(GLfloat *verts, GLfloat edge)
{
	verts[0]= -edge;verts[1]= -edge;verts[2]= -edge;
	verts[3]=	-edge;verts[4]=	-edge;verts[5]=	edge;
	verts[6]=	edge;verts[7]=	-edge;verts[8]=	edge;
	verts[9]=	edge;verts[10]=	-edge;verts[11]=	-edge; 
	verts[12]=	-edge;verts[13]=	edge;verts[14]=	-edge; 
	verts[15]=	edge;verts[16]=	edge;verts[17]=	-edge;
	verts[18]=	edge;verts[19]=	edge;verts[20]=	edge; 
	verts[21]=	-edge;verts[22]=	edge;verts[23]=	edge;

}
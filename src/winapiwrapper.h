#include "windows.h"
#include "windowsx.h"

#include <vector>
#include <iterator>
#include <map>
#include <sstream>

#ifndef WINAPIWRAPPER_H_
#define WINAPIWRAPPER_H_

class winapiwrapper;

typedef int (winapiwrapper::*tyMessageHandler)(HWND, long, long);	//��� tyMessageHandler -- �������, ������������ long
typedef std::map<long, tyMessageHandler> tyMessageMap;					//��� tyMessageMap -- ������������� ������ ������� � ����������� � ��� �����������
typedef tyMessageMap::iterator tyMessageIterator;						//��� tyMessageIterator -- �������� map ������ tyMessageMap
typedef bool (*tyCallBack)(int,long,long);

class winapiwrapper
{

private:

	HINSTANCE hInstance;
	HWND hWnd;
	HDC hDC; //device context

	tyCallBack vpFunc;
	tyMessageMap m_MsgHandlers;

	int wHeight, wWidth;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	tyMessageIterator GetMessageHandler(long message);						//���������� ������� �������������� ����������, ���������� ������ �� ������ �����
	void AddMessageHandler(long message, tyMessageHandler lpHandler);

	//�������

	int onDestroy(HWND hWnd, long param0, long param1);
	int onLeftMouseDown(HWND hWnd, long param0, long param1);
	int onRightMouseDown(HWND hWnd, long param0, long param1);
	int onMidMouseDown(HWND hWnd, long param0, long param1);
	int onKeyDown(HWND hWnd, long param0, long param1);
	int onResize(HWND hWnd, long param0, long param1);
	int onWheel(HWND hWnd, long param0, long param1);
	//

	static winapiwrapper* selfptr;

public:

	winapiwrapper(HINSTANCE hInstance);
	~winapiwrapper();

	BOOL createWindow(LPCSTR lpszClassName, int x, int y, int nWidth, int nHeight);	//���� ������������� ������
	BOOL createWindow(int x, int y, int nWidth, int nHeight);		//���� ���������� ������
	BOOL createWindow();

	BOOL addWindowClass(const WNDCLASSEX &WndClsEx);	//������������ ����� �����, � �������� ��������� -- ������ �� ����������� ���������� WNDCLASSEX. ������, ����� �� ���������� ������ ��� ��� ���������.
	BOOL addWindowClass(LPCSTR lpszClassName, UINT uStyle);	//�� ��, �� �� ���������� -- ��� ������ � �����
	BOOL addWindowClass(LPCSTR lpszClassName, UINT uStyle, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, HICON hIconSm); //�� ��, �� �� ���������� -- ��� ������, ����� � ������

	BOOL WshowWindow();

	HDC getHDC();
	HWND getHWND();

	int getWindowHeight() {return wHeight;}
	int getWindowWidth() {return wWidth;}

	void setCallBack (tyCallBack vpF) {vpFunc = vpF;}

	static LRESULT CALLBACK MsgRoute(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	void quit() {PostQuitMessage(0);}

};

#endif /* GAMECONTROLLER_H_ */

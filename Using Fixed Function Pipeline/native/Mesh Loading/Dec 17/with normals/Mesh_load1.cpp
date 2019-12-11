#include <windows.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR SzCmdLine , int nCmdShow)
{

	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclass;
	TCHAR szAppName[] = TCHAR "myObjParserApp";

	wndclass.cbSize = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.cbWndExtra = 0;
	wndclass.cbClsExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
}


LRESULT CALLBACK WndProc(HWND hwnd ,UINT iMsg ,WPARAM wParam ,LPARAM lParam);
{
	switch(iMsg)
	{


		default:
			break;
	}

	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}

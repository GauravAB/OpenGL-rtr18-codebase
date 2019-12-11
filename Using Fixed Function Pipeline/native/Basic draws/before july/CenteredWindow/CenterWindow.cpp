#include <Windows.h>
#include <gl\GL.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#pragma comment(lib,"opengl32.lib")
HWND ghwnd;
bool gbDone = false;
bool gbActive = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;
bool gbRemoveCap = false;

DWORD dwStyle = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	void initialize(void);
	void uninitialize(void);
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclass;
	TCHAR szAppName[] = TEXT("MyCenterWindow");

	int wMax, hMax , clientWMAX , clientHMAX;
	wMax = GetSystemMetrics(SM_CXSCREEN);
	hMax = GetSystemMetrics(SM_CYSCREEN);

	int hPos = wMax /2;
	int vPos = hMax / 2;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("WindowCentered"), WS_OVERLAPPEDWINDOW, (hPos-(WIN_WIDTH/2)), vPos-(WIN_HEIGHT/2), WIN_WIDTH, WIN_HEIGHT, 0, 0, hInstance, 0);
	ghwnd = hwnd;
	//initialize();
	ShowWindow(hwnd, nCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	
	while (gbDone == false)
	{
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{
			if (msg.wParam == WM_QUIT)
			{
				gbDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActive)
			{
				if (gbEscapeKeyIsPressed)
				{
					gbDone = true;
				}
			}
			//display call is potentially here
		}
	}
	//uninitialize();
	return((int)msg.wParam);
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	void display(void);
	void resize(int, int);
	void uninitialize(void);
	void ToggleFullScreen(HWND hwnd);
	void remove_capnborder(void);

	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case 0x41:
			if (gbRemoveCap == false)
			{
				remove_capnborder();
				gbRemoveCap = true;
			}
			else
			{
				remove_capnborder();
				gbRemoveCap = false;
			}

			break;
		case 0x46:
			if (gbFullScreen == false)
			{
				ToggleFullScreen(hwnd);
				gbFullScreen = true;
			}
			else
			{
				ToggleFullScreen(hwnd);
				gbFullScreen = false;
			}
			break;
		}
		break;
	case WM_ACTIVATE:
		gbActive = true;
		break;
	case WM_SIZE:
		resize(HIWORD(lParam), LOWORD(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, msg, wParam, lParam));
}


void ToggleFullScreen(HWND hwnd)
{
	MONITORINFO mi;
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(hwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = {sizeof(MONITORINFO)};
			if (GetWindowPlacement(hwnd,&wpPrev) && GetMonitorInfo(MonitorFromWindow(hwnd,MONITORINFOF_PRIMARY),&mi))
			{
				SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &wpPrev);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE);
		ShowCursor(TRUE);
	}
}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}


void remove_capnborder(void)
{

	if (gbRemoveCap == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (gbFullScreen == false)
		{
			GetWindowPlacement(ghwnd, &wpPrev);
		}
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
	}
}


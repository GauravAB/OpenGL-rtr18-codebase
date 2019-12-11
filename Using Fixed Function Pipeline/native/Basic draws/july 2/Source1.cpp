//header file
#include <Windows.h>
#include <stdlib.h>

//macros hardcoded values
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

bool gbFullscreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HWND ghwnd = NULL;

//window procedure callback function
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//start main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hwnd;
	TCHAR szAppname[] = TEXT("My OGL window");
	MSG msg;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.lpszClassName = szAppname;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppname, TEXT("my openGL window"), WS_OVERLAPPEDWINDOW, 0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, hInstance, 0);
	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("HWND failed"), TEXT("error"), MB_OK);
	}
	ghwnd = hwnd;

	UpdateWindow(hwnd);
	ShowWindow(hwnd, nCmdShow);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	void ToggleFullScreen(void);

	switch (msg)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:
				DestroyWindow(hwnd);
				break;
			case 0x46:
				if (gbFullscreen == false)
				{
					ToggleFullScreen();
					gbFullscreen = true;
				}
				else
				{
					ToggleFullScreen();
					gbFullscreen = false;
				}
				break;
			default:
				break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;
	}

	return(DefWindowProc(hwnd, msg, wParam, lParam));
}

void ToggleFullScreen()
{
	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);

	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode))
	{
		ChangeDisplaySettings(&devMode,)
	}
	else
	{
		MessageBox(NULL, TEXT("error from enumdisplaysettings"), TEXT("error"), MB_OK);
		DestroyWindow(ghwnd);
	}
	
	
	/*MONITORINFO mi;
	if (gbFullscreen == false)
	{
		if ((dwStyle = GetWindowLong(ghwnd, GWL_STYLE)) & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOSIZE);
		ShowCursor(TRUE);
	}
	*/
	
}






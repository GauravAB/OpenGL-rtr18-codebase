#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

bool bFullscreen = false;
HWND ghwnd;
DWORD dwStyle;
WINDOWPLACEMENT wPrev = { sizeof(WINDOWPLACEMENT) };

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CmdLine, int nCmdshow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclass;
	TCHAR szAppName[] = TEXT("WindowName");

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
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

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("MessageDealer"), WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, 0, 0, hInstance,0);
	ghwnd = hwnd;
	
	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("Error while creating window"), TEXT("error"), MB_OK);
		DestroyWindow(NULL);
		exit(EXIT_FAILURE);
	}

	ShowWindow(hwnd, nCmdshow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	void ToggleFullscreen();
	TCHAR str[512];

	switch(msg)
	{
		case WM_CREATE:
			MessageBox(hwnd, TEXT("wm_create received"), TEXT("WM_CREATE"), MB_OK);
			break;
		case WM_DESTROY:
			MessageBox(hwnd, TEXT("wm_destroy received"), TEXT("WM_DESTROY window"), MB_OK);
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:
				DestroyWindow(hwnd);
				break;
			case 0x46:
				if (bFullscreen == false)
				{
					
					ToggleFullscreen();
					bFullscreen = true;
				}
				else
				{
				
					ToggleFullscreen();
					bFullscreen = false;
				}
				break;
			default:
				break;
			}
			break;
		case WM_LBUTTONDOWN:
			wsprintf(str, "x coordinate is %d , y coordinate is %d \n", LOWORD(lParam), HIWORD(lParam));
			MessageBox(NULL, str, TEXT("WM_LBUTTONDOWN"), MB_OK);
			break;
		default:
			break;
	}

	return(DefWindowProc(hwnd, msg,wParam, lParam));
}

void ToggleFullscreen(void)
{
	MONITORINFO mi;
	if (bFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{

				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(TRUE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}




















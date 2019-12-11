#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

bool gbFullscreen = false;
int giCount;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HWND ghwnd;
RECT rect;
HBRUSH hBrush;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CmdLine, int ncmdShow)
{
	HWND hwnd;
	WNDCLASSEX wndclass;
	MSG iMsg;
	TCHAR szAppName[] = TEXT("myWindow");

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = wndProc;
	
	RegisterClassEx(&wndclass);
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("mywindow"), WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, 0, 0, hInstance, 0);
	ghwnd = hwnd;

	if (hwnd == NULL)
	{
		MessageBox(hwnd, TEXT("Error hwnd"), TEXT("error ahe bhavdya"), MB_OK);
		DestroyWindow(NULL);
		exit(EXIT_FAILURE);
	}

	ShowWindow(hwnd, ncmdShow);
	//UpdateWindow(hwnd);

	while(GetMessage(&iMsg, NULL,0,0))
	{
		TranslateMessage(&iMsg);
		DispatchMessage(&iMsg);
	}

	return((int)iMsg.wParam);
}


LRESULT CALLBACK wndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void ToggleFullscreen(void);

	PAINTSTRUCT ps;
	HDC hdc;
	

	switch (iMsg)
	{
		case WM_CREATE:
		
			break;
		case WM_KEYDOWN:
			switch (LOWORD(wParam))
			{
			case VK_ESCAPE:
				DestroyWindow(hwnd);
				break;
			case 0x46:
				if (gbFullscreen == false)
				{
					ToggleFullscreen();
					gbFullscreen = true;
				}
				else
				{
					ToggleFullscreen();
					gbFullscreen = false;
				}
				break;
			case 0x52:
				giCount = 1;
				InvalidateRect(hwnd, &rect, TRUE);
				break;
			case 0x47:
				giCount = 2;
				InvalidateRect(hwnd, &rect, TRUE);
				break;
			case 0x42:
				giCount = 3;
				InvalidateRect(hwnd, &rect, TRUE);
				break;	
			case 0x43:
				giCount = 4;
				InvalidateRect(hwnd, &rect, TRUE);
				break;
			case 0x59:
				giCount = 5;
				InvalidateRect(hwnd, &rect, TRUE);
				break;
			case 0x4D:
				giCount = 6;
				InvalidateRect(hwnd, &rect, TRUE);
				break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			if (giCount == 1)
			{
				hBrush = CreateSolidBrush(RGB(255, 0, 0));
			}
			else if(giCount == 2)
			{
				hBrush = CreateSolidBrush(RGB(0, 255, 0));
			}
			else if(giCount == 3)
			{
				hBrush = CreateSolidBrush(RGB(0, 0, 255));
			}
			else if (giCount == 4)
			{
				hBrush = CreateSolidBrush(RGB(0,255,255));
			}
			else if (giCount == 5)
			{
				hBrush = CreateSolidBrush(RGB(255, 255, 0));
			}
			else if (giCount == 6)
			{
				hBrush = CreateSolidBrush(RGB(255,0,255));
			}
			GetClientRect(hwnd, &rect);
			hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &rect, hBrush);
			EndPaint(hwnd, &ps);
			break;
		}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}




void ToggleFullscreen(void)
{
	MONITORINFO mi;
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{

				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
					
}








#include<Windows.h>
#include <stdio.h>
#include <stdlib.h>


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool gbFullScreen = false;
HWND ghwnd;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int ncmdshow)
{
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	RegisterClassEx(&wndclass);
	
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,szAppName,TEXT("mywindow"),WS_OVERLAPPEDWINDOW,0,0,800,600,0,0,hInstance,0);
	ghwnd = hwnd;

	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("Error to create window"), TEXT("Error"), MB_OK);
		exit(EXIT_FAILURE);
	}
	
	UpdateWindow(hwnd);
	ShowWindow(hwnd, ncmdshow);

	while (GetMessage(&msg,hwnd,0,0))
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
		case 0x46:
			if (gbFullScreen == false)
			{
				ToggleFullScreen();
				gbFullScreen = TRUE;
			}
			else
			{
				ToggleFullScreen();
				gbFullScreen = FALSE;
			}
			break;
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
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
	LPCTSTR lpszDeviceName;
	DWORD iModeNum;
	DEVMODE dm = { 0 };
	int change , i , flag = 0;
	dm.dmSize = sizeof(DEVMODE);
	DWORD dwStyle;
	WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPOS) };

	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
	if ( !(GetWindowPlacement(ghwnd, &wpPrev)))
	{
		MessageBox(ghwnd, TEXT("window placement retrieval failes"), TEXT("message"), MB_OK);
	}

	if (gbFullScreen == false)
	{
		
		i = 0;
		while ( (EnumDisplaySettings(NULL, i, &dm)) && flag == 0)
		{
			if (dm.dmPelsWidth == 800 && dm.dmPelsHeight == 600) //set it to current window dimension
			{
				flag = 1;
			}
			i++;
		}
		
		/*
		memset(&dm, 0, sizeof(dm));
		dm.dmSize = sizeof(dm);
		dm.dmPelsWidth =800;
		dm.dmPelsHeight =600;
		dm.dmBitsPerPel = 32;
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		*/
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		}
	
		if ((change = ChangeDisplaySettings(&dm, CDS_FULLSCREEN)) != DISP_CHANGE_SUCCESSFUL)
		{
			MessageBox(NULL, TEXT("display change failed"), TEXT("error"), MB_OK);
		}
		ShowCursor(FALSE);
	}
	else
	{	
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		while ((EnumDisplaySettings(NULL, i, &dm)) && flag == 0)
		{
			i++;
		}

		if ((change = ChangeDisplaySettings(NULL, 0)) != DISP_CHANGE_SUCCESSFUL)
		{
			MessageBox(NULL, TEXT("display change failed"), TEXT("Display Settings"), MB_OK);
		}
		ShowCursor(TRUE);
	}
}





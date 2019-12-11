#include <windows.h>
#include <gl/GL.h>
#include <math.h>
#include <gl\GLU.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"GLU32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.1415926535898


HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
int wPos = GetSystemMetrics(SM_CXFULLSCREEN) / 2;
int hPos = GetSystemMetrics(SM_CYFULLSCREEN) / 2;
GLfloat X = 1.0;
GLfloat Y = 1.0;
GLfloat X1 = 0.0;
GLfloat Y1 = 0.0;

static GLfloat xStep = 0.001;
static GLfloat yStep = 0.001;
static GLfloat xoffset;
GLsizei giParts = 1;

bool gbEscapeKeyIsPressed = false;
bool gbActiveWindow = false;
bool gbFullScreen = false;
bool gbAnimate = false;



LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLing, int nCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("myOGLApp");
	bool bDone = false;

	void initialize(void);
	void uninitialize(void);
	void display(void);
	void animate(void);

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = wndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("myOGLCircle1"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, wPos - (WIN_WIDTH / 2), hPos - (WIN_HEIGHT / 2), WIN_WIDTH, WIN_HEIGHT, 0, 0, hInstance, 0);

	ghwnd = hwnd;
	initialize();
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while (bDone == false)
	{
		while (PeekMessage(&msg, hwnd, 0, 0, PM_NOREMOVE) == TRUE)
		{
			if (GetMessage(&msg, hwnd, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				return TRUE;
			}
		}

		if (gbActiveWindow == true)
		{
			if (gbEscapeKeyIsPressed == true)
			{
				bDone = true;
			}
			animate();
			display();
		}
	}

	uninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	void display(void);
	void resize(int, int);
	void uninitialize(void);
	void ToggleFullScreen(HWND);


	switch (msg)
	{
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
			case 0x31:
				giParts = 1;
				break;
			case 0x32:
				giParts = 2;
				break;
			case 0x33:
				giParts = 3;
				break;
			case 0x34:
				giParts = 4;
				break;
			case 0x35:
				giParts = 5;
				break;
			case 0x36:
				giParts = 6;
				break;
			case 0x37:
				giParts = 7;
				break;
			
			case 0x41:
				if (gbAnimate == false)
				{
					gbAnimate = true;
				}
				else
				{
					gbAnimate = false;
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
			case VK_ESCAPE:
				gbEscapeKeyIsPressed = true;
				break;
			default:
				break;
		}
		break;
	case WM_CLOSE:
		uninitialize();
		break;
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
		{
			gbActiveWindow = true;
		}
		else
		{
			gbActiveWindow = false;
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

void ToggleFullScreen(HWND hwnd)
{
	MONITORINFO mi;

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(hwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };

			if (GetWindowPlacement(hwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_FRAMECHANGED);

			}
		}
		ShowCursor(TRUE);
	}
	else
	{
		SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &wpPrev);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE);
		ShowCursor(TRUE);
	}
}


void initialize(void)
{
	void resize(int, int);
	void ToggleFullScreen(HWND);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 0;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;

	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	xoffset = -1.0f;
	//handshake complete
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	resize(WIN_WIDTH, WIN_HEIGHT); //warmup call
}	

static int count = 0;

void display(void)
{
	GLclampf x, y, angle,a;
	GLsizei circle_points = 1000;
	GLfloat theta = 360.0;

	glClear(GL_COLOR_BUFFER_BIT);

	//Center circle
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -3.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i <= circle_points; i++)
	{
		angle = 2 * PI * i / circle_points;
		glVertex3f(1.135*cos(angle), 1.135*sin(angle), 0.0f);
		glVertex3f(1.135*cos(angle + 0.01), 1.135*sin(angle + 0.01),0.0f);
	}
	glEnd();
	//smaller circles

		theta = 0;

		glLoadIdentity();
		glColor3f(1.0f, 1.0f, 1.0f);
		glPushMatrix();
		
		glTranslatef((X)*(cos(theta)), Y*(sin(theta)), -3.0f);
		
		glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0.0f, 0.0f, 0.0f);

		for (int i = 0; i <= circle_points; i++)
		{
			angle = 2 * PI * i / circle_points;
			glVertex3f(cos(angle)/8, sin(angle)/8, 0.0f);
			glVertex3f(cos(angle + 0.01)/8, sin(angle + 0.01)/8, 0.0f);
		}
		glPopMatrix();
	
		glEnd();


		theta += 2 * PI / 7;
		glLoadIdentity();
		glColor3f(1.0f, 1.0f, 1.0f);
		glPushMatrix();

		glTranslatef((X)*(cos(theta)), Y*(sin(theta)), -3.0f);

		glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0.0f, 0.0f, 0.0f);

		for (int i = 0; i <= circle_points; i++)
		{
			angle = 2 * PI * i / circle_points;
			glVertex3f(cos(angle) / 8, sin(angle) / 8, 0.0f);
			glVertex3f(cos(angle + 0.01) / 8, sin(angle + 0.01) / 8, 0.0f);
		}
		glPopMatrix();

		glEnd();

		theta += 2 * PI / 7;
		glLoadIdentity();
		glColor3f(1.0f, 1.0f, 1.0f);
		glPushMatrix();

		glTranslatef((X)*(cos(theta)), Y*(sin(theta)), -3.0f);

		glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0.0f, 0.0f, 0.0f);

		for (int i = 0; i <= circle_points; i++)
		{
			angle = 2 * PI * i / circle_points;
			glVertex3f(cos(angle) / 8, sin(angle) / 8, 0.0f);
			glVertex3f(cos(angle + 0.01) / 8, sin(angle + 0.01) / 8, 0.0f);
		}
		glPopMatrix();

		glEnd();


	SwapBuffers(ghdc);
	

}

void animate(void)
{
	if (X > 1.0 || X < -1.0)
	{
		xStep = -xStep;
    }

	if (Y > 1.0 || Y < -1.0)
	{
		yStep = -yStep;
	}

	X += xStep;
	Y += yStep;

	
}


void uninitialize(void)
{
	if (gbFullScreen == true)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(ghrc);
	ghrc = NULL;
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;
	DestroyWindow(ghwnd);

}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}
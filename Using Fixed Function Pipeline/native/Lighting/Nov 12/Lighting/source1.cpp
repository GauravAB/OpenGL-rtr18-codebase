#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdlib.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

void trace(const TCHAR *msg, const TCHAR *msgFrom) { MessageBox(NULL, msg, msgFrom, MB_OK); }

//global variables
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
DWORD dwStyle = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
GLUquadric *quadric = NULL;

bool gbEscapeKeyIsPressed = false;
bool gbIsActive = false;
bool gbFullScreen = false;
bool gbIsLighting = false;
enum LightPosition {front,left,right,top,bottom};

LightPosition LP = front;

//global light arrays

GLfloat light_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat light_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_position1[] = { 0.0f,0.0f,2.0f,1.0f };
GLfloat light_position2[] = { 0.4f,0.5f,2.0f,1.0f };
GLfloat light_position3[] = { -0.4f,0.5f,2.0f,1.0f };
GLfloat light_position4[] = { 0.0f,0.5f,2.0f,1.0f };
GLfloat light_position5[] = { 0.0f,-0.5f,2.0f,1.0f };


GLfloat material_diffuse[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_ambient[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess[] = { 50.0f };

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	void initialize(void);
	void uninitialize(void);
	void display(void);


	HWND hwnd = NULL;
	TCHAR szAppName[] = { "My OGL window" };
	WNDCLASSEX wndclass;
	MSG msg;
	bool bDone = false;

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);
	
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("OGL lights"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
	
	if (hwnd == NULL)
	{
		trace(TEXT("hwnd creation failed"), TEXT("Initialize"));
		exit(1);
	}

	ghwnd = hwnd;
	
	initialize();
	

	ShowWindow(hwnd,SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	

	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbIsActive == true)
			{
				if (gbEscapeKeyIsPressed == true)
				{
					bDone = true;
				}
			}
			display();
		}
	}

	uninitialize();

	return(msg.wParam);

}

void initialize(void)
{
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormat;
	HDC hdc;

	pfd.nVersion = 0;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;

	ghdc = GetDC(ghwnd);

	iPixelFormat = ChoosePixelFormat(ghdc,&pfd);
	if (iPixelFormat == 0)
	{
		trace(TEXT("Choose Pixel Format failed"), TEXT("inside Initialize"));
		ReleaseDC(ghwnd, ghdc);
	}

	//SetPixelFormat returns BOOL return value
	if (SetPixelFormat(ghdc, iPixelFormat, &pfd) == FALSE)
	{
		trace(TEXT("SetPixel failed"), TEXT("Initialize"));
		ReleaseDC(ghwnd,ghdc);
	}


	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		trace(TEXT("CreateContext Failed"), TEXT("Initialize"));
		ReleaseDC(ghwnd, ghdc);
		wglDeleteContext(ghrc);
	}

	//wglMakeCurrent also returns BOOL
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		trace(TEXT("make current failed"), TEXT("Initialize"));
		ReleaseDC(ghwnd, ghdc);
		wglDeleteContext(ghrc);
	}
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);

	//glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	glEnable(GL_LIGHT0);

	resize(WIN_WIDTH, WIN_HEIGHT);
}


LRESULT CALLBACK WndProc(HWND hwnd , UINT iMsg , WPARAM wParam, LPARAM lParam)
{
	void ToggleFullScreen(void);
	void resize(int, int);

	switch (iMsg)
	{
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ACTIVATE:
		if(HIWORD(wParam) == 0)
		{ 
			gbIsActive = true;
		}
		else
		{
			gbIsActive = false;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x31:
			LP = front;
			break;
		case 0x32:
			LP = left;
			break;
		case 0x33:
			LP = right;
			break;
		case 0x34:
			LP = top;
			break;
		case 0x35:
			LP = bottom;
			break;
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;
		case 'f':
		case 'F':
			if (gbFullScreen == false)
			{
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;
		case 'l':
		case 'L':
			if (gbIsLighting == false)
			{
				glEnable(GL_LIGHTING);
				gbIsLighting = true;
			}
			else
			{
				glDisable(GL_LIGHTING);
				gbIsLighting = false;
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

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	switch (LP)
	{
	case front:
		glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
		break;
	case left:
		glLightfv(GL_LIGHT0, GL_POSITION, light_position2);
		break;
	case right:
		glLightfv(GL_LIGHT0, GL_POSITION, light_position3);
		break;
	case top:
		glLightfv(GL_LIGHT0, GL_POSITION, light_position4);
		break;
	case bottom:
		glLightfv(GL_LIGHT0, GL_POSITION, light_position5);
		break;
	default:
		break;
	}

	
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.75f, 1000, 1000);

	SwapBuffers(ghdc);
}

void ToggleFullScreen(void)
{
	MONITORINFO mi;

	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW)
	{
		mi = { sizeof(MONITORINFO) };
		if (GetWindowPlacement(ghwnd, &wpPrev), GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
		{
			SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,SWP_NOZORDER | SWP_FRAMECHANGED);
			
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	
	}
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

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);

}

void uninitialize(void)
{
	if (ghrc)
	{
		wglMakeCurrent(ghdc, ghrc);
	}
	wglDeleteContext(ghrc);

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
	}

	SetWindowPlacement(ghwnd, &wpPrev);
	SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
	SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	ShowCursor(TRUE);
}
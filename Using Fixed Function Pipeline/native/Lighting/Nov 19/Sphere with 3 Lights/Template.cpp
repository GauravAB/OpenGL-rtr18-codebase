#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdlib.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

void trace(const char*msg) { MessageBox(NULL, TEXT(msg), TEXT("error"), MB_OK); exit(1); }


//globals
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
DWORD dwStyle = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
GLUquadric *quadric = NULL;
GLfloat angle_red = 0.0f;
GLfloat angle_blue = 0.0f;
GLfloat angle_green = 0.0f;
//bools
bool gbToggleFullScreen = false;
bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;
bool gbIsLighting = false;
GLboolean gbSphere = GL_FALSE;

//light arrays
//light 0

GLfloat redLight_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat redLight_diffuse[] = { 1.0f,0.0f,0.0f,0.0f };//making it red
GLfloat redLight_specular[] = { 1.0f,0.0f,0.0f,0.0f };//highlight red
GLfloat redLight_position[] = { 0.0f,0.0f,0.0f,0.0f };//directional

//light 1
GLfloat greenLight_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat greenLight_diffuse[] = { 0.0f,1.0f,0.0f,0.0f };//making it green
GLfloat greenLight_specular[] = { 0.0f,1.0f,0.0f,0.0f };//highlight green
GLfloat greenLight_position[] = { 0.0f,0.0f,0.0f,0.0f };//directional

//light 2
GLfloat blueLight_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat blueLight_diffuse[] = { 0.0f,0.0f,1.0f,0.0f };//making it blue
GLfloat blueLight_specular[] = { 0.0f,0.0f,1.0f,0.0f };//highlight blue
GLfloat blueLight_position[] = { 0.0f,0.0f,0.0f,0.0f };//directional

//material set
GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess[] = { 20.0f };


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	void initialize(void);
	void display(void);
	void uninitialize(void);
	void update(void);


	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclass;
	TCHAR szAppName[] = TEXT("MyOGLApplication");
	bool bDone = false;


	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("myOGLWindow"), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		trace("hwnd Creation failed");
	}

	ghwnd = hwnd;

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	
	initialize();

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
			if (gbIsActiveWindow)
			{
				if (gbIsEscapeKeyPressed)
				{
					bDone = true;
				}
			}

			update();
			display();
		}
	}

	uninitialize();
	return(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void ToggleFullScreen(void);
	void resize(int, int);

	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbIsEscapeKeyPressed = true;
			break;
		case 'f':
		case 'F':
			if (gbToggleFullScreen == false)
			{
				ToggleFullScreen();
				gbToggleFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbToggleFullScreen = false;
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
		case 's':
		case 'S':
			if (gbSphere == GL_FALSE)
			{
				gbSphere = GL_TRUE;
			}
			else
			{
				gbSphere = GL_FALSE;
			}
			break;
		}
		break;
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
		{
			gbIsActiveWindow = true;
		}
		else
		{
			gbIsActiveWindow = false;
		}
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void initialize(void)
{
	void resize(int,int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormat;

	pfd.nVersion = 0;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;
	
	ghdc = GetDC(ghwnd);

	iPixelFormat = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormat == 0)
	{
		ReleaseDC(ghwnd, ghdc);
	}

	if (SetPixelFormat(ghdc,iPixelFormat, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
	}

	ghrc = wglCreateContext(ghdc);
	
	if (ghrc == NULL)
	{
		wglDeleteContext(ghrc);
		ReleaseDC(ghwnd, ghdc);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ReleaseDC(ghwnd, ghdc);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	quadric = gluNewQuadric();

	//light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, redLight_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, redLight_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR, redLight_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, redLight_position);
	glEnable(GL_LIGHT0);

	//light1
	glLightfv(GL_LIGHT1, GL_AMBIENT, greenLight_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, greenLight_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, greenLight_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, greenLight_position);
	glEnable(GL_LIGHT1);

	//light2
	glLightfv(GL_LIGHT2, GL_AMBIENT, blueLight_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, blueLight_specular);
	glLightfv(GL_LIGHT2, GL_POSITION, blueLight_position);
	glEnable(GL_LIGHT2);

	//material
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	resize(WIN_WIDTH, WIN_HEIGHT);
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


void ToggleFullScreen(void)
{
	MONITORINFO mi;

	if (gbToggleFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	
	}
}

void uninitialize(void)
{
	if (gbToggleFullScreen == true)
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (ghrc)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(ghrc);
	}
	ghrc = NULL;

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	gluLookAt(0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	glPushMatrix();
	glRotatef(angle_red, 1.0f, 0.0f, 0.0f);
	redLight_position[1] = angle_red;
	glLightfv(GL_LIGHT0, GL_POSITION, redLight_position);
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle_green, 0.0f, 1.0f, 0.0f);
	greenLight_position[0] = angle_green;
	glLightfv(GL_LIGHT1, GL_POSITION, greenLight_position);
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle_blue, 0.0f, 0.0f, 1.0f);
	blueLight_position[0] = angle_blue;
	glLightfv(GL_LIGHT2, GL_POSITION, blueLight_position);
	glPopMatrix();

   if (gbSphere)
	{
		glTranslatef(0.0f, 0.0f, -3.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		gluSphere(quadric, 0.75f, 1000, 1000);
	}
	glPopMatrix();
	
	SwapBuffers(ghdc);
}

void update(void)
{
	if (angle_red >= 360.0f)
	{
		angle_red = 0.0f;
	}

	if (angle_green >= 360.0f)
	{
		angle_green = 0.0f;
	}

	if (angle_blue <= 0.0f)
	{
		angle_blue = 360.0f;
	}

	angle_red += 1.0f;
	angle_blue -= 1.0f;
	angle_green += 0.5f;

}






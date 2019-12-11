



#include <windows.h>
#include <gl/GL.h>
#include <math.h>
#include <gl\GLU.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"GLU32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 800
#define PI 3.1415926535898

#define X_FACTOR(i) i ## x_factor  
#define Y_FACTOR(i) i ## y_factor 

#define X_MINUS(i) i ## x_minus
#define Y_MINUS(i) i ## y_minus

#define X(i) i ## x
#define Y(i) i ## y

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
int wPos = GetSystemMetrics(SM_CXFULLSCREEN) / 2;
int hPos = GetSystemMetrics(SM_CYFULLSCREEN) / 2;
GLfloat z_5 = 0.0f, x_1 = 0.0f, y_1 = 0.0f, x_2 = 0.0f, y_2 = 0.0f, x_3 = 0.0f, y_3 = 0.0f, x_4 = 0.0f, y_4 = 0.0f, y_5 = 0.0f, x_5 = 0.0f, y_6 = 0.0f, x_6 = 0.0f, y_7 = 0.0f, x_7 = 0.0f, x_8 = 0.0f, y_8 = 0.0f;

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
	//void animate(void);

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
			display();
			//animate();
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
	//handshake complete
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//ToggleFullScreen(ghwnd);
	//resize(WIN_WIDTH, WIN_HEIGHT); //warmup call
}


void display(void)
{
	GLclampf x, y, angle, a;
	GLsizei circle_points = 1000;

	glClear(GL_COLOR_BUFFER_BIT);
	//BIG RED CIRCLE
	glLoadIdentity();
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i <= circle_points; i++)
	{
		angle = 2 * PI * i / circle_points;
		glVertex3f(cos(angle) / 2, sin(angle) / 2, 0.0f);
		glVertex3f(cos(angle + 0.01) / 2, sin(angle + 0.01) / 2, 0.0f);
	}
	glEnd();

	//CIRCLE 1
	//static int i = 1;


	GLsizei theta = 12;
	static int j = 0;
	for (int i = 0; i < 1; i++)
	{
		static int k = 1;
		static GLclampf x_minus[5] = { 0 }, y_minus[5] = { 0 };
		glLoadIdentity();
		angle = 2 * PI * i / theta;
		static GLclampf x[5], y[5];
		x[i] = (-k)*cos(angle) / 2;
		y[i] = (-k)*sin(angle) / 2;
		glTranslatef(x[i] + x_minus[i], y[i] + y_minus[i], 0.0f);
		//	glTranslatef(x_1, y_1, 0.0f);
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 1.0f, 1.0f);
		
		for (int i = 0; i <= circle_points; i++)
		{
			angle = 2 * PI * i / circle_points;
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(cos(angle) / 25, sin(angle) / 25, 0.0f);
			glVertex3f(cos(angle + 0.1) / 25, sin(angle + 0.1) / 25, 0.0f);
		}
		glEnd();
		/*
		static GLclampf x_factor[5] = { 0 }, y_factor[5] = { 0 };
		if (x_factor[i] == 0)
		{
			x_factor[i] = -(2 * x[i]) / 500;
		}
		if (y_factor[i] == 0)
		{
			y_factor[i] = -(2 * y[i]) / 500;
		}

		if (i < j)
		{
			x_minus[i] += x_factor[i];
			y_minus[i] += y_factor[i];

			if (x_minus[i] < -2 * x[i])
			{

				x_factor[i] = -x_factor[i];
				y_factor[i] = -y_factor[i];
			}
			if (x_minus[i] >= 0)
			{
				x_factor[i] = -x_factor[i];
				y_factor[i] = -y_factor[i];
			}
		}
		
	}
	if (j < 6)
	{
		j++;
	}*/
	Sleep(10);
	SwapBuffers(ghdc);
	return;
}
//	//i++;
//	//CIRCLE 2
//
//	glLoadIdentity();
////	glTranslatef(0.15f, 0.35f, 0.0f);
//	glTranslatef(x_5, y_5, z_5);
//	glBegin(GL_TRIANGLES);
//	glColor3f(1.0f, 1.0f, 0.0f);
//	for (int i = 0; i <= circle_points; i++)
//	{
//		a = 2 * PI * i / circle_points;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(cos(a) / 25, sin(a) / 25, 0.0f);
//		glVertex3f(cos(a + 0.1) / 25, sin(a + 0.1) / 25, 0.0f);
//	}
//	glEnd();
//
//	//CIRCLE 3
//	glLoadIdentity();
//	glTranslatef(0.18f, 0.18f, 0.0f);
//	glTranslatef(x_3, y_3, 0.0f);
//	glBegin(GL_TRIANGLES);
//	glColor3f(1.0f, 1.0f, 1.0f);
//	for (int i = 0; i <= circle_points; i++)
//	{
//		a = 2 * PI * i / circle_points;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(cos(a) / 25, sin(a) / 25, 0.0f);
//		glVertex3f(cos(a + 0.1) / 25, sin(a + 0.1) / 25, 0.0f);
//	}
//	glEnd();
//
//	//CIRCLE 4
//	glLoadIdentity();
//	glTranslatef(0.06f, 0.025f, 0.0f);
//	//glTranslatef(x_4, y_4, 0.0f);
//	glBegin(GL_TRIANGLES);
//	glColor3f(1.0f, 1.0f, 1.0f);
//	for (int i = 0; i <= circle_points; i++)
//	{
//		a = 2 * PI * i / circle_points;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(cos(a) / 25, sin(a) / 25, 0.0f);
//		glVertex3f(cos(a + 0.1) / 25, sin(a + 0.1) / 25, 0.0f);
//	}
//	glEnd();
//
//
//	//CIRCLE 5
//	glLoadIdentity();
//	glTranslatef(-0.13f, 0.0f, 0.0f);
//	glTranslatef(x_2, y_2, 0.0f);
//	glBegin(GL_TRIANGLES);
//	glColor3f(1.0f, 1.0f, 1.0f);
//	for (int i = 0; i <= circle_points; i++)
//	{
//		a = 2 * PI * i / circle_points;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(cos(a) / 25, sin(a) / 25, 0.0f);
//		glVertex3f(cos(a + 0.1) / 25, sin(a + 0.1) / 25, 0.0f);
//	}
//	glEnd();
//
//	//CIRCLE 6
//	glLoadIdentity();
//	glTranslatef(-0.27f, 0.12f, 0.0f);
//	//glTranslatef(x_4, y_4, 0.0f);
//	glBegin(GL_TRIANGLES);
//	glColor3f(1.0f, 1.0f, 1.0f);
//	for (int i = 0; i <= circle_points; i++)
//	{
//		a = 2 * PI * i / circle_points;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(cos(a) / 25, sin(a) / 25, 0.0f);
//		glVertex3f(cos(a + 0.1) / 25, sin(a + 0.1) / 25, 0.0f);
//	}
//	glEnd();
//
//	//CIRCLE 7
//	glLoadIdentity();
//	glTranslatef(-0.29f, 0.29f, 0.0f);
//	glTranslatef(x_4, y_4, 0.0f);
//	glBegin(GL_TRIANGLES);
//	glColor3f(1.0f, 1.0f, 1.0f);
//	for (int i = 0; i <= circle_points; i++)
//	{
//		a = 2 * PI * i / circle_points;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(cos(a) / 25, sin(a) / 25, 0.0f);
//		glVertex3f(cos(a + 0.1) / 25, sin(a + 0.1) / 25, 0.0f);
//	}
//	glEnd();
//
//
//	//CIRCLE 8
//	glLoadIdentity();
//	glTranslatef(-0.175f, 0.425f, 0.0f);
//	//glTranslatef(x_4, y_4, 0.0f);
//	glBegin(GL_TRIANGLES);
//	glColor3f(1.0f, 1.0f, 1.0f);
//	for (int i = 0; i <= circle_points; i++)
//	{
//		a = 2 * PI * i / circle_points;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(cos(a) / 25, sin(a) / 25, 0.0f);
//		glVertex3f(cos(a + 0.1) / 25, sin(a + 0.1) / 25, 0.0f);
//	}
//	glEnd();
//
//
//	glLoadIdentity();
//	glColor3f(0.0f, 0.0f, 0.0f);
//	glBegin(GL_LINES);
//	glVertex3f(0.0f, 1.0f, 0.0f);
//	glVertex3f(0.0f, -1.0f, 0.0f);
//
//	/*glVertex3f(0.1f, 1.0f, 0.0f);
//	glVertex3f(0.1f, -1.0f, 0.0f);
//	glVertex3f(0.2f, 1.0f, 0.0f);
//	glVertex3f(0.2f, -1.0f, 0.0f);
//	glVertex3f(0.3f, 1.0f, 0.0f);
//	glVertex3f(0.3f, -1.0f, 0.0f);
//	glVertex3f(0.4f, 1.0f, 0.0f);
//	glVertex3f(0.4f, -1.0f, 0.0f);
//	glVertex3f(0.5f, 1.0f, 0.0f);
//	glVertex3f(0.5f, -1.0f, 0.0f);
//	glVertex3f(-0.1f, 1.0f, 0.0f);
//	glVertex3f(-0.1f, -1.0f, 0.0f);
//	glVertex3f(-0.2f, 1.0f, 0.0f);
//	glVertex3f(-0.2f, -1.0f, 0.0f);
//	glVertex3f(-0.3f, 1.0f, 0.0f);
//	glVertex3f(-0.3f, -1.0f, 0.0f);
//	glVertex3f(-0.4f, 1.0f, 0.0f);
//	glVertex3f(-0.4f, -1.0f, 0.0f);
//	glVertex3f(-0.5f, 1.0f, 0.0f);
//	glVertex3f(-0.5f, -1.0f, 0.0f);
//	*/
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glVertex3f(-1.0f, 0.0f, 0.0f);
//	/*glVertex3f(1.0f, 0.1f, 0.0f);
//	glVertex3f(-1.0f, 0.1f, 0.0f);
//	glVertex3f(1.0f, 0.2f, 0.0f);
//	glVertex3f(-1.0f, 0.2f, 0.0f);
//	glVertex3f(1.0f, 0.3f, 0.0f);
//	glVertex3f(-1.0f, 0.3f, 0.0f);
//	glVertex3f(1.0f, 0.4f, 0.0f);
//	glVertex3f(-1.0f, 0.4f, 0.0f);
//	glVertex3f(1.0f, 0.5f, 0.0f);
//	glVertex3f(-1.0f, 0.5f, 0.0f);
//
//	glVertex3f(1.0f, -0.1f, 0.0f);
//	glVertex3f(-1.0f, -0.1f, 0.0f);
//	glVertex3f(1.0f, -0.2f, 0.0f);
//	glVertex3f(-1.0f, -0.2f, 0.0f);
//	glVertex3f(1.0f, -0.3f, 0.0f);
//	glVertex3f(-1.0f, -0.3f, 0.0f);
//	glVertex3f(1.0f, -0.4f, 0.0f);
//	glVertex3f(-1.0f, -0.4f, 0.0f);
//	glVertex3f(1.0f, -0.5f, 0.0f);
//	glVertex3f(-1.0f, -0.5f, 0.0f);
//	*/
//	glVertex3f(1.0f, 1.0f, 0.0f);
//	glVertex3f(-1.0f, -1.0f, 0.0f);
//
//	glVertex3f(-1.0f, 1.0f, 0.0f);
//	glVertex3f(1.0f, -1.0f, 0.0f);
//
//	glVertex3f(-cos((22.5 *PI) / 180), sin((22.5 * PI) / 180), 0.0f);
//	glVertex3f(cos((22.5 * PI) / 180), -sin((22.5* PI) / 180), 0.0f);
//
//	glVertex3f(-sin((22.5 *PI) / 180), cos((22.5 * PI) / 180), 0.0f);
//	glVertex3f(sin((22.5 * PI) / 180), -cos((22.5* PI) / 180), 0.0f);
//
//	glVertex3f(cos((22.5 *PI) / 180), sin((22.5 * PI) / 180), 0.0f);
//	glVertex3f(-cos((22.5 * PI) / 180), -sin((22.5* PI) / 180), 0.0f);
//
//	glVertex3f(sin((22.5 *PI) / 180), cos((22.5 * PI) / 180), 0.0f);
//	glVertex3f(-sin((22.5 * PI) / 180), -cos((22.5* PI) / 180), 0.0f);
//
//	//	glVertex3f(-sin(22.5), cos(22.5), 0.0f);
//	//glVertex3f(sin(22.5), -cos(22.5), 0.0f);
//
//	glEnd();
//
//	SwapBuffers(ghdc);
//}
//void animate(void)
//{
//
//	static bool down = true;
//	static bool up = false;
//	static bool right1 = true;
//	static bool left1 = false;
//	static bool across1_down = true;
//	static bool across1_up = false;
//	static bool across2_down = true;
//	static bool across2_up = false;
//	static bool across3_down = true;
//	static bool across3_up = false;
//	static bool across4_down = true;
//	static bool across4_up = false;
//	static bool across5_down = true;
//	static bool across5_up = false;
//	static bool across6_down = true;
//	static bool across6_up = false;
//
//
//	/*
//	if (gbAnimate == true)
//	{
//	if (y_1 >= -0.5)
//	{
//	while (y_1 >= -0.5)
//	{
//	y_1 -= 0.00000001f;
//	}
//	}
//	else
//	{
//	while (y_1 <= 0.5)
//	{
//	y_1 += 0.00000001f;
//	}
//	}
//	}
//	*/
//
//	//down routine
//	if (down == true && y_1 >= -0.89)
//	{
//		y_1 = y_1 - 0.001;
//	}
//	else
//	{
//		down = false;
//		up = true;
//	}
//
//	//up routine
//	if (up == true && y_1 <= 0.03)
//	{
//		y_1 = y_1 + 0.001;
//	}
//	else
//	{
//		up = false;
//		down = true;
//	}
//
//	//  right routine
//	if (right1 == true && x_2 <= 0.59)
//	{
//		x_2 = x_2 + 0.001;
//	}
//	else
//	{
//		right1 = false;
//		left1 = true;
//	}
//
//	//left routine
//	if (left1 == true && x_2 >= -0.33)
//	{
//		x_2 = x_2 - 0.001;
//	}
//	else
//	{
//		left1 = false;
//		right1 = true;
//	}
//
//	//across1_down routine
//	if (across1_down == true && x_3 >= -0.51 && y_3 > -0.51)
//	{
//		x_3 = x_3 - 0.001;
//		y_3 = y_3 - 0.001;
//	}
//	else
//	{
//		across1_down = false;
//		across1_up = true;
//	}
//
//	//across1_up routine
//	if (across1_up == true && x_3 <= 0.15 && y_3 <= 0.15)
//	{
//		x_3 = x_3 + 0.001;
//		y_3 = y_3 + 0.001;
//	}
//	else
//	{
//		across1_up = false;
//		across1_down = true;
//	}
//
//
//	//across2_down routine
//	if (across2_down == true && x_4 <= 0.62 && y_4 >= -0.62)
//	{
//		x_4 = x_4 + 0.001;
//		y_4 = y_4 - 0.001;
//	}
//	else
//	{
//		across2_down = false;
//		across2_up = true;
//	}
//
//	//across2_up routine
//	if (across2_up == true && x_4 >= -0.04 && y_4 <= 0.04)
//	{
//		x_4 = x_4 - 0.001;
//		y_4 = y_4 + 0.001;
//	}
//	else
//	{
//		across2_up = false;
//		across2_down = true;
//	}
//
//	//across3_down routine
//	if (across3_down == true && x_5 >= -0.35 && y_5 >= -0.35)
//	{
//		x_5 = x_5 - 0.001;
//		y_5 = y_5 - 0.001;
//	}
//	else
//	{
//		across3_down = false;
//		across3_up = true;
//	}
//
//	//across3_up routine
//	if (across3_up == true && x_5 <= 0.0 && y_5 <= 0.0)
//	{
//		x_5 = x_5 + 0.001;
//		y_5 = y_5 + 0.001;
//	}
//	else
//	{
//		across3_up = false;
//		across3_down = true;
//	}
//
//}
//

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
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

}

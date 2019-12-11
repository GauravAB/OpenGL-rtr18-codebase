#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdlib.h>

#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 800

//globals
HWND ghwnd;
HDC ghdc;
HGLRC ghrc;
DWORD dwStyle = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
GLUquadric *quadric = NULL;
GLfloat angle_rotate = 0.0f;
static unsigned int iterator;

//bools
bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;
bool gbFullScreen = false;
bool gbIslighting = false;
bool gbNext = false;
bool gbPrev = false;

//enums
enum display_mode {multi_sphere ,single_sphere};
enum light_pos_enum {alongX , alongY , alongZ ,default_pos};

light_pos_enum LP = alongX;
display_mode mode = multi_sphere;

//tracer 
void trace(const char *msg, const char *fromMsg) { MessageBox(NULL, TEXT(msg), TEXT(fromMsg), MB_OK); }

//global light arrays

GLfloat light_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat light_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_position[] = { 1.0f,1.0f,1.0f,0.0f };
GLfloat light_model_ambient[] = { 0.2f,0.2f,0.2f,0.0f };
GLfloat light_model_local_viewer[] = { 0.0f };

typedef GLfloat material_property[4];

material_property m_ambient[24] = { { 0.0215f,0.1745f,0.0215f,1.0f },
									{0.135f,0.2225f,0.1575f,1.0f},
									{0.05375f,0.05f,0.06625f,1.0f},
									{0.25f,0.20725f,0.20725f,1.0f},
									{0.1745f,0.01175f,0.01175f,1.0f} ,
									{0.1f,0.18725f,0.1745f,1.0f},
									{0.329412f,0.223529f,0.27451f,1.0f} ,
									{0.2125f,0.1275f,0.054f,1.0f} ,
									{0.25f,0.25f,0.25f,1.0f},
									{0.19125f,0.0735f,0.0225f,1.0f},
									{0.24725f,0.1995f,0.0745f,1.0f},
									{0.19225f,0.19225f,0.19225f,1.0f},
									{0.0f,0.0f,0.0f,1.0f},
									{0.0f,0.1f,0.06f,1.0f},
									{0.0f,0.0f,0.0f,1.0f},
									{0.0f,0.0f,0.0f,1.0f},
									{0.0f,0.0f,0.0f,1.0f},
									{0.0f,0.0f,0.0f,1.0f},
									{0.02f,0.02f,0.02f,1.0f},
									{0.0f,0.05f,0.05f,1.0f},
									{0.0f,0.05f,0.0f,1.0f},
									{0.05f,0.0f,0.0f,1.0f},
									{0.05f,0.05f,0.05f,1.0f},
									{0.05f,0.05f,0.0f,1.0f}
				};

material_property m_diffuse[24] = { { 0.07568f,0.61424f,0.07568f,1.0f },
									{0.54f,0.89f,0.63f,1.0f},
									{0.18275f,0.17f,0.22525f,1.0f},
									{1.0f,0.829f,0.829f,1.0f},
									{0.61424f,0.04136f,0.04136f,1.0f},
									{0.396f,0.74151f,0.69102f,1.0f} ,
									{0.780392f,0.568627f,0.113725f,1.0f} ,
									{0.714f,0.4284f,0.18144f,1.0f} ,
									{0.4f,0.4f,0.4f,1.0f},
									{0.7038f,0.27048f,0.0828f,1.0f},
									{0.75164f,0.60648f,0.22648f,1.0f},
									{0.50745f,0.50745f,0.50745f,1.0f},
									{0.01f,0.01f,0.01f,1.0f},
									{0.0f,0.50980392f,0.50980392f,1.0f},
									{0.1f,0.35f,0.1f,1.0f},
									{0.5f,0.0f,0.0f,1.0f},
									{0.55f,0.55f,0.55f,1.0f},
									{0.5f,0.5f,0.0f,1.0f},
									{0.01f,0.01f,0.01f,1.0f},
									{0.4f,0.5f,0.5f,1.0f},
									{0.4f,0.5f,0.4f,1.0f},
									{0.5f,0.4f,0.4f,1.0f},
									{0.5f,0.5f,0.5f,1.0f},
									{0.5f,0.5f,0.4f,1.0f}																											
};


material_property m_specular[24] = { {0.633f,0.727811f,0.633f,1.0f},
									{0.316228f,0.316228f,0.316228f,1.0f},
									{0.332741f,0.328634f,0.346435f,1.0f},
									{0.296648f,0.296648f,0.296648f,1.0f},
									{0.727811f,0.626959f,0.626959f,1.0f},
									{0.297254f,0.30829f,0.306678f,1.0f},
									{0.992157f,0.941176f,0.807843f,1.0f},
									{0.393548f,0.271906f,0.166721f,1.0f} ,
									{0.774597f,0.774597f,0.774597f,1.0f},
									{0.256777f,0.137622f,0.086014f,1.0f},
									{0.628281f,0.555802f,0.366065f,1.0f},
									{0.508273f,0.508273f ,0.508273f ,1.0f},
									{0.50f,0.50f,0.50f,1.0f},
									{0.50196078f,0.50196078f,0.50196078f,1.0f},
									{0.45f,0.55f,0.45f,1.0f},
									{0.7f,0.6f,0.6f,1.0f},
									{0.70f,0.70f,0.70f,1.0f},
									{0.60f,0.60f,0.50f,1.0f},
									{0.4f,0.4f,0.4f,1.0f},
									{0.04f,0.7f,0.7f,1.0f},
									{0.04f,0.7f,0.04f,1.0f},
									{0.7f,0.04f,0.04f,1.0f},
									{0.7f,0.7f,0.7f,1.0f},
									{0.7f,0.7f,0.04f,1.0f}
};


material_property m_shininess[24] = { {0.6f * 128 },{1.0f * 128},{0.3f * 128},{0.088f * 128} ,{0.6f * 128},{0.1f * 128},{0.21794872f * 128},{0.2f * 128} ,{0.6f * 128} , {0.1f * 128} , {0.4f * 128} , {0.4f * 128} ,{0.25f * 128} , {0.25f * 128} ,{0.25f * 128} , {0.25f * 128} ,{0.25f * 128} , {0.25f * 128} ,{0.078125f * 128} , {0.078125f * 128} , {0.78125f * 128} , {0.078125f * 128} ,{0.078125f * 128} ,{0.78125f * 128} };


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	
	void initialize(void);
	void display(void);
	void uninitialize(void);
	void update(void);

	//locals
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclass;
	TCHAR szAppName[] = TEXT("myOGLapplication");
	bool bDone = false;

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("mySixthAttempt"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		trace("hwnd creation failed", "from Main");
		exit(1);
	}
	ghwnd = hwnd;

	initialize();

	ShowWindow(hwnd, SW_SHOW);
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
	void resize(int, int);
	void ToggleFullScreen(void);

	switch (iMsg)
	{
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
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
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbIsEscapeKeyPressed = true;
			break;
		case 'n':
		case 'N':
			gbNext = true;
			break;
		case 'p':
		case 'P':
			gbPrev = true;
			break;
		case 'm':
		case 'M':
			if (mode == multi_sphere)
			{
				mode = single_sphere;
			}
			else
			{
				mode = multi_sphere;
			}
			break;
		case 'x':
		case 'X':
			LP = alongX;
			break;
		case 'y':
		case 'Y':
			LP = alongY;
			break;
		case 'z':
		case 'Z':
			LP = alongZ;
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
			if (gbIslighting == false)
			{
				glEnable(GL_LIGHTING);
				gbIslighting = true;
			}
			else
			{
				glDisable(GL_LIGHTING);
				gbIslighting = false;
			}
			break;
		default:
			LP = default_pos;
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;

	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}







void initialize(void)
{
	void resize(int, int);
	//void setup_materials();

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormat;

	pfd.nVersion = 0;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_SUPPORT_OPENGL;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;

	ghdc = GetDC(ghwnd);
	iPixelFormat = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormat == 0)
	{
		ReleaseDC(ghwnd, ghdc);
	}

	if (SetPixelFormat(ghdc, iPixelFormat, &pfd) == FALSE)
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
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(ghrc);
		ReleaseDC(ghwnd, ghdc);
	}

	quadric = gluNewQuadric();

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	//new commands for light normals
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	//lights
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viewer);
	glEnable(GL_LIGHT0);

	iterator = 0;
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


		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 200.0f);

}

void update()
{
	angle_rotate += 0.1f;

	if (angle_rotate >= 360.0f)
	{
		angle_rotate = 0.0f;
		iterator+=4;
	}

}

void display(void)
{
	GLfloat tX, tY;
	static GLsizei count;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (mode)
	{
		case multi_sphere:

			glPushMatrix();

			switch (LP)
			{
			case alongX:
				glRotatef(angle_rotate, 1.0f, 0.0f, 0.0f);

				light_position[0] = 0.0f;
				light_position[1] = angle_rotate;
				light_position[2] = 0.0f;
				light_position[3] = 1.0f;

				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glPopMatrix();
				break;
			case alongY:
				glRotatef(angle_rotate, 0.0f, 1.0f, 0.0f);
				light_position[0] = angle_rotate;
				light_position[1] = 0.0f;
				light_position[2] = 0.0f;
				light_position[3] = 1.0f;

				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glPopMatrix();
				break;
			case alongZ:
				glRotatef(angle_rotate, 0.0f, 0.0f, 1.0f);
				light_position[0] = angle_rotate;
				light_position[1] = 0.0f;
				light_position[2] = 0.0f;
				light_position[3] = 1.0f;

				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glPopMatrix();
				break;
			default:
				light_position[0] = 0.0f;
				light_position[1] = 0.0f;
				light_position[2] = 0.0f;
				light_position[3] = 1.0f;

				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glPopMatrix();

				break;
			}

			count = 0;

			for (tY = 5.0f; tY >= -5.0f; tY -= 2.0f)
			{

				for (tX = -6.0f; tX <= 6.0f; tX += 4.0f)
				{
					glPushMatrix();

					glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient[count]);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse[count]);
					glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular[count]);
					glMaterialfv(GL_FRONT, GL_SHININESS, m_shininess[count]);

					glTranslatef(tX, tY, -15.0f);
					glPolygonMode(GL_FRONT, GL_FILL);

					gluSphere(quadric, 0.85f, 30, 30);

					glPopMatrix();
					count = count + 1;
				}
			}
			break;
			
		case single_sphere:
			glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
				if (gbNext)
				{
					if (iterator >= 24)
					{
						iterator = 0;
					}
					iterator++;
					gbNext = false;
				}
				else if (gbPrev)
				{
					if (iterator <= 0)
					{
						iterator = 0;
					}
					iterator--;
					gbPrev = false;
				}

				glPushMatrix();
				light_position[0] = 0.5;
				light_position[1] = 1.5;
				light_position[2] = 1.0;
				
				glPushMatrix();

				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient[iterator]);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse[iterator]);
				glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular[iterator]);
				glMaterialfv(GL_FRONT, GL_SHININESS, m_shininess[iterator]);

				glRotatef(angle_rotate, 0.0f, 1.0f, 0.0f);
				glTranslatef(0.0f, 0.0f, -3.0f);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				gluSphere(quadric, 0.85f, 1000, 1000);
				glPopMatrix();

				glPushMatrix();

				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient[iterator+1]);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse[iterator+1]);
				glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular[iterator+1]);
				glMaterialfv(GL_FRONT, GL_SHININESS, m_shininess[iterator+1]);

				glRotatef(angle_rotate+90, 0.0f, 1.0f, 0.0f);
				glTranslatef(0.0f, 0.0f, -3.0f);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				gluSphere(quadric, 0.85f, 1000, 1000);
				glPopMatrix();

				glPushMatrix();

				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient[iterator+5]);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse[iterator+5]);
				glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular[iterator+5]);
				glMaterialfv(GL_FRONT, GL_SHININESS, m_shininess[iterator+5]);

				glRotatef(angle_rotate+180,0.0f, 1.0f, 0.0f);
				glTranslatef(0.0f, 0.0f, -3.0f);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				gluSphere(quadric, 0.85f, 1000, 1000);
				glPopMatrix();

				glPushMatrix();

				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient[iterator + 10]);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse[iterator + 10]);
				glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular[iterator + 10]);
				glMaterialfv(GL_FRONT, GL_SHININESS, m_shininess[iterator +10]);

				glRotatef(angle_rotate + 270, 0.0f, 1.0f, 0.0f);
				glTranslatef(0.0f, 0.0f, -3.0f);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				gluSphere(quadric, 0.85f, 1000, 1000);
				glPopMatrix();

				glPopMatrix();
		

			break;
			
		default:
			break;
			
		}
		
		
	
	SwapBuffers(ghdc);

}


void uninitialize(void)
{
	if (gbFullScreen == true)
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}
	wglDeleteContext(ghrc);

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
	}
}

void ToggleFullScreen(void)
{
	MONITORINFO mi;

	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

	if (dwStyle & WS_OVERLAPPEDWINDOW)
	{
		mi = { sizeof(MONITORINFO) };

		if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
		{
			SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
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










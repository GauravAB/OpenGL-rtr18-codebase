#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/GL.h>

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

enum
{
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

FILE *gpFile = NULL;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;


DWORD dwStyle ;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};


bool gbIsEscapeKeyIsPressed = false;
bool gbIsActiveWindow = false;
bool gbFullScreen = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;


int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR szCmdline , int nCmdShow)
{
	void initialize(void);
	void display(void);
	void uninitialize(void);


	HWND hwnd ;
	MSG msg;
	TCHAR szAppName[] = TEXT("programmable pipeline boiler");
	WNDCLASSEX wndclass ;

	bool bDone = false;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,szAppName,TEXT("OGL pp boiler"),WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,0,0, WIN_WIDTH ,WIN_HEIGHT,NULL,NULL,hInstance,NULL);


	if(hwnd == NULL)
		{
			MessageBox(NULL,TEXT("message failed"),TEXT("error window"),MB_OK);
			exit(0);
		}

	ghwnd = hwnd;
	initialize();

	ShowWindow(hwnd,SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


	while(bDone == false)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.wParam == WM_QUIT)
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
			if(gbIsActiveWindow == true)
			{
				if(gbIsEscapeKeyIsPressed == true)
				{
					bDone = true;
				}
			}
			display();
		}
	}

	uninitialize();
	return (msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int,int);
	void ToggleFullScreen(HWND hwnd);


	switch(iMsg)
	{
		case WM_KEYDOWN:
			switch(wParam)
			{
				case 'f':
				case 'F':
					if(gbFullScreen == false)
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
					gbIsEscapeKeyIsPressed = true;
					break;
				default:
					break;
			}
			break;
		case WM_SIZE:
			resize(LOWORD(lParam),LOWORD(wParam));
			break;
		case WM_ACTIVATE:
			if(HIWORD(wParam) == 0)
			{
				gbIsActiveWindow = true;
			}
			else
			{
				gbIsActiveWindow = false;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}

void ToggleFullScreen(HWND hwnd)
{
	MONITORINFO mi;

	if(gbFullScreen == false)
	{

		dwStyle = GetWindowLong(hwnd , GWL_STYLE);

		if(dwStyle & WS_OVERLAPPEDWINDOW)
		{	
			mi = {sizeof(MONITORINFO)};

			if(GetWindowPlacement(hwnd,&wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi))
			{
				SetWindowLong(hwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,mi.rcMonitor.right - mi.rcMonitor.left,mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);

			}
		}

		ShowCursor(TRUE);
	}
	else
	{
		SetWindowLong(hwnd,GWL_STYLE , dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd,&wpPrev);
		SetWindowPos(hwnd,HWND_TOP,0,0,0,0,SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

}

void initialize(void)
{
	void resize(int,int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormat;

	pfd.nVersion = 1;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 64;

	ghdc = GetDC(ghwnd);

	iPixelFormat = ChoosePixelFormat(ghdc,&pfd);

	if(iPixelFormat == 0)
	{
		ReleaseDC(ghwnd,ghdc);
		MessageBox(NULL,TEXT("iPixelFormat is 0"),TEXT("error"),MB_OK);
	}

	if(SetPixelFormat(ghdc,iPixelFormat,&pfd) == NULL)
	{
		ReleaseDC(ghwnd,ghdc);
		exit(1);
	}

	ghrc = wglCreateContext(ghdc);

	if(ghrc == NULL)
	{
		wglDeleteContext(ghrc);
		ReleaseDC(ghwnd,ghdc);
		exit(1);		
	}

	if((wglMakeCurrent(ghdc,ghrc)) == FALSE)
	{
		ReleaseDC(ghwnd,ghdc);
		wglDeleteContext(ghrc);
		exit(1);	
	}

	glewInit();


	// vertex shader

	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertexShaderSourceCode =
	"void main(void)"\
	"{"\
	"{";

	//attach source code to object file
	glShaderSource(gVertexShaderObject,1,(const char**)&vertexShaderSourceCode,NULL);

	glCompileShader(gVertexShaderObject);


	//fragment shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//provide source code to shader
	const GLchar* fragmentShaderSourceCode = 
	"void main(void)"\
	"{"\
	"}";

	//attach source code to the object file
	glShaderSource(gFragmentShaderObject,1, (const GLchar **)&fragmentShaderSourceCode,NULL);

	glCompileShader(gFragmentShaderObject);


	//shader program
	gShaderProgramObject = glCreateProgram();
	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObject,gVertexShaderObject);
	///attach fragment shader to the shader program
	glAttachShader(gShaderProgramObject,gFragmentShaderObject);
	//linking phase
	glBindAttribLocation(gShaderProgramObject,VDG_ATTRIBUTE_VERTEX,"vPosition");


	glLinkProgram(gShaderProgramObject);		//linking program at once
	


	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glShadeModel(GL_SMOOTH);

	resize(WIN_WIDTH,WIN_HEIGHT);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//start using openGL program
	glUseProgram(gShaderProgramObject);
	glUseProgram(0);

	SwapBuffers(ghdc);
}	

void resize(int width , int height)
{
	if(height == 0)
	{
		height = 1;
	}

	glViewport(0,0,(GLsizei)width,(GLsizei)height);
}

void uninitialize(void)
{
	if(gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	//detach shader from shader program object
	glDetachShader(gShaderProgramObject,gVertexShaderObject);
	glDetachShader(gShaderProgramObject,gFragmentShaderObject);

	//delete vertex shader object

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;


	//delete shader program object
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	//unlink shader Program
	glUseProgram(0);

	//deselect rendering context
	wglDeleteContext(ghrc);
	ghrc = NULL
	;

	//delete device context
	ReleaseDC(ghwnd,ghdc);
	ghdc = NULL;


	if(gpFile)
	{
		fprintf(gpFile,"Log file is successfully closed\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}















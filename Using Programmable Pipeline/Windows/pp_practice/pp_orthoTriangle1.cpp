#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include "vmath.h"


#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

enum
{
	GAB_ATTRIBUTE_VERTEX = 0,
	GAB_ATTRIBUTE_COLOR,
	GAB_ATTRIBUTE_NORMAL,
	GAB_ATTRIBUTE_TEXTURE0,
};


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

FILE *gpFile = NULL;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;


DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };


bool gbIsEscapeKeyIsPressed = false;
bool gbIsActiveWindow = false;
bool gbFullScreen = false;

unsigned int gVertexShaderObject;
unsigned int gFragmentShaderObject;
unsigned int gProgramShaderObject;

unsigned int gVbo;
unsigned int gVao;
unsigned int gMVPUniform;

mat4 OrthographicProjectionMatrix;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdline, int iCmdShow)
{
	void initialize(void);
	void display(void);
	void uninitialize(void);


	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("programmable pipeline boiler");
	WNDCLASSEX wndclass;
	bool bDone = false;


	//create log file 
	if ((fopen_s(&gpFile, "Log.txt", "w")) != 0)
	{
		MessageBox(NULL, TEXT("File creation failed"), TEXT("error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file opened succesfully \n");
	}


	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("OGL pp boiler"), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);


	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("message failed"), TEXT("error window"), MB_OK);
		exit(0);
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
			if (gbIsActiveWindow == true)
			{
				if (gbIsEscapeKeyIsPressed == true)
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
	void resize(int, int);
	void ToggleFullScreen(HWND hwnd);
	void unitialize(void);

	//variable declarations
	static WORD xMouse = NULL;
	static WORD yMouse = NULL;

	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'f':
		case 'F':
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
			gbIsEscapeKeyIsPressed = true;
			break;
		default:
			break;
		}
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), LOWORD(wParam));
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
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
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
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);

			}
		}

		ShowCursor(TRUE);
	}
	else
	{
		SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &wpPrev);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

}

void initialize(void)
{

	void resize(int, int);
	void uninitialize(void);

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

	iPixelFormat = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormat == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		MessageBox(NULL, TEXT("iPixelFormat is 0"), TEXT("error"), MB_OK);
	}

	if (SetPixelFormat(ghdc, iPixelFormat, &pfd) == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		exit(1);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		wglDeleteContext(ghrc);
		ReleaseDC(ghwnd, ghdc);
		exit(1);
	}

	if ((wglMakeCurrent(ghdc, ghrc)) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		wglDeleteContext(ghrc);
		exit(1);
	}

	GLenum glew_error = glewInit();
	{
		if (glew_error != GLEW_OK)
		{

			wglDeleteContext(ghrc);
			ghrc = NULL;
			ReleaseDC(ghwnd, ghdc);
			ghdc = NULL;
		}

	}

	//******VERTEX SHADER*******
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar* vertexShaderSourceCode =
		"#version 440 core"\
		"\n"\
		"in vec4 vPosition;"
		"\n"\
		"uniform mat4 u_mvp_matrix;"\
		"\n"\
		"void main(void)"\
		"{"\
		"glPosition = u_mvp_matrix * vPosition;"\
		"}";
	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);
	//ERROR CHECK
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);

	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//******Fragment Shader*******
	
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCode =
		"#version 440 core"\
		"\n"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = vec4(1.0,1.0,1.0,1.0);"\
		"}";
	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(gFragmentShaderObject);
	//ERROR CHECK
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);

	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//*************SHADER PROGRAM**************
	gProgramShaderObject = glCreateProgram();
	
	glAttachShader(gProgramShaderObject, gVertexShaderObject);
	glAttachShader(gProgramShaderObject, gFragmentShaderObject);


	// data layout specification for vertex shader
	glBindAttribLocation(gProgramShaderObject, GAB_ATTRIBUTE_VERTEX,"vPosition");

	//linking shaders
	glLinkProgram(gProgramShaderObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);

	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength >0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program link status: %s\n", szInfoLog);
				free();
				uninitialize();
				exit(0);
			}
		}
	}

	gMVPUniform = glGetUniformLocation(gProgramShaderObject, "g_mvp_matrix");

	//create some data to input 

	const GLfloat triangleVertices[] = { 0.0f,50.0f,0.0f,-50.0f,-50.0f,0.0f,50.0f,-50.0f,0.0f };

	glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao);

	glGenBuffers(1, &gVbo);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangleVertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE,0,NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);

	//unbind calls
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);


	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(gProgramShaderObject);
	mat4 modelViewMatrix = mat4::identity;
	mat4 modelViewProjectionMatrix = mat4::identity;
	

	modelViewProjectionMatrix = OrthographicProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE,modelViewProjectionMatrix)

	SwapBuffers(ghdc);
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	OrthographicProjectionMatrix = ortho(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);
}

void uninitialize(void)
{
	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	//deselect rendering context
	wglDeleteContext(ghrc);
	ghrc = NULL;

	//delete device context
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;


}















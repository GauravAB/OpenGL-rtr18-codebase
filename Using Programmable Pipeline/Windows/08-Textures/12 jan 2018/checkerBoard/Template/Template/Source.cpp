#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
//always put glew above gl header
#include <gl/glew.h>
#include<gl/GL.h>
#include "vmath.h"
#include "resource.h"


#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
DWORD dwStyle = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
FILE *gpFile = NULL;

//global bools
bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;
bool gbFullScreen = false;


//texture ID
GLuint gCheckerTex;

//shader stuff

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao;
GLuint gVbo_pos;
GLuint gVbo_texture;

GLuint gTextureSamplerUniform;
GLuint gMVPMatrix;

//texture
static const GLint checkerImageWidth = 64;
static const GLint checkerImageHeight = 64;

GLubyte checkImage[checkerImageHeight][checkerImageWidth][4];

enum
{
	GAB_ATTRUBUTE_POSITION = 0,
	GAB_ATTRIBUTE_COLOR,
	GAB_ATTRIBUTE_NORMAL,
	GAB_ATTRIBUTE_TEXTURE0,
};


mat4 gPerspectiveProjectionMatrix;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
	void initialize(void);
	void uninitialize(void);
	void display(void);


	WNDCLASSEX wndclass;
	MSG msg;
	HWND hwnd;
	TCHAR szAppName[] = TEXT("OGL 2D-Texture Map");


	int widthMax, heightMax , hPos , vPos;
	bool bDone = false;


	heightMax = GetSystemMetrics(SM_CXSCREEN);
	widthMax = GetSystemMetrics(SM_CYSCREEN);

	hPos = widthMax / 2;
	vPos = heightMax / 2;


	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("OpenGL 2D Texture Mapping"),WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,0,0,WIN_WIDTH,WIN_HEIGHT,NULL,NULL,hInstance,NULL);
	
	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("CreateWindow failed"), TEXT("Error window"), MB_OK);
		exit(0);
	}

	ghwnd = hwnd;
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//game loop

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
				if (gbIsEscapeKeyPressed == true)
				{
					bDone = true;
				}
			}

			display();
		}
	}

	uninitialize();

	return( (int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd , UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void ToggleFullScreen(HWND hwnd);
	void resize(int width, int height);
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
				gbIsActiveWindow == true;
			}
		break;
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
				gbIsEscapeKeyPressed = true;
				break;
			default:
				break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void initialize(void)
{

	void resize(int width, int height);
	void initShader(GLuint& vertexShaderObject, GLuint& fragmentShaderObject, GLuint& shaderProgramObject);
	void LoadGLTextures(GLuint* texture);

	int err = fopen_s(&gpFile, "log.txt", "w");

	if (err != 0)
	{
		MessageBox(NULL, TEXT("failed to create file"), TEXT("error"), MB_OK);
		exit(1);
	}
	else
	{
		fprintf(gpFile, "Log File Created Success\n");
	}



	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormat;

	pfd.nVersion = 0;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;


	ghdc = GetDC(ghwnd);

	iPixelFormat = ChoosePixelFormat(ghdc, &pfd);


	if ((SetPixelFormat(ghdc, iPixelFormat, &pfd)) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		MessageBox(NULL, TEXT("HandShake failed"), TEXT("Initialize error"), MB_OK);
		exit(1);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		ghrc = NULL;
		MessageBox(NULL, TEXT("HandShake failed"), TEXT("Initialize error"), MB_OK);
		exit(1);
	}

	if ((wglMakeCurrent(ghdc, ghrc)) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		wglDeleteContext(ghrc);
		ghdc = NULL;
		ghrc = NULL;
		MessageBox(NULL, TEXT("HandShake failed"), TEXT("Initialize error"), MB_OK);
		exit(1);
	}

	GLenum error_no;

	error_no = glewInit();

	if (error_no != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed\n");

	}

	const GLubyte *version;

	version = glGetString(GL_VERSION);
	fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));

	
	//Create Shader
	initShader(gVertexShaderObject, gFragmentShaderObject, gShaderProgramObject);
	
	
	const float square_tex_coords[] =
	{
		1.0f,1.0f,
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,
	};


	glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao);

	glGenBuffers(1, &gVbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(GAB_ATTRUBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRUBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_tex_coords), square_tex_coords, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	LoadGLTextures(&gCheckerTex);
	glEnable(GL_TEXTURE);

	gPerspectiveProjectionMatrix = mat4::identity();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	resize(WIN_WIDTH, WIN_HEIGHT);
}


void LoadGLTextures(GLuint *texture)
{
	//glVertex3f()

	int i, j, c;
	
	for (i = 0; i<checkerImageHeight; i++)
	{
		for (j = 0; j < checkerImageWidth; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = 255;
		}
	}

	
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenBuffers(1,texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkerImageWidth, checkerImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
}




void initShader(GLuint &gVertexShaderObject, GLuint &gFragmentShaderObject, GLuint	&gShaderProgramObject)
{
	void uninitialize(void);

	GLint iInfoLogLength;
	GLint iCompileStatus;

	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertexshadercode =
		"#version 460 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec2 vTex0coords;"\
		"out vec2 fTex0coords;"\
		"uniform mat4 uMVPMatrix;"\
		"void main()"\
		"{"\
		"fTex0coords = vTex0coords;"\
		"gl_Position = uMVPMatrix * vPosition;"\
		"}";

	glShaderSource(gVertexShaderObject, 1,(GLchar**)&vertexshadercode, NULL);
	
	glCompileShader(gVertexShaderObject);
	char *szErrorString = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szErrorString = (char*)malloc(sizeof(iInfoLogLength));
			
			GLsizei written;
			glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written,szErrorString);
			fprintf(gpFile, "Vertex shader compile error: %s\n", szErrorString);
			free(szErrorString);
			uninitialize();
			exit(0);
		}
	}



	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const char* fragmentshadercode =
		"#version 460 core"\
		"\n"\
		"in vec2 fTex0coords;"\
		"out vec4 FragColor;"\
		"uniform sampler2D u_texture0_sampler;"\
		"void main()"\
		"{"\
		"FragColor = texture(u_texture0_sampler,fTex0coords);"\
		"}";


	glShaderSource(gFragmentShaderObject, 1, (GLchar**)&fragmentshadercode, NULL);

	glCompileShader(gFragmentShaderObject);
    szErrorString = NULL;
	iCompileStatus = 0;
	iInfoLogLength = 0;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iCompileStatus);

	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			szErrorString = (char*)malloc(sizeof(iInfoLogLength));

			glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szErrorString);
			fprintf(gpFile, "Fragment shader compile error: %s\n", szErrorString);
			free(szErrorString);
			uninitialize();
			exit(0);
		}
	}


	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	glBindAttribLocation(gShaderProgramObject, GAB_ATTRUBUTE_POSITION,"vPosition");
	glBindAttribLocation(gShaderProgramObject, GAB_ATTRIBUTE_TEXTURE0, "vTex0coords");

	glLinkProgram(gShaderProgramObject);
	szErrorString = NULL;
	GLint iLinkStatus = 0;
	iInfoLogLength = 0;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iLinkStatus);

	if (iLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			szErrorString = (char*)malloc(sizeof(iInfoLogLength));
			glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szErrorString);
			fprintf(gpFile, "link error: %s\n", szErrorString);
			free(szErrorString);
			uninitialize();
			exit(0);
		}
	}

	gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");
	gMVPMatrix = glGetUniformLocation(gShaderProgramObject, "uMVPMatrix");

}


void ToggleFullScreen(HWND hwnd)
{
	MONITORINFO mi;

	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

	if (gbFullScreen == false)
	{
		if (dwStyle && WS_OVERLAPPEDWINDOW)
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
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	modelViewMatrix = translate(0.0f, 0.0f, -3.6f);
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(gMVPMatrix, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gCheckerTex);
	glUniform1i(gTextureSamplerUniform, 0);

	float square[12];

	square[0] = -2.0f;
	square[1] = -1.0f;
	square[2] =  0.0f;
	square[3] = -2.0f;
	square[4] =  1.0f;
	square[5] =  0.0f;
	square[6] =  0.0f;
	square[7] =  1.0f;
	square[8] =  0.0f;
	square[9] =  0.0f;
	square[10] = -1.0f;
	square[11] =  0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_DYNAMIC_DRAW);
	glBindVertexArray(gVao);	

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);



	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	modelViewMatrix = translate(0.0f, 0.0f, -3.6f);
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(gMVPMatrix, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gCheckerTex);
	glUniform1i(gTextureSamplerUniform, 0);

	square[0] = 1.0f;
	square[1] = -1.0f;
	square[2] = 0.0f;
	square[3] = 1.0f;
	square[4] = 1.0f;
	square[5] = 0.0f;
	square[6] = 2.41421f;
	square[7] = 1.0f;
	square[8] = -1.41421f;
	square[9] = 2.41421f;
	square[10] = -1.0f;
	square[11] = -1.41421f;

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_DYNAMIC_DRAW);
	glBindVertexArray(gVao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);



	glUseProgram(0);

	SwapBuffers(ghdc);
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	if (width >= height)
	{
		gPerspectiveProjectionMatrix = perspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 30.0f);
	}
	else
	{
		gPerspectiveProjectionMatrix = perspective(60.0f,(GLfloat)height /(GLfloat)width, 1.0f, 30.0f);
	}
}



void uninitialize(void)
{
	if (gbFullScreen)
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (ghrc)
	{
		wglMakeCurrent(ghdc, NULL);
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gVertexShaderObject)
	{
		glDetachShader(gShaderProgramObject, gVertexShaderObject);
		glDeleteShader(gVertexShaderObject);
		gVertexShaderObject = 0;
	}

	if (gFragmentShaderObject)
	{
		glDetachShader(gShaderProgramObject, gFragmentShaderObject);
		glDeleteShader(gFragmentShaderObject);
		gFragmentShaderObject = 0;
	}

	glDeleteProgram(gShaderProgramObject);

	glUseProgram(0);
	
	if (gVbo_pos)
	{
		glDeleteBuffers(GL_ARRAY_BUFFER, &gVbo_pos);
		gVbo_pos = 0;
	}


	if (gVbo_texture)
	{
		glDeleteBuffers(GL_ARRAY_BUFFER, &gVbo_texture);
		gVbo_texture = 0;
	}

}


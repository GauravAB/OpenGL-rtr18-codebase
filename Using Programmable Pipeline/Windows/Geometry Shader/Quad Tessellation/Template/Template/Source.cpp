#include <Windows.h>
#include <stdio.h>
#include "glFiles.h"
#include <stdlib.h>
#include "vmath.h"
#include "setShader.h"


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glu32.lib")

using namespace vmath;

//global vars
HWND ghwnd;
HDC ghdc;
HGLRC ghrc;
FILE *gpFile = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;
bool gbFullScreen = false;

enum
{
	GAB_ATTRIBUTE_VERTEX = 0,
	GAB_ATTRIBUTE_COLOR,
	GAB_ATTRIBUTE_NORMAL ,
	GAB_ATTRIBUTE_TEXTURE0,
};

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao;
GLuint gVbo;
GLuint gVeo;
GLuint gModelUniform;
GLuint gProjectionUniform;
GLuint gResolution;
GLuint gTimer;
//for tessellation shader
GLuint gTessellationControlShaderObject;
GLuint gTessellationEvaluationShaderObject;
GLuint gGeometryShaderObject;
GLuint gNumberOfSegmentsUniform;
GLuint gNumberOfStripsUniform;
GLuint gLineColorUniform;
GLuint gLightPositionUniform;
GLuint gAmbientMaterialUniform;
GLuint gDiffuseMaterialUniform;

unsigned int gNumberOfLineSegments;
unsigned int gNumberOfStrips;
unsigned int gNumElements;



mat4 gPerspectiveProjectionMatrix;
vec2 resolutionUniform;
float timerUniform;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	void initialize(void);
	void uninitialize(void);
	void resize(int, int);
	void display(void);

	bool bDone = false;
	HWND hwnd;
	WNDCLASSEX wndclass;
	MSG msg;
	TCHAR szAppName[] = TEXT("Perspective Triangle");

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("OGL window"), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("Failed to create window"), TEXT("error"), MB_OK);
		return 1;
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
				if (gbIsEscapeKeyPressed == true)
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
	void uninitialize(void);
	int LoadGLTextures(GLuint *texture, TCHAR imageResourceId[]);
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormat;
	int err = fopen_s(&gpFile, "log.txt", "w");
	if (err != 0)
	{
		MessageBox(NULL, TEXT("file open failed"), TEXT("error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "File Created Successfully");
	}


	//set required pfd
	pfd.nVersion = 0;
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
		MessageBox(NULL, TEXT("iPixelFormat is 0"), TEXT("Error"), MB_OK);
		exit(1);
	}

	if (SetPixelFormat(ghdc, iPixelFormat, &pfd) == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		MessageBox(NULL, TEXT("SetPixelFormat failed"), TEXT("Error"), MB_OK);
		exit(1);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		wglDeleteContext(ghrc);
		ReleaseDC(ghwnd, ghdc);
		MessageBox(NULL, TEXT("Failed to Create Context"), TEXT("Error"), MB_OK);
		exit(1);
	}

	if ((wglMakeCurrent(ghdc, ghrc)) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		wglDeleteContext(ghrc);
		exit(1);
	}

	GLenum err_no = glewInit();
	if (err_no != GLEW_OK)
	{
		fprintf(gpFile, "Error: %s\n", glewGetErrorString(err));
		fclose(gpFile);
		exit(0);
	}
	const GLubyte *version, *glslVersion, *vendor;

	version = glGetString(GL_VERSION);
	vendor = glGetString(GL_VENDOR);
	glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	fprintf(gpFile, "OpenGL vendor: %s\n", vendor);
	fprintf(gpFile, "OpenGL version: %s\n", version);
	fprintf(gpFile, "GLSLVersion: %s\n", glslVersion);

	//Create vertex shader
	gVertexShaderObject = setShader("vertex", "vertexShader.glsl",gpFile);	
	//Create fragment shader
	gFragmentShaderObject = setShader("fragment", "fragmentShader.glsl",gpFile);
	//Create tessellationControlShader
	gTessellationControlShaderObject = setShader("tessControl", "tessellationControlShader.glsl", gpFile);

	//Create tessellationEvaluationShader
	gTessellationEvaluationShaderObject = setShader("tessEvaluation", "tessellationEvaluationShader.glsl", gpFile);
	
	//create GeometryShaderObject
	gGeometryShaderObject = setShader("geometry", "geometryShader.glsl", gpFile);

	gShaderProgramObject = glCreateProgram();
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	glAttachShader(gShaderProgramObject, gTessellationControlShaderObject);
	glAttachShader(gShaderProgramObject, gTessellationEvaluationShaderObject);
	glAttachShader(gShaderProgramObject, gGeometryShaderObject);

	//prelinking vertex data layout 
	glBindAttribLocation(gShaderProgramObject, GAB_ATTRIBUTE_VERTEX, "vPosition");
	glLinkProgram(gShaderProgramObject);
	GLint iShaderLinkStatus;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderLinkStatus);
	GLint iInfoLogLength;
	char *szInfoLog;
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "program link error log : %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gModelUniform = glGetUniformLocation(gShaderProgramObject, "u_modelview_matrix");
	gProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	gResolution = glGetUniformLocation(gShaderProgramObject, "uResolution");
	gTimer = glGetUniformLocation(gShaderProgramObject, "uTimer");
	gNumberOfSegmentsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfSegments");
	gNumberOfStripsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfStrips");

	gLightPositionUniform = glGetUniformLocation(gShaderProgramObject, "lightPosition");
	gAmbientMaterialUniform = glGetUniformLocation(gShaderProgramObject, "ambientMaterial");
	gDiffuseMaterialUniform = glGetUniformLocation(gShaderProgramObject, "diffuseMaterial");



	//data array
		const unsigned int Faces[] = {
			2, 1, 0,
			3, 2, 0,
			4, 3, 0,
			5, 4, 0,
			1, 5, 0,
			11, 6,  7,
			11, 7,  8,
			11, 8,  9,
			11, 9,  10,
			11, 10, 6,
			1, 2, 6,
			2, 3, 7,
			3, 4, 8,
			4, 5, 9,
			5, 1, 10,
			2,  7, 6,
			3,  8, 7,
			4,  9, 8,
			5, 10, 9,
			1, 6, 10 
		};

		const float Verts[] = {
			0.000f,  0.000f,  1.000f,
			0.894f,  0.000f,  0.447f,
			0.276f,  0.851f,  0.447f,
			-0.724f,  0.526f,  0.447f,
			-0.724f, -0.526f,  0.447f,
			0.276f, -0.851f,  0.447f,
			0.724f,  0.526f, -0.447f,
			-0.276f,  0.851f, -0.447f,
			-0.894f,  0.000f, -0.447f,
			-0.276f, -0.851f, -0.447f,
			0.724f, -0.526f, -0.447f,
			0.000f,  0.000f, -1.000f 
		};


		gNumElements = sizeof(Faces) / sizeof(Faces[0]);


	//create vertex buffer array
	glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao);

	glGenBuffers(1, &gVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo);

	//copy the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Verts),Verts, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVeo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVeo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glBindVertexArray(0);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	gPerspectiveProjectionMatrix = mat4::identity();
	gNumberOfLineSegments = 1;
	gNumberOfStrips = 1;


	resize(WIN_WIDTH, WIN_HEIGHT);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);
	void ToggleFullScreen(void);

	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RIGHT:
			gNumberOfStrips++;
			if (gNumberOfStrips >= 50)
			{
				gNumberOfStrips = 50;
			}
			break;
		case VK_LEFT:
			gNumberOfStrips--;
			if (gNumberOfStrips <= 0)
			{
				gNumberOfStrips = 1;
			}
			break;
		case VK_UP:
			gNumberOfLineSegments++;
			if (gNumberOfLineSegments >= 50)
			{
				gNumberOfLineSegments = 50;
			}
			break;
		case VK_DOWN:
			gNumberOfLineSegments--;
			if (gNumberOfLineSegments <= 0)
			{
				gNumberOfLineSegments = 1;
			}
			break;

		case 0x46:
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
		case VK_ESCAPE:
			gbIsEscapeKeyPressed = true;
			break;
		default:
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
	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void display(void)

{
	mat3 getUpper3x3(mat4);

	TCHAR str[255];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);

	timerUniform += 0.02;


	mat4 modelViewMatrix = mat4::identity();
	mat3 normalMatrix = mat3::identity();

	modelViewMatrix = modelViewMatrix * translate(0.0f, 0.0f, -3.0f);
	
	
	glUniformMatrix4fv(gModelUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(gProjectionUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	glUniform2fv(gResolution, 1, resolutionUniform);
	glUniform1f(gTimer, timerUniform);
	glUniform1i(gNumberOfSegmentsUniform, gNumberOfLineSegments);
	glUniform1i(gNumberOfStripsUniform,gNumberOfStrips);
	glUniform3f(gAmbientMaterialUniform, 0.04f, 0.04f, 0.04f);
	glUniform3f(gDiffuseMaterialUniform, 0.0f, 0.75f, 0.75f);
	glUniform3f(gLightPositionUniform, 0.25f, 0.25f, 1.0f);

	glBindVertexArray(gVao);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVeo);
	glDrawElements(GL_PATCHES, gNumElements, GL_UNSIGNED_INT, 0);
	wsprintf(str, TEXT("OpenGL Programmable pipeline window [Segments  %d Strips %d]"), gNumberOfLineSegments,gNumberOfStrips);
	SetWindowText(ghwnd, str);
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

	resolutionUniform = vec2((GLfloat)width,(GLfloat)height);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	if (width >= height)
	{

		gPerspectiveProjectionMatrix = perspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	}
	else
	{
		gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)height / (GLfloat)width, 0.1f, 100.0f);
	}
}



void ToggleFullScreen(void)
{
	MONITORINFO mi;

	if (gbFullScreen == false)
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
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void uninitialize(void)
{

	if (gbFullScreen)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (gVao)
	{
		glDeleteVertexArrays(1, &gVao);
		gVao = 0;
	}

	if (gVbo)
	{
		glDeleteBuffers(1,&gVbo);
		gVbo = 0;
	}

	glDetachShader(gShaderProgramObject, gVertexShaderObject);

	glDetachShader(gShaderProgramObject, gFragmentShaderObject);
	glDetachShader(gShaderProgramObject, gTessellationControlShaderObject);
	glDeleteShader(gTessellationControlShaderObject);
	glDetachShader(gShaderProgramObject, gTessellationEvaluationShaderObject);
	glDeleteShader(gTessellationEvaluationShaderObject);


	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;


	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	glUseProgram(0);



	if (ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}
	wglDeleteContext(ghrc);
	ghrc = NULL;

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "File Closed Successfully");
		fclose(gpFile);
	}
}


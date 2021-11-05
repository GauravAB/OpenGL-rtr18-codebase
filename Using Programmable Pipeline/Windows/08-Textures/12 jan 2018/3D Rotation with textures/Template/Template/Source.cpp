#include <Windows.h>
#include <stdio.h>
#include <gl\glew.h>
#include <gl\GL.h>
#include <stdlib.h>
#include "vmath.h"
#include "resource.h"


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
GLfloat angleCube = 0.0f;
GLfloat anglePyramid = 0.0f;

enum
{
	GAB_ATTRIBUTE_VERTEX = 0,
	GAB_ATTRIBUTE_COLOR,
	GAB_ATTRIBUTE_NORMAL,
	GAB_ATTRIBUTE_TEXTURE0,
};

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVaoPyramid;
GLuint gVaoCube;
GLuint gVboCubePos;
GLuint gVboCubeTexture;
GLuint gVboPyramidPos;
GLuint gVboPyramidTexture;
GLuint gMVPUniform;
GLuint gTexture_sampler_uniform;

//IDs for textures
GLuint gStoneTexture;
GLuint gKundaliTexture;


mat4 gPerspectiveProjectionMatrix;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	void initialize(void);
	void uninitialize(void);
	void resize(int, int);
	void display(void);
	void update(void);
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
			update();
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
	int LoadGLTextures(GLuint *texture, TCHAR imageResourceID[]);



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
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode =
		"#version 460 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec4 vColor;"\
		"in vec2 vTexture0_Coord;"\
		"out vec2 out_texture0_coord;"\
		"uniform mat4 u_mvp_matrix;"\
		"void main(void)"\
		"{"\
		"gl_Position = u_mvp_matrix * vPosition;"\
		"out_texture0_coord = vTexture0_Coord;"\
		"}";

	glShaderSource(gVertexShaderObject, 1, (GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(gVertexShaderObject);
	GLint iCompileStatus;
	GLint iInfoLogLength;
	char *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Vertex Shader compile error Log : %s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}

	//Create fragment shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode =
		"#version 460 core"\
		"\n"\
		"out vec4 FragColor;"\
		"in vec2 out_texture0_coord;"\
		"uniform sampler2D u_texture0_sampler;"\
		"void main(void)"\
		"{"\
		"FragColor = texture(u_texture0_sampler,out_texture0_coord);"\
		"}";
	glShaderSource(gFragmentShaderObject, 1, (GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(gFragmentShaderObject);

	iCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Fragment Shader compile error Log : %s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}

	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//prelinking vertex data layout 
	glBindAttribLocation(gShaderProgramObject, GAB_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, GAB_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(gShaderProgramObject, GAB_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");

	glLinkProgram(gShaderProgramObject);
	GLint iShaderLinkStatus;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderLinkStatus);

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

	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");
	//data array
	const GLfloat pyramidVertices[] =
	{
		//front side
		0.0f,0.5f,0.0f,
		-0.5f,-0.5f,0.5f,
		0.5f,-0.5f,0.5f,
		//right side
		0.0f,0.5f,0.0f,
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		//back side
		0.0f,0.5f,0.0f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		//left side
		0.0f,0.5f,0.0f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f

	};
	const GLfloat pyramidTexCoords[] =
	{
		0.5,1.0,
		0.0,0.0,
		1.0f,0.0,

		0.5,1.0,
		1.0,0.0,
		0.0,0.0,
		
		0.5,1.0,
		1.0,0.0,
		0.0,0.0,
		
		0.5,1.0,
		0.0,0.0,
		1.0,0.0
	};



	 GLfloat cubeVertices[] =
	{
		1.0f,1.0f,-1.0f,
		-1.0f,1.0f,-1.0f,
		-1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,

		1.0,-1.0f,1.0f,
		-1.0f,-1.0f,1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,

		1.0f,1.0f,1.0f,
		-1.0f,1.0f,1.0f,
		-1.0f,-1.0f,1.0f,
		1.0f,-1.0f,1.0f,

		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,1.0f,-1.0f,
		1.0f,1.0f,-1.0f,

		-1.0f,1.0f,1.0f,
		-1.0f,1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,1.0f,

		1.0f,1.0f,-1.0f,
		1.0f,1.0f,1.0f,
		1.0f,-1.0f,1.0f,
		1.0f,-1.0f,-1.0f,
	};

	const GLfloat cubeTexcoords[] =
	{
		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,
		
		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,
	};

	for (int i = 0; i < 72; i++)
	{
		if (cubeVertices[i] > 0.0f)
		{
			cubeVertices[i] = cubeVertices[i]-0.25f;
		}
		else if (cubeVertices[i] < 0.0f)
		{
			cubeVertices[i] = cubeVertices[i] + 0.25f;
		}
		else
		{
			cubeVertices[i] = cubeVertices[i];
		}
	}



	//------------------PYRAMID VAO START-------------------------------------------
	//create vertex buffer array
	glGenVertexArrays(1, &gVaoPyramid);
	glBindVertexArray(gVaoPyramid);

	glGenBuffers(1, &gVboPyramidPos);
	glBindBuffer(GL_ARRAY_BUFFER, gVboPyramidPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVboPyramidTexture);
	glBindBuffer(GL_ARRAY_BUFFER, gVboPyramidTexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexCoords), pyramidTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//-------------------------------TRIANGLE VAO END---------------------------------------------------

	//-------------------------------CUBE VAO START---------------------------------------------------

	glGenVertexArrays(1, &gVaoCube);
	glBindVertexArray(gVaoCube);

	glGenBuffers(1, &gVboCubePos);
	glBindBuffer(GL_ARRAY_BUFFER, gVboCubePos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVboCubeTexture);
	glBindBuffer(GL_ARRAY_BUFFER, gVboCubeTexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//-------------------------------CUBE VAO END---------------------------------------------------



	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	gPerspectiveProjectionMatrix = mat4::identity();
	
	LoadGLTextures(&gStoneTexture, MAKEINTRESOURCE(MYBITMAP2));
	LoadGLTextures(&gKundaliTexture, MAKEINTRESOURCE(MYBITMAP1));

	glEnable(GL_TEXTURE_2D);


	resize(WIN_WIDTH, WIN_HEIGHT);
}

int LoadGLTextures(GLuint *texture, TCHAR imageResourceID[])
{
	int iStatus = -1;
	HBITMAP hBitmap;
	BITMAP bmp;

	//load the texture resource in memory
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	
	//if loading success
	if (hBitmap)
	{
		iStatus = 0;
		//get the details extracted from the image in memory
		GetObject(hBitmap, sizeof(bmp), &bmp);

		//generate an ID for a memory section on GPU memory
		glGenTextures(1, texture);

		//Bind to that memory
		glBindTexture(GL_TEXTURE_2D, *texture);

		//set texture mapping settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
		//specify a 2D texture image to the memory bound

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);

		//create mipmaps for this texture for better image quality
		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);	//delete unwanted bitmap

		return (iStatus);		
	}
	
	
	return iStatus;
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
void update(void)
{
	anglePyramid += 1.0f;
	if (anglePyramid >= 360.0f)
	{
		anglePyramid -= 360.0f;
	}

	angleCube += 1.0f;
	if (angleCube >= 360.0f)
	{
		angleCube -= 360.0f;
	}
}

void display(void)

{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);

	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	modelViewMatrix = modelViewMatrix * translate(-0.9f, 0.0f, -3.5f);
	modelViewMatrix = modelViewMatrix * rotate(anglePyramid,0.0f, 1.0f, 0.0f);

	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;
	
	

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	//bind with pyramid textures
	glActiveTexture(GL_TEXTURE0); //0th texture corresponds to GAB_ATTRIBUTE_TEXTURE0
	glBindTexture(GL_TEXTURE_2D, gStoneTexture);
	glUniform1i(gTexture_sampler_uniform, 0);
	 
	glBindVertexArray(gVaoPyramid);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 12);

	glBindVertexArray(0);



	 modelViewMatrix = mat4::identity();
	 modelViewProjectionMatrix = mat4::identity();
	 rotationMatrix = mat4::identity();

	modelViewMatrix = modelViewMatrix * translate(1.7f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * rotate(angleCube, angleCube, angleCube);

	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

	glActiveTexture(GL_TEXTURE0); //0th texture corresponds to GAB_ATTRIBUTE_TEXTURE0
	glBindTexture(GL_TEXTURE_2D, gKundaliTexture);
	glUniform1i(gTexture_sampler_uniform, 0);

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);


	glBindVertexArray(gVaoCube);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

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

	if (gVaoPyramid)
	{
		glDeleteVertexArrays(1, &gVaoPyramid);
		gVaoPyramid = 0;
	}

	if (gVboPyramidPos)
	{
		glDeleteBuffers(1,&gVboPyramidPos);
		gVboPyramidPos = 0;
	}

	if (gVboPyramidTexture)
	{
		glDeleteBuffers(1, &gVboPyramidTexture);
		gVboPyramidTexture = 0;
	}
	if (gVaoCube)
	{
		glDeleteVertexArrays(1, &gVaoCube);
		gVaoCube = 0;
	}

	if (gVboCubePos)
	{
		glDeleteBuffers(1, &gVboCubePos);
		gVboCubePos = 0;
	}

	if (gVboCubeTexture)
	{
		glDeleteBuffers(1, &gVboCubeTexture);
		gVboCubeTexture = 0;
	}

	glDetachShader(gShaderProgramObject, gVertexShaderObject);

	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

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
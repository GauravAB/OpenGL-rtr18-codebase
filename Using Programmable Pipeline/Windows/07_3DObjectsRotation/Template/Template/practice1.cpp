#include <Windows.h>
#include <stdio.h>
#include <gl\glew.h>
#include <gl\GL.h>
#include <stdlib.h>
#include "vmath.h"


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")

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

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//shader objects
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;

//program object
GLuint gShaderProgramObject;

//buffers
GLuint gVao_Cube;
GLuint gVbo_pos;
GLuint gVbo_color;

//uniform variable
GLuint gMVPUniform;

GLfloat angle_cube;


mat4 gProjectionTransformMatrix;



enum
{
	GAB_ATTRIBUTE_VERTEX = 0,
	GAB_ATTRIBUTE_COLOR,
	GAB_ATTRIBUTE_NORMAL,
	GAB_ATTRIBUTE_TEXTURE,
};




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
		fprintf(gpFile,"Error: %s\n", glewGetErrorString(err));
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

	//Vertex shader ------------------------------------------------------------------------------------------

	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const char* vertexShaderSourceCode =
		"#version 450 core"\
		"\n"
		"in vec4 vPosition;"\
		"in vec4 vColor;"\
		"out vec4 outColor;"\
		"uniform mat4 u_mvp_matrix;"\
		"void main(void)"\
		"{"\
		"gl_Position = u_mvp_matrix * vPosition ;"\
		"outColor = vColor;"\
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(gVertexShaderObject);

	GLint iShaderCompileStatus;
	GLint iInfoLogLength;
	char *szError;

	glGetShaderiv(gVertexShaderObject,GL_COMPILE_STATUS,&iShaderCompileStatus);

	if(iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szError = (char*)malloc(sizeof(iInfoLogLength));
			GLsizei written;

			glGetShaderInfoLog(gVertexShaderObject,iInfoLogLength,&written,szError);
			fprintf(gpFile,"Vertex shader compiler error: %s\n",szError);
			fclose(gpFile);
			free(szError);
			uninitialize();
			exit(0);
		}
	}

	//fragment shader----------------------------------------------------------------------------------
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const char* fragmentShaderSourceCode =
		"#version 450 core"\
		"\n"
		"in vec4 outColor;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = outColor;"\
		"}";

	glShaderSource(gFragmentShaderObject,1,(const GLchar**)&fragmentShaderSourceCode,NULL);


	glCompileShader(gFragmentShaderObject);

	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szError = NULL;

	glGetShaderiv(gFragmentShaderObject,GL_COMPILE_STATUS,&iShaderCompileStatus);

	if(iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szError = (char*)malloc(sizeof(iInfoLogLength));
			GLsizei written;

			glGetShaderInfoLog(gFragmentShaderObject,iInfoLogLength,&written,szError);
			fprintf(gpFile,"Fragment shader compiler error: %s\n",szError);
			fclose(gpFile);
			free(szError);
			uninitialize();
			exit(0);
		}
	}


	// Create shader program
	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject,gVertexShaderObject);
	glAttachShader(gShaderProgramObject,gFragmentShaderObject);

	//prelinking layout mapping attribute variables
	glBindAttribLocation(gShaderProgramObject,GAB_ATTRIBUTE_VERTEX,"vPosition");
	glBindAttribLocation(gShaderProgramObject, GAB_ATTRIBUTE_COLOR, "vColor");

	glLinkProgram(gShaderProgramObject);

	GLint iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	szError = NULL;

	glGetProgramiv(gShaderProgramObject,GL_LINK_STATUS,&iShaderLinkStatus);

	if(iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szError = (char*)malloc(sizeof(iInfoLogLength));
			GLsizei written;

			glGetProgramInfoLog(gShaderProgramObject,iInfoLogLength,&written,szError);

			fprintf(gpFile,"Shader Program linking Error: %s\n",szError);
			fclose(gpFile);
			free(szError);
			uninitialize();
			exit(0);
		}
	}

	//postlink layout mapping non-attribute variables
	gMVPUniform = glGetUniformLocation(gShaderProgramObject,"u_mvp_matrix");
	//----------------------------------------------------------------------------------------------------

	//data required for geometry -> pos , color;

	 GLfloat cubeVertices[] = 
		{
			//top face
			1.0f,1.0f,-1.0f,
			-1.0f,1.0f,-1.0f,
			-1.0f,1.0f,1.0f,
			1.0f,1.0f,1.0f,

			//bottom face
			1.0,-1.0f,1.0f,
			-1.0f,-1.0f,1.0f,
			-1.0f,-1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,

			//front face
			1.0f,1.0f,1.0f,
			-1.0f,1.0f,1.0f,
			-1.0f,-1.0f,1.0f,
			1.0f,-1.0f,1.0f,

			//back face
			1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f,1.0f,-1.0f,
			1.0f,1.0f,-1.0f,

			//left face
			-1.0f,1.0f,1.0f,
			-1.0f,1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,1.0f,

			//right face
			1.0f,1.0f,-1.0f,
			1.0f,1.0f,1.0f,
			1.0f,-1.0f,1.0f,
			1.0f,-1.0f,-1.0f,
		};

		const GLfloat cubeColors[] = 
		{
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,

			1.0f,0.5f,0.0f,
			1.0f,0.5f,0.0f,
			1.0f,0.5f,0.0f,
			1.0f,0.5f,0.0f,

			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,

			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f,

			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,

			1.0f,0.0f,1.0f,
			1.0f,0.0f,1.0f,
			1.0f,0.0f,1.0f,
			1.0f,0.0f,1.0f,
		};
		
		
		//mathematical scaling , reducing the size by 25%
		for (int i = 0 ; i < 72 ; i++)
		{
			if(cubeVertices[i] > 0.0f)
			{
				cubeVertices[i] = cubeVertices[i]- 0.25f;
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
		
		//----------------------------------------------------------------------------------------------

		//Vertex array object VAO 
		glGenVertexArrays(1,&gVao_Cube);
		glBindVertexArray(gVao_Cube);

		//attribute buffer for position
		glGenBuffers(1,&gVbo_pos);
		glBindBuffer(GL_ARRAY_BUFFER,gVbo_pos);
		//used to push data on the memory
		glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);
		glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL);
		glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	
		//attribute buffer for color
		glGenBuffers(1,&gVbo_color);
		glBindBuffer(GL_ARRAY_BUFFER,gVbo_color);
		//used to push data on the memory
		glBufferData(GL_ARRAY_BUFFER,sizeof(cubeColors),cubeColors,GL_STATIC_DRAW);
		glVertexAttribPointer(GAB_ATTRIBUTE_COLOR,3,GL_FLOAT,GL_FALSE,0,NULL);
		glEnableVertexAttribArray(GAB_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	

		//unbind vertex array call
		glBindVertexArray(0);





	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	//setting the matrix to identity
	gProjectionTransformMatrix = mat4 :: identity();

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
	angle_cube += 1.0f;

	if (angle_cube >= 360.0f)
	{
		angle_cube = 0.0f;
	}
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);

	mat4 modelViewMatrix = mat4 ::identity();
	mat4 modelViewProjectionMatrix = mat4 ::identity();
	
	modelViewMatrix = modelViewMatrix * translate(0.0f,0.0f,-6.0f);
	modelViewMatrix = modelViewMatrix * rotate(angle_cube, angle_cube, angle_cube);
	//modelViewMatrix = modelViewMatrix *	scale(0.75f, 0.75f, 0.75f);
	modelViewProjectionMatrix =  gProjectionTransformMatrix * modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);

	//BIND THE REREQUIRED DATA TO DRAW
	glBindVertexArray(gVao_Cube);

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

	if(width >= height)
	{
		gProjectionTransformMatrix = perspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	}
	else
	{
		gProjectionTransformMatrix = perspective(45.0f,(GLfloat)height/(GLfloat)width,0.1f,100.0f);
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

	if(gVao_Cube)
	{
		glDeleteBuffers(1,&gVao_Cube);
		gVao_Cube = 0;
	}

	if(gVbo_color)
	{
		glDeleteBuffers(1,&gVbo_color);
		gVbo_color = 0;
	}

	if(gVbo_pos)
	{
		glDeleteBuffers(1,&gVbo_pos);
		gVbo_pos = 0;
	}

	if(gVertexShaderObject)
	{
		glDetachShader(gShaderProgramObject,gVertexShaderObject);
		glDeleteShader(gVertexShaderObject);
		gVertexShaderObject = 0;
	}

	if(gFragmentShaderObject)
	{
		glDetachShader(gShaderProgramObject,gFragmentShaderObject);
		glDeleteShader(gFragmentShaderObject);
		gFragmentShaderObject = 0;
	}

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;
	
	glUseProgram(0);


	if (gbFullScreen)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

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
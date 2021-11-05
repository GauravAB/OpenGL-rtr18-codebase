#include <Windows.h>
#include <stdio.h>
#include <gl\glew.h>
#include <gl\GL.h>
#include <stdlib.h>
#include "vmath.h"
#include "Sphere.h"


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"sphere.lib")


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
bool gbLight= false;
bool gbAnimate = false;
bool gbCube = false;
bool gbPyramid = false;

GLfloat gCubeAngle = 0.0f;
GLfloat gPyramidAngle = 0.0f;
GLfloat gSphereAngle = 0.0f;


enum
{
	GAB_ATTRIBUTE_VERTEX = 0,
	GAB_ATTRIBUTE_COLOR,
	GAB_ATTRIBUTE_NORMAL ,
	GAB_ATTRIBUTE_TEXTURE0,
};

enum shapetodraw
{
	CUBE,
	PYRAMID,
	SPHERE
};

shapetodraw current_shape = CUBE;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao_cube;
GLuint gVbo_cube_position;
GLuint gVbo_cube_normals;

GLuint gVao_pyramid;
GLuint gVbo_Pyramid_position;
GLuint gVbo_Pyramid_normals;

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint gModelMatrixUniform;	//only model matrix
GLuint gViewMatrixUniform;	//only view matrix
GLuint gProjectionMatrixUniform;//only projection matrix
GLuint gLaUniform_left;
GLuint gLdUniform_left;
GLuint gLsUniform_left;

GLuint gLaUniform_right;
GLuint gLdUniform_right;
GLuint gLsUniform_right;

GLuint gKaUniform;
GLuint gKdUniform;
GLuint gKsUniform;
GLuint gLightPositionUniform_left;
GLuint gLightPositionUniform_right;

GLuint gLKeyPressedUniform;		//L key notification to enable lights
GLuint gMaterialShininessUniform;
mat4 gPerspectiveProjectionMatrix;

GLfloat light_ambient_right[] = { 0.25f,0.25f,0.25f,1.0f };
GLfloat light_diffuse_right[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat light_specular_right[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat light_position_right[] = { 200.0f,100.0f,100.0f,1.0f };


GLfloat light_ambient_left[] = { 0.25f,0.25f,0.25f,1.0f };
GLfloat light_diffuse_left[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat light_specular_left[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat light_position_left[] = { -200.0f,100.0f,100.0f,1.0f };

GLfloat material_ambient[] = { 0.25f,0.25f,0.25f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;

unsigned short sphere_elements[2280];
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
GLuint gNumVertices;
GLuint gNumElements;

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

			if (gbAnimate == true)
			{
				update();
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
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode =
		"#version 440 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec3 vNormal;"\
		"uniform mat4 u_projection_matrix;"\
		"uniform mat4 u_model_matrix;"\
		"uniform mat4 u_view_matrix;"\
		"uniform int u_LKeyPressed;"\
		"uniform vec3 u_La_left;"\
		"uniform vec3 u_Ld_left;"\
		"uniform vec3 u_Ls_left;"\

		"uniform vec3 u_La_right;"\
		"uniform vec3 u_Ld_right;"\
		"uniform vec3 u_Ls_right;"\

		"uniform vec3 u_Ka;"\
		"uniform vec3 u_Kd;"\
		"uniform vec3 u_Ks;"\
		"uniform float u_material_shininess;"\
		"uniform vec4 u_light_position_right;"\
		"uniform vec4 u_light_position_left;"\
		"out vec3 phong_ads_color;"\
		"void main(void)"\
		"{"\
		"if(u_LKeyPressed == 1)"\
		"{"\
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"\
		"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"\
		"vec3 light_direction_right = normalize(vec3(u_light_position_right)- eye_coordinates.xyz);"\
		"vec3 light_direction_left = normalize(vec3(u_light_position_left)- eye_coordinates.xyz);"\
		"float td_dot_ld_right = max(dot(transformed_normals,light_direction_right),0.0);"\
		"float td_dot_ld_left = max(dot(transformed_normals,light_direction_left),0.0);"\
		"vec3 ambient_right = u_La_right * u_Ka;"\
		"vec3 ambient_left = u_La_left * u_Ka;"\
		"vec3 diffuse_right = u_Ld_right * u_Kd * td_dot_ld_right;"\
		"vec3 diffuse_left = u_Ld_left * u_Kd * td_dot_ld_left;"\
		"vec3 reflection_vector_right = reflect(-light_direction_right,transformed_normals);"\
		"vec3 reflection_vector_left = reflect(-light_direction_left,transformed_normals);"\
		"vec3 viewer_vector = normalize(-eye_coordinates.xyz);"\
		"vec3 specular_right = u_Ls_right * u_Ks * pow(max(dot(reflection_vector_right,viewer_vector),0.0),u_material_shininess);"\
		"vec3 specular_left = u_Ls_left * u_Ks * pow(max(dot(reflection_vector_left,viewer_vector),0.0),u_material_shininess);"\
		"vec3 ambient = ambient_right + ambient_left;"\
		"vec3 diffuse = diffuse_right + diffuse_left;"\
		"vec3 specular = specular_right+specular_left;"\
		"phong_ads_color = ambient + diffuse + specular;"\
		"}"\
		"else"\
		"{"\
		"phong_ads_color = vec3(1.0f,1.0f,1.0f);"\
		"}"\
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"\
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
		"#version 440 core"\
		"\n"\
		"in vec3 phong_ads_color;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = vec4(phong_ads_color,1.0);"\
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
	glBindAttribLocation(gShaderProgramObject, GAB_ATTRIBUTE_NORMAL, "vNormal");



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

	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	gLightPositionUniform_right = glGetUniformLocation(gShaderProgramObject, "u_light_position_right");
	gLightPositionUniform_left = glGetUniformLocation(gShaderProgramObject, "u_light_position_left");

	gLaUniform_left = glGetUniformLocation(gShaderProgramObject, "u_La_left");
	gLdUniform_left = glGetUniformLocation(gShaderProgramObject, "u_Ld_left");
	gLsUniform_left = glGetUniformLocation(gShaderProgramObject, "u_Ls_left");

	gLaUniform_right = glGetUniformLocation(gShaderProgramObject, "u_La_right");
	gLdUniform_right = glGetUniformLocation(gShaderProgramObject, "u_Ld_right");
	gLsUniform_right = glGetUniformLocation(gShaderProgramObject, "u_Ls_right");

	gKaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	gKdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	gKsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	gMaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	//CUBE ARRAY
	GLfloat cubeVertices[] =
	{
		1.0f,1.0f,-1.0f,
		-1.0f,1.0f,-1.0f,
		-1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,

		1.0f,-1.0f,1.0f,
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

	//loop to scale the cube
	for (int i = 0; i < 72; i++)
	{
		if (cubeVertices[i] < 0.0f)
		{
			cubeVertices[i] = cubeVertices[i] + 0.25f;
		}
		else if (cubeVertices[i] > 0.0f)
		{
			cubeVertices[i] = cubeVertices[i] - 0.25f;
		}
		else
		{
			cubeVertices[i] = cubeVertices[i];
		}
	}

	const float cubeNormals[] =
	{
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,

		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,

		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,

		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,

		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,

		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f
	};


	const GLfloat pyramidVertices[] = {

		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,1.0f,
		1.0f,-1.0f,1.0f,

		0.0f,1.0f,0.0f,
		1.0f,-1.0f,1.0f,
		1.0f,-1.0f,-1.0f,

		0.0f,1.0f,0.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,

		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,1.0f,
	};

	const GLfloat pyramidNormals[] = {
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,

		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,

		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
	};

	//get sphere data from the dll
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);

	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);
	
	//VBO VERTICES
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO NORMALS
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//vbo elements
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//create vertex buffer array
	glGenVertexArrays(1, &gVao_cube);
	glBindVertexArray(gVao_cube);

	glGenBuffers(1, &gVbo_cube_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube_position);
	//copy the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 72, cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_cube_normals);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube_normals);
	//copy the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 72, cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &gVao_pyramid);
	glBindVertexArray(gVao_pyramid);
	glGenBuffers(1, &gVbo_Pyramid_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Pyramid_position);
	//copy the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 , pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Pyramid_normals);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Pyramid_normals);
	//copy the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, pyramidNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glBindVertexArray(0);



	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);

	gPerspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);
	void ToggleFullScreen(void);

	static bool bLKeyIsPressed = false;
	static bool bAKeyIsPressed = false;


	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 's':
		case 'S':
			current_shape = SPHERE;
			break;
		case 'p':
		case 'P':
			current_shape = PYRAMID;
			break;
		case 'c':
		case 'C':
			current_shape = CUBE;
			break;
		case 0x41:
			if (bAKeyIsPressed == false)
			{
				gbAnimate = true;
				bAKeyIsPressed = true;
			}
			else
			{
				gbAnimate = false;
				bAKeyIsPressed = false;
			}
			break;
		case 0x4C:
			if (bLKeyIsPressed == false)
			{
				gbLight = true;
				bLKeyIsPressed = true;
			}
			else
			{
				gbLight = false;
				bLKeyIsPressed = false;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);
	//handle lights
	if (gbLight == true)
	{
		glUniform1i(gLKeyPressedUniform, 1);
		glUniform3fv(gLaUniform_right,1,light_ambient_right);
		glUniform3fv(gLdUniform_right,1,light_diffuse_right);
		glUniform3fv(gLsUniform_right,1,light_specular_right);

		glUniform3fv(gLaUniform_left, 1, light_ambient_left);
		glUniform3fv(gLdUniform_left, 1, light_diffuse_left);
		glUniform3fv(gLsUniform_left, 1, light_specular_left);

		glUniform3fv(gKaUniform,1,material_ambient);
		glUniform3fv(gKdUniform,1,material_diffuse);
		glUniform3fv(gKsUniform,1,material_specular);
		glUniform1f(gMaterialShininessUniform, material_shininess);
		glUniform4fv(gLightPositionUniform_left, 1, (GLfloat*)light_position_left);
		glUniform4fv(gLightPositionUniform_right, 1, (GLfloat*)light_position_right);

	}
	else
	{
		glUniform1i(gLKeyPressedUniform, 0);
	}

	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();

	switch (current_shape)
	{
	case CUBE:
	
		//translate back
		modelMatrix = translate(0.0f, 0.0f, -5.0f);
		rotationMatrix = rotate(gCubeAngle, gCubeAngle, gCubeAngle);
		//all axis rotate
		modelMatrix = modelMatrix * rotationMatrix;
		glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
		glBindVertexArray(gVao_cube);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
		glBindVertexArray(0);
		glUseProgram(0);
		break;

	case PYRAMID:
		modelMatrix = mat4::identity();
		viewMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//translate back
		modelMatrix = translate(0.0f, 0.0f, -5.0f);
		rotationMatrix = rotate(gPyramidAngle, 0.0f, 1.0f, 0.0f);

		modelMatrix = modelMatrix * rotationMatrix;
		glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
		glBindVertexArray(gVao_pyramid);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		glBindVertexArray(0);
		break;
	case SPHERE:
		modelMatrix = mat4::identity();
		viewMatrix = mat4::identity();
		rotationMatrix = mat4::identity();
		//translate back
		modelMatrix = translate(0.0f, 0.0f, -3.0f);
		rotationMatrix = rotate(gPyramidAngle, 0.0f, 1.0f, 0.0f);
		scaleMatrix = scale(2.0f, 2.0f, 2.0f);
		modelMatrix = modelMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;

		glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
		glBindVertexArray(gVao_sphere);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		break;
	default:
		break;
	}

	glUseProgram(0);
	SwapBuffers(ghdc);
}

void update(void)
{
	if (gPyramidAngle >= 360.0f)
	{
		gPyramidAngle = 0.0f;
	}
	gPyramidAngle += 1.0f;

	if (gCubeAngle >= 360.0f)
	{
		gCubeAngle = 0.0f;
	}
	
	gCubeAngle += 1.0f;

	if (gSphereAngle >= 360.0f)
	{
		gSphereAngle = 0.0f;
	}
	gSphereAngle += 1.5f;
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

	if (gVao_cube)
	{
		glDeleteVertexArrays(1, &gVao_cube);
		gVao_cube = 0;
	}

	if (gVao_pyramid)
	{
		glDeleteVertexArrays(1, &gVao_pyramid);
		gVao_pyramid = 0;
	}
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}

	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	if (gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	if (gVbo_cube_position)
	{
		glDeleteBuffers(1,&gVbo_cube_position);
		gVbo_cube_position = 0;
	}

	if (gVbo_cube_normals)
	{
		glDeleteBuffers(1, &gVbo_cube_normals);
		gVbo_cube_normals = 0;
	}

	if (gVbo_Pyramid_normals)
	{
		glDeleteBuffers(1, &gVbo_Pyramid_normals);
		gVbo_Pyramid_normals;
	}

	if (gVbo_Pyramid_position)
	{
		glDeleteBuffers(1, &gVbo_Pyramid_position);
		gVbo_Pyramid_position;
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








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

enum
{
	GAB_ATTRIBUTE_VERTEX = 0,
	GAB_ATTRIBUTE_COLOR,
	GAB_ATTRIBUTE_NORMAL ,
	GAB_ATTRIBUTE_TEXTURE0,
};


enum light_direction
{
	light_x,
	light_y,
	light_z
};

light_direction dir_light = light_x;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;
GLuint model_matrix_uniform ,view_matrix_uniform , projection_matrix_uniform, model_matrix_scaled_uniform;
GLuint gNumElements;
GLuint gNumVertices;

unsigned short sphere_elements[2280];

mat4 gPerspectiveProjectionMatrix;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];

GLuint L_KeyPressed_uniform;
GLuint La_uniform;
GLuint Ld_uniform;
GLuint Ls_uniform;
GLuint light_position_uniform;

GLuint Ka_uniform;
GLuint kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

GLuint phong_uniform;
bool gbLight;
bool gAnimate;
bool gPhong_Enable = false;

GLfloat gAngle;
GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
vec4 lightPosition;

typedef GLfloat material_property[3];

material_property material_ambient[24] = { { 0.0215f,0.1745f,0.0215f },
{ 0.135f,0.2225f,0.1575f },
{ 0.05375f,0.05f,0.06625f },
{ 0.25f,0.20725f,0.20725f },
{ 0.1745f,0.01175f,0.01175f } ,
{ 0.1f,0.18725f,0.1745f },
{ 0.329412f,0.223529f,0.27451f } ,
{ 0.2125f,0.1275f,0.054f } ,
{ 0.25f,0.25f,0.25f },
{ 0.19125f,0.0735f,0.0225f },
{ 0.24725f,0.1995f,0.0745f },
{ 0.19225f,0.19225f,0.19225f },
{ 0.0f,0.0f,0.0f },
{ 0.0f,0.1f,0.06f },
{ 0.0f,0.0f,0.0f, },
{ 0.0f,0.0f,0.0f },
{ 0.0f,0.0f,0.0f },
{ 0.0f,0.0f,0.0f },
{ 0.02f,0.02f,0.02f },
{ 0.0f,0.05f,0.05f },
{ 0.0f,0.05f,0.0f },
{ 0.05f,0.0f,0.0f },
{ 0.05f,0.05f,0.05f },
{ 0.05f,0.05f,0. }
};





material_property material_specular[24] = { { 0.633f,0.727811f,0.633f },
{ 0.316228f,0.316228f,0.316228f },
{ 0.332741f,0.328634f,0.346435f },
{ 0.296648f,0.296648f,0.296648f },
{ 0.727811f,0.626959f,0.626959f },
{ 0.297254f,0.30829f,0.306678f },
{ 0.992157f,0.941176f,0.807843f },
{ 0.393548f,0.271906f,0.166721f } ,
{ 0.774597f,0.774597f,0.774597f },
{ 0.256777f,0.137622f,0.086014f },
{ 0.628281f,0.555802f,0.366065f },
{ 0.508273f,0.508273f ,0.508273f },
{ 0.50f,0.50f,0.50f },
{ 0.50196078f,0.50196078f,0.50196078f },
{ 0.45f,0.55f,0.45f },
{ 0.7f,0.6f,0.6f },
{ 0.70f,0.70f,0.70f },
{ 0.60f,0.60f,0.50f },
{ 0.4f,0.4f,0.4f },
{ 0.04f,0.7f,0.7f },
{ 0.04f,0.7f,0.04f },
{ 0.7f,0.04f,0.04f },
{ 0.7f,0.7f,0.7f },
{ 0.7f,0.7f,0.04f }
};





material_property material_diffuse[24] = { { 0.07568f,0.61424f,0.07568f},
{ 0.54f,0.89f,0.63f },
{ 0.18275f,0.17f,0.22525f },
{ 1.0f,0.829f,0.829f},
{ 0.61424f,0.04136f,0.04136f },
{ 0.396f,0.74151f,0.69102f } ,
{ 0.780392f,0.568627f,0.113725f} ,
{ 0.714f,0.4284f,0.18144f} ,
{ 0.4f,0.4f,0.4f},
{ 0.7038f,0.27048f,0.0828f },
{ 0.75164f,0.60648f,0.22648f },
{ 0.50745f,0.50745f,0.50745f},
{ 0.01f,0.01f,0.01f},
{ 0.0f,0.50980392f,0.50980392f },
{ 0.1f,0.35f,0.1f },
{ 0.5f,0.0f,0.0f},
{ 0.55f,0.55f,0.55f },
{ 0.5f,0.5f,0.0f },
{ 0.01f,0.01f,0.01f },
{ 0.4f,0.5f,0.5f },
{ 0.4f,0.5f,0.4f },
{ 0.5f,0.4f,0.4f },
{ 0.5f,0.5f,0.5f },
{ 0.5f,0.5f,0.4f}
};


GLfloat  material_shininess[24] = { 0.6f * 128 ,
1.0f * 128 ,
0.3f * 128 ,
0.088f * 128,
0.6f * 128,
0.1f * 128,
0.21794872f * 128,
0.2f * 128,
0.6f * 128,
0.1f * 128.0f,
0.4f * 128.0f,
0.4f * 128.0f, 
0.25f * 128.0f,
0.25f * 128 ,
0.25f * 128 ,
0.25f * 128 ,
0.25f * 128 ,
0.25f * 128 ,
0.078125f * 128 ,
0.078125f * 128  ,
0.78125f * 128  ,
0.078125f * 128 ,
0.078125f * 128 , 
0.78125f * 128  };


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
	TCHAR szAppName[] = TEXT("Sphere with per vertex Lighting");

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
			if (gAnimate == true)
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
		"#version 450 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec3 vNormal;"\
		"uniform int u_phong;"\
		"uniform mat4 u_model_matrix;"\
		"uniform mat4 u_model_matrix_scaled;"\
		"uniform mat4 u_view_matrix;"\
		"uniform mat4 u_projection_matrix;"\
		"uniform int u_lighting_enabled;"\
		"uniform vec4 u_light_position;"\
		"out vec3 light_direction;"\
		"out vec3 transformed_normals;"\
		"out vec3 viewer_vector;"\
		"out vec3 attrib_phong_ads_color;"\
		"uniform vec3 u_La;"\
		"uniform vec3 u_Ld;"\
		"uniform vec3 u_Ls;"\
		"uniform vec3 u_Ka;"\
		"uniform vec3 u_Kd;"\
		"uniform vec3 u_Ks;"\
		"uniform float u_material_shininess;"\
		"void main(void)"\
		"{"\
		"if(u_lighting_enabled == 1)"\
		"{"\
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix_scaled * vPosition;"\
		"if(u_phong==1)"\
		"{"\
		"transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;"\
		"light_direction = vec3(u_light_position)-eye_coordinates.xyz;"\
		"viewer_vector = -eye_coordinates.xyz;"\
		"}"\
		"else"\
		"{"\
		"vec3 normalized_transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"\
		"vec3 normalized_light_direction = normalize(vec3(u_light_position)-eye_coordinates.xyz);"\
		"vec3 normalized_viewer_vector = normalize(-eye_coordinates.xyz);"\
		"vec3 ambient = u_La * u_Ka;"\
		"float tn_dot_ld = max(dot(normalized_transformed_normals,normalized_light_direction),0.0);"\
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"\
		"vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);"\
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);"\
		"attrib_phong_ads_color = ambient + diffuse + specular;"\
		"}"\
		"}"\
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix_scaled * vPosition;"\
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
		"#version 450 core"\
		"\n"\
		"in vec3 transformed_normals;"\
		"in vec3 light_direction;"\
		"in vec3 viewer_vector;"\
		"in vec3 attrib_phong_ads_color;"\
		"out vec4 FragColor;"\
		"uniform int u_phong;"\
		"uniform vec3 u_La;"\
		"uniform vec3 u_Ld;"\
		"uniform vec3 u_Ls;"\
		"uniform vec3 u_Ka;"\
		"uniform vec3 u_Kd;"\
		"uniform vec3 u_Ks;"\
		"uniform float u_material_shininess;"\
		"uniform int u_lighting_enabled;"\
		"void main(void)"\
		"{"\
		"vec3 phong_ads_color;"\
		"if(u_lighting_enabled ==1)"\
		"{"\
		"if(u_phong == 1)"
		"{"\
		"vec3 normalized_transformed_normals = normalize(transformed_normals);"\
		"vec3 normalized_light_direction = normalize(light_direction);"\
		"vec3 normalized_viewer_vector = normalize(viewer_vector);"\
		"vec3 ambient = u_La * u_Ka;"\
		"float tn_dot_ld = max(dot(normalized_transformed_normals,normalized_light_direction),0.0);"\
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"\
		"vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);"\
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);"\
		"phong_ads_color = ambient + diffuse + specular;"\
		"}"\
		"else"
		"{"
		"phong_ads_color = attrib_phong_ads_color;"\
		"}"
		"}"\
		"else"
		"{"\
		"phong_ads_color = vec3(1.0,1.0,1.0);"\
		"}"\
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
	GLint iShaderLinkStatus =0;
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

	model_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	//Light toggle
	L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");

	//ambient color intensity
	La_uniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	//diffuse color intensity
	Ld_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	//specular color intensity
	Ls_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
	//position of light
	light_position_uniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	
	//ambient reflective intensity of material
	Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
	model_matrix_scaled_uniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix_scaled");
	phong_uniform = glGetUniformLocation(gShaderProgramObject, "u_phong");

	//data array
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);

	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	//create vertex buffer array
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);
	
	//vbo vertices
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	//copy the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//vbo normals
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	//copy the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(GAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(GAB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//vbo elements
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	//copy the data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	glClearColor(0.20f, 0.20f, 0.20f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	
	gPerspectiveProjectionMatrix = mat4::identity();
	gbLight = false;

	resize(WIN_WIDTH, WIN_HEIGHT);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);
	void ToggleFullScreen(void);

	static bool bIsAKeyPressed = false;
	static bool bIsLKeyPressed = false;

	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'f':
		case 'F':
			if (gPhong_Enable == false)
			{
				gPhong_Enable = true;
			}
			
			break;
		case 'v':
		case 'V':
			
			if (gPhong_Enable == true)
			{
				gPhong_Enable = false;
			}

			break;
		case 'a':
		case 'A':
			if (gAnimate == false)
			{
				gAnimate = true;
			}
			else
			{
				gAnimate = false;
			}
			break;
		case 'x':
		case 'X':
			dir_light = light_x;
			break;
		case 'y':
		case 'Y':
			dir_light = light_y;
			break;
		case 'z':
		case 'Z':
			dir_light = light_z;
			break;
		case 0x4C:
			if (bIsLKeyPressed == false)
			{
				gbLight = true;
				bIsLKeyPressed = true;
			}
			else
			{
				gbLight = false;
				bIsLKeyPressed = false;
			}
			break;
		case VK_SPACE:
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
	if (gAngle >= 360.0f)
	{
		gAngle = 0.0f;
	}

	gAngle += 0.6f;
}


void display(void)

{

	int a_count = 0;
	int d_count = 0;
	int s_count = 0;
	int ms_count = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	mat4 modelMatrix = mat4::identity();
	mat4 modelMatrixScaled = mat4::identity();

	mat4 viewMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 rotateMatrix = mat4::identity();


	glUseProgram(gShaderProgramObject);

	if (gbLight == true)
	{
		if (gPhong_Enable == true)
		{
			glUniform1i(phong_uniform, 1);
		
		}
		else
		{
			glUniform1i(phong_uniform, 0);
		}
		glUniform1i(L_KeyPressed_uniform, 1);
		glUniform3fv(La_uniform, 1, lightAmbient);
		glUniform3fv(Ld_uniform, 1, lightDiffuse);
		glUniform3fv(Ls_uniform, 1, lightSpecular);
		
		switch (dir_light)
		{
		case light_x:
			lightPosition = { 0.0f, 0.0f, 100.0f, 1.0f };
			rotateMatrix = rotate(gAngle, 1.0f, 0.0f, 0.0f);
			lightPosition = lightPosition * rotateMatrix;
			glUniform4fv(light_position_uniform, 1, lightPosition);
			break;
		case light_y:
			lightPosition = { 0.0f, 0.0f, 100.0f, 1.0f };
			rotateMatrix = rotate(gAngle, 0.0f, 1.0f, 0.0f);
			lightPosition = lightPosition * rotateMatrix;
			glUniform4fv(light_position_uniform, 1, lightPosition);
			break;
		case light_z:
			lightPosition = {100.0f, 0.0f, 0.0f, 1.0f };
			rotateMatrix = rotate(gAngle, 0.0f, 0.0f, 1.0f);
			lightPosition = lightPosition * rotateMatrix;
			glUniform4fv(light_position_uniform, 1, lightPosition);
			break;
		default:
			dir_light = light_x;
			break;
		}
	}
	else
	{
		glUniform1i(L_KeyPressed_uniform, 0);
	}

	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);


	
	glBindVertexArray(gVao_sphere);
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-6.0f, 5.6f, -16.0f);
	modelMatrix = modelMatrix + translate(-6.0f, 5.6f, -16.0f);

	
	glUniform3fv(Ka_uniform, 1, material_ambient[0]);
	glUniform3fv(kd_uniform, 1, material_diffuse[0]);
	glUniform3fv(Ks_uniform, 1, material_specular[0]);
	glUniform1f(material_shininess_uniform, material_shininess[0]);

	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);
	
	glBindVertexArray(gVao_sphere);
	//setting material properties
	glUniform3fv(Ka_uniform, 1, material_ambient[1]);
	glUniform3fv(kd_uniform, 1, material_diffuse[1]);
	glUniform3fv(Ks_uniform, 1, material_specular[1]);
	glUniform1f(material_shininess_uniform, material_shininess[1]);
	modelMatrix = mat4::identity();
	modelMatrixScaled = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-2.0f, 5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-2.0f, 5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);
	
	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[2]);
	glUniform3fv(kd_uniform, 1, material_diffuse[2]);
	glUniform3fv(Ks_uniform, 1, material_specular[2]);
	glUniform1f(material_shininess_uniform, material_shininess[2]);
	modelMatrix = mat4::identity();
	modelMatrixScaled = mat4::identity();

	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(2.0f, 5.6f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(2.0f, 5.6f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	
	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[3]);
	glUniform3fv(kd_uniform, 1, material_diffuse[3]);
	glUniform3fv(Ks_uniform, 1, material_specular[3]);
	glUniform1f(material_shininess_uniform, material_shininess[3]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	modelMatrixScaled = mat4::identity();

	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(6.0f, 5.6f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(6.0f, 5.6f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	
	//row 2-------------------------------------------------------------------------------------
	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[4]);
	glUniform3fv(kd_uniform, 1, material_diffuse[4]);
	glUniform3fv(Ks_uniform, 1, material_specular[4]);
	glUniform1f(material_shininess_uniform, material_shininess[4]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-6.0f, 3.35f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-6.0f, 3.35f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[5]);
	glUniform3fv(kd_uniform, 1, material_diffuse[5]);
	glUniform3fv(Ks_uniform, 1, material_specular[5]);
	glUniform1f(material_shininess_uniform, material_shininess[5]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-2.0f, 3.35f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-2.0f, 3.35f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[6]);
	glUniform3fv(kd_uniform, 1, material_diffuse[6]);
	glUniform3fv(Ks_uniform, 1, material_specular[6]);
	glUniform1f(material_shininess_uniform, material_shininess[6]);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(2.0f, 3.35f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(2.0f, 3.35f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);


	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[7]);
	glUniform3fv(kd_uniform, 1, material_diffuse[7]);
	glUniform3fv(Ks_uniform, 1, material_specular[7]);
	glUniform1f(material_shininess_uniform, material_shininess[7]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(6.0f, 3.35f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(6.0f, 3.35f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	
	//row3------------------------------------------------------------------------------------------------------
	glBindVertexArray(gVao_sphere);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-6.0f, 1.10f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-6.0f, 1.10f, -16.0f);
	glUniform3fv(Ka_uniform, 1, material_ambient[8]);
	glUniform3fv(kd_uniform, 1, material_diffuse[8]);
	glUniform3fv(Ks_uniform, 1, material_specular[8]);
	glUniform1f(material_shininess_uniform, material_shininess[8]);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);


	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[9]);
	glUniform3fv(kd_uniform, 1, material_diffuse[9]);
	glUniform3fv(Ks_uniform, 1, material_specular[9]);
	glUniform1f(material_shininess_uniform, material_shininess[9]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	
	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-2.0f, 1.10f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-2.0f, 1.10f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[10]);
	glUniform3fv(kd_uniform, 1, material_diffuse[10]);
	glUniform3fv(Ks_uniform, 1, material_specular[10]);
	glUniform1f(material_shininess_uniform, material_shininess[10]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(2.0f,1.10f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(2.0f, 1.10f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);


	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[11]);
	glUniform3fv(kd_uniform, 1, material_diffuse[11]);
	glUniform3fv(Ks_uniform, 1, material_specular[11]);
	glUniform1f(material_shininess_uniform, material_shininess[11]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(6.0f, 1.10f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);


	modelMatrix = modelMatrix + translate(6.0f, 1.10f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);


	//row4----------------------------------------------------------------------------------------------------------------------------
	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[12]);
	glUniform3fv(kd_uniform, 1, material_diffuse[12]);
	glUniform3fv(Ks_uniform, 1, material_specular[12]);
	glUniform1f(material_shininess_uniform, material_shininess[12]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-6.0f, -1.15f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-6.0f, -1.15f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[13]);
	glUniform3fv(kd_uniform, 1, material_diffuse[13]);
	glUniform3fv(Ks_uniform, 1, material_specular[13]);
	glUniform1f(material_shininess_uniform, material_shininess[13]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-2.0f, -1.15f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-2.0f, -1.15f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);


	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[14]);
	glUniform3fv(kd_uniform, 1, material_diffuse[14]);
	glUniform3fv(Ks_uniform, 1, material_specular[14]);
	glUniform1f(material_shininess_uniform, material_shininess[14]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	modelMatrixScaled = mat4::identity();

	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(2.0f, -1.15f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(2.0f, -1.15f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);


	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[15]);
	glUniform3fv(kd_uniform, 1, material_diffuse[15]);
	glUniform3fv(Ks_uniform, 1, material_specular[15]);
	glUniform1f(material_shininess_uniform, material_shininess[15]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(6.0f, -1.15f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(6.0f, -1.15f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	//row5---------------------------------------------------------------------------------------------------------------------------
	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[16]);
	glUniform3fv(kd_uniform, 1, material_diffuse[16]);
	glUniform3fv(Ks_uniform, 1, material_specular[16]);
	glUniform1f(material_shininess_uniform, material_shininess[16]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-6.0f, -3.40f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-6.0f, -3.40f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[17]);
	glUniform3fv(kd_uniform, 1, material_diffuse[17]);
	glUniform3fv(Ks_uniform, 1, material_specular[17]);
	glUniform1f(material_shininess_uniform, material_shininess[17]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	
	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-2.0f, -3.40f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-2.0f, -3.40f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[18]);
	glUniform3fv(kd_uniform, 1, material_diffuse[18]);
	glUniform3fv(Ks_uniform, 1, material_specular[18]);
	glUniform1f(material_shininess_uniform, material_shininess[18]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(2.0f, -3.40f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(2.0f, -3.40f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);


	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[19]);
	glUniform3fv(kd_uniform, 1, material_diffuse[19]);
	glUniform3fv(Ks_uniform, 1, material_specular[19]);
	glUniform1f(material_shininess_uniform, material_shininess[19]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(6.0f, -3.40f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(6.0f, -3.40f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	
	//row6-----------------------------------------------------------------------------------------------------------------
	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[20]);
	glUniform3fv(kd_uniform, 1, material_diffuse[20]);
	glUniform3fv(Ks_uniform, 1, material_specular[20]);
	glUniform1f(material_shininess_uniform, material_shininess[20]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-6.0f, -5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);


	modelMatrix = modelMatrix + translate(-6.0f, -5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[21]);
	glUniform3fv(kd_uniform, 1, material_diffuse[21]);
	glUniform3fv(Ks_uniform, 1, material_specular[21]);
	glUniform1f(material_shininess_uniform, material_shininess[21]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(-2.0f, -5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(-2.0f, -5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);

	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[22]);
	glUniform3fv(kd_uniform, 1, material_diffuse[22]);
	glUniform3fv(Ks_uniform, 1, material_specular[22]);
	glUniform1f(material_shininess_uniform, material_shininess[22]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(2.0f, -5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(2.0f, -5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
	glBindVertexArray(0);


	glBindVertexArray(gVao_sphere);
	glUniform3fv(Ka_uniform, 1, material_ambient[23]);
	glUniform3fv(kd_uniform, 1, material_diffuse[23]);
	glUniform3fv(Ks_uniform, 1, material_specular[23]);
	glUniform1f(material_shininess_uniform, material_shininess[23]);
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	scaleMatrix = scale(1.0f, 1.0f, 0.0f);
	

	modelMatrixScaled = mat4::identity();
	modelMatrixScaled = scaleMatrix * modelMatrixScaled;
	modelMatrixScaled = modelMatrixScaled + translate(6.0f, -5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_scaled_uniform, 1, GL_FALSE, modelMatrixScaled);

	modelMatrix = modelMatrix + translate(6.0f, -5.55f, -16.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//Unbind vertex array
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
	
	gPerspectiveProjectionMatrix = perspective(50.0f, (GLfloat)width / (GLfloat)height, 0.1f, 400.0f);
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

	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1,&gVbo_sphere_element);
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
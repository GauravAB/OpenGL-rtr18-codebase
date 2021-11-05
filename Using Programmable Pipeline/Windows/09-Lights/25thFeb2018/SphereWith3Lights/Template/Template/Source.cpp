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

enum shadeModel
{
	GOURAD,
	PHONG
};

GLuint g_light_model = PHONG;

GLuint gVertexShaderObject;
GLuint gVertexShaderObject_gourad;
GLuint gFragmentShaderObject;
GLuint gFragmentShaderObject_gourad;
GLuint gShaderProgramObject_gourad;
GLuint gShaderProgramObject_phong;
GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;
GLuint model_matrix_uniform ,view_matrix_uniform , projection_matrix_uniform;
GLuint gNumElements;
GLuint gNumVertices;

unsigned short sphere_elements[2280];

mat4 gPerspectiveProjectionMatrix;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];

GLuint L_KeyPressed_uniform;
GLuint La_uniform;

GLuint Ld_x_uniform;
GLuint Ls_x_uniform;

GLuint Ld_y_uniform;
GLuint Ls_y_uniform;

GLuint Ld_z_uniform;
GLuint Ls_z_uniform;

GLuint light_position_x_uniform;
GLuint light_position_y_uniform;
GLuint light_position_z_uniform;

GLuint rotation_matrix_uniform;

GLuint Ka_uniform;
GLuint kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

bool gbLight;

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };

GLfloat lightDiffuse_x[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat lightSpecular_x[] = { 1.0f,0.0f,0.0f,1.0f };

GLfloat lightDiffuse_y[] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat lightSpecular_y[] = { 0.0f,1.0f,0.0f,1.0f };

GLfloat lightDiffuse_z[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat lightSpecular_z[] = { 0.0f,0.0f,1.0f,1.0f };

vec4 lightPosition_x;
vec4 lightPosition_y;
vec4 lightPosition_z;

GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f; 

GLfloat gAngle = 0.0f;

mat4 gRotation_matrix;


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

	//------------------------------------------------------------------------------------------------
	//GOURAD SHADING PROGRAM
	//Create vertex shader 
	gVertexShaderObject_gourad = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode_gourad =
		"#version 430 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec3 vNormal;"\
		"uniform mat4 u_model_matrix;"\
		"uniform mat4 u_view_matrix;"\
		"uniform mat4 u_projection_matrix;"\
		"uniform int u_lighting_enabled;"\
		"uniform vec3 u_La;"\
		"uniform vec3 u_Ld_x;"\
		"uniform vec3 u_Ls_x;"\
		"uniform vec4 u_light_position_x;"\
		"uniform vec3 u_Ld_y;"\
		"uniform vec3 u_Ls_y;"\
		"uniform vec4 u_light_position_y;"\
		"uniform vec3 u_Ld_z;"\
		"uniform vec3 u_Ls_z;"\
		"uniform vec4 u_light_position_z;"\
		"uniform vec3 u_Ka;"\
		"uniform vec3 u_Kd;"\
		"uniform vec3 u_Ks;"\
		"uniform float u_material_shininess;"\
		"out vec3 phong_ads_color;"\
		"void main(void)"\
		"{"\
		"if(u_lighting_enabled == 1)"\
		"{"\
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"\
		"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix)*vNormal);"\
		"vec3 viewer_vector = normalize(-eye_coordinates.xyz);"\
		"vec3 light_direction = normalize(vec3(u_light_position_x)-eye_coordinates.xyz);"\
		"float tn_dot_ld = max(dot(transformed_normals,light_direction),0.0f);"\
		"vec3 ambient = u_La * u_Ka;"\
		"vec3 diffuse_x = u_Ld_x * u_Kd * tn_dot_ld;"\
		"vec3 reflection_vector = reflect(-light_direction,transformed_normals);"\
		"vec3 specular_x = u_Ls_x * u_Ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);"\
		"light_direction = normalize(vec3(u_light_position_y)-eye_coordinates.xyz);"\
		"tn_dot_ld = max(dot(transformed_normals,light_direction),0.0f);"\
		"vec3 diffuse_y = u_Ld_y * u_Kd * tn_dot_ld;"\
		"reflection_vector = reflect(-light_direction,transformed_normals);"\
		"vec3 specular_y = u_Ls_y * u_Ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);"\
		"light_direction = normalize(vec3(u_light_position_z)-eye_coordinates.xyz);"\
		"tn_dot_ld = max(dot(transformed_normals,light_direction),0.0f);"\
		"vec3 diffuse_z = u_Ld_z * u_Kd * tn_dot_ld;"\
		"reflection_vector = reflect(-light_direction,transformed_normals);"\
		"vec3 specular_z = u_Ls_z * u_Ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);"\
		"vec3 diffuse = diffuse_x + diffuse_y + diffuse_z;"\
		"vec3 specular = specular_x + specular_y + specular_z;"\
		"phong_ads_color = ambient + diffuse + specular;"\
		"}"\
		"else"\
		"{"\
		"phong_ads_color = vec3(1.0f,1.0f,1.0f);"\
		"}"\
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"\
		"}";

	glShaderSource(gVertexShaderObject_gourad, 1, (GLchar**)&vertexShaderSourceCode_gourad, NULL);
	glCompileShader(gVertexShaderObject_gourad);
	GLint iCompileStatus;
	GLint iInfoLogLength;
	char *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_gourad, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_gourad, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(gVertexShaderObject_gourad, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Vertex Shader compile error Log : %s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}

	//Create fragment shader
	gFragmentShaderObject_gourad = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode_gourad =
		"#version 460 core"\
		"\n"\
		"in vec3 phong_ads_color;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = vec4(phong_ads_color,1.0);"\
		"}";
	glShaderSource(gFragmentShaderObject_gourad, 1, (GLchar**)&fragmentShaderSourceCode_gourad, NULL);
	glCompileShader(gFragmentShaderObject_gourad);
	
	iCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject_gourad, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_gourad, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(gFragmentShaderObject_gourad, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Fragment Shader compile error Log : %s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}

	gShaderProgramObject_gourad = glCreateProgram();

	glAttachShader(gShaderProgramObject_gourad, gVertexShaderObject_gourad);
	glAttachShader(gShaderProgramObject_gourad, gFragmentShaderObject_gourad);

	//prelinking vertex data layout 
	glBindAttribLocation(gShaderProgramObject_gourad, GAB_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject_gourad, GAB_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(gShaderProgramObject_gourad);
	GLint iShaderLinkStatus =0;
	glGetProgramiv(gShaderProgramObject_gourad, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_gourad, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(gShaderProgramObject_gourad, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "program link error log : %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	glUseProgram(0);


//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//PHONG SHADING PROGRAM
//Create vertex shader 
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);


	const char* vertexShaderSourceCode =
		"#version 460 core"\
		"\n"
		"in vec4 vPosition;"\
		"in vec3 vNormal;"\
		"uniform mat4 u_model_matrix;"\
		"uniform mat4 u_view_matrix;"\
		"uniform mat4 u_projection_matrix;"\
		"uniform int u_lighting_enabled;"\
		"uniform vec4 u_light_position_x;"\
		"uniform vec4 u_light_position_y;"\
		"uniform vec4 u_light_position_z;"\
		"out vec3 light_direction_x;"\
		"out vec3 light_direction_y;"\
		"out vec3 light_direction_z;"\
		"out vec3 transformed_normals;"\
		"out vec3 viewer_vector;"\
		"void main(void)"\
		"{"\
		"if(u_lighting_enabled==1)"\
		"{"\
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"\
		"transformed_normals = mat3(u_view_matrix*u_model_matrix)*vNormal;"\
		"light_direction_x = vec3(u_light_position_x)-eye_coordinates.xyz;"\
		"light_direction_y = vec3(u_light_position_y)-eye_coordinates.xyz;"\
		"light_direction_z = vec3(u_light_position_z)-eye_coordinates.xyz;"\
		"viewer_vector = -eye_coordinates.xyz;"\
		"}"\
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"\
		"}";

	glShaderSource(gVertexShaderObject, 1, (GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(gVertexShaderObject);
	 iCompileStatus;
	 iInfoLogLength;
	szInfoLog = NULL;

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

	const char* fragmentShaderSourceCode =
		"#version 460 core"\
		"\n"\
		"in vec3 transformed_normals;"\
		"in vec3 light_direction_x;"\
		"in vec3 light_direction_y;"\
		"in vec3 light_direction_z;"\
		"in vec3 viewer_vector;"\
		"out vec4 FragColor;"\
		"uniform vec3 u_La;"\
		"uniform vec3 u_Ld_x;"\
		"uniform vec3 u_Ls_x;"\
		"uniform vec3 u_Ld_y;"\
		"uniform vec3 u_Ls_y;"\
		"uniform vec3 u_Ld_z;"\
		"uniform vec3 u_Ls_z;"\
		"uniform vec3 u_Ka;"\
		"uniform vec3 u_Kd;"\
		"uniform vec3 u_Ks;"\
		"uniform float u_material_shininess;"\
		"uniform int u_lighting_enabled;"\
		"void main(void)"\
		"{"\
		"vec3 phong_ads_color;"\
		"if(u_lighting_enabled == 1)"\
		"{"\
		"vec3 normalized_transformed_normals = normalize(transformed_normals);"\
		"vec3 normalized_light_direction_x = normalize(light_direction_x);"\
		"vec3 normalized_light_direction_y = normalize(light_direction_y);"\
		"vec3 normalized_light_direction_z = normalize(light_direction_z);"\
		"vec3 normalized_viewer_vector = normalize(viewer_vector);"\
		"vec3 ambient = u_La * u_Ka;"\
		"float tn_dot_ld_x = max(dot(normalized_transformed_normals,normalized_light_direction_x),0.0);"\
		"vec3 diffuse_x = u_Ld_x * u_Kd * tn_dot_ld_x;"\
		"vec3 reflection_vector = reflect(-normalized_light_direction_x,normalized_transformed_normals);"\
		"vec3 specular_x = u_Ls_x * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);"\

		"float tn_dot_ld_y = max(dot(normalized_transformed_normals,normalized_light_direction_y),0.0);"\
		"vec3 diffuse_y = u_Ld_y * u_Kd * tn_dot_ld_y;"\
		"reflection_vector = reflect(-normalized_light_direction_y,normalized_transformed_normals);"\
		"vec3 specular_y = u_Ls_y * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);"\
		"float tn_dot_ld_z = max(dot(normalized_transformed_normals,normalized_light_direction_z),0.0f);"\
		"vec3 diffuse_z = u_Ld_z * u_Kd * tn_dot_ld_z;"\
		"reflection_vector = reflect(-normalized_light_direction_z,normalized_transformed_normals);"\
		"vec3 specular_z = u_Ls_z * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);"\
		"vec3 diffuse = diffuse_x + diffuse_y + diffuse_z;"\
		"vec3 specular = specular_x + specular_y + specular_z;"\
		"phong_ads_color = ambient + diffuse + specular;"\
		"FragColor = vec4(phong_ads_color,1.0);"\
		"}"
		"else"\
		"{"\
		"FragColor = vec4(1.0f,1.0f,1.0f,1.0f);"\
		"}"\
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

	gShaderProgramObject_phong = glCreateProgram();

	glAttachShader(gShaderProgramObject_phong, gVertexShaderObject);
	glAttachShader(gShaderProgramObject_phong, gFragmentShaderObject);

	//prelinking vertex data layout 
	glBindAttribLocation(gShaderProgramObject_phong, GAB_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject_phong, GAB_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(gShaderProgramObject_phong);
	 iShaderLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_phong, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_phong, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(gShaderProgramObject_phong, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "program link error log : %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------



	model_matrix_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_projection_matrix");

	//Light toggle
	L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_lighting_enabled");

	
	//ambient color intensity
	La_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_La");
	//diffuse color intensity
	Ld_x_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Ld_x");
	//specular color intensity
	Ls_x_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Ls_x");
	//position of light
	light_position_x_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_light_position_x");
	
	
	Ld_y_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Ld_y");
	//specular color intensity
	Ls_y_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Ls_y");
	//position of light
	light_position_y_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_light_position_y");
	
	Ld_z_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Ld_z");
	//specular color intensity
	Ls_z_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Ls_z");
	//position of light
	light_position_z_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_light_position_z");
	
	
	//ambient reflective intensity of material
	Ka_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Ka");
	kd_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Kd");
	Ks_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_Ks");
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject_gourad, "u_material_shininess");


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

	mat3 gRotation_matrix = mat3::identity();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
		case 'v':
		case 'V':
			g_light_model = GOURAD;
			break;
		case 'f':
		case 'F':
			g_light_model = PHONG;
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
	gAngle += 1.0f;

	if (gAngle == 360.0f)
	{
		gAngle = 0.0f;
	}
}

void display(void)

{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (g_light_model == GOURAD)
	{
		glUseProgram(gShaderProgramObject_gourad);
	}
	else if(g_light_model == PHONG)
	{
		glUseProgram(gShaderProgramObject_phong);
	}

	gRotation_matrix = mat4::identity();
	gRotation_matrix = rotate(gAngle, 1.0f, 0.0f, 0.0f);
	lightPosition_x = { 0.0f,0.0f,100.0f,1.0f };
	lightPosition_x = lightPosition_x * gRotation_matrix ;

	gRotation_matrix = mat4::identity();
	gRotation_matrix = rotate(gAngle, 0.0f, 1.0f, 0.0f);
	lightPosition_y = { 0.0f,0.0f,100.0f,1.0f };
	lightPosition_y = lightPosition_y * gRotation_matrix;

	gRotation_matrix = mat4::identity();
	gRotation_matrix = rotate(gAngle, 0.0f, 0.0f, 1.0f);
	lightPosition_z = { 100.0f,0.0f,0.0f,1.0f };
	lightPosition_z = lightPosition_z * gRotation_matrix;

	if (gbLight == true)
	{
		glUniform1i(L_KeyPressed_uniform, 1);
		glUniform3fv(La_uniform, 1, lightAmbient);
		glUniform3fv(Ld_x_uniform, 1, lightDiffuse_x);
		glUniform3fv(Ls_x_uniform, 1, lightSpecular_x);
		glUniform4fv(light_position_x_uniform, 1, lightPosition_x);
	
		glUniform3fv(Ld_y_uniform, 1, lightDiffuse_y);
		glUniform3fv(Ls_y_uniform, 1, lightSpecular_y);
		glUniform4fv(light_position_y_uniform, 1, lightPosition_y);

		glUniform3fv(Ld_z_uniform, 1, lightDiffuse_z);
		glUniform3fv(Ls_z_uniform, 1, lightSpecular_z);
		glUniform4fv(light_position_z_uniform, 1, lightPosition_z);

		//setting material properties
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(kd_uniform, 1, material_diffuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shininess);
	}
	else
	{
		glUniform1i(L_KeyPressed_uniform, 0);
	}

	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	modelMatrix = translate(0.0f, 0.0f, -2.0f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	glBindVertexArray(gVao_sphere);

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
	
	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
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

	glDetachShader(gShaderProgramObject_gourad, gVertexShaderObject);

	glDetachShader(gShaderProgramObject_gourad, gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	glDeleteProgram(gShaderProgramObject_gourad);
	gShaderProgramObject_gourad = 0;

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










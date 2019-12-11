#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <stdio.h>
#include <stdlib.h>

#include <vector>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"user32.lib")


using std::vector;


#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 256
#define S_EQUAL 0			//return value of strcmp when strings are equal

#define NR_TEXTURE_COORDS 2	//number of texture coordinates
#define NR_NORMAL_COORDS 3
#define NR_POINT_COORDS 3	//number of point coords
#define MIN_NR_FACE_TOKENS 3	//min entries of face data
#define MAX_NR_FACE_TOKENS 4	//maximum number of entries in face data

#define VK_F	0x46
#define VK_f	0x60

#define TEAPOT_X_TRANSLATE -2.1f
#define TEAPOT_Y_TRANSLATE -0.6f
#define TEAPOT_Z_TRANSLATE -5.0f

#define TEAPOT_X_SCALE_FACTOR 0.75f
#define TEAPOT_Y_SCALE_FACTOR 0.75f
#define TEAPOT_Z_SCALE_FACTOR 0.75f

#define START_ANGLE_POS 0.0f
#define END_ANGLE_POS 360.0f 
#define TEAPOT_ANGLE_INCREMENT 0.0f

#define ERRORBOX1(lpszErrorMessage,lpszCaption) {MessageBox((HWND)NULL,TEXT(lpszErrorMessage),TEXT(lpszCaption),MB_ICONERROR); ExitProcess(EXIT_FAILURE);}
#define ERRORBOX2(hwnd,lpszErrorMessage,lpszCaption) {MessageBox((HWND)NULL,TEXT(lpszErrorMessage),TEXT(lpszCaption),MB_ICONERROR); DestroyWindow(hwnd);}


bool gbIsEscapeKeyPressed = false;
bool gbIsActiveWindow = false;
bool gbToggleFullScreen = false;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

GLfloat g_angleRotate;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//vector of vector of floats to hold the vertex data
vector<vector<float>> g_vertices;

//vector of vector of floats to hold texture data
vector<vector<float>> g_texture;

//vector of vector of floats to hold the normal data
vector<vector<float>> g_normals;

//vector of vector of int to hold index data in g_vertices
vector<vector<int>> g_face_tri, g_face_texture, g_face_normals;

//handle to a mesh file
FILE *g_fp_meshfile = NULL;
FILE *g_fp_logfile = NULL;

//hold line in a file
char line[BUFFER_SIZE];




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
	void initialize(void);
	void display(void);
	void uninitialize(void);
	void update(void);

	bool bDone = true;
	HWND hwnd;
	WNDCLASSEX wndclass;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyOGLApplication");

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("MyOGLMeshLoader"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd;

	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	
	while(bDone)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{	
			if (msg.message == WM_QUIT)
			{
				bDone = false;
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
					bDone = false;
				}
			}
			update();
			display();
		}
	}

	uninitialize();
	return ((int)msg.wParam);
}

void initialize(void)
{
	void resize(int width, int height);
	void LoadMeshData(void);
	void uninitialize(void);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormat;

	g_fp_logfile = fopen("MESHLOADER.LOG", "w");

	if (!g_fp_logfile)
	{
		uninitialize();
		DestroyWindow(ghwnd);
	}
	ZeroMemory((void*)&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nVersion = 1;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;
	
	ghdc = GetDC(ghwnd);

	iPixelFormat = ChoosePixelFormat(ghdc, &pfd);
	
	if (!SetPixelFormat(ghdc, iPixelFormat, &pfd))
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (!wglMakeCurrent(ghdc,ghrc))
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	LoadMeshData();

	resize(WIN_WIDTH, WIN_HEIGHT);
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
		if (HIWORD(lParam) != 0)
		{
			gbIsActiveWindow = false;
		}
		else
		{
			gbIsActiveWindow = true;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbIsEscapeKeyPressed = true;
			break;
		case VK_F:
		case VK_f:
			if (gbToggleFullScreen == false)
			{
				ToggleFullScreen();
				gbToggleFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbToggleFullScreen = false;
			}
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}


	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}



void ToggleFullScreen(void)
{
	if (gbToggleFullScreen == false)
	{
		MONITORINFO mi;
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);
			if (GetWindowPlacement(ghwnd, &wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom
					- mi.rcMonitor.top,SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void update(void)
{
	g_angleRotate = g_angleRotate + TEAPOT_ANGLE_INCREMENT;

	if (g_angleRotate >= END_ANGLE_POS)
	{
		g_angleRotate = START_ANGLE_POS;
	}
}

void display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(TEAPOT_X_TRANSLATE, TEAPOT_Y_TRANSLATE, TEAPOT_Z_TRANSLATE);
	glScalef(TEAPOT_X_SCALE_FACTOR, TEAPOT_Y_SCALE_FACTOR, TEAPOT_Z_SCALE_FACTOR);
	glRotatef(g_angleRotate, 0.0f, 1.0f, 0.0f);
	//keep counter clockwise winding of vertices of geometry
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i != g_face_tri.size(); ++i)
	{
		glBegin(GL_TRIANGLES);
		for (int j = 0; j != g_face_tri[i].size(); j++)
		{
			int vi = g_face_tri[i][j] - 1;
			glVertex3f(g_vertices[vi][0], g_vertices[vi][1], g_vertices[vi][2]);
		}
		glEnd();
	}

	SwapBuffers(ghdc);
}

void resize(int width , int height)
{

	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 200.0f);
}

void uninitialize(void)
{
	
	if (gbToggleFullScreen)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
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

	fclose(g_fp_logfile);
	g_fp_logfile = NULL;

	DestroyWindow(ghwnd);
}





void LoadMeshData(void)
{
	void uninitialize(void);

	//first step is open the obj file

	g_fp_meshfile = fopen("sphere.OBJ", "r");

	if (!g_fp_meshfile)
	{
		uninitialize();
	}

	//strings which carry separator mark needed for strtok
	char *sep_space = " ";
	char *sep_fslash = "/";

	//Token pointers to hold the tokens returned by strtok
	char *first_token = NULL;
	char *token = NULL;
	char *face_tokens[MAX_NR_FACE_TOKENS];
	int nr_tokens;
	char *token_vertex_index = NULL;
	char *token_texture_index = NULL;
	
	//you go line by line on the 
	while (fgets(line, BUFFER_SIZE, g_fp_meshfile) != NULL)
	{

		//binding the line and getting the first token
		first_token = strtok(line, sep_space);
		fprintf(g_fp_logfile, "line: %s\n",line);


		if (strcmp(first_token,"v") == S_EQUAL)
		{
			vector<float> vec_point_coord(NR_POINT_COORDS);
			for (int i = 0; i != NR_POINT_COORDS; i++)
			{
				vec_point_coord[i] = atof(strtok(NULL, sep_space));
			}
			g_vertices.push_back(vec_point_coord);
		}

		else if(strcmp(first_token,"vt") == S_EQUAL)
		{
			//MessageBox(NULL, TEXT("Here"), TEXT("asd"), MB_OK);

			vector<float> vec_texture_coord(NR_TEXTURE_COORDS);

			for (int i = 0; i != NR_TEXTURE_COORDS; i++)
			{
				vec_texture_coord[i] = atof(strtok(NULL, sep_space));
				fprintf(g_fp_logfile, "%f\n", vec_texture_coord[i]);

			}
			g_texture.push_back(vec_texture_coord);

		}
		else if (strcmp(first_token,"vn") == S_EQUAL)
		{
			vector<float> vec_normal_coord(NR_NORMAL_COORDS);

			for(int i =0 ; i != NR_NORMAL_COORDS; i++)
			{
				vec_normal_coord[i] = atof(strtok(NULL,sep_space));
				fprintf(g_fp_logfile,"%f\n",vec_normal_coord[i]);
			}
			g_normals.push_back(vec_normal_coord);
		}
		else if (strcmp(first_token, "f") == S_EQUAL)
		{

			vector<int> triangle_vertex_indices(3), texture_vertex_indices(3);

			//initialize face_tokens array
			memset((void*)face_tokens, 0, MAX_NR_FACE_TOKENS);
			nr_tokens = 0;

			while (token = strtok(NULL,sep_space))
			{
				if (strlen(token)<3)
				{
					break;
				}
				face_tokens[nr_tokens] = token;
				nr_tokens++;
			}

			for (int i = 0; i != MIN_NR_FACE_TOKENS; ++i)
			{
				token_vertex_index = strtok(face_tokens[i], sep_fslash);
				token_texture_index = strtok(NULL, sep_fslash);
				triangle_vertex_indices[i] = atoi(token_vertex_index);
				texture_vertex_indices[i] = atoi(token_texture_index);
			}
			g_face_tri.push_back(triangle_vertex_indices);
			g_face_texture.push_back(texture_vertex_indices);
			

			if (nr_tokens == MAX_NR_FACE_TOKENS)
			{
				triangle_vertex_indices[1] = triangle_vertex_indices[2];
				triangle_vertex_indices[2] = atoi(token_vertex_index);
				
				texture_vertex_indices[1] = texture_vertex_indices[2];
				texture_vertex_indices[2] = atoi(token_texture_index);

				token_vertex_index = strtok(face_tokens[MAX_NR_FACE_TOKENS - 1], sep_fslash);
				token_texture_index = strtok(NULL, sep_fslash);


				//add constructed vectors to global vectors
				g_face_tri.push_back(triangle_vertex_indices);
				g_face_texture.push_back(texture_vertex_indices);
			}

		}

		memset((void*)line, (int)'\0', BUFFER_SIZE);
	}

	fclose(g_fp_meshfile);

	fprintf(g_fp_logfile, "g_vertices: %zu g_texture:%zu g_normals:%zu g_face_tri:%zu\n", g_vertices.size(), g_texture.size(),g_normals.size(), g_face_tri.size());
}











#include <freeglut.h>
#include <gl\GL.h>

#pragma comment(lib,"opengl32.lib")


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

bool gbFullScreen = false;

int main(int argc, char *argv[])
{
	void initialize(void);
	void display(void);
	void keyboard(unsigned char, int, int);
	void resize(int, int);
	void uninitialize(void);

	int wSize = GetSystemMetrics(SM_CXFULLSCREEN);
	int hSize = GetSystemMetrics(SM_CYFULLSCREEN);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(WIN_WIDTH,WIN_HEIGHT);
	glutInitWindowPosition(200,200);
	glutCreateWindow("myFirstGlutWindow");

	initialize();
	//register callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutCloseFunc(uninitialize);
	glutReshapeFunc(resize);

	glutMainLoop();
	//return 0;
}

void initialize(void)
{
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
}

void uninitialize(void)
{
	//code
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	case 'f':
	case 'F':
		if (gbFullScreen == false)
		{
			glutFullScreen();
			gbFullScreen = true;
		}
		else
		{
			glutLeaveFullScreen();
			gbFullScreen = false;
		}
		break;
	default:
		break;
	}
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}




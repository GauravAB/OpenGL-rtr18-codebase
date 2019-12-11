#include <freeglut.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"GLU32.lib")

struct GLPoint
{
	GLfloat x, y;
};


void drawpoint(GLfloat, GLfloat);
void koch(GLint length, GLPoint start, GLPoint end);
void koch_curve(void);


int main(int argc, char *argv[])
{
	void initialize(void);
	void resize(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Sierpinski");

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	
	initialize();

	glutMainLoop();
	//return 0
}



void initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glPointSize(2.0);
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-100.0, 100.0, -100.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}



void display(void)
{
	void sierpinski(void);

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 0.0f);
	
	//sierpinski();
	GLPoint triangle[3] = { {0.0,0.0} , {-50.0,-50.0} , {50.0,-50.0} };

	kochCurve(triangle, 0);
	
	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	case 'f':
		glutFullScreen();
		break;
	default:
		break;
	}
}


void drawpoint(GLfloat x , GLfloat y)
{
	glBegin(GL_POINTS);
		glVertex2f(x, y);
	glEnd();
}


void sierpinski()
{
	GLPoint T[3] = { {0,50},{-50,-50},{50,-50} };

	int index = rand() % 3;

	GLPoint point = T[index];

	for (int i = 0; i < 55000; i++)
	{
		index = rand() % 3;
		point.x = (point.x + T[index].x) / 2;
		point.y = (point.y + T[index].y) / 2;

		drawpoint(point.x , point.y);
	}
}

void drawLine(GLPoint x1 , GLPoint x2)
{
	//we use line strip
	glBegin(GL_LINES);
	glVertex2f(x1.x, x1.y);
	glVertex2f(x2.x, x2.y);
	glEnd();
}

void kochCurve(GLPoint triangle[3], int divisions)
{

}








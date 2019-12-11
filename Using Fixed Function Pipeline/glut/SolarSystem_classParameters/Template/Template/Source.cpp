#include <freeglut.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"GLU32.lib")

static int year = 0, day = 0;
GLUquadric *quadric = NULL;


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
	glutCreateWindow("Solar System");

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	
	initialize();

	glutMainLoop();
	
	
	return 0;
}





void initialize(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
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
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	quadric = gluNewQuadric();
	glColor3f(1.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.75, 30, 30);	//draw sun
	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);
	glTranslatef(2.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)day, 0.0f, 1.0f, 0.0f);
	glColor3f(0.3f, 0.7f, 1.0f);
	gluSphere(quadric, 0.25, 30, 30);

	glPopMatrix();
	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'f':
		glutFullScreen();
		break;
	case 27:
		glutLeaveMainLoop();
		break;
	case 'd':
		day += 15;
		if (day >= 360)
		{
			day = 0;
		}
		glutPostRedisplay();
		break;
	case 'D':
		day = (day-2) % 360;
		glutPostRedisplay();
		break;
	case 'y':
		year += 5;
		if (year >= 360)
		{
			year = 0;
		}
		glutPostRedisplay();
		break;
	case 'Y':
		year = (year - 5) % 360;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}


















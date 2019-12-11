#include <freeglut.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"GLU32.lib")

static int year = 0, day = 0;


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
	//return 0
}



void initialize(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}



void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0.0f, 0.0f, (GLfloat)width, (GLfloat)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);
	//saving this state and creating something new
	glPushMatrix();
	glutWireSphere(1.0f, 40.0f, 30.0f);	//draw sun
	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);	//rotate along y axis
	glTranslatef(2.0f, 0.0f, 0.0f);				//revolution with radius 2
	glRotatef((GLfloat)day, 0.0f, 1.0f, 0.0f);	//rotation along its own axis 
	glutWireSphere(0.2f, 20.0f, 10.0f);	//draw earth

	glPopMatrix();
	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	case 'd':
		day += 15.0;
		if (day >= 360.0)
		{
			day = 0.0;
		}
		glutPostRedisplay();
		break;
	case 'D':
		day = (day-2) % 360;
		glutPostRedisplay();
		break;
	case 'y':
		year += 5.0;
		if (year >= 360.0)
		{
			year = 0.0;
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


















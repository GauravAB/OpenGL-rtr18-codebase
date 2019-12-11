#include <freeglut.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include "stdafx.h"


#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")


int main(int argc, char*argv[])
{
	void display(void);
	void keyboard(unsigned char, int, int);
	void resize(int, int);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(800, 600);
	glutCreateWindow("TemplateWindow");


	//Register callbacks here
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);


	glutMainLoop();

	//return 0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}



void resize(int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	glMatrixMode(GL_PROJECTION_MATRIX);
	glLoadIdentity();

	gluOrtho2D(-100.0, 100.0, -100.0, 100.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
		default:
			break;
	}
}



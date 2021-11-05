#include <Windows.h>
#include <iostream>
#include "parser.h"
#include <fstream>
#include <gl\glew.h>
#include <gl\GL.h>


#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")

using std::ofstream;
using std::endl;







int main(void)
{
	ofstream file("vertices.txt");
	ofstream file1("textures.txt");
	ofstream file2("normals.txt");
	ofstream file3("elements.txt");


	Parser parse;
	parse.parseObj("Sphere.obj");

	float *vertices;
	float *textures;
	float *normals;
	int *elements;

	parse.getModelVertexData(&vertices, &textures, &normals, &elements);
	
	int numElements = parse.getNumElements();
	int numVertices = parse.getNumVertices();

	for (int i = 0; i < numVertices; i++)
	{
		file << i << " " << vertices[i] << endl;
	}

	for (int i = 0; i < numVertices; i++)
	{
		file1 << i << " " << textures[i] << endl;
	}

	for (int i = 0; i < numVertices; i++)
	{
		file2 << i << " " << normals[i] << endl;
	}

	for (int i = 0; i < numElements; i++)
	{
		file3 << i << " " << elements[i] << endl;
	}


	file.close();
	file1.close();
	file2.close();
	file3.close();

	return 0;
}




#ifndef __PARSER_H_
#define __PARSER_H_

#include <Windows.h>


const unsigned int maxLine = 100;
//points = x,y,z 
const unsigned int point_coordinates = 3;
//textures = u, v 
const unsigned int texture_coordinates = 2;
//normals = x, y, z
const unsigned int normal_coordinates = 3;
const unsigned int elements_size = 3;
const unsigned int number_of_elements = 3;





class Parser
{
public:
	Parser();
	~Parser();
	int parseObj(const char*);
	void addTriangle(float[3][3], float[3][2], float[3][3]);
	void getModelVertexData(float **vertices, float **textures, float **normals, int** Elements);
	void processModelData(void);
	unsigned int getNumVertices();
	unsigned int getNumElements();


private:
	unsigned int numVertices;
	unsigned int numElements;

	float *vertexPositions;
	float *vertexNormals;
	float *vertexTextures;
	int *triangleIndices;

	float *modelPositions;
	float *modelTextures;
	float *modelNormals;
	int *modelElements;

	unsigned int Vertices;
	unsigned int Normals;
	unsigned int Textures;
	unsigned int numIndices;
	unsigned int maxElements;
};





#endif
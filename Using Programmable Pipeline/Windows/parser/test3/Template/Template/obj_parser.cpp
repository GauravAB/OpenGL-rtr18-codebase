
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "parser.h"



using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;


ofstream file("addTriangle_log.txt");

void trace(const char* msg)
{
	MessageBoxA(NULL, msg, "Obj_parser", MB_OK);
}


Parser::~Parser()
{
	//MessageBox(NULL, L"destructor called", "error", MB_OK);
	trace("destructor called");
}

Parser :: Parser():numVertices(0),numElements(0),Vertices(0),Textures(0),Normals(0),numIndices(0)
{
	vertexPositions = NULL;
	vertexNormals = NULL;
	vertexTextures = NULL;
	triangleIndices = NULL;

	modelElements = NULL;
	modelNormals = NULL;
	modelPositions = NULL;
	modelTextures = NULL;
}


int Parser :: parseObj(const char* szFileName)
{

	//delimeters
	//spaces
	const char space_delimeter[] = " ";
	//slash
	const char slash_delimeter[] = "/";

	char *token;
	char* tokenElement[3];

	ofstream dataFile("dataFile.txt");

	char lineBuffer[maxLine];

	if(!szFileName)
	{
		trace("File path is NULL");
		return -1;
	}

	ifstream inFile(szFileName);
	
	if(!inFile)
	{
		trace("error in opening file");
		return -1;
	}

	while (inFile.getline(lineBuffer, maxLine))
	{
		if (token = strtok(lineBuffer, space_delimeter))
		{
			if (strcmp(token, "v") == 0)
			{
				float vertex_points[point_coordinates];

				for (int i = 0; i < point_coordinates; i++)
				{
					vertex_points[i] = atof(strtok(NULL, space_delimeter));
					//dataFile<<" "<<vertex_points[i];
				}
				float *ptr = vertexPositions;
				if (ptr == NULL)
				{
					ptr = (float*)calloc(point_coordinates, sizeof(float));
					vertexPositions = ptr;

					for (int i = 0; i < point_coordinates; i++)
					{
						vertexPositions[i] = vertex_points[i];
					}

				}
				else
				{

					vertexPositions = (float*)realloc(ptr, sizeof(float)*((Vertices + 1)*point_coordinates));
						
					for (int i = 0; i != point_coordinates; i++)
					{
						vertexPositions[(Vertices * point_coordinates) + i] = vertex_points[i];
					}
				}

				Vertices++;
			}
			
			else if (strcmp(token, "vt") == 0)
			{
				float vertex_texture[texture_coordinates];

				for (int i = 0; i < texture_coordinates; i++)
				{
					vertex_texture[i] = atof(strtok(NULL, space_delimeter));
					//dataFile<<" "<<vertex_textures[i];
				}
				float *ptr = vertexTextures;
				if (ptr == NULL)
				{
					ptr = (float*)calloc(texture_coordinates, sizeof(float));
					vertexTextures = ptr;
					for (int i = 0; i < texture_coordinates; i++)
					{
						vertexTextures[i] = vertex_texture[i];
					}
				}
				else
				{

					vertexTextures = (float*)realloc(ptr, sizeof(float)*((Textures+1)*texture_coordinates));

					vertexTextures[Textures*texture_coordinates] = vertex_texture[0];
					vertexTextures[(Textures*texture_coordinates)+1] = vertex_texture[1];

				}

				//dataFile<<endl;
				Textures++;
			}
			
			else if(strcmp(token,"vn") ==0)
			{
				float vertex_normals[normal_coordinates];

				for(int i = 0 ; i < normal_coordinates; i++)
				{
					vertex_normals[i] = atof(strtok(NULL,space_delimeter));
					//dataFile<<" "<<vertex_normals[i];
				}
				float *ptr = vertexNormals;
				if (ptr == NULL)
				{
					vertexNormals = (float*)calloc(normal_coordinates, sizeof(float));

					for (int i = 0; i < point_coordinates; i++)
					{
						vertexNormals[i] = vertex_normals[i];
					}

				}
				else
				{

					vertexNormals = (float*)realloc(ptr, sizeof(float)* ((Normals + 1)*normal_coordinates));

					for (int i = 0; i < normal_coordinates; i++)
					{
						vertexNormals[ (Normals*normal_coordinates) + i] = vertex_normals[i];
					}
				}
					//dataFile<<endl;
					Normals++;
			}
			else if(strcmp(token,"f") ==0)
			{
				//each element consist of 1 point , texture and a normal
				float face_index[elements_size * number_of_elements];

				tokenElement[0] = strtok(NULL,space_delimeter);

				tokenElement[1] = strtok(NULL,space_delimeter);

				tokenElement[2] = strtok(NULL,space_delimeter);

				//023 are vertices 345 are textures and 678 are normals
				face_index[0] = atof(strtok(tokenElement[0],slash_delimeter));
				face_index[3] = atof(strtok(NULL,slash_delimeter));
				face_index[6] = atof(strtok(NULL,slash_delimeter));

				face_index[1] = atof(strtok(tokenElement[1],slash_delimeter));
				face_index[4] = atof(strtok(NULL,slash_delimeter));
				face_index[7] = atof(strtok(NULL,slash_delimeter));

				face_index[2] = atof(strtok(tokenElement[2],slash_delimeter));
				face_index[5] = atof(strtok(NULL,slash_delimeter));
				face_index[8] = atof(strtok(NULL,slash_delimeter));


				int *ptr = triangleIndices;
				if (ptr == NULL)
				{
					
					triangleIndices = (int*)calloc(number_of_elements*elements_size, sizeof(int));

					for (int i = 0; i < number_of_elements*elements_size ; i++)
					{
						triangleIndices[i] = face_index[i];
						dataFile << face_index[i] << endl;
					}
				}
				else
				{
					triangleIndices = (int*)realloc(ptr,sizeof(int)*( (numIndices+1) * (number_of_elements*elements_size)));
					//dataFile<<"sizeof triangleIndices: " << sizeof(triangleIndices)<<" ";
					for (int i = 0; i != 9; i++)
					{
						triangleIndices[(numIndices * 9) + i] = face_index[i];
					}
				}
				//	dataFile<<endl;
				numIndices++;
			}
		}
	}
		
			dataFile << "number of vertices: " << Vertices << endl;
			dataFile << "number of textures: " << Textures << endl;
			dataFile << "number of normals : " << Normals << endl;
			dataFile << "number of indices : " << numIndices << endl;
			dataFile.close();

		return 0;
}

void Parser::getModelVertexData(float** vert, float** norm, float** tex, int **elements)
{

	maxElements = numIndices * 3;

	
	processModelData();
	

		*vert = modelPositions;
		*tex = modelTextures;
		*norm = modelNormals;
		*elements = modelElements;

}


void Parser :: processModelData(void)
{

	ofstream fileout("processModelDatalog.txt");
	//vertices for one triangle 
    float vert[3][3];
	float tex[3][2];
	float norm[3][3];

	

	fileout << triangleIndices[(0 * 9) + 0] << vertexPositions[((triangleIndices[(0 * 9) + 0] - 1) * 3)] << vertexPositions[((triangleIndices[(0 * 9) + 0] - 1) * 3) + 1] << vertexPositions[((triangleIndices[(0 * 9) + 0] - 1) * 3) + 2] << endl<<endl;

		//till we cover data from all indices
		for(int i = 0 ; i != numIndices; i++)
		{

			//rows interation
			//fill one element at a time
			for(int j = 0 ; j != 3 ; j++)
			{	
				vert[j][0] = vertexPositions[((triangleIndices[(i * 9) + j] - 1) * 3) + 0];
				vert[j][1] = vertexPositions[((triangleIndices[(i * 9) + j] - 1) * 3) + 1];
				vert[j][2] = vertexPositions[((triangleIndices[(i * 9) + j] - 1) * 3) + 2];
				//fileout << vert[j][0] << " " << vert[j][1] << " " << vert[j][2] << endl;
				fileout << triangleIndices[(i * 9) + j]  << vertexPositions[((triangleIndices[(i * 9) + j] - 1) * 3)]<< vertexPositions[((triangleIndices[(i * 9) + j] - 1) * 3)+1]<<vertexPositions[((triangleIndices[(i * 9) + j] - 1) * 3)+2]<<endl;

				tex[j][0] = vertexTextures[((triangleIndices[((i * 9) + j) + 3] - 1) * 2) + 0];
				tex[j][1] = vertexTextures[((triangleIndices[((i * 9) + j) + 3] - 1) * 2) + 1];
				
				norm[j][0] = vertexNormals[((triangleIndices[((i * 9) + j) + 6] - 1) * 3) + 0];
				norm[j][1] = vertexNormals[((triangleIndices[((i * 9) + j) + 6] - 1) * 3) + 1];
				norm[j][2] = vertexNormals[((triangleIndices[((i * 9) + j) + 6] - 1) * 3) + 2];
			
				//fileout << "triangleIndex: " << (triangleIndices[((i * 9) + j+6 )]) << " " << "vertexNormals: " << vertexNormals[((triangleIndices[(i * 9) + j+6]-1) * 3) + 0] << endl;
				//fileout << "triangleIndex: " << (triangleIndices[((i * 9) + j+6 )]) << " " << "vertexNormals: " << vertexNormals[((triangleIndices[(i * 9) + j+6]-1) * 3) + 1] << endl;
			    //fileout << "triangleIndex: " << (triangleIndices[((i * 9) + j+6 )]) << " " << "vertexNormals: " << vertexNormals[((triangleIndices[(i * 9) + j+6]-1) * 3) + 2] << endl;

			}
			//fileout << endl;
			addTriangle(vert,tex,norm);
		}

		fileout<<maxElements<<endl;
		fileout << numVertices << endl;
		fileout<<numElements<<endl;
		fileout.close();

}



void Parser::addTriangle(float single_vertexPos[3][3],float single_texture[3][2] , float single_normal[3][3])
{

	bool isFoundIdentical(float val1, float val2, float diff);
	void normalizeVector(float normal[]);

	float diff = 0.000001f;
	int i , j;

	if (modelElements == NULL)
	{
		modelElements = (int*)malloc(sizeof(int) * 3);
		modelPositions = (float*)malloc(sizeof(float) * 3 * 3);
		modelTextures = (float*)malloc(sizeof(float) * 3 * 2);
		modelNormals = (float*)malloc(sizeof(float) * 3 * 3);
	}
	else 
	{
		modelElements = (int*)realloc(modelElements, sizeof(int)*(numElements + 3));
		modelPositions = (float*)realloc(modelPositions, sizeof(float)*(numVertices * 3)*elements_size*point_coordinates);
		modelNormals = (float*)realloc(modelNormals, sizeof(float)*(numVertices * 3)*elements_size*normal_coordinates);
		modelTextures = (float*)realloc(modelTextures, sizeof(float)*(numVertices * 3)*elements_size*texture_coordinates);
	}
		

	//normals should be of unit length
	normalizeVector(single_normal[0]);
	normalizeVector(single_normal[1]);
	normalizeVector(single_normal[2]);

	
	for( i = 0 ; i != 3 ; i++)
	{
		
		for (j = 0; j != numVertices ; j++)
		{

			if (isFoundIdentical(modelPositions[j * 3], single_vertexPos[i][0], diff) &&
				isFoundIdentical(modelPositions[(j * 3) + 1], single_vertexPos[i][1], diff) &&
				isFoundIdentical(modelPositions[(j * 3) + 2], single_vertexPos[i][2], diff) &&

				isFoundIdentical(modelTextures[j * 2], single_texture[i][0], diff) &&
				isFoundIdentical(modelTextures[(j * 2)+1], single_texture[i][1], diff) &&

				isFoundIdentical(modelNormals[j * 3], single_normal[i][0], diff) &&
				isFoundIdentical(modelNormals[(j * 3) + 1], single_normal[i][1], diff) &&
				isFoundIdentical(modelNormals[(j * 3) + 2], single_normal[i][2], diff))
			{
				//add the common element to the entry and break
				modelElements[numElements] = j;
				numElements++;
				break;
			}

		}
			
			//if not common and unique add the element
			if (j == numVertices && numVertices < maxElements && numElements < maxElements)
			{
				
				modelPositions[numVertices *3] = single_vertexPos[i][0];
				modelPositions[(numVertices *3)+1] = single_vertexPos[i][1];
				modelPositions[(numVertices *3)+2] = single_vertexPos[i][2];
				file << modelPositions[numVertices * 3] << modelPositions[(numVertices * 3) + 1] << modelPositions[(numVertices * 3) + 2] << endl;

				modelNormals[numVertices *3] = single_normal[i][0];
				modelNormals[(numVertices *3)+1] = single_normal[i][1];
				modelNormals[(numVertices *3)+2] = single_normal[i][2];

				modelTextures[numVertices *2] = single_texture[i][0];
				modelTextures[(numVertices *2)+1] = single_texture[i][1];
				
				modelElements[numElements] = numVertices;

				numElements++;
				numVertices++;
			}	
	 }
}



void normalizeVector(float v[])
{
	//squaring the vector length
	float squareVectorLength = (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

	float squareRootOfSquareVectorLength = (float)sqrt(squareVectorLength);

	//normalize each position
	v[0] = v[0] * 1.0f/squareRootOfSquareVectorLength;
	v[1] = v[1] * 1.0f/squareRootOfSquareVectorLength;
	v[2] = v[2] * 1.0f/squareRootOfSquareVectorLength;
}


bool isFoundIdentical(float val1 , float val2 , float diff )
{
	if(abs(val1 - val2) < diff)
	{
		return true;
	}
	else
	{
		return false;
	}
}


unsigned int Parser::getNumVertices()
{
	return numVertices;
}

unsigned int Parser::getNumElements()
{
	return numElements;
}











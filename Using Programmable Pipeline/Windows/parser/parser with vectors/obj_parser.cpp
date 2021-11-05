#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>


using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;

void trace(const char* msg)
{
	cout<<"Parser: "<<msg<<endl;
}

int parseObj(const char*);
bool isFoundIdentical(float,float,float);
void addTriangle(float[3][3] , float[3][2] , float[3][3]);

//globals
	static vector<vector<float>> vertices;
	static vector<vector<float>> normals;
	static vector<vector<float>> textures;
	static vector<vector<float>> face_indices;

	unsigned int Vertices =0; 
	unsigned int Normals =0; 
	unsigned int Textures = 0; 
	unsigned int numIndices =0;
	unsigned int maxElements = 0;

	unsigned int numVertices = 0;
	unsigned int numElements = 0;


	vector<float> modelPositionCoords;
	vector<float> modelNormalCoords;
	vector<float> modelTexCoords;
	vector<short> modelElements;

int main(void)
{
	if( 0 != parseObj("Sphere.obj"))
	{
		trace("processObj failed");
	}
	else
	{
		trace("parseObj succeeded");
	}

	return 0;
}



int parseObj(const char* szFileName)
{

	const unsigned int maxLine = 100;
	//points = x,y,z textures = u,v normals = x,y,z
	const unsigned int point_coordinates = 3;
	const unsigned int texture_coordinates = 2;
	const unsigned int normal_coordinates =3 ;
	const unsigned int elements_size = 3;
	const unsigned int number_of_elements = 3;

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

	while(inFile.getline(lineBuffer,maxLine))
	{
		if(token = strtok(lineBuffer,space_delimeter))
		{
			if(strcmp(token,"v") == 0)
			{
				vector<float> vertex_points(point_coordinates);

				for(int i = 0 ; i < point_coordinates; i++)
				{
					vertex_points[i] = atof(strtok(NULL,space_delimeter));
					//dataFile<<" "<<vertex_points[i];
				}
					vertices.push_back(vertex_points);
					//dataFile<<endl;
					Vertices++;
			}
			else if(strcmp(token,"vt") == 0)
			{
				vector<float> vertex_textures(texture_coordinates);

				for(int i = 0; i < texture_coordinates; i++)
				{
					vertex_textures[i] = atof(strtok(NULL,space_delimeter));
					//dataFile<<" "<<vertex_textures[i];
				}
				textures.push_back(vertex_textures);
				//dataFile<<endl;
				Textures++;
			}
			else if(strcmp(token,"vn") ==0)
			{
				vector<float> vertex_normals(normal_coordinates);

				for(int i = 0 ; i < normal_coordinates; i++)
				{
					vertex_normals[i] = atof(strtok(NULL,space_delimeter));
					//dataFile<<" "<<vertex_normals[i];
				}
					normals.push_back(vertex_normals);
					//dataFile<<endl;
					Normals++;
			}
			else if(strcmp(token,"f") ==0)
			{
				
				//each element consist of 1 point , texture and a normal
				vector<float> face_index(elements_size * number_of_elements);

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

				face_indices.push_back(face_index);
			//	dataFile<<endl;
				numIndices++;
			}
		}

	}

	dataFile<<"number of vertices: "<<Vertices<<endl;	
	dataFile<<"number of textures: "<<Textures<<endl;
	dataFile<<"number of normals : "<<Normals<<endl;
	dataFile<<"number of indices : "<<numIndices<<endl;

	dataFile.close();

	void processModelData(void);
	processModelData();
	return 0;
}



void getModelVertexData(float *vertices , float *textures , float *normals , short numElements)
{
	void processModelData(void);


	processModelData();
}


void processModelData(void)
{

	ofstream fileout("triangleData.txt");
	maxElements  = numIndices * 3;

	for(int i =0 ; i != maxElements ; i++)
	{
		modelElements.push_back(0);	
	}

	for(int i =0 ; i != Vertices; i++)
	{
		modelPositionCoords.push_back(0);
	}

	for(int i =0 ; i != Normals; i++)
	{
		modelNormalCoords.push_back(0);
	}

	for(int i =0 ; i != Textures; i++)
	{
		modelTexCoords.push_back(0);
	}


	//vertices for one triangle 
	static float vert[3][3];
	static float tex[3][2];
	static float norm[3][3];


		//till we cover data from all indices
		for(int i = 0 ; i < face_indices.size() ; i++)
		{
			//rows interation
			for(int j = 0 ; j < 3 ; j++)
			{	
				//fileout << " " << face_indices[i][j];
				vert[j][0] = vertices[(face_indices[i][j])-1 ][0];
				vert[j][1] = vertices[(face_indices[i][j])-1 ][1];
				vert[j][2] = vertices[(face_indices[i][j])-1 ][2];
		
				tex[j][0] = textures[ (face_indices[i][j+3])-1 ][0];
				tex[j][1] = textures[ (face_indices[i][j+3])-1 ][1];
			
				norm[j][0] = normals[ (face_indices[i][j+6])-1 ][0];
				norm[j][1] = normals[ (face_indices[i][j+6])-1 ][1];
				norm[j][2] = normals[ (face_indices[i][j+6])-1 ][2];
													
			}

			addTriangle(vert,tex,norm);
		}
		fileout<<maxElements<<endl;
		fileout<<numVertices<<endl;
		fileout<<numElements<<endl;
		fileout.close();
}



void addTriangle(float single_vertexPos[3][3],float single_texture[3][2] , float single_normal[3][3])
{


	void normalizeVector(float normal[]);

	float diff = 0.000001f;
	int i , j;


	//normals should be of unit length
	normalizeVector(single_normal[0]);
	normalizeVector(single_normal[1]);
	normalizeVector(single_normal[2]);

	
	for( i = 0 ; i < 3 ; i++)
	{
		for( j = 0 ; j < numVertices; j++)
		{
			if    (	isFoundIdentical(modelPositionCoords[j * 3], single_vertexPos[i][0], diff) &&
					isFoundIdentical(modelPositionCoords[(j*3)+1], single_vertexPos[i][1], diff) &&
					isFoundIdentical(modelPositionCoords[(j*3)+2], single_vertexPos[i][2], diff) &&
				
					isFoundIdentical(modelTexCoords[j*3], single_texture[i][0], diff) &&
					isFoundIdentical(modelTexCoords[(j*3)+1], single_texture[i][1],diff) &&
					isFoundIdentical(modelTexCoords[(j*3)+2], single_texture[i][2],diff) &&
				
					isFoundIdentical(modelNormalCoords[j*3], single_normal[i][0],diff) &&
					isFoundIdentical(modelNormalCoords[(j*3)+1], single_normal[i][1],diff) &&
					isFoundIdentical(modelNormalCoords[(j*3)+2], single_normal[i][2],diff))
			{
				//add the common element to the entry and break
				modelElements[numElements] = (short)j;
				numElements++;
				break;
			}
		}

		
		//if not common and unique add the element
		if(j == numVertices && numVertices < maxElements && numElements < maxElements)
		{
			modelPositionCoords.push_back(single_vertexPos[i][0]);
			modelPositionCoords.push_back(single_vertexPos[i][1]);
			modelPositionCoords.push_back(single_vertexPos[i][2]);
			
			modelNormalCoords.push_back(single_normal[i][0]);
			modelNormalCoords.push_back(single_normal[i][1]);
			modelNormalCoords.push_back(single_normal[i][2]);

			modelTexCoords.push_back(single_texture[i][0]);
			modelTexCoords.push_back(single_texture[i][1]);

			modelElements[numElements] = (short)numVertices;
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




#pragma once

extern "C" void getSphereVertexData(float spherePositionCoords[1146], float sphereNormalCoords[1146], float sphereTexCoords[764], unsigned short sphereElements[2280]);

extern "C" unsigned int getNumberOfSphereVertices(void);

extern "C" unsigned int getNumberOfSphereElements(void);

unsigned int getNumVertices(void);
unsigned int getNumElements(void);


extern vector<vector<float>> vertices;
extern vector<vector<float>> normals;
extern vector<vector<float>> textures;
extern vector<vector<float>> face_indices;

extern vector<float> modelPositionCoords;
extern vector<float> modelNormalCoords;
extern vector<float> modelTexCoords;
extern vector<short> modelElements;

extern unsigned int numVertices;
extern unsigned int numElements;


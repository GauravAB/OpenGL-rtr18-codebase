#pragma once

#ifdef MODELPARSER_EXPORTS
#define MODELPARSER_API __declspec(dllexport)
#else
#define MODELPARSER_API __declspec(dllimport)
#endif



class IModelLoader {
public:
	///<summary>The difference for comparison of equal coordinates.</summary>
	float diff = 0.00001f;
	///<summary><c>LoadMeshData</c> returns the parsed data from given .OBJ model.</summary>
	///<param name='*fileName'>The path of the model file.</param>
	///<param name='**out_vertices'>The pointer to pointer of type float to hold vertices data.</param>
	///<param name='**out_textures'>The pointer to pointer of type float to hold texture data.</param>
	///<param name='**out_normals'>The pointer to pointer of type float to hold normal data.</param>
	///<param name='**out_elements'>The pointer to pointer of type float to hold element indices data.</param>
	///<param name='**out_num_vertices'>The pointer of type unsigned int to hold number of vertices.</param>
	///<param name='**out_num_textures'>The pointer of type unsigned int to hold number of textures.</param>
	///<param name='**out_num_normals'>The pointer of type unsigned int to hold number of normals.</param>
	///<param name='**out_num_elements'>The pointer of type unsigned int to hold number of elements.</param>
	virtual void LoadMeshData(char *fileName, float **out_vertices, float **out_textures, float **out_normals, int **out_elements, int *out_num_vertices, int *out_num_textures, int *out_num_normals, int *out_num_elements) = 0;
	virtual void Release(void) {
		delete this;
	}
};

MODELPARSER_API IModelLoader *getModelLoader();
#pragma once

#include <vector>
#include "ModelParser.h"

#define BUFFER_SIZE 256
#define NR_POINTS_COORDS 3
#define S_EQUAL 0
#define NR_TEXTURE_COORDS 2
#define NR_NORMAL_COORDS 3
#define NR_FACE_TOKENS 3

IModelLoader *gpModelLoader = NULL;
class ModelLoader:public IModelLoader
{
private:
	float *model_vertices, *model_normals, *model_textures;
	int *model_elements, model_num_vertices, model_num_normals, model_num_textures, model_num_elements;

public:void LoadMeshData(char *fileName, float **out_vertices, float **out_textures, float **out_normals, int **out_elements, int *out_num_vertices, int *out_num_textures, int *out_num_normals, int *out_num_elements)
{
	// Variable declarations
	FILE *meshFile = NULL;
	model_vertices = model_normals = model_textures = NULL;
	model_elements = NULL;
	model_num_elements = model_num_normals = model_num_textures = model_num_vertices = 0;
	meshFile = fopen(fileName, "r");
	if (!meshFile)
		exit;

	char *sep_space = " ";
	char *sep_fslash = "/";
	char *first_token = NULL;
	char *token = NULL;
	char line[BUFFER_SIZE];

	char *face_tokens[3];
	int nr_tokens;

	char *token_vertex_index = NULL;
	char *token_texture_index = NULL;
	char *token_normal_index = NULL;

	std::vector<std::vector<float>> g_vertices;

	std::vector<std::vector<float>> g_texture;

	std::vector<std::vector<float>> g_normals;

	std::vector<std::vector<int>> g_face_tri, g_face_texture, g_face_normals;

	while (fgets(line, BUFFER_SIZE, meshFile) != NULL)
	{
		first_token = strtok(line, sep_space);
		if (strcmp(first_token, "v") == S_EQUAL)
		{
			std::vector<float> vec_point_coord(NR_POINTS_COORDS);
			for (int i = 0; i != NR_POINTS_COORDS; i++)
				vec_point_coord[i] = atof(strtok(NULL, sep_space));
			g_vertices.push_back(vec_point_coord);
		}

		else if (strcmp(first_token, "vt") == S_EQUAL)
		{
			std::vector<float> vec_texture_coord(NR_TEXTURE_COORDS);

			for (int i = 0; i != NR_TEXTURE_COORDS; i++)
			{
				vec_texture_coord[i] = atof(strtok(NULL, sep_space));
			}
			g_texture.push_back(vec_texture_coord);
		}

		else if (strcmp(first_token, "vn") == S_EQUAL)
		{
			std::vector<float> vec_normal_coord(NR_NORMAL_COORDS);
			for (int i = 0; i != NR_NORMAL_COORDS; i++)
				vec_normal_coord[i] = atof(strtok(NULL, sep_space));
			g_normals.push_back(vec_normal_coord);
		}

		else if (strcmp(first_token, "f") == S_EQUAL)
		{
			std::vector<int> triangle_vertex_indices(3), texture_vertex_indices(3), normal_vertex_indices(3);
			memset((void*)face_tokens, 0, NR_FACE_TOKENS);
			nr_tokens = 0;
			while (token = strtok(NULL, sep_space))
			{
				if (strlen(token) < 3)
					break;
				face_tokens[nr_tokens] = token;
				nr_tokens++;
			}

			for (int i = 0; i != NR_FACE_TOKENS; i++)
			{
				token_vertex_index = strtok(face_tokens[i], sep_fslash);
				token_texture_index = strtok(NULL, sep_fslash);
				token_normal_index = strtok(NULL, sep_fslash);
				triangle_vertex_indices[i] = atoi(token_vertex_index);
				texture_vertex_indices[i] = atoi(token_texture_index);
				normal_vertex_indices[i] = atoi(token_normal_index);
			}
			g_face_tri.push_back(triangle_vertex_indices);
			g_face_texture.push_back(texture_vertex_indices);
			g_face_normals.push_back(normal_vertex_indices);
		}
		memset((void*)line, (int)'\0', BUFFER_SIZE);
	}

	float single_vertices[3][3];
	float single_texture[3][2];
	float single_normals[3][3];

	for (int i = 0; i != g_face_tri.size(); i++)
	{
		for (int j = 0; j != g_face_tri[i].size(); j++)
		{
			int vi = g_face_tri[i][j] - 1;
			int ni = g_face_normals[i][j] - 1;
			int ti = g_face_texture[i][j] - 1;
			single_texture[j][0] = g_texture[ti][0];
			single_texture[j][1] = g_texture[ti][1];
			single_normals[j][0] = g_normals[ni][0];
			single_normals[j][1] = g_normals[ni][1];
			single_normals[j][2] = g_normals[ni][2];
			single_vertices[j][0] = g_vertices[vi][0];
			single_vertices[j][1] = g_vertices[vi][1];
			single_vertices[j][2] = g_vertices[vi][2];
		}
		this->addTriangle(single_vertices, single_texture, single_normals);
	}

	*out_vertices = model_vertices;
	*out_normals = model_normals;
	*out_textures = model_textures;
	*out_elements = model_elements;
	*out_num_vertices = model_num_vertices;
	*out_num_normals = model_num_vertices;
	*out_num_textures = model_num_vertices;
	*out_num_elements = model_num_elements;

	fclose(meshFile);
	meshFile = NULL;
}

private:void addTriangle(float vertex[3][NR_POINTS_COORDS], float texture[3][NR_TEXTURE_COORDS], float normal[3][NR_NORMAL_COORDS])
	   {
		   int j = 0;
		   model_vertices = (float*)realloc(model_vertices, (model_num_vertices * 3 + (NR_POINTS_COORDS * 3)) * sizeof(float));
		   model_textures = (float*)realloc(model_textures, (model_num_vertices * 2 + (NR_TEXTURE_COORDS * 3))*sizeof(float));
		   model_normals = (float*)realloc(model_normals, (model_num_vertices * 3 + (NR_NORMAL_COORDS * 3)) * sizeof(float));
		   model_elements = (int*)realloc(model_elements, (model_num_elements + 3) * sizeof(int));
		   for (size_t i = 0; i < 3; i++)
		   {
			   for (j = 0; j < model_num_vertices; j++)
			   {
				   if (isIdentical(model_vertices[j * 3], vertex[i][0]) &&
					   isIdentical(model_vertices[(j * 3) + 1], vertex[i][1]) &&
					   isIdentical(model_vertices[(j * 3) + 2], vertex[i][2]) &&

					   isIdentical(model_normals[j * 3], normal[i][0]) &&
					   isIdentical(model_normals[(j * 3) + 1], normal[i][1]) &&
					   isIdentical(model_normals[(j * 3) + 2], normal[i][2]) &&

					   isIdentical(model_textures[j * 2], normal[i][0]) &&
					   isIdentical(model_textures[(j * 2) + 1], texture[i][1]))
				   {
					   model_elements[model_num_elements] = j;
					   model_num_elements++;
					   break;
				   }
			   }
			   if (j == model_num_vertices)
			   {
				   model_vertices[model_num_vertices * 3] = vertex[i][0];
				   model_vertices[(model_num_vertices * 3) + 1] = vertex[i][1];
				   model_vertices[(model_num_vertices * 3) + 2] = vertex[i][2];

				   model_normals[model_num_vertices * 3] = normal[i][0];
				   model_normals[(model_num_vertices * 3) + 1] = normal[i][1];
				   model_normals[(model_num_vertices * 3) + 2] = normal[i][2];

				   model_textures[model_num_vertices * 2] = texture[i][0];
				   model_textures[(model_num_vertices * 2) + 1] = texture[i][1];

				   model_elements[model_num_elements] = model_num_vertices;

				   model_num_vertices++;
				   model_num_elements++;
			   }
		   }
	   }
private:bool isIdentical(float val1, float val2)
{
	if (abs(val1 - val2) < diff)
		return true;
	else
		return false;
}

};

IModelLoader *getModelLoader()
{
	if (!gpModelLoader)
		gpModelLoader = new ModelLoader;
	return gpModelLoader;
}
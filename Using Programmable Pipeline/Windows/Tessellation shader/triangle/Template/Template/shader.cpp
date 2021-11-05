#include <cstdlib>
#include <iostream>
#include <fstream>
#include "setShader.h"
#include "glFiles.h"

using namespace std;

//function protypes

//Function to read text file
char* readTextFile(char* fileName);


int setShader(char* shaderType, char* shaderFile , FILE* gpFile)
{
	void uninitialize(void);
	int shaderId;
	
	char* shaderSourceFile = readTextFile(shaderFile);
	char *str;

	if (shaderType == "vertex")
	{
		str = (char*)malloc(sizeof("vertex"));
		strcpy(str, "vertex");

		shaderId = glCreateShader(GL_VERTEX_SHADER);

	}
	else if (shaderType == "tessControl")
	{
		str = (char*)malloc(sizeof("tessellationControl"));
		strcpy(str, "tessellationControl");
		shaderId = glCreateShader(GL_TESS_CONTROL_SHADER);

	}
	else if (shaderType == "tessEvaluation")
	{
		str = (char*)malloc(sizeof("tesselationEvaluationShader"));
		strcpy(str, "tessellationEvaluation");
		shaderId = glCreateShader(GL_TESS_EVALUATION_SHADER);

	}
	else if (shaderType == "geometry")
	{
		str = (char*)malloc(sizeof("geometry"));
		strcpy(str, "geometry");

		shaderId = glCreateShader(GL_GEOMETRY_SHADER);
	}
	else if (shaderType == "fragment")
	{
		str = (char*)malloc(sizeof("fragment"));
		strcpy(str, "fragment");

		shaderId = glCreateShader(GL_FRAGMENT_SHADER);
	}

	glShaderSource(shaderId, 1, (const char**)&shaderSourceFile, NULL);
	glCompileShader(shaderId);


	GLint iCompileStatus;
	GLint iInfoLogLength;
	char *szInfoLog = NULL;

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			GLsizei written;
			glGetShaderInfoLog(shaderId, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "%s Shader compile error Log : %s",str,szInfoLog);
			free(szInfoLog);
			free(str);
			uninitialize();
			exit(0);
		}
	}
	
	free(str);
	return shaderId;
}



char* readTextFile(char* fileName)
{
	FILE* fp = fopen(fileName, "rb");
	char* content = NULL;
	long numVal = 0;
	
	//go till end
	fseek(fp, 0L, SEEK_END);

	//get count bytes
	numVal = ftell(fp);
	
	fseek(fp, 0L, SEEK_SET); //reset
	content = (char*)malloc( (numVal+1) * sizeof(char));

	fread(content, 1, numVal, fp);	//read into buffer
	content[numVal] = '\0';	//end with null termination

	fclose(fp);
	return content;
}












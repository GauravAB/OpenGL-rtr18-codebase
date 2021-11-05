#version 440 core
layout (vertices =4)out;
uniform int numberOfSegments;
uniform int numberOfStrips;


void main(void)
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	gl_TessLevelOuter[0] = float(numberOfStrips);
	gl_TessLevelOuter[1] = float(numberOfSegments);
}


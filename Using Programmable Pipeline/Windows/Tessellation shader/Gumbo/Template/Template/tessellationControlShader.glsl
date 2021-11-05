#version 440 core
layout (vertices = 16)out;

uniform int numberOfSegments;
uniform int numberOfStrips;

void main(void)
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	if(gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = numberOfSegments;
		gl_TessLevelOuter[1] = numberOfSegments;
		gl_TessLevelOuter[2] = numberOfSegments;
		gl_TessLevelOuter[3] = numberOfSegments;

		gl_TessLevelInner[0] = numberOfStrips;
		gl_TessLevelInner[1] = numberOfStrips;
	
	}
}


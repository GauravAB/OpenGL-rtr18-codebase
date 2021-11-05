#version 440 core

layout(triangles,  ccw) in;

uniform mat4 u_mvp_matrix;

void main(void)
{
	vec4 vPosition = gl_TessCoord.x * gl_in[0].gl_Position +
					 gl_TessCoord.y * gl_in[1].gl_Position +
					 gl_TessCoord.z * gl_in[2].gl_Position;

	gl_Position = u_mvp_matrix * vPosition;	
}


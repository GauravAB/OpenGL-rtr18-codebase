#version 440 core

layout(triangles,equal_spacing,cw) in;

out vec3 tePosition;
out vec3 tePatchDistance;

uniform mat4 u_modelview_matrix;
uniform mat4 u_projection_matrix;

void main(void)
{
	vec3 p0 =  gl_TessCoord.x * gl_in[0].gl_Position.xyz;
	vec3 p1 =  gl_TessCoord.y * gl_in[1].gl_Position.xyz;
	vec3 p2 =  gl_TessCoord.z * gl_in[2].gl_Position.xyz;
	
	tePatchDistance = gl_TessCoord;
	tePosition = normalize(p0+p1+p2);

	gl_Position = u_projection_matrix * u_modelview_matrix * vec4(tePosition,1.0);	
}


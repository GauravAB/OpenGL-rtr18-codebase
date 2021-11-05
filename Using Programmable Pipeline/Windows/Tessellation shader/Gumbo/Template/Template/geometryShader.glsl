#version 440 core

uniform mat4 u_modelview_matrix;
uniform mat4 u_projection_matrix;

layout(triangles)in;
layout(triangle_strip,max_vertices = 3) out;
in vec4 tePatchDistance[3];
out vec3 gFacetNormal;
out vec4 gPatchDistance;
out vec3 gTriDistance;


void main()
{
	vec3 A = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 B = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	gFacetNormal  = mat3(u_modelview_matrix) * normalize(cross(A,B));

	gPatchDistance = tePatchDistance[0];
	gTriDistance = vec3(1.0,0.0,0.0);
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gPatchDistance = tePatchDistance[1];
	gTriDistance = vec3(0.0,1.0,0.0);
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	gPatchDistance = tePatchDistance[2];
	gTriDistance = vec3(0.0,0.0,1.0);
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}
